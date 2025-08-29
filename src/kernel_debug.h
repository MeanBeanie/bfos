#ifndef IO_H
#define IO_H
#include "limine.h"

#define va_list  __builtin_va_list
#define va_start __builtin_va_start
#define va_end   __builtin_va_end
#define va_arg   __builtin_va_arg

void kdbg_printf(struct limine_framebuffer* fb, int x, int y, const char* fmt, ...);

#endif // IO_H
