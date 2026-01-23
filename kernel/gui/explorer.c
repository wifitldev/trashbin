#include "gui/explorer.h"
#include "gui/gui.h"
#include "fs/vfs.h"
#include "drivers/mouse.h"
#include "drivers/keyboard.h"
#include "lib/string.h"

static char current_path[64] = "/";

static void draw_entries(void) {
    vfs_entry_t entries[32];
    int count = vfs_listdir(current_path, entries, 32);
    gui_draw_window(40, 40, 500, 400, "Explorer");
    if (count < 0) {
        gui_draw_text(60, 80, "Failed to list directory", 0xFFFFFFFF);
        return;
    }
    int y = 80;
    for (int i = 0; i < count; i++) {
        gui_draw_text(60, y, entries[i].name, entries[i].type == 2 ? 0xFF7CFC00 : 0xFFFFFFFF);
        y += 12;
    }
}

void explorer_task(void* arg) {
    (void)arg;
    for (;;) {
        mouse_state_t mouse = mouse_state();
        gui_draw_rect(0, 0, 1024, 768, 0xFF202020);
        draw_entries();
        gui_draw_cursor(mouse);
        char c;
        if (keyboard_read(&c)) {
            if (c == '\n') {
                if (strcmp(current_path, "/") != 0) {
                    strncpy(current_path, "/", sizeof(current_path) - 1);
                }
            }
        }
    }
}
