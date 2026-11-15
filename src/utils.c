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
	if (isspace(c)
	|| c == ',')
		return false;

	return true;
}

int8_t chkHead_NE(const char *s, const char *t)
{
	while (*s) {
		if (*t == '\0')
			return true;

		if (t[0] == '%' && t[1] == 'd') {
			if (!isdigit((uint8_t)*s))
				return false;

			while (*s
			&& isdigit((uint8_t)*s)) {
				++s;
			}

			t += 2;
		}

		if (*s != *t)
			return false;

		++s; ++t;
	}

	return *t == '\0';
}

int8_t chkHead(const char *s, const char *t)
{
	while (!isPssEnd(*s) && isPssCode(*s)) {
		if (*t == '\0')
			return true;

		if (*t == '%' && t[1] == 'd') {
			if (!isdigit(*s))
				return false;
			while (!isPssEnd(*s) && isdigit(*s))
				++s;
			t += 2;
			continue;
		}

		if (*s != *t)
			return false;

		++s; ++t;
	}
	return *t == '\0';
}

static int8_t hexValue(const char c)
{
	if (c >= '0' && c <= '9')
		return c - '0';
	if (c >= 'a' && c <= 'f')
		return (int8_t)(c - 'a' + 10);
	if (c >= 'A' && c <= 'F')
		return (int8_t)(c - 'A' + 10);
	return -1;
}
