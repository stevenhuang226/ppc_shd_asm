#include "../../include/global.h"
#include "../../include/token.h"
#include "../../include/utils.h"

size_t enc_and_base(const Slot *slots, int8_t _rc)
{
	if (slots[0].type != SLOT_REG
	|| slots[0].reg.type != REG_GPR
	|| slots[1].type != SLOT_REG
	|| slots[1].reg.type != REG_GPR
	|| slots[2].type != SLOT_REG
	|| slots[2].reg.type != REG_GPR) {
		printf("and: slot check failed\n");
		return 0;
	}

	uint32_t ra = (uint32_t)slots[0].reg.code;
	uint32_t rs = (uint32_t)slots[1].reg.code;
	uint32_t rb = (uint32_t)slots[2].reg.code;
	uint32_t rc;

	if (_rc) rc = 1u;

	uint32_t instruction
	= (31u << 26)
	| (rs << 21)
	| (ra << 16)
	| (rb << 11)
	| (28u << 1)
	| rc;

	if (checkOutputChunk(4) < 0) return 0;

	writeU32BE(
		outputChunk + outputChunkPtr,
		instruction
	);
	outputChunkPtr += 4;

	return 4;
}

size_t enc_and(const Slot *slots)
{
	return enc_and_base(slots, false);
}
