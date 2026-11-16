#include "../include/lookup.h"

#include "encoder/addi.c"
#include "encoder/addis.c"
#include "encoder/ori.c"
#include "encoder/andi.c"
#include "encoder/xori.c"

EncoderEntry encoders[] = {
	{"addi", enc_addi},
	{"addis", enc_addis},
	{"ori", enc_ori},
	{"andi", enc_andi},
	{"xori", enc_xori}
};
