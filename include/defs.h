#ifndef _DEFS_H
#define _DEFS_H

#define NULL 0

#define bool byte
#define true 1
#define false 0

#define va_start(v,l) __builtin_va_start(v,l)
#define va_arg(v,l)   __builtin_va_arg(v,l)
#define va_end(v)     __builtin_va_end(v)
#define va_copy(d,s)  __builtin_va_copy(d,s)
typedef __builtin_va_list va_list;

typedef unsigned long __uint64_t;
typedef __uint64_t uint64_t;
typedef long __int64_t;
typedef __int64_t int64_t;
typedef unsigned int __uint32_t;
typedef __uint32_t uint32_t;
typedef int __int32_t;
typedef __int32_t int32_t;
typedef unsigned short __uint16_t;
typedef __uint16_t uint16_t;
typedef char __int8_t;
typedef __int8_t int8_t;
typedef unsigned char __uint8_t;
typedef __uint8_t uint8_t;

#define MAX_ROWS 25
#define MAX_COLS 80

#endif
