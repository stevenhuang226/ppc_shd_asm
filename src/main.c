#include "../include/global.h"
#include "../include/setting.h"
#include "../include/translator.v3.h"
#include "../include/hash_label.h"

#include "source.c"
#include "parse.c"
#include "translator.v3.c"

int main(int argc, char **argv)
{
	int err = 1;
	size_t parsedSize = 0;

	char *outputName = NULL;
	HSMStack *hsm = NULL;

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

	Parsed *parsed = parseSource(srcArr, &parsedSize);

	// computeAddress(parsed, parsedSize);

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

	/* debug begin */
	/*
	for (int i = 0; i < parsedSize; ++i) {
		int srcId = parsed[i].srcId;
		PssType type = parsed[i].type;

		printf("souce: %s", srcArr->lines[srcId].content);
		if (type == PSS_LABEL) {
			printf("label: %s, address: %u\n", parsed[i].label.name, parsed[i].label.address);
		} else if (type == PSS_SCOPE_BEGIN ||
			type == PSS_SCOPE_END) {
			printf("scope: %u\n", parsed[i].id);
		} else if (type == PSS_DATA) {
			printf("data: len: %d\n", parsed[i].data.size);
		} else if (type == PSS_INSTRUCTION) {
			printf("instruction: %s\n", parsed[i].instruction.cmd);
		} else if (type == PSS_BRANCH) {
			printf("branch: link: %u, aa: %u, address: %u, target: %s\n", (int8_t)parsed[i].branch.link, (int8_t)parsed[i].branch.aa, parsed[i].branch.address, parsed[i].branch.target);
		} else if (type == PSS_ALIGN) {
			printf("align\n");
		}
	}
	*/

	printf("outputChunkPtr: %d\n", outputChunkPtr);

	for (size_t i = 0; i < outputChunkPtr; ++i) {
		printf("%02x ", outputChunk[i]);
	}

	/* debug end */

	free(srcArr); srcArr = NULL;
	for (int i = 0; i < parsedSize; ++i) {
		if (parsed[i].type == PSS_DATA) {
			free(parsed[i].data.ptr);
			parsed[i].data.ptr = NULL;
		}
	}

	err = 0;
cleanup:
	free(inputNames); inputNames = NULL;
	free(sourceChunk); sourceChunk = NULL;
	for (int i = 0; i < parsedSize; ++i) {
		if (parsed[i].type == PSS_DATA) {
			free(parsed[i].data.ptr);
			parsed[i].data.ptr = NULL;
		}
	}
	free(parsed); parsed = NULL;
	free(outputChunk); outputChunk = NULL;
	free(hsm); hsm = NULL;
	return err;
}
