#include "../../include/global.h"
#include "../../include/token.h"
#include "../../include/utils.h"

size_t enc_add_base(const Slot *slots, int8_t _oe, int8_t _rc)
{
	if (slots[0].type != SLOT_REG
	|| slots[0].reg.type != REG_GPR
	|| slots[1].type != SLOT_REG
	|| slots[1].reg.type != REG_GPR
	|| slots[2].type != SLOT_REG
	|| slots[2].reg.type != REG_GPR) {
		printf("add: register check failed\n");
		return 0;
	}

	uint32_t rt = (uint32_t)slots[0].reg.code;
	uint32_t ra = (uint32_t)slots[1].reg.code;
	uint32_t rb = (uint32_t)slots[2].reg.code;
	uint32_t oe, rc;

	if (_oe) oe = 1u;
	if (_rc) rc = 1u;

	uint32_t instruction
	= (31u << 26)
	| (rt << 21)
	| (ra << 16)
	| (rb << 11)
	| (oe << 10)
	| (266u << 9)
	| rc;

	checkOutputChunk(4);

	writeU32BE(
		outputChunk + outputChunkPtr,
		instruction
	);
	outputChunkPtr += 4;

	return 4;
}
