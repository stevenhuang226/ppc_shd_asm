#include "../../include/global.h"

int8_t checkOutputChunk(size_t size)
{
	if (outputChunkPtr + size < outputChunkSize)
		return 0;

	outputChunkSize *= 2;
	uint8_t *new = realloc(
		outputChunk,
		outputChunkSize
	);

	if (new == NULL) return -1;
	else return 0;
}

int8_t immi16Check(const int32_t n)
{
	if (n > INT16_MAX || n < INT16_MIN) {
		return false;
	}

	return true;
}

int8_t immu16Check(const int32_t n)
{
	if (n < 0 || n > UINT16_MAX) {
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
