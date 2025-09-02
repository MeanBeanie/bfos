#ifndef IO_H
#define IO_H
#include "limine.h"

#define va_list  __builtin_va_list
#define va_start __builtin_va_start
#define va_end   __builtin_va_end
#define va_arg   __builtin_va_arg

#define KERNEL_PRINT_OFFSET 10

struct kernel_debug_printer {
	struct limine_framebuffer* fb;
	int x, y;
};

// prints an 8x8 font at 2x scale, large spacing for ease of reading
void kdbg_printf(struct kernel_debug_printer* printer, const char* fmt, ...);

#endif // IO_H
