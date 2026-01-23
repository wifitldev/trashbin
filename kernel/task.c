#include "task.h"
#include "mm/heap.h"
#include "lib/string.h"
#include "console.h"

extern void context_switch(context_t* old, context_t* new);

static task_t* task_list_head;
static task_t* current;
static uint64_t next_id = 1;

static void task_trampoline(void) {
    task_entry_t entry = (task_entry_t)current->context.rdi;
    void* arg = (void*)current->context.rsi;
    entry(arg);
    current->active = 0;
    task_yield();
    for (;;) {
        __asm__ volatile ("hlt");
    }
}

void task_init(void) {
    task_list_head = NULL;
    current = NULL;
}

uint64_t task_spawn(const char* name, task_entry_t entry, void* arg) {
    task_t* task = (task_t*)kmalloc(sizeof(task_t));
    if (!task) {
        return 0;
    }
    memset(task, 0, sizeof(task_t));
    task->id = next_id++;
    task->active = 1;
    strncpy(task->name, name, sizeof(task->name) - 1);

    uint8_t* stack = (uint8_t*)kmalloc(8192);
    if (!stack) {
        return 0;
    }
    uint64_t stack_top = (uint64_t)(stack + 8192);
    stack_top -= sizeof(uint64_t);
    *(uint64_t*)stack_top = (uint64_t)task_trampoline;

    task->context.rdi = (uint64_t)entry;
    task->context.rsi = (uint64_t)arg;
    task->context.rsp = stack_top;

    task->next = NULL;
    if (!task_list_head) {
        task_list_head = task;
    } else {
        task_t* iter = task_list_head;
        while (iter->next) {
            iter = iter->next;
        }
        iter->next = task;
    }

    if (!current) {
        current = task;
    }
    return task->id;
}

void task_yield(void) {
    if (!current || !current->next) {
        return;
    }
    task_t* prev = current;
    task_t* next = current->next;

    while (next && !next->active) {
        next = next->next;
    }
    if (!next) {
        next = task_list_head;
        while (next && !next->active) {
            next = next->next;
        }
    }
    if (next && next != current) {
        current = next;
        context_switch(&prev->context, &current->context);
    }
}

void task_tick(void) {
    task_yield();
}

void task_list(void) {
    task_t* iter = task_list_head;
    console_write("Tasks:\n");
    while (iter) {
        console_write(" - ");
        console_write(iter->name);
        console_putc('\n');
        iter = iter->next;
    }
}
