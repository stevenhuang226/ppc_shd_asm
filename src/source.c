#include "../include/global.h"
#include "../include/setting.h"

#include "source.h"

SourceArray *loadFiles(char **filenames, int size)
{
	if (sourceChunk != NULL || sourceChunkSize != 0)
		return NULL;

	SourceArray *srcArr = malloc(
			sizeof(SourceArray) +
			SOURCE_ARR_DEF * sizeof(SourceLine)
		);

	srcArr->capacity = SOURCE_ARR_DEF;
	srcArr->count = 0;

	sourceChunk = malloc(4096 * sizeof(char));
	sourceChunkSize = 4096;

	int chunkPtr = 0;

	for (int i = 0; i < size; ++i) {
		char *name = filenames[i];

		FILE *file = fopen(name, "r");

		char buffer[1024];
		int localLineCount = 0;

		while (fgets(buffer, sizeof(buffer), file) != NULL) {
			++localLineCount;

			char *s = buffer;
			while (isspace(*s)) {
				++s;
			}

			if (s[0] == '\0' || (s[0] == '/' && s[1] == '/')) {
				continue;
			}

			size_t len = strlen(s);

			if (chunkPtr + len + 1 > sourceChunkSize) {
				sourceChunkSize *= 2;
				sourceChunk = realloc(
						sourceChunk,
						sourceChunkSize
					);

				if (sourceChunk == NULL) goto cleanup;
			}

			if (srcArr->count >= srcArr->capacity) {
				srcArr->capacity *= 2;
				srcArr = realloc(srcArr, srcArr->capacity);

				if (srcArr == NULL) goto cleanup;
			}

			//TODO consider remove space inside buffer

			srcArr->lines[srcArr->count].filename = name;
			srcArr->lines[srcArr->count].content =
				sourceChunk + chunkPtr;
			srcArr->lines[srcArr->count].lineNumber =
				localLineCount;

			chunkPtr += 1 + snprintf(sourceChunk + chunkPtr,
			sourceChunkSize - chunkPtr,
			"%s", s);

			++srcArr->count;
		}

		if (ferror(file)) {
			perror("fgets");
			goto cleanup;
		}

		fclose(file);
	}

	return srcArr;
cleanup:
	free(srcArr);

	free(sourceChunk);
	sourceChunkSize = 0;

	return NULL;
}
