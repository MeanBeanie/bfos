#include "limine.h"
#include "kernel_debug.h"
#include "pic.h"
#include "memory.h"
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

// entry point, runs init stuff before moving to kernel_main
void kernel_early(void){
	if(LIMINE_BASE_REVISION_SUPPORTED == false){
		hcf();
	}

	if(framebuffer_request.response == NULL
	|| framebuffer_request.response->framebuffer_count < 1){
		hcf();
	}

	pic_remap(0x20, 0xA0);

	kernel_main();
}

// kernel entry point
void kernel_main(void){
	struct limine_framebuffer* framebuffer = framebuffer_request.response->framebuffers[0];

	kdbg_printf(framebuffer, 10, 10, "Hello, World!");

	hcf();
}
