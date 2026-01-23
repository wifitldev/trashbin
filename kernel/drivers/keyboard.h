#pragma once
#include "types.h"

void keyboard_init(void);
void keyboard_isr(void);
int keyboard_read(char* out);
