#include "../include/global.h"

#include "../include/lookup.h"

#include "lookupTable.c"

Encoder lookupEncoder(const char *name)
{
	size_t encoderCount = sizeof(encoders) / sizeof(encoders[0]);

	for (size_t i = 0; i < encoderCount; ++i) {
		if (strcmp(name, encoders[i].name) == 0)
			return encoders[i].func;
	}

	return NULL;
}
