#include "../../include/global.h"
#include "../../include/parse.h"
#include "../../include/encode.h"

size_t enc_ori(const Register *regs)
{
	if (regs[0].type != REG_GPR
	|| (regs[1].type != REG_GPR && regs[1].type != REG_NR)
	|| regs[2].type != REG_NR) {
		printf("ori: register check failed\n");
		return 0;
	}

	if (regs[2].code < 0 || regs[2].code > 0xFFFF) {
		printf("ori: imm(u16) check failed\n");
		return 0;
	}

	if (checkChunk(4) < 0) return 0;

	uint32_t ra = regs[0].code;
	uint32_t rs = regs[1].code;
	uint16_t ui = (uint16_t)regs[2].code;

	uint32_t instruction
	= (24u << 26)
	| (rs << 21)
	| (ra << 16)
	| ui;

	writeU32BE(
		outputChunk + outputChunkPtr,
		instruction
	);
	outputChunkPtr += 4;

	return 4;
}
