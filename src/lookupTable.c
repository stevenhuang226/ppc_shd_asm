#include "../include/lookup.h"

#include "encoder/addi.c"
#include "encoder/addis.c"
#include "encoder/ori.c"

EncoderEntry encoders[] = {
	{"addi", enc_addi},
	{"addis", enc_addis},
	{"ori", enc_ori}
};
