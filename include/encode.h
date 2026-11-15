#include "global.h"

#ifndef ENCODE_H
#define ENCODE_H

int8_t checkChunk(size_t append);
int8_t immi16Check(int64_t n);
int8_t immu16Check(int64_t n);
void writeU32BE(uint8_t *dst, uint32_t value);

#endif
