#ifndef TOKEN_H
#define TOKEN_H

#include "setting.h"

typedef enum {
	TKN_INSTRUCTION,
	TKN_LABEL,
	TKN_DATA,
	TKN_SCOPE_BEGIN,
	TKN_SCOPE_END,
	TKN_ALIGN
} TokenType;

typedef enum {
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
	SLOT_EMPTY,
	SLOT_LABEL,
	SLOT_REG,
	SLOT_IMM,
	SLOT_EXCEPTION
} SlotType;

typedef struct {
	uint8_t fixed;
	char name[LABEL_MAXLEN];
} TargetLabel;

typedef struct {
	SlotType type;

	union {
		struct {
			int8_t fixed;	/* T/F compute fixed address (request linker) */
			char name[LABEL_MAXLEN];
		} label;

		struct {
			RegisterType type;
			uint16_t code;	/* register's code U16 */
		} reg;

		int32_t imm;

		char exception[INSTRUCTION_SLOT_MAXLEN];
	};
} Slot;

typedef struct {
	uint32_t srcId;
	TokenType type;

	union {
		struct {
			char cmd[INSTRUCTION_CMD_MAXLEN];
			Slot slots[INSTRUCTION_MAXREGC];
		} instruction;

		struct {
			uint8_t *ptr;
			size_t size;
		} data;

		struct {
			char name[LABEL_MAXLEN];
			uint32_t address;
		} label;

		uint32_t id;
	};
} Token;

#endif
