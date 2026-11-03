#include "../include/global.h"
#include "../include/setting.h"
#include "./hashLabel.c"

#include "./encoder/encSizeof.c"

int8_t translater()
{
	int8_t err = -1;
	int scopeMaxLevel = SCOPE_MAX_LEVEL;
	int maxLabelNums = HASH_LABEL_POOL;


	HashStrMap *hmStack[scopeMaxLevel];
	int hmTop = -1;

	hmStack[++hmTop] = HashStrMapCreate(maxLabelNums, INT32_MAX, 1);

	// First loop => fill address into all labels
	// Second loop => deal scope inside
	// Third loop => deal actual code


cleanup:
	while (hmTop >= 0) {
		free(hmStack[hmTop--])
	}

	return err;
}
