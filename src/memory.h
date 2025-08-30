#ifndef MEMORY_H
#define MEMORY_H
#include <stddef.h>

// GCC and CLang both expect these functions to be decalred
// they are implemented as the C standard mandates

void *memcpy(void *dest, const void *src, size_t n);
void *memset(void *s, int c, size_t n);
void *memmove(void *dest, const void *src, size_t n);
int memcmp(const void *s1, const void *s2, size_t n);

void hcf(void);

#endif // MEMORY_H
