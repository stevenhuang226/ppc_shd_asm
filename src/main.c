#include "../include/global.h"
#include "../include/setting.h"

#include "source.c"

int main(int argc, char **argv)
{
	int err = 1;

	char *outputName = NULL;

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



	for (int i = 0; i < inputNums; ++i) {
		printf("input %d: %s\n", i, inputNames[i]);
	}
	printf("out: %s\n", outputName);

	// source.c test
	SourceArray *srcArr = loadFiles(inputNames, inputNums);


	for (int i = 0; i < srcArr->count; ++i) {
		printf("%s:%d\n%s\n",
			srcArr->lines[i].filename,
			srcArr->lines[i].lineNumber,
			srcArr->lines[i].content
		);
	}

	free(srcArr);

	err = 0;
cleanup:
	free(inputNames);
	free(sourceChunk);
	return err;
}
