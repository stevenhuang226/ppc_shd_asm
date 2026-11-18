#include "../include/source.h"
#include "../include/token.h"
#include "../include/global.h"
#include "../include/utils.h"

//#include "../include/lookup.v2.h"


int8_t isUpdateLabel(const char *s)
{
	if (*s == UPDATE_LABEL_C)
		return true;

	return false;
}

const char *parseUpdateLabel(const char *s, Token *t)
{
	t->type = TKN_LABEL;

	size_t len = 0;
	while (!isPssEnd(*s)
	&& isPssCode(*s)
	&& len+1 < LABEL_MAXLEN) {
		t->label.name[len++] = *s++;
	}
	t->label.name[len] = '\0';
	t->label.address = 0;

	return s;
}

int8_t isData(const char *s)
{
	if (chkHead(s, "dauto")
	|| chkHead(s, "d%d")) return true;

	return false;
}

const char *parseDataAuto(const char *s, Token *t)
{
	while (isPssSpace(*s)) ++s;

	if (*s != '"') {
		printf("dauto: support string only\n");
		return NULL;
	}

	const char *head = ++s;
	size_t size = 0;
	while (*s != '"') {
		if (*s == '\\') ++s;
		++size; ++s;
	}
	++size;

	t->data.ptr = malloc(size);
	if (t->data.ptr == NULL) return NULL;
	t->data.size = size;

	s = head;
	size_t cpy = 0;
	while (*s != '"') {
		if (*s == '\\') ++s;
		t->data.ptr[cpy++] = (uint8_t)*s++;
	}
	t->data.ptr[cpy] = '\0';

	return s + 1;
}

const char *parseDataFixedString(
	const char *s,
	uint8_t *w,
	size_t limit
) {
	if (*s != '"') return NULL;
	++s;

	size_t count = 0;

	while (*s != '"'
	&& count < limit) {
		if (*s == '\\') ++s;
		w[count++] = *s++;
	}

	return s + (*s == '"');
}

const char *parseDataFixedBits(
	const char *s,
	uint8_t *w,
	size_t limit
) {
	if (*s != 'b') return NULL;
	++s;

	size_t len = 0;
	const char *head = s;
	while (*s == '1' || *s == '0') {
		++len; ++s;
	}
	if (len == 0) return NULL;

	size_t bits = 0;
	while (len > 0 && bits < limit * 8) {
		if (head[--len] == '1') {
			w[limit - bits / 8 - 1]
			|= 1u << (bits % 8);
		}

		++bits;
	}

	return s;
}

const char *parseDataFixedHex(
	const char *s,
	uint8_t *w,
	size_t limit
) {
	if (*s != 'x') return NULL;
	++s;

	size_t len = 0;
	const char *head = s;
	while (*s && hexValue(*s) > 0) {
		++len; ++s;
	}

	if (len == 0 || len > limit * 2) return NULL;

	size_t bytes = 0;
	while (len > 0 && bytes < limit) {
		int8_t l = hexValue(head[--len]);
		int8_t h = 0;

		if (len > 0) h = hexValue(head[--len]);

		if (h < 0 || l < 0) {
			printf("hex data: wrong value\n");
			return NULL;
		}

		w[limit - bytes - 1]
		= (uint8_t)h << 4 | (uint8_t)l;

		++bytes;
	}

	return s;
}

const char *parseDataFixedDigit(
	const char *s,
	uint8_t *w,
	size_t limit
) {
	if (*s == 'd') ++s;

	int8_t negative = (*s == '-');
	if (negative || *s == '+') ++s;

	const char *head = s;

	size_t len = 0;
	while (*s &&
	(*s >= '0' && *s <= '9')) {
		++len; ++s;
	}
	if (len == 0) return s;

	uint8_t buffer[len];
	for (size_t i = 0; i < len; ++i) {
		buffer[i] = head[i] - '0';
	}

	for (size_t byte = 0; byte < limit; ++byte) {
		uint16_t remaind = 0;
		int8_t nonzero = false;

		for (size_t i = 0; i < len; ++i) {
			uint16_t value
			= remaind * 10 + buffer[i];

			buffer[i] = value / 256;
			remaind = value % 256;

			if (buffer[i] != 0) nonzero = true;
		}

		w[limit - byte - 1] = (uint8_t)remaind;

		if (!nonzero) break;
	}

	if (negative) {
		uint16_t carry = 1;
		for (size_t i = limit; i > 0; --i) {
			uint16_t value
			= (uint8_t)~w[i - 1] + carry;

			w[i - 1] = (uint8_t)value;
			carry = value >> 8;
		}
	}

	return s;
}

const char *parseData(const char *s, Token *t)
{
	t->type = TKN_DATA;
	if (chkHead(s, "auto"))
		return parseDataAuto(s+4, t);

	int32_t immValue;
	parseImm(s, 10, &immValue);

	if (immValue <= 0) return NULL;

	size_t size = (size_t)immValue;

	t->data.ptr = malloc(size);
	t->data.size = size;
	if (t->data.ptr == NULL) return NULL;
	memset(t->data.ptr, 0x00, size);

	while (isPssSpace(*s)) ++s;

	if (*s == '"') {
		s = parseDataFixedString(
			s,
			t->data.ptr,
			size
		);
	} else if (*s == 'b') {
		s = parseDataFixedBits(
			s,
			t->data.ptr,
			size
		);
	} else if (*s == 'x') {
		s = parseDataFixedHex(
			s,
			t->data.ptr,
			size
		);
	} else {
		s = parseDataFixedDigit(
			s,
			t->data.ptr,
			size
		);
	}

	if (s == NULL) {
		free(t->data.ptr); t->data.ptr = NULL;
		t->data.size = 0;
	}

	return s;
}

