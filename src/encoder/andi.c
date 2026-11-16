#include "../../include/global.h"
#include "../../include/parse.h"
#include "../../include/encode.h"

/*
 * auth: ChatGPT
 */
size_t enc_andi(const Register *regs)
{
	if (regs[0].type != REG_GPR
	|| regs[1].type != REG_GPR
	|| regs[2].type != REG_NR) {
		printf("andi.: register check failed\n");
		return 0;
	}

	if (!immu16Check(regs[2].code)) {
		printf("andi: imm(u16) check failed\n");
		return 0;
	}

	if (checkChunk(4) < 0) return 0;

	uint32_t ra = (uint32_t)regs[0].code;
	uint32_t rs = (uint32_t)regs[1].code;
	uint16_t ui = (uint16_t)regs[2].code;

	uint32_t instruction
	= (28u << 26)
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

