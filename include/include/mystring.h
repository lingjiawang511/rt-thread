#pragma once

#include "stdio.h"

#define ISHEX(b) (((b) >= 'a' && (b) <= 'f') || ((b) >= 'A' && (b) <= 'F') || ((b) >= '0' && (b) <= '9'))
#define ISDIGIT(x) ((x) >= '0' && (x) <= '9')

char * strncpy(char *, const char *, size_t);

size_t strnlen(const char *, size_t);

size_t strlen(const char *);

int strcmp(const char *, const char *);

int strncmp(const char *, const char *, size_t);

char *strcpy(char *, const char *);

char *strcat(char *, const char *);

char *strncat(char *, const char *, size_t);

long memcmp(const void*, const void*, size_t);

void *memcpy(void *, const void*, size_t);

void *memmove(void *, const void*, size_t);

void *memset(void *, int, size_t);

char *strchr(const char *, size_t);

char *strrchr(const char *, size_t);
void stradd(char *str1, char *str2);
