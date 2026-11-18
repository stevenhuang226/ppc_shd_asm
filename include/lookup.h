#ifndef LOOKUP_H
#define LOOKUP_H

#include "token.h"

typedef size_t (*Encoder)(const Slot *slots);

typedef struct {
	const char *name;
	Encoder func;
} EncoderEntry;

extern EncoderEntry encoders[];

Encoder lookupEncoder(const char *name);
#endif
