#include "../../include/global.h"
#include "../../include/token.h"
#include "../../include/utils.h"

size_t enc_addi(const Slot *slots)
{
	if (slots[0].type != SLOT_REG
	|| (slots[1].type != SLOT_REG && slots[1].type != SLOT_IMM)
	|| slots[2].type != SLOT_IMM) {
		printf("addi: slot type check failed\n");
		return 0;
	}

	if (!immi16Check(slots[2].imm)) {
		printf("addi: imm(i16) check failed\n");
	}

	checkOutputChunk(4);
}
