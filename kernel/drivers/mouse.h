#pragma once
#include "types.h"

typedef struct {
    int x;
    int y;
    int buttons;
} mouse_state_t;

void mouse_init(void);
mouse_state_t mouse_state(void);
