#include <malloc.h>
#include <math.h>
#include <stdbool.h>
#include <stdio.h>
#include <string.h>

#define err(msg, ...) fprintf(stderr, msg, __VA_ARGS__)
#define fail(msg, ...) err("%s" msg ".\n", failedTo, __VA_ARGS__)

typedef float vertex[5];

const char *failedTo = "Failed to ";

void showUsage(const char *arg0);
bool saveDimensions(const char *filename,
					int vertexListLength,
					vertex *vertexList);

int main(int argc, const char *argv[]) {
	// Fail if no arguments were provided to the executable.
	if(argc < 2) {
		err("No input file(s) provided.\n%c", '\0');
		showUsage(argv[0]);

		return 1;
	}

	// First search for any "help" options, show usage and exit if applicable.
	int i;
	for(i = 1; i < argc; i++)
		if(!strcmp(argv[i], "-h") || !strcmp(argv[i], "--help")) {
			showUsage(argv[0]);
			return 0;
		}

	// Loop through all input files...
	for(i = 1; i < argc; i++) {
		printf("Indexing vertices in file \"%s\"...", argv[i]);
		fflush(stdout);

		// Read out vertices (whole file).
		FILE *f = fopen(argv[i], "rb");
		if(!f) {
			fail("open file \"%s\" for reading", argv[i]);
			continue;
		}

		fseek(f, 0, SEEK_END);
		const int size = ftell(f);

		vertex *vertices = malloc(size);
		if(!vertices) {
			fail("allocate memory for vertices%c", '\0');
			fclose(f);

			continue;
		}

		fseek(f, 0, SEEK_SET);
		int result = fread(vertices, size, 1, f);
		fclose(f);
		if(!result) {
			fail("read vertices from file \"%s\"", argv[i]);
			free(vertices);

			continue;
		}

		const int vertexCount = size / sizeof(vertex);
		printf("%i vertices...", vertexCount);
		fflush(stdout);

		// Build a new vertex list holding only unique vertices (remove
		// duplicates).
		int j, k, l;
		bool match;
		int vertexListLength = 0;
		vertex *vertexList = malloc(0);

		// Loop through all vertices...
		for(j = 0; j < vertexCount; j++) {
			bool matchFound = false;

			// Loop from next vertex to the end of the list, checking previous
			// vertices is pointless as they've already been checked in the
			// reverse order.
			for(k = j + 1; k < vertexCount; k++) {
				match = true;

				// Check if they match...
				for(l = 0; l < 5; l++)
					if(vertices[j][l] != vertices[k][l]) {
						match = false;
						break;
					}

				// If they do, break. A duplicate was found, there's no need to
				// continue beyond here.
				if(match) {
					matchFound = true;
					break;
				}
			}

			// If no match was found, then the vertex is unique and can be added
			// to the indexed vertex list.
			if(!matchFound) {
				const int index = vertexListLength++;
				vertexList =
					realloc(vertexList, vertexListLength * sizeof(vertex));
				if(!vertexList) {
					fail("reallocate memory for vertices%c", '\0');
					break;
				}

				memcpy(vertexList[index], vertices[j], sizeof(vertex));
			}
		}

		// Bail on the current file if memory re-allocation failed in the loop
		// above.
		if(j < vertexCount) {
			free(vertices);
			continue;
		}

		printf("reduced to %i indexed vertices...", vertexListLength);

		// Generate the index list by re-looping through all vertices and
		// finding their match in the vertex index list and storing that index
		// in the newly-generated index list.
		const int indexListSize = vertexCount * sizeof(unsigned int);
		unsigned int *indexList = malloc(indexListSize);

		// Loop through vertices...
		for(j = 0; j < vertexCount; j++)
			// Loop through indexed vertices
			for(k = 0; k < vertexListLength; k++) {
				match = true;

				// Check if there's a match...
				for(l = 0; l < 5; l++)
					if(vertices[j][l] != vertexList[k][l]) {
						match = false;
						break;
					}

				// If there is, store the index and break out of the loop.
				if(match) {
					indexList[j] = (unsigned int)k;
					break;
				}
			}

		free(vertices);

		// Generate output file's filename...
		const char *prefix = "indexed-";
		const int nameLength = strlen(prefix) + strlen(argv[i]) + 1;
		char *name = malloc(nameLength);
		if(!name) {
			fail("allocate memory for output filename%c", '\0');
			continue;
		}

		sprintf(name, "%s%s%c", prefix, argv[i], '\0');
		printf("output file \"%s\"...", name);
		fflush(stdout);

		// Create output file holding the vertex list size in bytes, followed by
		// the vertex list itself, followed by the index list size in bytes,
		// followed by the index list itself.
		f = fopen(name, "wb");
		if(!f) {
			fail("open output file \"%s\" for writing", name);

			free(name);
			free(indexList);
			free(vertexList);

			continue;
		}

		const int vertexListSize = vertexListLength * sizeof(vertex);
		result = fwrite((const void *)&vertexListSize, sizeof(int), 1, f);
		if(!result) {
			fail("write indexed vertex list size to output file \"%s\"", name);

			free(name);
			free(indexList);
			free(vertexList);

			continue;
		}

		result = fwrite(vertexList, vertexListSize, 1, f);
		if(!result) {
			fail("write indexed vertex list to output file \"%s\"", name);

			free(name);
			free(indexList);
			free(vertexList);

			continue;
		}

		result = fwrite((const void *)&indexListSize, sizeof(int), 1, f);
		if(!result) {
			fail("write index size to output file \"%s\"", name);

			free(name);
			free(indexList);
			free(vertexList);

			continue;
		}

		result = fwrite(indexList, indexListSize, 1, f);
		free(indexList);
		if(!result) {
			fail("write index to output file \"%s\"", name);

			free(name);
			free(vertexList);

			continue;
		}

		result = fclose(f);
		if(result) {
			fail("close output file \"%s\"", name);

			free(name);
			free(vertexList);

			continue;
		}

		free(name);

		result = saveDimensions(argv[i], vertexListLength, vertexList);
		free(vertexList);
		if(!result) continue;

		printf("done.\n");
	}

	printf("\nDone.\n");

	return 0;
}

