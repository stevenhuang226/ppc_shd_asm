#include "../include/global.h"
#include "../include/setting.h"
//#include "../include/translator.v3.h"
#include "../include/hash_label.h"

#include "source.c"
#include "parse_token.c"

int main(int argc, char **argv)
{
	int err = 1;
	Token *token = NULL;
	size_t tokenSize = 0;

	char *outputName = NULL;
	//HSMStack *hsm = NULL;

	int inputNums = argc-3;
	if (inputNums <= 0)
		return err;

	char **inputNames = malloc(inputNums * sizeof(char *));

	int inputCount = 0;
	for (int i = 1; i < argc; ++i) {
		if (strcmp(argv[i], "-o") == 0 && i < argc-1) {
			outputName = argv[++i];
			continue;
		}

		if (inputCount >= inputNums)
			goto cleanup;

		inputNames[inputCount++] = argv[i];
	}

	if (inputCount != inputNums)
		goto cleanup;

	SourceArray *srcArr = loadFiles(inputNames, inputNums);

	token = parseToken(srcArr, &tokenSize);
	goto cleanup;

	// computeAddress(parsed, parsedSize);

	/*
	hsm = malloc(sizeof(HSMStack) + 64 * sizeof(HashStrMap));
	hsm->capacity = 64;
	hsm->top = -1;

	outputChunk = malloc(1024);
	outputChunkSize = 1024;

	int8_t tranRes = translateRegion(
		srcArr,
		parsed,
		0,
		parsedSize-1,
		hsm
	);

	if (tranRes < 0) {
		goto cleanup;
	}

	free(hsm); hsm = NULL;
	*/

	/* debug begin */

	/*
	printf("outputChunkPtr: %d\n", outputChunkPtr);

	for (size_t i = 0; i < outputChunkPtr; ++i) {
		printf("%02x ", outputChunk[i]);
	}
	*/

	/* debug end */

	free(srcArr); srcArr = NULL;
	/*
	for (int i = 0; i < parsedSize; ++i) {
		if (parsed[i].type == PSS_DATA) {
			free(parsed[i].data.ptr);
			parsed[i].data.ptr = NULL;
		}
	}
	*/

	err = 0;
cleanup:
	free(inputNames); inputNames = NULL;
	free(sourceChunk); sourceChunk = NULL;
	for (int i = 0; i < tokenSize; ++i) {
		if (token[i].type == TKN_DATA) {
			free(token[i].data.ptr);
			token[i].data.ptr = NULL;
		}
	}
	free(outputChunk); outputChunk = NULL;
	//free(hsm); hsm = NULL;
	free(token); token = NULL;
	return err;
}
