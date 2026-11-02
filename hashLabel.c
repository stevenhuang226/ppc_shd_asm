#include "include/global.h"

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
const int step) {
	if (capacity == 0)
		return NULL;

	HashStrMap *map = malloc(sizeof(HashStrMap) +
		capacity * sizeof(HashStrNode));

	map->step = step;
	map->count = 0;

	map->capacity = capacity;
	map->defVal = defVal;

	for (int i = 0; i < map->capacity; ++i) {
		map->data[i].string[0] = '\0';
		map->data[i].val = defVal;
	}

	return map;
}

uint32_t hashStr64(const char *s)
{
	uint32_t h = 2166136261u;
	while (*s) {
		h ^= (uint8_t)*s++;
		h *= 16777619u;
	}
	return h;
}

uint32_t hash32(const uint32_t src, const uint32_t lim)
{
	return src % lim;
}

void HashStrMapUpdate(
HashStrMap *map,
const char *key,
const uint32_t val) {
	if (key == NULL)
		return;

	uint32_t u32Key = hashStr64(key);
	uint32_t h = hash32(u32Key, map->capacity);
	while (map->data[h].string[0]) {
		if (strcmp(map->data[h].string, key) == 0) {
			map->data[h].val = val;
			return;
		}
		h = hash32(h + map->step, map->capacity);
	}

	snprintf(map->data[h].string, sizeof(map->data[h].string),
	"%s", key);
	map->data[h].val = val;
	return;
}

uint32_t HashStrMapSearch(
HashStrMap *map,
const char *key) {
	if (key == NULL)
		return map->defVal;

	uint32_t u32Key = hashStr64(key);
	uint32_t h = hash32(u32Key, map->capacity);
	while (map->data[h].string[0]) {
		if (strcmp(map->data[h].string, key) == 0)
			return map->data[h].val;

		h = hash32(h + map->step, map->capacity);
	}

	return map->defVal;
}
