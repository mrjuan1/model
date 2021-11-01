#include <malloc.h>
#include <stdbool.h>
#include <stdio.h>
#include <string.h>

#define err(msg, ...) fprintf(stderr, msg, __VA_ARGS__)
#define fail(thing, ...) err("%s%s.\n", failedTo, thing, __VA_ARGS__)

typedef float vertex[5];

void showUsage(const char *arg0);

int main(int argc, const char *argv[]) {
	if(argc < 2) {
		err("No input file(s) provided.\n", 0);
		showUsage(argv[0]);

		return 1;
	}

	int i;
	for(i = 1; i < argc; i++)
		if(!strcmp(argv[i], "-h") || !strcmp(argv[i], "--help")) {
			showUsage(argv[0]);
			return 0;
		}

	for(i = 1; i < argc; i++) {
		const char *failedTo = "Failed to ";

		FILE *f = fopen(argv[i], "rb");
		if(!f) {
			fail("open file \"%s\" for reading.\n", argv[i]);
			return 1;
		}

		fseek(f, 0, SEEK_END);
		const int size = ftell(f);

		vertex *vertices = malloc(size);
		if(!vertices) {
			fail("allocate memory for vertices.\n", 0);
			fclose(f);

			return 1;
		}

		fseek(f, 0, SEEK_SET);
		int result = fread(vertices, size, 1, f);
		if(!result) {
			fail("read data from file \"%s\".\n", argv[i]);

			free(vertices);
			fclose(f);

			return 1;
		}

		fclose(f);

		const int vertexCount = size / sizeof(vertex);
		printf("Vertex count: %i\n", vertexCount);

		int vertexListLength = 0;
		vertex *vertexList = malloc(0);

		for(int j = 0; j < vertexCount; j++) {
			bool matchFound = false;

			for(int k = j + 1; k < vertexCount; k++) {
				bool match = true;

				for(int l = 0; l < 5; l++)
					if(vertices[j][l] != vertices[k][l]) {
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
				vertexList =
					realloc(vertexList, vertexListLength * sizeof(vertex));
				memcpy(vertexList[index], vertices[j], sizeof(vertex));
			}
		}

		printf("Vertex list length: %i\n", vertexListLength);

		const int indexListSize = vertexCount * sizeof(vertex);
		unsigned int *indexList = malloc(indexListSize);

		for(int j = 0; j < vertexCount; j++)
			for(int k = 0; k < vertexListLength; k++) {
				bool match = true;

				for(int l = 0; l < 5; l++)
					if(vertices[i][l] != vertexList[k][l]) {
						match = false;
						break;
					}

				if(match) {
					indexList[j] = (unsigned int)k;
					break;
				}
			}

		free(vertices);

		const char *prefix = "indexed-";
		const int nameLength = strlen(prefix) + strlen(argv[i]) + 1;
		char *name = malloc(nameLength);
		sprintf(name, "%s%s%c", prefix, argv[i], '\0');
		f = fopen(name, "wb");
		if(!f) {
			fail("open file \"%s\" for writing.\n", name);

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
	}

	return 0;
}

void showUsage(const char *arg0) {
	printf("\nUsage: %s [<option(s)...>] | <input file(s)...>\n", arg0);
	printf("Options:");
	printf("  -h  --help  Show this message\n");
	printf("\n");
}
