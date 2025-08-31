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

struct __attribute__((packed)) gdt_entry {
	uint16_t low_limit;
	uint16_t low_base;
	uint8_t mid_base;
	uint8_t access;
	uint8_t flags;
	uint8_t high_base;
};

// need 13 bytes of data split into 26 nibbles
// named based on the starting indices of the two nibbles
struct __attribute__((packed)) tss {
	uint8_t x00_04;
	uint8_t x08_0C;
	uint8_t x10_14;
	uint8_t x18_1C;
	uint8_t x20_24;
	uint8_t x28_2C;
	uint8_t x30_34;
	uint8_t x38_3C;
	uint8_t x40_44;
	uint8_t x48_4C;
	uint8_t x50_54;
	uint8_t x58_5C;
	uint8_t x60_64;
};

extern void set_gdt(uint32_t limit, uint64_t base);
extern void reload_segments(void);

void kernel_main(void);

// entry point, runs init stuff before moving to kernel_main
void kernel_early(void){
	if(LIMINE_BASE_REVISION_SUPPORTED == false){
		hcf();
	}

	struct tss tss = {
		.x00_04 = 0,

	};

	size_t tss_size = sizeof(tss)-1;
	uintptr_t tss_addr = (uintptr_t)&tss;

	struct gdt_entry gdt[7] = {
		(struct gdt_entry) {0}, // null entry
		(struct gdt_entry) { // kernel code segment
			.low_base = 0,
			.mid_base = 0,
			.high_base = 0,
			.low_limit = 0xFFFF,
			.flags = 0xAF,
			.access = 0x9A
		},
		(struct gdt_entry) { // kernel data segment
			.low_base = 0,
			.mid_base = 0,
			.high_base = 0,
			.low_limit = 0xFFFF,
			.flags = 0xCF,
			.access = 0x92
		},
		(struct gdt_entry) { // user code segment
			.low_base = 0,
			.mid_base = 0,
			.high_base = 0,
			.low_limit = 0xFFFF,
			.flags = 0xAF,
			.access = 0xFA
		},
		(struct gdt_entry) { // user data segment
			.low_base = 0,
			.mid_base = 0,
			.high_base = 0,
			.low_limit = 0xFFFF,
			.flags = 0xCF,
			.access = 0xF2
		},
		(struct gdt_entry) { // task state segment (lower)
			.low_base = tss_addr & 0xFFFF,
			.mid_base = (tss_addr >> 16) & 0xFF,
			.high_base = (tss_addr >> 24) & 0xFF,
			.low_limit = tss_size & 0xFFFF,
			.flags = 0x0 | tss_size >> 16,
			.access = 0x89,
		},
		(struct gdt_entry) { // task state segment (upper)
			.low_limit = (tss_addr >> 32) & 0xFFFF,
			.low_base = (tss_addr >> 48) & 0xFFFF,
			// rest of the upper half is reserved
			.mid_base = 0,
			.access = 0,
			.flags = 0,
			.high_base = 0,
		}
	};

	set_gdt(sizeof(gdt)-1, (uint64_t)&gdt);
	reload_segments();

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
