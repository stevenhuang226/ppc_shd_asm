#include "../../include/global.h"
#include "../../include/token.h"
#include "../../include/utils.h"


size_t enc_andi(const Slot *slots)
{
	if (slots[0].type != SLOT_REG
	|| slots[0].reg.type != REG_GPR
	|| slots[1].type != SLOT_REG
	|| slots[1].reg.type != REG_GPR
	|| slots[2].type != SLOT_IMM) {
		printf("andi: slot type check failed\n");
		return 0;
	}

	if (!immu16Check(slots[2].imm)) {
		printf("andi: imm(u16) check failed\n");
		return 0;
	}

	uint32_t ra = (uint32_t)slots[0].reg.code;
	uint32_t rs = (uint32_t)slots[1].reg.code;
	uint16_t ui = (uint32_t)slots[2].imm;

	uint32_t instruction
	= (28u << 26)
	| (rs << 21)
	| (ra << 16)
	| ui;

	if (checkOutputChunk(4) < 0) return 0;

	writeU32BE(
		outputChunk + outputChunkPtr,
		instruction
	);
	outputChunkPtr += 4;

	return 0;
}
