#ifndef MEMORY_H
#define MEMORY_H
#include <stddef.h>
#include <stdint.h>

#define RAM_START 0xFFFF800000000000

// GCC and CLang both expect these functions to be decalred
// they are implemented as the C standard mandates

void *memcpy(void *dest, const void *src, size_t n);
void *memset(void *s, int c, size_t n);
void *memmove(void *dest, const void *src, size_t n);
int memcmp(const void *s1, const void *s2, size_t n);

void hcf(void);

int init_memory_stuff(void);

// returns NULL on failure, sets allocated memory to 0
void* malloc(size_t size);
// does not zero memory
void free(void* ptr);
void* realloc(void* ptr, size_t size);

#endif // MEMORY_H
