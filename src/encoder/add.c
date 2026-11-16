#include "../../include/global.h"
#include "../../include/parse.h"
#include "../../include/encode.h"

/* Auth: ChatGPT */
/*
 * add RT, RA, RB
 *
 * Extended registers:
 *   regs[3]: OE (Overflow Enable), 0 or 1
 *   regs[4]: Rc (Record), 0 or 1
 *
 * OE = 0: Do not enable overflow exception recording.
 * OE = 1: Enable overflow exception recording.
 *
 * Rc = 0: Do not update CR0.
 * Rc = 1: Update CR0.
 *
 * Default: OE = 0, Rc = 0.
 */
size_t enc_add(const Register *regs)
{
	if (regs[0].type != REG_GPR
	|| regs[1].type != REG_GPR
	|| regs[2].type != REG_GPR
	|| regs[3].type != REG_NR
	|| regs[4].type != REG_NR) {
		printf("add: register check failed\n");
		return 0;
	}

	if ((regs[3].code != 0 && regs[3].code != 1)
	|| (regs[4].code != 0 && regs[4].code != 1)) {
		printf("add: OE/Rc check failed\n");
		return 0;
	}

	if (checkChunk(4) < 0) return 0;

	uint32_t ra = (uint32_t)regs[0].code;
	uint32_t rt = (uint32_t)regs[1].code;
	uint32_t rb = (uint32_t)regs[2].code;
	uint32_t oe = (uint32_t)regs[3].code;
	uint32_t rc = (uint32_t)regs[4].code;

	uint32_t instruction
	= (31u << 26)
	| (rt << 21)
	| (ra << 16)
	| (rb << 11)
	| (266u << 1)
	| (oe << 10)
	| rc;

	writeU32BE(
		outputChunk + outputChunkPtr,
		instruction
	);
	outputChunkPtr += 4;

	return 4;
}
