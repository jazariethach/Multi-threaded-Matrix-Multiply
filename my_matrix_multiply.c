//sharon levy and jazarie thach
//fix trailing whitespace segfault in files
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
int main(int argc, char *argv[]) {
	int threads = atoi(argv[6]);
	char *aFileName = argv[2];
	char *bFileName = argv[4];
	FILE *aFile, *bFile;
	aFile = fopen(aFileName, "r");
	bFile = fopen(bFileName, "r");
	char *line = malloc(1000);;
	char num[1];
	char *n;
	int aRow, aCol, bRow, bCol, i, j;
	//read in a matrix
	if (fgets(line, 100, aFile) != NULL) {
		n = strtok(line, " ");
		aRow = atoi(n);
		n = strtok(NULL, "\n");
		aCol = atoi(n);
	}
	float *a = malloc(sizeof(float) * aRow * aCol);
	int aCount = 0;
	while (fgets(line, 1000, aFile) != NULL) {
		if (line[0] != '#') {
			n = strtok(line, "\n");
			a[aCount] = atof(n);
			aCount++;
		}
	}
	//read in b matrix
	if (fgets(line, 100, bFile) != NULL) {
		n = strtok(line, " ");
		bRow = atoi(n);
		n = strtok(NULL, "\n");
		bCol = atoi(n);
	}
	float *b = malloc(sizeof(float) * bRow * bCol);
	int bCount = 0;
	while (fgets(line, 1000, bFile) != NULL) {
		if (line[0] != '#') {
			n = strtok(line, "\n");
			b[bCount] = atof(n);
			bCount++;
		}
	}
	fclose(aFile);
	fclose(bFile);
	//need to check valid matrix sizes
	float *c = malloc(sizeof(float) * aRow * bCol);
	memset(c,0,sizeof(float));
	int aCounter, bCounter;
	for (i = 0; i< aRow*bCol; i++) {
		c[i] = 0;
		aCounter = i/aRow;
		bCounter = i%bCol;
		for (j = 0; j< aCol; j++) {
			c[i] += a[aCounter] * b[bCounter];
			aCounter++;
			bCounter +=bCol;
		}
	}
	for (i = 0; i<aRow*bCol; i++)
		printf("%f ", c[i]);
}