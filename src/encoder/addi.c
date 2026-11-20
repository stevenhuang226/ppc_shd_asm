#include "../../include/global.h"
#include "../../include/token.h"
#include "../../include/utils.h"

size_t enc_addi(const Slot *slots)
{
	int8_t regCheck = true;

	if (slots[0].type != SLOT_REG
	|| slots[0].reg.type != REG_GPR
	|| slots[2].type != SLOT_IMM) {
		regCheck = false;
	}

	if (slots[1].type == SLOT_REG
	&& slots[1].reg.type != REG_GPR) {
		regCheck = false;
	} else if (slots[1].type != SLOT_IMM) {
		regCheck = false;
	}

	if (!regCheck) {
		printf("addi: slot type check failed\n");
		return 0;
	}

	if (!immi16Check(slots[2].imm)) {
		printf("addi: imm(i16) check failed\n");
	}

	uint32_t rt = (uint32_t)slots[0].reg.code;
	uint32_t ra = (uint32_t)slots[1].reg.code;
	uint16_t imm = (uint16_t)slots[2].imm;

	uint32_t instruction
	= (14u << 26)
	| (rt << 21)
	| (ra << 16)
	| imm;

	if (checkOutputChunk(4) < 0) return 0;

	writeU32BE(
		outputChunk + outputChunkPtr,
		instruction
	);
	outputChunkPtr += 4;

	return 4;
}
