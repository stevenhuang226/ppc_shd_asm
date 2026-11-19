#include "../include/global.h"
#include "../include/token.h"
#include "../include/utils.h"
#include "../include/lookup.h"
#include "../include/source.h"
#include "../include/hash_label.h"

void computeAddress(Token *token, size_t size)
{
	uint32_t address = 0;
	for (size_t i = 0; i < size; ++i) {
		TokenType t = token[i].type;

		switch (t) {
		case TKN_INSTRUCTION:
			address += 4;
			break;
		case TKN_DATA:
			address += token[i].data.size;
			break;
		case TKN_LABEL:
			token[i].label.address = address;
			break;
		case TKN_ALIGN:
			address = (address + 3u) & ~3u;
			break;
		}
	}
}

void translateData(const Token *t)
{
	size_t size = t->data.size;
	checkOutputChunk(size);

	memcpy(
		outputChunk + outputChunkPtr,
		t->data.ptr
	);

	outputChunkPtr += size;
}

int8_t translateInstruction(const Token *t)
{
	Encoder enc = lookupEncoder(t->instruction.cmd);

	size_t w = enc(t->instruction.slots);

	if (w == 0) return -1;
	return 0;
}

int8_t translateRegion(
	const SourceArray *src,
	const Token *token,
	const size_t begin,
	const size_t end,
	HSMStack *hsm
) {
	if (src == NULL
	|| token == NULL
	|| hsm == NULL
	|| (begin >= end)
	|| (hsm->top >= (int)hsm->capacity))
		return -1;

	hsm->stack[++hsm->top] = HashStrMapCreate(
		LABEL_REGION_MAXCNT,
		0,
		1
	);

	uint32_t skipId = 0;
	for (size_t i = begin; i <= end; ++i) {
		const Token *t = &token[i];

		if (t->type == TKN_SCOPE_BEGIN
		&& skipId == 0) {
			skipId = t->id;
			continue;
		}

		if (t->type == TKN_SCOPE_END
		&& t->id == skipId) {
			skipId = 0;
			continue;
		}

		if (skipId || t->type != TKN_LABEL)
			continue;

		HashStrMapUpdate(
			hsm->stack[hsm->top],
			t->label.name,
			t->label.address
		);
	}

	size_t childBegin = 0;
	uint32_t childId = 0;

	for (size_t i = begin; i <= end; ++i) {
		const Token *t = &token[i];

		if (childId == 0
		&& t->type == TKN_SCOPE_BEGIN) {
			childId = t->id;
			childBegin = i+1;
			continue;
		}

		if (childId != 0
		&& t->type == TKN_SCOPE_END) {
			translateRegion(
				src,
				token,
				childBegin,
				i-1,
				hsm
			);

			childBegin = childId = 0;
			continue;
		}

		if (childId) continue;

		if (t->type == TKN_ALIGN) {
			outputChunkPtr
			= (outputChunkPtr + 3u) & ~3u;
		} else if (t->type == TKN_DATA) {
			translateData(t);
		} else if (t->type == TKN_INSTRUCTION) {
			translateInstruction(t);
		}
	}

	free(hsm->stack[hsm->top]);
	hsm->stack[hsm->top] = NULL;
	--hsm->top;

	return 0;
}