// Function to print the usage.
void showUsage(const char *arg0) {
	printf("\nUsage: %s [<option(s)...>] | <input file(s)...>\n", arg0);
	printf("Options:");
	printf("  -h  --help  Show this message\n");
	printf("\n");
}

bool saveDimensions(const char *filename,
					int vertexListLength,
					vertex *vertexList) {
	const char *prefix = "dimensions-";
	const int nameLength = strlen(prefix) + strlen(filename) + 1;
	char *name = malloc(nameLength);
	if(!name) {
		fail("allocate memory for dimensions filename%c", '\0');
		return false;
	}

	sprintf(name, "%s%s%c", prefix, filename, '\0');
	printf("dimensions file \"%s\"...", name);
	fflush(stdout);

	FILE *f = fopen(name, "wb");
	if(!f) {
		fail("open dimensions file \"%s\" for writing", name);
		free(name);

		return false;
	}

	float radius = 0.0f;
	float smallest[3] = { 0.0f, 0.0f, 0.0f };
	float largest[3] = { 0.0f, 0.0f, 0.0f };

	int i;
	for(i = 0; i < vertexListLength; i++) {
		for(int j = 0; j < 3; j++) {
			const float absoluteVertexValue = fabsf(vertexList[i][j]);
			if(absoluteVertexValue > radius) radius = absoluteVertexValue;

			if(vertexList[i][j] < smallest[j])
				smallest[j] = vertexList[i][j];
			else if(vertexList[i][j] > largest[j])
				largest[j] = vertexList[i][j];
		}
	}

	int result = fwrite((const void *)&radius, sizeof(float), 1, f);
	if(!result) {
		fail("write radius to dimensions file \"%s\"", name);

		fclose(f);
		free(name);

		return false;
	}

	for(i = 0; i < 3; i++) {
		result = fwrite((const void *)&smallest[i], sizeof(float), 1, f);
		if(!result) {
			fail("write smallest point to dimensions file \"%s\"", name);

			fclose(f);
			free(name);

			return false;
		}
	}

	for(i = 0; i < 3; i++) {
		result = fwrite((const void *)&largest[i], sizeof(float), 1, f);
		if(!result) {
			fail("write largest point to dimensions file \"%s\"", name);

			fclose(f);
			free(name);

			return false;
		}
	}

	result = fclose(f);
	if(result) {
		fail("close dimensions file \"%s\"", name);
		free(name);

		return false;
	}

	free(name);

	return true;
}
