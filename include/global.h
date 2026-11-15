#ifndef _PSA
#define _PSA
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <stdio.h>

#define MAX(x, y) (((x) > (y)) ? (x) : (y))
#define MIN(x, y) (((x) < (y)) ? (x) : (y))

extern char *sourceChunk;
extern size_t sourceChunkSize;

extern uint8_t *outputChunk;
extern size_t outputChunkSize;
extern size_t outputChunkPtr;

#endif
