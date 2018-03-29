#include <stddef.h>
#include <string.h>
#include <assert.h>

char* strcpy(char *dest, const char *src)
{
    char *p = dest;

    while (*src != '\0') {
        *dest++ = *src++;
    }

    return p;
}

char* strncpy(char *dest, const char *src, size_t n)
{
    char *p = dest;

    while (*src != '\0' && n--) {
        *dest++ = *src++;
    }

    return p;
}

size_t strlen(const char *s)
{
    const char *p = s;

    while (*s != '\0')
        s++;

    return s - p;
}

char* strcat(char *dest, const char *src)
{
  char *p = dest;

  dest += strlen(dest);
  while (*src != '\0') {
    *dest++ = *src++;
  }
  *dest = '\0';

  return p;
}

int strcmp(const char *stra, const char *strb)
{
    int ret;

    while (1) {
        ret = *stra - *strb;
        if (!ret || !*stra)
            break;
        stra++;
        strb++;
    }

    return ret;
}

int memcmp(const void *s1, const void *s2, size_t n)
{
    unsigned char *p1, *p2;
    p1 = (unsigned char *)s1;
    p2 = (unsigned char *)s2;

    while (n-- > 0) {
        if (*p1 > *p2)
            return 1;
        else if (*p1 < *p2)
            return -1;

        p1++;
        p2++;
    }

    return 0;
}

void *memcpy(void *dest, void*src, size_t n)
{
    unsigned char *p1 = (unsigned char*)dest;
    unsigned char *p2 = (unsigned char*)src;

    while (n-- > 0) {
        *p1++ = *p2++;
    }

    return dest;
}

void *memset(void *s, int c, size_t n)
{
    unsigned char *p = (unsigned char*)s;
    unsigned char pat = (unsigned char)c;

    while (n-- > 0) {
        *p++ = pat;
    }

    return s;
}

