#pragma once

typedef unsigned long long uint64_t;
typedef unsigned int uint32_t;
typedef unsigned short uint16_t;
typedef unsigned char uint8_t;
typedef long long int64_t;
typedef int int32_t;
typedef short int16_t;
typedef signed char int8_t;
typedef unsigned long size_t;
typedef long ssize_t;
typedef unsigned long uintptr_t;

#define NULL ((void*)0)
#define ARRAY_SIZE(x) (sizeof(x) / sizeof((x)[0]))
