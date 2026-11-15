#include "global.h"

#ifndef HASH_LABEL_H
#define HASH_LABEL_H

typedef struct HashStrNode {
	char string[64];
	uint32_t val;
} HashStrNode;

typedef struct HashStrMap {
	int count;
	int step;

	size_t capacity;
	uint32_t defVal;
	HashStrNode data[];
} HashStrMap;

HashStrMap *HashStrMapCreate(
	const size_t capacity,
	const uint32_t defVal,
	const int step
);

void HashStrMapUpdate(
	HashStrMap *map,
	const char *key,
	const uint32_t val
);

uint32_t HashStrMapSearch(
	const HashStrMap *map,
	const char *key
);

#endif
