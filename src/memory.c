#include "memory.h"
#include "limine.h"
#include <stdint.h>

// implementations taken from https://wiki.osdev.org/Limine_Bare_Bones

void *memcpy(void *restrict dest, const void *restrict src, size_t n) {
	uint8_t *restrict pdest = (uint8_t *restrict)dest;
	const uint8_t *restrict psrc = (const uint8_t *restrict)src;

	for (size_t i = 0; i < n; i++) {
		pdest[i] = psrc[i];
	}

	return dest;
}

void *memset(void *s, int c, size_t n) {
	uint8_t *p = (uint8_t *)s;

	for (size_t i = 0; i < n; i++) {
		p[i] = (uint8_t)c;
	}

	return s;
}

void *memmove(void *dest, const void *src, size_t n) {
	uint8_t *pdest = (uint8_t *)dest;
	const uint8_t *psrc = (const uint8_t *)src;

	if (src > dest) {
		for (size_t i = 0; i < n; i++) {
			pdest[i] = psrc[i];
		}
	} else if (src < dest) {
		for (size_t i = n; i > 0; i--) {
			pdest[i-1] = psrc[i-1];
		}
	}

	return dest;
}

int memcmp(const void *s1, const void *s2, size_t n) {
	const uint8_t *p1 = (const uint8_t *)s1;
	const uint8_t *p2 = (const uint8_t *)s2;

	for (size_t i = 0; i < n; i++) {
		if (p1[i] != p2[i]) {
			return p1[i] < p2[i] ? -1 : 1;
		}
	}

	return 0;
}

// Halt and catch fire function.
void hcf(void) {
	for (;;) {
		asm ("hlt");
	}
}

struct {
	struct limine_memmap_entry* arr[128];
	size_t count;
} usable_entries = {0};

__attribute__((used, section(".limine_requests")))
static volatile struct limine_memmap_request memmap_request = {
	.id = LIMINE_MEMMAP_REQUEST,
	.revision = 0
};

int init_memory_stuff(void){
	if(memmap_request.response == NULL
	|| memmap_request.response->entry_count < 1){
		return 0;
	}

	for(size_t i = 0; i < memmap_request.response->entry_count; i++){
		if(memmap_request.response->entries[i]->type == LIMINE_MEMMAP_USABLE){
			usable_entries.arr[usable_entries.count] = memmap_request.response->entries[i];
			usable_entries.count++;
		}
	}

	// creates header for data, marking it all as free memory
	uint8_t* ptr = (uint8_t*)(RAM_START + usable_entries.arr[0]->base);
	*ptr = 0b00100111; // sets it to free and contiguous and 5 bytes long
	for(int i = 0; i < 8; i++){
		ptr++;
		*ptr = (usable_entries.arr[0]->length >> (i*8)) & 0xFF;
	}

	return 1;
}

// aligns the size with the size of a pointer
size_t align(size_t size){
	return size + (sizeof(intptr_t)-(size % sizeof(intptr_t)));
}

// TODO!! support data being split up across sections
void* malloc(size_t size){
	size = align(size);

	// find the next free section of memory
	for(size_t i = 0; i < usable_entries.count; i++){
		// j will be moved by the headers
		for(size_t j = 0; j < usable_entries.arr[0]->length;){
			uint8_t* ptr = (uint8_t*)(RAM_START + usable_entries.arr[0]->base + j);
			uint64_t len = 0;
			for(int k = 0; k < 8; k++){
				len |= (*(ptr+k+1)) << (k*8);
			}
			
			if((*ptr) & 0x1 && len >= size){ // if space is free and large enough
				*ptr &= ~(1 << 0); // mark the space as not free
				for(int k = 0; k < 8; k++){
					*(ptr+k+1) = (size >> (k*8)) & 0xFF;
				}
				memset(ptr+9, 0, size);
				*(ptr+9+size) = 0b00100111;
				len -= size;
				for(int k = 0; k < 8; k++){
					*(ptr+9+size+k+1) = (len >> (k*8)) & 0xFF;
				}
				return ptr+9;
			}
			else{
				j += 9+len; // adding 9 skips the header, then len skips the data
			}
		}
	}

	return NULL;
}

// TODO!! combine adjacent memory locations
void free(void* ptr){
	uint8_t* header = ptr-9;
	*header |= 0x1; // set free flag
}

void* realloc(void* ptr, size_t size);
