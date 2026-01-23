#pragma once
#include "types.h"

void timer_init(uint32_t hz);
uint64_t timer_ticks(void);
void timer_tick_internal(void);
