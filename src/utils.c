#include "../include/global.h"

int8_t isPssEnd(const char c)
{
	if (c == '\0'
	|| c == '\n'
	|| c == '#') {
		return true;
	}
	return false;
}

int8_t isPssCode(const char c)
{
	if (isspace(c) || c == ',')
		return false;

	return true;
}

int8_t isPssSpace(const char c)
{
	return !isPssCode(c);
}

int8_t chkHead_NE(const char *s, const char *t)
{
	while (*s) {
		if (t[0] == '%' && t[1] == 'd') {
			if (!isdigit(*s))
				return false;

			while (*s
			&& isdigit(*s)) ++s;

			t += 2;
		}

		if (t[0] == '%' && t[1] == 's') {
			if (!isPssSpace(*s))
				return false;
			while (*s
			&& isPssSpace(*s)) ++s;

			t += 2;
		}

		if (*t == '\0')
			return true;

		if (*s != *t)
			return false;

		++s; ++t;
	}

	return *t == '\0';
}

int8_t chkHead(const char *s, const char *t)
{
	while (!isPssEnd(*s) && isPssCode(*s)) {
		if (t[0] == '%' && t[1] == 'd') {
			if (!isdigit((uint8_t)*s))
				return false;
			while (!isPssEnd(*s)
			&& isdigit((uint8_t)*s))
				++s;
			t += 2;
			continue;
		}

		if (*t == '\0')
			return true;

		if (*s != *t)
			return false;

		++s; ++t;
	}
	return *t == '\0';
}

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

	if (negative) *retValue = -res;
	*retValue = res;

	return s;
}

size_t parsePssWord(
	const char *s,
	char *w,
	size_t limit
) {
	size_t len = 0;
	while (!isPssEnd(*s)
	&& isPssCode(*s)
	&& len + 1 < limit) {
		w[len++] = *s++;
	}
	w[len] = '\0';

	return len;
}
