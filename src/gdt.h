#ifndef GDT_H
#define GDT_H
#include <stdint.h>

struct {
	uint16_t limit;
	uint64_t base;
} __attribute__((packed)) gdtr;

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

extern void set_gdt(void);
extern void reload_segments(void);

void let_there_be_gdt(void);

#endif // GDT_H
