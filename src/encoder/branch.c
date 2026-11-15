#include "../../include/global.h"
#include "../../include/parse.h"
#include "../../include/encode.h"

size_t enc_branch_no(
	int8_t link,
	int8_t aa,
	uint32_t displacement
) {
	if (displacement > 0x01FFFFFCu
	&& displacement < 0xFE000000u) {
		printf("ERROR: branch(no): displacement out of range (26bit)\n");
		return 0;
	}

	uint32_t instruction
	= (18u << 26)
	| (displacement & 0x03FFFFFCu)
	| ((uint32_t)aa << 1)
	| (uint32_t)link;

	writeU32BE(
		outputChunk + outputChunkPtr,
		instruction
	);

	outputChunkPtr += 4;

	return 4;
}

size_t enc_branch_con(
	int8_t link,
	int8_t aa,
	int8_t cr,
	uint32_t displacement,
	Condition condition
) {
	static const uint8_t boTable[] = {
		[CON_GT] = 12,
		[CON_GE] = 4,
		[CON_LT] = 12,
		[CON_LE] = 4,
		[CON_EQ] = 12,
		[CON_NE] = 4,
		[CON_SO] = 12
	};

	static const uint8_t biTable[] = {
		[CON_GT] = 1,
		[CON_GE] = 0,
		[CON_LT] = 0,
		[CON_LE] = 1,
		[CON_EQ] = 2,
		[CON_NE] = 2,
		[CON_SO] = 3
	};

	if (cr < 0 || cr >= 8) {
		printf("ERROR: branch(con): invalid CR field\n");
		return 0;
	}

	if (condition < CON_GT || condition > CON_SO) {
		printf("ERROR: branch(con): wrong condition\n");
		return 0;
	}

	if (displacement > 0x00007FFCu
	&& displacement < 0xFFFF8000u) {
		printf("ERROR: branch(con): displacement out of range (16bit)\n");
		return 0;
	}

	uint32_t bo = boTable[condition];
	uint32_t bi
	= (uint32_t)cr * 4u + biTable[condition];

	uint32_t instruction
	= (16u << 26)
	| (bo << 21)
	| (bi << 16)
	| (displacement & 0x0000FFFCu)
	| ((uint32_t)aa << 1)
	| (uint32_t)link;

	writeU32BE(
		outputChunk + outputChunkPtr,
		instruction
	);

	outputChunkPtr += 4;

	return 4;
}


size_t enc_branch(
	int8_t link,		// set link register T/F
	int8_t aa,		// absolute address T/F
	int8_t cr,		// condition register 0~7
	uint32_t displacement,	// aa absolute address
	Condition condition	// condition gt(>),ge(>=),eq(==),le(<=),lt(<),so(summary overflow)
) {
	link = link ? 1u : 0u;
	aa = aa ? 1u : 0u;

	if (displacement & 3u) {
		printf("ERROR: branch: displacement not aligned\n");
		return 0;
	}

	checkChunk(4);

	if (condition == CON_NO)
		return enc_branch_no(
			link,
			aa,
			displacement
		);

	return enc_branch_con(
		link,
		aa,
		cr,
		displacement,
		condition
	);
}
