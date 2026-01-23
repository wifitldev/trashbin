#pragma once
#include "types.h"
#include "drivers/mouse.h"

void gui_init(uint64_t fb_addr, uint32_t width, uint32_t height, uint32_t pitch, uint32_t bpp);
void gui_tick(void);
void gui_draw_text(int x, int y, const char* text, uint32_t color);
void gui_draw_rect(int x, int y, int w, int h, uint32_t color);
void gui_draw_window(int x, int y, int w, int h, const char* title);
void gui_draw_cursor(mouse_state_t mouse);
