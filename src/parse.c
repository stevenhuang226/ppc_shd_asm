#include "../include/global.h"
#include "../include/utils.h"
#include "../include/setting.h"
#include "../include/parse.h"
#include "../include/source.h"

/*
 * .<label name>
 */
int8_t isLabel(const char *s)
{
	if (*s++ != '.')
		return false;
	if (*s && isPssCode(*s))
		return true;
	return false;
}

const char *parseLabel(const char *s, Parsed *p)
{
	p->type = PSS_LABEL;

	size_t len = 0;
	++s;
	while (*s
	&& isPssCode(*s)
	&& len+1 < LABEL_MAXLEN) {
		p->label.name[len++] = *s++;
	}
	p->label.name[len++] = '\0';
	p->label.address = 0;

	return s;
}

/*
 * bc <cr value>(can be empty) <condition> <label name>
 * bl <cr value>(can be empty) <condition> <label name>
 */
int8_t isBranch(const char *s)
{
	if (chkHead_NE(s, "bc ")
	|| chkHead_NE(s, "bl ")
	|| chkHead_NE(s, "bca ")
	|| chkHead_NE(s, "bla "))
		return true;
	return false;
}

const char *parseBranch(const char *s, Parsed *p)
{
	p->type = PSS_BRANCH;
	if (*s++ != 'b') return NULL;

	if (*s++ == 'l') p->branch.link = true;
	else p->branch.link = false;
	if (*s == 'a') {
		p->branch.aa = true;
		++s;
	} else {
		p->branch.aa = false;
	}
	++s;

	while (*s && !isPssCode(*s)) ++s;

	int8_t cr = 0;
	while (isdigit((uint8_t)*s)) {
		cr = cr * 10 + *s - '0';
		++s;
	}
	p->branch.cr = cr;

	while (*s && !isPssCode(*s)) ++s;

	Condition condition;
	if (chkHead(s, "no")) {
		condition = CON_NO; s += 2;
	} else if (chkHead(s, "gt")) {
		condition = CON_GT; s += 2;
	} else if (chkHead(s, "ge")) {
		condition = CON_GE; s += 2;
	} else if (chkHead(s, "lt")) {
		condition = CON_LT; s += 2;
	} else if (chkHead(s, "le")) {
		condition = CON_LE; s += 2;
	} else if (chkHead(s, "eq")) {
		condition = CON_EQ; s += 2;
	} else if (chkHead(s, "ne")) {
		condition = CON_NE; s += 2;
	} else if (chkHead(s, "so")) {
		condition = CON_SO; s += 2;
	} else {
		return NULL;
	}
	p->branch.condition = condition;

	while (*s && !isPssCode(*s)) ++s;

	size_t len = 0;
	while (*s
	&& isPssCode(*s)
	&& len+1 < LABEL_MAXLEN) {
		p->branch.target[len++] = *s++;
	}
	p->branch.target[len++] = '\0';

	return s;
}

/*
 * dauto
 * + "<string>"
 *
 * d<size>
 * + d<digit>
 * + x<hex value>
 * + b<bits>
 * + "<string>"
 */
int8_t isData(const char *s)
{
	if (chkHead(s, "dauto") || chkHead(s, "d%d"))
		return true;
	return false;
}

const char *parseDataAuto(const char *s, Parsed *p)
{
	while (*s && !isPssCode(*s)) ++s;

	if (*s != '"')
		return NULL;

	const char *head = ++s;
	size_t size = 0;
	while (*s != '"') {
		if (*s == '\\') ++s;
		++size; ++s;
	}
	++size;

	p->data.ptr = malloc(size);
	if (p->data.ptr == NULL)
		return NULL;
	p->data.size = size;

	s = head;
	size_t cpy = 0;
	while (*s != '"') {
		if (*s == '\\') ++s;
		p->data.ptr[cpy++] = (uint8_t)*s++;
	}
	p->data.ptr[cpy++] = '\0';

	return s+1;
}

