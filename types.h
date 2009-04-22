#pragma once

#include <stdbool.h>
#include <stdint.h>

#define asizeof(x) (sizeof(x)/sizeof(*x))

#define struct(name) typedef struct name name; struct name
#define union(name) typedef union name name; union name
#define enum(name) typedef enum name name; enum name

typedef unsigned long ulong;
typedef unsigned int uint;
typedef unsigned short ushort;
typedef unsigned char uchar;

typedef int16_t s16;
typedef uint16_t u16;
typedef int32_t s32;
typedef uint32_t u32;
typedef int64_t s64;
typedef uint64_t u64;
