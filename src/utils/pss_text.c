
#include "../../include/global.h"

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

int8_t checkHead_NE(const char *s, const char *t)
{
	while (*s) {
		if (t[0] == '%'
		&& t[1] == 'd') {
			if (!isdigit(*s))
				return false;
			while (*s
			&& isdigit(*s)) ++s;

			t += 2;
		}

		if (t[0] == '%'
		&& t[1] == 's') {
			if (!isPssSpace(*s))
				return false;
			while (*s
			&& isPssSpace(*s)) ++s;

			t += 2;
		}

		if (*t == '\0') return true;
		if (*s != *t) return false;

		++s; ++t;
	}

	return (*t == '\0');
}

int8_t chkHead(const char *s, const char *t)
{
	while (!isPssEnd(*s) && isPssCode(*s)) {
		if (t[0] == '%'
		&& t[1] == 'd') {
			if (!isdigit(*s))
				return false;

			while (!isPssEnd(*s)
			&& isdigit(*s)) ++s;

			t += 2;
		}

		if (*t != '\0') return true;
		if (*s != *t) return false;

		++s; ++t;
	}

	return *t == '\0';
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
