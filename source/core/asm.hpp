#pragma once

#if defined(__INTELLISENSE__)
#define ASM_VOLATILE(...)
#elif defined(__GNUC__) || defined(__clang__)
#define ASM_VOLATILE(...) __asm__ __volatile__(__VA_ARGS__)
#else
#define ASM_VOLATILE(...)
#endif
