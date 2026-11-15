#ifndef LOOKUP_H
#define LOOKUP_H

#include "parse.h"

typedef size_t (*Encoder)(const Register *regs);

typedef struct {
	const char *name;
	Encoder func;
} EncoderEntry;

extern EncoderEntry encoders[];

Encoder lookupEncoder(const char *name);
#endif
