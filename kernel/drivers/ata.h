#pragma once
#include "types.h"

int ata_read(uint64_t lba, uint8_t* buf, uint32_t sectors);
