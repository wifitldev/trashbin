#pragma once
#include "types.h"

void fs_init(uint64_t lba, uint64_t sectors);
int fs_read_file(const char* path, uint8_t* buf, size_t max, size_t* out_len);
int fs_list_dir(const char* path, void* out, size_t max_entries);
int fs_make_dir(const char* path);
int fs_make_file(const char* path);
