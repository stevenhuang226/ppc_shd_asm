#ifndef SOURCE_H
#define SOURCE_H

#include "../include/global.h"

typedef struct {
	char *filename;
	char *content;
	uint32_t lineNumber;
} SourceLine;

typedef struct {
	size_t count;
	size_t capacity;
	SourceLine lines[];
} SourceArray;

#endif
