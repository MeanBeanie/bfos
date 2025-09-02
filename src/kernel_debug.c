#include "kernel_debug.h"
#include <stdbool.h>
#include <stdint.h>

// from https://github.com/dhepper/font8x8 
// ascii compliant so a char can index it
#include "font8x8_basic.h"

void putchar(struct limine_framebuffer* fb, char c, int* x, int* y, uint32_t color){
	volatile uint32_t* fb_ptr = fb->address; 

	for(int i = 0; i < 8; i++){
		for(int j = 0; j < 8; j++){
			int vert_mod = (*y)+(i*2);
			uint32_t col = (font8x8_basic[(int)c][i] >> j) & 1 ? color : 0x000000;
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

void kdbg_printf(struct kernel_debug_printer* printer, const char* fmt, ...){
	if(printer->x < KERNEL_PRINT_OFFSET || (unsigned)printer->x+24 >= printer->fb->width){
		printer->x = KERNEL_PRINT_OFFSET;
	}

	va_list va;
	va_start(va, fmt);
	uint32_t color = 0xFFFFFF;

	// loops until null terminator
	while(*fmt){
		if(*fmt == '\n'){
			printer->y += 24;
		}
		else if(*fmt == '\r'){
			printer->x = KERNEL_PRINT_OFFSET;
		}
		else if(*fmt == '%'){
			fmt++;

			switch(*fmt){
				case 'm':
				{
					int value = va_arg(va, int);
					switch(value){
						case 1: color = 0xFF0000; break;
						case 2: color = 0x00FF00; break;
						case 3: color = 0x0000FF; break;
						case 0:
						default: color = 0xFFFFFF; break;
					}
					break;
				}
				case 's':
				{
					const char* str = va_arg(va, const char*);
					while(*str){
						putchar(printer->fb, *str, &printer->x, &printer->y, color);
						str++;
					}
					break;
				}
				case 'd':
				{
					int value = va_arg(va, int);
					if(value == 0){
						putchar(printer->fb, '0', &printer->x, &printer->y, color);
						break;
					}
					else if(value < 0){
						putchar(printer->fb, '-', &printer->x, &printer->y, color);
						value *= -1;
					}

					int temp = value;
					int p10 = 0;
					while(temp > 0){
						p10 *= 10;
						temp /= 10;
					}

					for(int i = p10; i >= 0; i--){
						putchar(printer->fb, '0'+(value / p10), &printer->x, &printer->y, color);
					}

					break;
				}
				case 'x':
				{
					int value = va_arg(va, int);

					putchar(printer->fb, '0', &printer->x, &printer->y, color); 
					putchar(printer->fb, 'x', &printer->x, &printer->y, color); 

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

						putchar(printer->fb, "0123456789ABCDEF"[nibble], &printer->x, &printer->y, color);
					}

					break;
				}
			}
		}
		else{
			putchar(printer->fb, *fmt, &printer->x, &printer->y, color);
		}

		fmt++;
	}

	va_end(va);
}