const char *parseDataFixedString(
	const char *s,
	char *w,
	size_t limit
) {
	if (*s != '"') return NULL;

	size_t count = 0;

	++s;
	while (*s != '"' && count < limit) {
		if (*s == '\\') ++s;
		w[count++] = *s++;
	}

	return s + (*s == '"');
}

const char *parseDataFixedBits(
	const char *s,
	char *w,
	size_t limit
) {
	if (*s != 'b') return NULL;

	size_t bits = 0;

	++s;
	while ((*s == '0' || *s == '1')
	&& (bits/8) < limit) {
		if (*s == '1') {
			w[bits/8] |= 1 << (7-(bits%8));
		}

		++bits; ++s;
	}

	return s;
}

const char *parseDataFixedHex(
	const char *s,
	char *w,
	size_t limit
) {
	if (*s != 'x') return NULL;

	++s;
	size_t bytes = 0;
	while (s[0] && s[1] && bytes < limit) {
		int8_t h = hexValue(s[0]);
		int8_t l = hexValue(s[1]);

		if (h < 0 || l < 0) return NULL;

		w[bytes] = (uint8_t)h << 4 | (uint8_t)l;

		++bytes; s += 2;
	}

	return s;
}

const char *parseDataFixedDigit(
	const char *s,
	char *w,
	size_t limit
) {
	if (*s == 'd') ++s;

	if (!isdigit((uint8_t)*s) && *s != '-')
		return NULL;

	int8_t negative = false;
	if (*s == '-') {
		negative = true; ++s;
	}

	uint32_t value = 0;
	while (isdigit((uint8_t)*s)) {
		value = value * 10 + *s - '0';
		++s;
	}

	if (negative) {
		value = 0 - value;
	}

	for (size_t i = 1; i <= limit; ++i) {
		w[limit-i] = (uint8_t)(value & 0xFF);
		value >>= 8;
	}

	return s;
}

const char *parseDataFixed(
	const char *s,
	Parsed *p,
	size_t limit
) {
	p->data.ptr = malloc(limit);
	if (p->data.ptr == NULL)
		return NULL;
	p->data.size = limit;
	memset(p->data.ptr, 0x00, limit);

	while (!isPssCode(*s)) ++s;

	if (*s == '"') {
		s = parseDataFixedString(
			s,
			p->data.ptr,
			limit
		);
	} else if (*s == 'b') {
		s = parseDataFixedBits(
			s,
			p->data.ptr,
			limit
		);
	} else if (*s == 'x') {
		s = parseDataFixedHex(
			s,
			p->data.ptr,
			limit
		);
	} else {
		s = parseDataFixedDigit(
			s,
			p->data.ptr,
			limit
		);
	}

	if (s == NULL) {
		free(p->data.ptr); p->data.ptr = NULL;
	}

	return s;
}

const char *parseData(const char *s, Parsed *p)
{
	p->type = PSS_DATA;

	if (chkHead(s, "dauto")) {
		return parseDataAuto(s+5, p);
	}

	size_t size = 0;

	++s;
	while (isdigit((uint8_t)*s)) {
		size = size * 10 + *s - '0';
		++s;
	}

	return parseDataFixed(s, p, size);
}

/*
 * { or }
 */
int8_t isScope(const char *s)
{
	if (*s == '{' || *s == '}')
		return true;
	return false;
}

const char *parseScope(
	const char *s,
	Parsed *p,
	uint32_t *scopeId
) {
	if (*s == '{') {
		p->type = PSS_SCOPE_BEGIN;
		p->id = ++(*scopeId);
	} else if (*s == '}') {
		p->type = PSS_SCOPE_END;
		p->id = (*scopeId)--;
	} else {
		return NULL;
	}

	return s + 1;
}

/*
 * instruction
 */
