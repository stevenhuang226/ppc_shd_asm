#include "../../include/global.h"
#include "../../include/token.h"
#include "../../include/utils.h"

size_t enc_addis(const Slot *slots)
{
	if (slots[0].type != SLOT_REG
	|| (slots[1].type != SLOT_REG && slots[1].type != SLOT_IMM)
	|| slots[2].type != SLOT_IMM) {
		printf("addis: slot type check failed\n");
		return 0;
	}

	if (!immi16Check(slots[2].imm)) {
		printf("addis: imm(i16) check failed\n");
	}

	uint32_t rt = slots[0].reg.code;
	uint32_t ra = slots[1].reg.code;
	uint16_t imm = (uint16_t)slots[2].imm;

	uint32_t instruction
	= (15u << 26)
	| (rt << 21)
	| (ra << 16)
	| imm;

	if (checkOutputChunk(4) < 0) return 0;

	writeU32BE(
		outputChunk + outputChunkPtr,
		instruction
	);
	outputChunk += 4;

	return 4;
}
