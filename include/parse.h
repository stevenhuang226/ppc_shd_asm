#ifndef PARSE_H
#define PARSE_H

#include "global.h"
#include "setting.h"

typedef enum {
	PSS_INSTRUCTION,
	PSS_BRANCH,
	PSS_DATA,
	PSS_LABEL,
	PSS_SCOPE_BEGIN,
	PSS_SCOPE_END,
	PSS_ALIGN
} PssType;

typedef enum {
	REG_NR,
	REG_GPR,
	REG_FPR,
	REG_CR,
	REG_SPR,
	REG_MSR,
	REG_FPSCR,
	REG_TBU,
	REG_SR,
	REG_V
} RegisterType;

typedef enum {
	CON_NO,	// none condition
	CON_GT,	// greater
	CON_GE,	// greater equal
	CON_LT,	// less
	CON_LE,	// less equal
	CON_EQ,	// equal
	CON_NE,	// not equal
	CON_SO	// summary overflow
} Condition;

typedef struct {
	RegisterType type;
	int32_t code;
} Register;

typedef struct {
	uint32_t srcId;

	PssType type;

	union {
		struct {
			char cmd[INSTRUCTION_MAXLEN];
			Register registers[INSTRUCTION_REGCS];
		} instruction;

		struct {
			int8_t link;
			int8_t aa;
			int8_t cr;
			Condition condition;
			uint32_t address;
			char target[LABEL_MAXLEN];
		} branch;

		struct {
			uint8_t *ptr;
			size_t size;
		} data;

		struct {
			char name[LABEL_MAXLEN];
			uint32_t address;
		} label;

		uint32_t id;	// scope id
	};
} Parsed;

#endif
