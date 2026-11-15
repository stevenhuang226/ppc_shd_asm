#include "../../include/global.h"

int8_t checkChunk(const size_t append)
{
	if (outputChunkPtr + append < outputChunkSize)
		return 0;

	outputChunkSize *= 2;

	outputChunk
	= realloc(outputChunk, outputChunkSize);

	if (outputChunk == NULL) {
		printf("output chunk realloc failed\n");
		return -1;
	}

	return 0;
}

int8_t imm16Check(const int64_t n)
{
	if (n > INT16_MAX || n < INT16_MIN) {
		return false;
	}

	return true;
}

void writeU32BE(uint8_t *dst, uint32_t value)
{
	dst[0] = (uint8_t)(value >> 24);
	dst[1] = (uint8_t)(value >> 16);
	dst[2] = (uint8_t)(value >> 8);
	dst[3] = (uint8_t)value;
}
