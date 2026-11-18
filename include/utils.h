#ifndef UTILS_H
#define UTILS_H

int8_t isPssEnd(const char c);
int8_t isPssCode(const char c);
int8_t isPssSpace(const char c);
int8_t chkHead_NE(const char *s, const char *t);
int8_t chkHead(const char *s, const char *t);
int8_t hexValue(const char c);
const char *parseImm(
	const char *s,
	const int32_t base,
	int32_t *retValue
);
size_t parsePssWord(
	const char *s,
	char *w,
	size_t limit
);

#endif
