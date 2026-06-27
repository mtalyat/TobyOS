#pragma once

#if defined(__INTELLISENSE__)
#define ASM_VOLATILE(...)
#elif defined(__GNUC__) || defined(__clang__)
#define ASM_VOLATILE(...) __asm__ __volatile__(__VA_ARGS__)
#else
#define ASM_VOLATILE(...)
#endif

#define NOOP() ASM_VOLATILE("nop")
#define HALT() ASM_VOLATILE("hlt")
#define DISABLE_INTERRUPTS() ASM_VOLATILE("cli")
#define ENABLE_INTERRUPTS() ASM_VOLATILE("sti")