const char *parseRegister(const char *s, Register *reg)
{
	reg->type = REG_NR;

	if (chkHead(s, "r")) {
		reg->type = REG_GPR; s += 1;
	} else if (chkHead(s, "f")) {
		reg->type = REG_FPR; s += 1;
	} else if (chkHead(s, "cr")) {
		reg->type = REG_CR; s += 2;
	} else if (chkHead(s, "spr")) {
		reg->type = REG_SPR; s += 3;
	} else if (chkHead(s, "msr")) {
		reg->type = REG_MSR; s += 3;
	} else if (chkHead(s, "fpscr")) {
		reg->type = REG_FPSCR; s += 5;
	} else if (chkHead(s, "tbu")) {
		reg->type = REG_TBU; s += 3;
	} else if (chkHead(s, "sr")) {
		reg->type = REG_SR; s += 2;
	} else if (chkHead(s, "v")) {
		reg->type = REG_V; s += 1;
	}

	int32_t code = 0;
	int8_t negative = false;
	if (*s == '-') {
		negative = true; ++s;
	}
	while (isdigit((uint8_t)*s)) {
		code = code * 10 + *s - '0';
		++s;
	}
	if (negative) code = -code;

	reg->code = code;

	return s;
}

const char *parseInstruction(const char *s, Parsed *p)
{
	p->type = PSS_INSTRUCTION;

	size_t len = 0;
	while (!isPssEnd(*s)
	&& isPssCode(*s)
	&& len+1 < INSTRUCTION_MAXLEN) {
		p->instruction.cmd[len++] = *s++;
	}
	p->instruction.cmd[len++] = '\0';

	if (len <= 1) return NULL;

	size_t regc = 0;
	while (!isPssEnd(*s)
	&& regc < INSTRUCTION_REGCS) {
		while (!isPssCode(*s)) ++s;
		s = parseRegister(
			s,
			&p->instruction.registers[regc++]
		);

		if (s == NULL) break;
	}

	return s;
}

/* align */
int8_t isAlign(const char *s)
{
	if (chkHead(s, "align"))
		return true;
	return false;
}

const char *parseAlign(const char *s, Parsed *p)
{
	p->type = PSS_ALIGN;
	return s + 5;
}



const char *parseStr(
	const char *s,
	Parsed *p,
	uint32_t *scopeId
) {
	if (isLabel(s)) {
		return parseLabel(s, p);
	} else if (isData(s)) {
		return parseData(s, p);
	} else if (isBranch(s)) {
		return parseBranch(s, p);
	} else if (isScope(s)) {
		return parseScope(s, p, scopeId);
	} else if (isAlign(s)) {
		return parseAlign(s, p);
	} else {
		return parseInstruction(s, p);
	}
}

Parsed *parseSource(
	SourceArray *src,
	size_t *returnSize
) {
	if (!src || !returnSize)
		return NULL;

	size_t parsedCapacity = src->capacity;
	size_t parsedCount = 0;

	Parsed *parsed = malloc(
		parsedCapacity * sizeof(Parsed)
	);

	if (parsed == NULL) goto cleanup;

	uint32_t scopeId = 0;
	for (size_t i = 0; i < src->count; ++i) {
		if (parsedCount + 32 >= parsedCapacity) {
			parsedCapacity *= 2;

			Parsed *new = realloc(
				parsed,
				parsedCapacity * sizeof(Parsed)
			);

			if (new == NULL) goto cleanup;

			parsed = new;
		}

		const char *s = src->lines[i].content;
		int8_t failed = false;

		for (;;) {
			while (!isPssCode(*s)) ++s;
			if (isPssEnd(*s)) break;

			Parsed *p = &parsed[parsedCount++];
			p->srcId = i;

			s = parseStr(s, p, &scopeId);

			if (s == NULL) {
				failed = true; break;
			}
		}

		if (failed) {
			printf("Parse ERROR\n");
			printf(
				"file: %s, line:%d\n",
				src->lines[i].filename,
				src->lines[i].lineNumber
			);
			printf(
				"%s",
				src->lines[i].content
			);
		}
	}

	*returnSize = parsedCount;
	return parsed;
cleanup:
	*returnSize = 0;
	free(parsed); parsed = NULL;
	return NULL;
}
