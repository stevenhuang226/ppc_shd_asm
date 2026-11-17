#include "../include/global.h"
#include "../include/setting.h"

#include "../include/translator.v3.h"
#include "../include/parse.h"
#include "../include/source.h"
#include "../include/lookup.h"

void computeAddress(Parsed *parsed, size_t size)
{
	uint32_t address = 0;
	for (size_t i = 0; i < size; ++i) {
		PssType type = parsed[i].type;

		if (type == PSS_LABEL) {
			parsed[i].label.address = address;
		} else if (type == PSS_DATA) {
			address += parsed[i].data.size;
		} else if (type == PSS_INSTRUCTION) {
			address += 4;
		} else if (type == PSS_BRANCH) {
			parsed[i].branch.address = address;
			address += 4;
		} else if (type == PSS_ALIGN) {
			address = (address + 3u) & ~3u;
		}
	}
}

int8_t translateData(
	const Parsed *p
) {
	size_t size = p->data.size;
	if (outputChunkPtr + size >= outputChunkSize) {
		outputChunkSize *= 2;
		outputChunk
		= realloc(outputChunk, outputChunkSize);
	}

	memcpy(
		outputChunk + outputChunkPtr,
		p->data.ptr, size
	);
	outputChunkPtr += size;
	return 0;
}

int8_t translateInstruction(
	const Parsed *p
) {
	Encoder enc = lookupEncoder(p->instruction.cmd);

	size_t ws
	= enc(p->instruction.registers);

	if (ws == 0) return -1;

	return 0;
}

int8_t translateBranch(
	const Parsed *p,
	const HSMStack *hsm
) {
	uint32_t target
	= HashStrMapSearch(
		hsm->stack[hsm->top],
		p->branch.target
	);

	uint32_t displacement = 0;

	if (p->branch.aa) {
		printf("Warring: branch: linker not support, address not accurate\n");
		displacement = target;
	} else {
		displacement
		= p->branch.address - target;
	}

	return 0;
	/*
	 * TODO
	 * give stuff to branch encoder
	 */
}

int8_t translateRegion(
	const SourceArray *src,
	const Parsed *parsed,
	const size_t begin,
	const size_t end,
	HSMStack *hsm
) {
	if (src == NULL
	|| parsed == NULL
	|| hsm == NULL
	|| (begin >= end)) {
		return -1;
	}

	if (hsm->top >= (int)hsm->capacity) return -1;

	hsm->stack[++hsm->top]
	= HashStrMapCreate(LABEL_REGION_MAXCNT, 0, 1);

	uint32_t skipId = 0;
	for (size_t i = begin; i <= end; ++i) {
		const Parsed *p = &parsed[i];

		if (!skipId
		&& p->type == PSS_SCOPE_BEGIN) {
			skipId = p->id; continue;
		}

		if (p->type == PSS_SCOPE_END
		&& p->id == skipId) {
			skipId = 0; continue;
		}

		if (skipId || p->type != PSS_LABEL)
			continue;

		HashStrMapUpdate(
			hsm->stack[hsm->top],
			p->label.name,
			p->label.address
		);
	}

	size_t childBegin = 0;
	uint32_t childId = 0;

	for (size_t i = begin; i <= end; ++i) {
		const Parsed *p = &parsed[i];

		if (!childId
		&& p->type == PSS_SCOPE_BEGIN) {
			childId = p->id;
			childBegin = i+1;
			continue;
		}

		if (childId
		&& p->type == PSS_SCOPE_END) {
			translateRegion(
				src,
				parsed,
				childBegin,
				i-1,
				hsm
			);

			childBegin = childId = 0;
		}

		if (childId
		|| p->type == PSS_LABEL
		|| p->type == PSS_SCOPE_BEGIN
		|| p->type == PSS_SCOPE_END)
			continue;

		if (p->type == PSS_ALIGN) {
			outputChunkPtr
			= (outputChunkPtr + 3u) & ~3u;
		}

		if (p->type == PSS_INSTRUCTION) {
			translateInstruction(p);
		} else if (p->type == PSS_BRANCH) {
			translateBranch(p, hsm);
		} else if (p->type == PSS_DATA) {
			translateData(p);
		}
	}

	free(hsm->stack[hsm->top--]);

	return 0;
}
