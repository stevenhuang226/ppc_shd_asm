#include "../../include/global.h"
#include "../../include/parse.h"
#include "../../include/encode.h"

/* Auth: ChatGPT */
size_t enc_or(const Register *regs)
{
	if (regs[0].type != REG_GPR
	|| regs[1].type != REG_GPR
	|| regs[2].type != REG_GPR) {
		printf("or: register check failed\n");
		return 0;
	}

	if (checkChunk(4) < 0) return 0;

	uint32_t ra = (uint32_t)regs[0].code;
	uint32_t rs = (uint32_t)regs[1].code;
	uint32_t rb = (uint32_t)regs[2].code;

	uint32_t instruction
	= (31u << 26)
	| (rs << 21)
	| (ra << 16)
	| (rb << 11)
	| (444u << 1);

	writeU32BE(
		outputChunk + outputChunkPtr,
		instruction
	);
	outputChunkPtr += 4;

	return 4;
}

