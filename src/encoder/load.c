#include "../../include/global.h"
#include "../../include/parse.h"
#include "../../include/encode.h"

/*
 * Load instructions (zero-extend).
 *
 * Operand order:
 *   regs[0]: RT (destination GPR)
 *   regs[1]: RA (base GPR)
 *   regs[2]: D  (signed 16-bit displacement)
 *
 * Supported instructions:
 *   l4z: Load 4 bytes (lwz), opcode 32.
 *   l2z: Load 2 bytes (lhz), opcode 40.
 *   l1z: Load 1 byte  (lbz), opcode 34.
 *
 * Effective address:
 *   EA = (RA == 0) ? D : GPR[RA] + D
 *
 * Loaded values are zero-extended to 32 bits.
 *
 * enc_load() handles shared validation and encoding.
 */
size_t enc_load(const Register *regs, uint32_t opcode)
{
    if (regs[0].type != REG_GPR
    ||  regs[1].type != REG_GPR
    ||  regs[2].type != REG_NR) {
        printf("ERROR: load: invalid operand type\n");
        return 0;
    }

    if (regs[0].code < 0 || regs[0].code > 31
    ||  regs[1].code < 0 || regs[1].code > 31
    ||  !immi16Check(regs[2].code)) {
        printf("ERROR: load: operand out of range\n");
        return 0;
    }

    if (!checkChunk(4))
        return 0;

    uint32_t instruction
    = (opcode << 26)
    | ((uint32_t)regs[0].code << 21)
    | ((uint32_t)regs[1].code << 16)
    | ((uint32_t)regs[2].code & 0xFFFFu);

    writeU32BE(outputChunk + outputChunkPtr, instruction);
    outputChunkPtr += 4;

    return 4;
}

size_t enc_l4z(const Register *regs)
{
    return enc_load(regs, 32);
}

size_t enc_l2z(const Register *regs)
{
    return enc_load(regs, 40);
}

size_t enc_l1z(const Register *regs)
{
    return enc_load(regs, 34);
}
