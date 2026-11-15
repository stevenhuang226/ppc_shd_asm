#include "global.h"

#ifndef TRANSLATOR3_H
#define TRANSLATOR3_H
#include "hash_label.h"

typedef struct {
	size_t capacity;
	int top;
	HashStrMap *stack[];
} HSMStack;

#endif
