#ifndef STRING_H
#define STRING_H

#include <compiler.h>
#include <sys/types.h>

__BEGIN_CDECLS

char* strcpy(char *dest, const char *src);

char* strncpy(char *dest, const char *src, size_t n);

size_t strlen(const char *s);

char* strcat(char *dest, const char *src);

int strcmp(const char *stra, const char *strb);

int memcmp(const void *s1, const void *s2, size_t n);

void *memcpy(void *dest, void*src, size_t n);

void *memset(void *s, int c, size_t n);

__END_CDECLS

#endif // STRING_H

