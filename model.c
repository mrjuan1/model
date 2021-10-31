#include <malloc.h>
#include <stdbool.h>
#include <stdio.h>
#include <string.h>

typedef float vertex[5];

int main(int argc, const char *argv[]) {
	if(argc < 2) {
		fprintf(stderr, "No input file provided.\n");
		return 1;
	}

	FILE *f = fopen(argv[1], "rb");
	if(!f) {
		fprintf(stderr, "Failed to open file \"%s\" for reading.\n", argv[1]);
		return 1;
	}

	fseek(f, 0, SEEK_END);
	const int size = ftell(f);

	vertex *vertices = malloc(size);
	fseek(f, 0, SEEK_SET);
	fread(vertices, size, 1, f);
	fclose(f);

	const int vertexCount = size / sizeof(vertex);
	printf("Vertex count: %i\n", vertexCount);

	int vertexListLength = 0;
	vertex *vertexList = malloc(0);

	for(int i = 0; i < vertexCount; i++) {
		bool matchFound = false;

		for(int j = i + 1; j < vertexCount; j++) {
			bool match = true;

			for(int k = 0; k < 5; k++)
				if(vertices[i][k] != vertices[j][k]) {
					match = false;
					break;
				}

			if(match) {
				matchFound = true;
				break;
			}
		}

		if(!matchFound) {
			const int index = vertexListLength++;
			vertexList = realloc(vertexList, vertexListLength * sizeof(vertex));
			memcpy(vertexList[index], vertices[i], sizeof(vertex));
		}
	}

	printf("Vertex list length: %i\n", vertexListLength);

	const int indexListSize = vertexCount * sizeof(vertex);
	unsigned int *indexList = malloc(indexListSize);

	for(int i = 0; i < vertexCount; i++)
		for(int j = 0; j < vertexListLength; j++) {
			bool match = true;

			for(int k = 0; k < 5; k++)
				if(vertices[i][k] != vertexList[j][k]) {
					match = false;
					break;
				}

			if(match) {
				indexList[i] = (unsigned int)j;
				break;
			}
		}

	free(vertices);

	const char *prefix = "indexed-";
	const int nameLength = strlen(prefix) + strlen(argv[1]) + 1;
	char *name = malloc(nameLength);
	sprintf(name, "%s%s%c", prefix, argv[1], '\0');
	f = fopen(name, "wb");
	if(!f) {
		fprintf(stderr, "Failed to open file \"%s\" for writing.\n", name);

		free(name);
		free(indexList);
		free(vertexList);

		return 1;
	}

	free(name);

	const int vertexListSize = vertexListLength * sizeof(vertex);
	fwrite((const void *)&vertexListSize, sizeof(int), 1, f);
	fwrite(vertexList, vertexListSize, 1, f);
	free(vertexList);

	fwrite((const void *)&indexListSize, sizeof(int), 1, f);
	fwrite(indexList, indexListSize, 1, f);
	free(indexList);

	fclose(f);

	return 0;
}
