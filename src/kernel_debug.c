#include "kernel_debug.h"
#include <stdbool.h>
#include <stdint.h>

// from https://github.com/dhepper/font8x8 
// ascii compliant so a char can index it
#include "font8x8_basic.h"

void putchar(struct limine_framebuffer* fb, char c, int* x, int* y){
	if(c == '\n'){
		*y += 24;
		return;
	}
	else if(c == '\r'){
		*x = KERNEL_PRINT_OFFSET;
		return;
	}

	volatile uint32_t* fb_ptr = fb->address; 

	for(int i = 0; i < 8; i++){
		for(int j = 0; j < 8; j++){
			int vert_mod = (*y)+(i*2);
			uint32_t col = (font8x8_basic[(int)c][i] >> j) & 1 ? 0xFFFFFF : 0x000000;
			fb_ptr[(vert_mod)*(fb->pitch/4) + ((*x)+(j*2))] = col;
			fb_ptr[(vert_mod)*(fb->pitch/4) + ((*x)+(j*2)+1)] = col;
			fb_ptr[(vert_mod+1)*(fb->pitch/4) + ((*x)+(j*2))] = col;
			fb_ptr[(vert_mod+1)*(fb->pitch/4) + ((*x)+(j*2)+1)] = col;
		}
	}

	*x += 24;
	if((unsigned)(*x)+24 >= fb->width){
		*x = KERNEL_PRINT_OFFSET;
	}
}

void kdbg_printf(struct limine_framebuffer* fb, int x, int y, const char* fmt, ...){
	if(x < KERNEL_PRINT_OFFSET || (unsigned)x+24 >= fb->width){
		x = KERNEL_PRINT_OFFSET;
	}

	va_list va;
	va_start(va, fmt);

	// loops until null terminator
	while(*fmt){
		if(*fmt == '%'){
			fmt++;

			switch(*fmt){
				case 's':
				{
					const char* str = va_arg(va, const char*);
					while(*str){
						putchar(fb, *str, &x, &y);
						str++;
					}
					break;
				}
				case 'd':
				{
					int value = va_arg(va, int);
					if(value == 0){
						putchar(fb, '0', &x, &y);
						break;
					}
					else if(value < 0){
						putchar(fb, '-', &x, &y);
						value *= -1;
					}

					int temp = value;
					int p10 = 0;
					while(temp > 0){
						p10 *= 10;
						temp /= 10;
					}

					for(int i = p10; i >= 0; i--){
						putchar(fb, '0' + (value / p10), &x, &y);
					}

					break;
				}
				case 'x':
				{
					int value = va_arg(va, int);

					putchar(fb, '0', &x, &y); 
					putchar(fb, 'x', &x, &y); 

					bool had_non_zero = false;
					for(int i = 7; i >= 0; i--){
						int nibble = (value >> (i*4)) & 0xF;
						if(!had_non_zero){
							if(nibble == 0){
								continue;
							}
							else if(nibble != 0){
								had_non_zero = true;
							}
						}

						putchar(fb, "0123456789ABCDEF"[nibble], &x, &y);
					}

					break;
				}
			}
		}
		else{
			putchar(fb, *fmt, &x, &y);
		}

		fmt++;
	}

	va_end(va);
}
