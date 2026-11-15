#include "../../include/global.h"
#include "../../include/parse.h"
#include "../../include/encode.h"

size_t enc_addis(const Register *regs)
{
	if (regs[0].type != REG_GPR
	|| (regs[1].type != REG_NR && regs[1].type != REG_GPR)
	|| regs[2].type != REG_NR) {
		printf("addis: register check failed\n");
		return 0;
	}

	if (!imm16Check(regs[2].code)) {
		printf("addis: imm(16) check failed\n");
		return 0;
	}

	if (checkChunk(4) < 0) return 0;

	uint32_t ra = regs[0].code;
	uint32_t rt = regs[1].code;
	uint16_t imm = (uint16_t)regs[2].code;

	uint32_t instruction
	= (15u << 26)
	| (ra << 21)
	| (rt << 16)
	| imm;

	writeU32BE(
		outputChunk + outputChunkPtr,
		instruction
	);
	outputChunkPtr += 4;

	return 4;
}
