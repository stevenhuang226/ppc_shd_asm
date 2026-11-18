#ifndef _PSS
#define _PSS
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

#define UPDATE_LABEL_C '*'
#define LABEL_FIXED_C '^'
#define LABEL_RELATE_C '.'

#define INSTRUCTION_OPTION_C ':'

#endif