int8_t isScope(const char *s)
{
	if (*s == '{'
	|| *s == '}') return true;
	return false;
}

const char *parseScope(
	const char *s,
	Token *t,
	uint32_t *scopeId
) {
	if (*s == '{') {
		t->type = TKN_SCOPE_BEGIN;
		t->id = ++(*scopeId);
	} else if (*s == '}') {
		t->type = TKN_SCOPE_END;
		t->id = (*scopeId)--;
	} else return NULL;

	return s + 1;
}

int8_t isAlign(const char *s)
{
	if (chkHead(s, "align")) return true;
	return false;
}

const char *parseAlign(const char *s, Token *t)
{
	t->type = TKN_ALIGN;
	return s + 5;
}

const char *parseSlotLabel(
	const char *s,
	Slot *l
) {
	l->type = SLOT_LABEL;

	if (*s == LABEL_FIXED_C) l->label.fixed = true;
	else l->label.fixed = false;
	++s;

	size_t labelLen = parsePssWord(
		s,
		l->label.name,
		LABEL_MAXLEN
	);

	if (labelLen == 0) {
		printf("instruction slot: label\n");
		return NULL;
	}

	return s + labelLen;
}

const char *parseSlotImm(const char *s, Slot *l)
{
	l->type = SLOT_IMM;
	return parseImm(s, 10, &l->imm);
}

const char *parseSlot(const char *s, Slot *l)
{
	if (*s == LABEL_FIXED_C
	|| *s == LABEL_RELATE_C) {
		return parseSlotLabel(s, l);
	}

	if (isdigit(*s)) {
		return parseSlotImm(s, l);
	}

	l->type = SLOT_REG;
	if (chkHead(s, "r")) {
		l->reg.type = REG_GPR; s += 1;
	} else if (chkHead(s, "f")) {
		l->reg.type = REG_FPR; s += 1;
	} else if (chkHead(s, "cr")) {
		l->reg.type = REG_CR; s += 2;
	} else if (chkHead(s, "spr")) {
		l->reg.type = REG_SPR; s += 3;
	} else if (chkHead(s, "msr")) {
		l->reg.type = REG_MSR; s += 3;
	} else if (chkHead(s, "fpscr")) {
		l->reg.type = REG_FPSCR; s += 5;
	} else if (chkHead(s, "tbu")) {
		l->reg.type = REG_TBU; s += 3;
	} else if (chkHead(s, "sr")) {
		l->reg.type = REG_SR; s += 2;
	} else if (chkHead(s, "v")) {
		l->reg.type = REG_V; s += 1;
	} else {
		l->type = SLOT_EXCEPTION;
	}

	if (l->type == SLOT_EXCEPTION) {
		size_t len = parsePssWord(
			s,
			l->exception,
			INSTRUCTION_SLOT_MAXLEN
		);
		printf("instruction slot: exception chars\n");
		if (len == 0) return NULL;
		return s + len;
	}

	int32_t val;
	s = parseImm(s, 10, &val);

	if (s == NULL
	|| val < 0
	|| val > UINT16_MAX) {
		printf("instruction slot: imm (u16) check failed\n");
		return NULL;
	}

	l->reg.code = (uint16_t)val;
	return s;
}

const char *parseInstruction(const char *s, Token *t)
{
	t->type = TKN_INSTRUCTION;

	size_t cmdLen = parsePssWord(
		s,
		t->instruction.cmd,
		INSTRUCTION_CMD_MAXLEN
	);

	if (cmdLen == 0) {
		printf("parse instruction: cmd\n");
		return NULL;
	}

	s += cmdLen;

	size_t slotCount = 0;
	while (!isPssEnd(*s)
	&& slotCount < INSTRUCTION_MAXREGC) {
		while (isPssSpace(*s)) ++s;
		s = parseSlot(
			s,
			&t->instruction.slots[slotCount++]
		);
	}

	return s;
}

const char *parseStr(
	const char *s,
	Token *t,
	uint32_t *scopeId
) {
	if (isUpdateLabel(s)) {
		return parseUpdateLabel(s+1, t);
	} else if (isData(s)) {
		return parseData(s+1, t);
	} else if (isScope(s)) {
		return parseScope(s, t, scopeId);
	} else if (isAlign(s)) {
		return parseAlign(s, t);
	} else {
		return parseInstruction(s, t);
	}
}

Token *parseToken(
	SourceArray *src,
	size_t *retSize
) {
	if (src == NULL || retSize == NULL) return NULL;

	size_t tokenCapacity = src->capacity;
	size_t tokenCount = 0;

	Token *tokens = malloc(
		tokenCapacity * sizeof(Token)
	);

	if (tokens == NULL) goto cleanup;

	uint32_t scopeId = 0;
	for (size_t i = 0; i < src->count; ++i) {
		if (tokenCount + 32 >= tokenCapacity) {
			tokenCapacity *= 2;

			Token *new = realloc(
				tokens,
				tokenCapacity * sizeof(Token)
			);

			if (new == NULL) goto cleanup;

			tokens = new;
		}

		const char *s = src->lines[i].content;
		int8_t failed = false;

		for (;;) {
			while (isPssSpace(*s)) ++s;
			if (isPssEnd(*s)) break;

			Token *t = &tokens[tokenCount++];
			t->srcId = i;

			s = parseStr(s, t, &scopeId);
		}

		if (failed) {
			printf("Parse Token ERROR\n");
			printf(
				"file: %s, line: %d\n",
				src->lines[i].filename,
				src->lines[i].lineNumber
			);
			printf(
				"%s",
				src->lines[i].content
			);
		}
	}

	*retSize = tokenCount;
	return tokens;

cleanup:
	free(tokens); tokens = NULL;
	*retSize = 0;

	return 0;
}
