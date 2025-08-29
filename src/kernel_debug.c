#include "kernel_debug.h"
#include <stdint.h>

// from https://github.com/dhepper/font8x8 
// ascii compliant so a char can index it
#include "font8x8_basic.h"

void kdbg_printf(struct limine_framebuffer* fb, int x, int y, const char* fmt, ...){
	va_list va;
	va_start(va, fmt);

	volatile uint32_t* fb_ptr = fb->address;

	// loops until null terminator
	while(*fmt){
		if(*fmt == '%'){
			fmt++;
			// for now ignore
		}
		else{
			for(int i = 0; i < 8; i++){
				for(int j = 0; j < 8; j++){
					int index = ((y+i)*(fb->pitch/4)) + (x+j);
					fb_ptr[index] = (font8x8_basic[(int)(*fmt)][i] >> j) & 1 ? 0xFFFFFF : 0x000000;
				}
			}

			x += 10;
		}

		fmt++;
	}

	va_end(va);
}
