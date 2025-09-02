#include "gdt.h"
#include <stddef.h>

void let_there_be_gdt(){
	// TODO!! figure out what to put in the TSS
	struct tss tss = { 0 };

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

	gdtr.limit = sizeof(gdt)-1;
	gdtr.base = (uint64_t)&gdt;

	set_gdt();
	reload_segments();
}
