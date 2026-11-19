#include "../../include/global.h"

int8_t hexValue(const char c)
{
	if (c >= '0' && c <= '9')
		return c - '0';
	if (c >= 'a' && c <= 'f')
		return (int8_t)(c - 'a' + 10);
	if (c >= 'A' && c <= 'F')
		return (int8_t)(c - 'A' + 10);
	return -1;
}

const char *parseImm(
	const char *s,
	const int32_t base,
	int32_t *retValue
) {
	int8_t negative = (*s == '-');
	if (negative || *s == '+') ++s;

	int32_t res = 0;
	int8_t h;
	while ((h = hexValue(*s)) >= 0
	&& h < base) {
		res = res * base + h;
		++s;
	}

	if (negative) {
		*retValue = -res;
	} else {
		*retValue = res;
	}

	return s;
}
