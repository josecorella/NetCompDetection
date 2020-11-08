#ifndef _MEM_H_
#define _MEM_H_

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <errno.h>

char *strarrmem(int len);

int *intarrmem(int len);

uint8_t *unsgnintmem(int len);

#endif
