#pragma once

//#include "types.h"

extern void uart_send_byte_lowlevel(unsigned char);

//int putchar(int c);

//fixme: see stdio.c
//int getchar(void);

int raw_getchar(void);

char *gets(char *);

int puts(const char *);

int printk(const char *, ...);

int sprintk(char *, const char *, ...);

int snprintk(char *, int, const char *, ...);


//int fflush(int);

void ClearScreen(void);

#ifdef DEBUG
#define DPRINT(fmt, args ...)   printf(fmt, ##args)
#define GEN_DGB() printf("%s(): line %d\n", __FUNCTION__, __LINE__)
#else
#define DPRINT(fmt, args ...)
#define GEN_DGB()
#endif

