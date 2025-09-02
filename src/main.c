#include "limine.h"
#include "kernel_debug.h"
#include "memory.h"
#include "gdt.h"
#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>

__attribute__((used, section(".limine_requests")))
static volatile LIMINE_BASE_REVISION(3);

__attribute__((used, section(".limine_requests")))
static volatile struct limine_framebuffer_request framebuffer_request = {
	.id = LIMINE_FRAMEBUFFER_REQUEST,
	.revision = 0
};

__attribute__((used, section(".limine_requests_start")))
static volatile LIMINE_REQUESTS_START_MARKER;

__attribute__((used, section(".limine_requests_end")))
static volatile LIMINE_REQUESTS_END_MARKER;

void kernel_main(void);
struct limine_framebuffer* framebuffer = NULL;
struct kernel_debug_printer printer = {0};

// entry point, runs init stuff before moving to kernel_main
void kernel_early(void){
	if(LIMINE_BASE_REVISION_SUPPORTED == false){
		hcf();
	}

	let_there_be_gdt();

	if(framebuffer_request.response == NULL
	|| framebuffer_request.response->framebuffer_count < 1){
		hcf();
	}
	framebuffer = framebuffer_request.response->framebuffers[0];
	printer.fb = framebuffer;
	printer.x = 10;
	printer.y = 10;
	kdbg_printf(&printer, "%m[Y]%m Framebuffer\n\r", 2, 0);
	
	if(!init_memory_stuff()){
		kdbg_printf(&printer, "%m[N]%m Memory", 1, 0);
		hcf();
	}
	kdbg_printf(&printer, "%m[Y]%m Memory\n\r", 2, 0);

	kernel_main();
}

// kernel entry point
void kernel_main(void){
	char* string = malloc(10);
	if(string == NULL){
		kdbg_printf(&printer, "%m[N]%m String Test\n\r", 1, 0);
	}
	else{
		for(int i = 0; i < 9; i++){
			string[i] = 'A'+i;
		}
		string[9] = '\0';
		kdbg_printf(&printer, "%m[Y]%m %s\n\r", 2, 0, string);
		free(string);
	}
	hcf();
}
