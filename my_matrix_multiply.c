//sharon levy and jazarie thach
//fix trailing whitespace segfault in files
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#include <errno.h>
struct arg_struct {
	int id;
	int my_size;
	float *aMatrix, *bMatrix, *cMatrix;
	int my_startingIndex, my_aStartingIndex;
	int aRow, bCol, aCol;
};
void *MatrixMultiply(void *arg) {
	int aCounter, bCounter;
	struct arg_struct *my_args;
	int i, j;
	my_args = (struct arg_struct *)arg;
	int size = my_args->my_size;
	float *a = my_args->aMatrix;
	float *b = my_args->bMatrix;
	float *c = my_args->cMatrix;
	int startingIndex = my_args->my_startingIndex;
	int aStartingIndex = my_args->my_aStartingIndex;
	int aR = my_args->aRow;
	int bC = my_args->bCol;
	int aC = my_args->aCol;
	for (i = 0; i< size*bC; i++) {
		c[i+startingIndex] = 0;
		aCounter = aStartingIndex + (i/bC)*aC;
		bCounter = i%bC;
		for (j = 0; j< aC; j++) {
	//		printf("thread: %d aCounter: %d  bCounter: %d  i: %d  size: %d\n", my_args->id, aCounter, bCounter, i, startingIndex);
	//		printf("thread: %d a[aCounter]: %f  b[bCounter]: %f\n", my_args->id, a[aCounter], b[bCounter]);
			c[i+startingIndex] += a[aCounter] * b[bCounter];
			aCounter++;
			bCounter +=bC;
		}
	}
	fflush(stdout);
}
int main(int argc, char *argv[]) {
	int threads = atoi(argv[6]);
	int r, t, err;
	int startingCounter = 0;
	int aStartingCounter = 0;
	char *aFileName = argv[2];
	char *bFileName = argv[4];
	FILE *aFile, *bFile;
	pthread_t *thread_ids;
	aFile = fopen(aFileName, "r");
	bFile = fopen(bFileName, "r");
	char *line = malloc(1000);
	char num[1];
	char *n;
	int aRow, aCol, bRow, bCol, i, j;
	double d;
	struct arg_struct *args;
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
	  if (aCount > aRow*aCol) {
	    printf("Invalid number of elements for matrix A\n");
	    fflush(stdout);
	    exit(-1);
	  }
	  
	  if (line[0] != '#') {
	    n = strtok(line, "\n");
	    a[aCount] = atof(n);
	    aCount++;
	  }
	}

	if (aCount < aRow*aCol) {
	  printf("Invalid number of elements for matrix A\n");
	  fflush(stdout);
	  exit(-1);
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
	  if (bCount > bRow*bCol) {
	    printf("Invalid number of elements for matrix B\n");
	    fflush(stdout);
	    exit(-1);
	  }
	  if (line[0] != '#') {
	    n = strtok(line, "\n");
	    b[bCount] = atof(n);
	    bCount++;
	  }
	}

	if (bCount < bRow*bCol) {
	  printf("Invalid number of elements for matrix B\n");
	  fflush(stdout);
	  exit(-1);
	}
	

	
/*	for (i = 0; i<aRow*aCol; i++)
		printf("%f\n", a[i]);
	for (i = 0; i<bRow*bCol; i++)
		printf("%f\n", b[i]);*/
	fclose(aFile);
	fclose(bFile);
	//need to check valid matrix sizes
	if (aCol != bRow) {
	  printf("Invalid matrix size\n");
	  fflush(stdout);
	  exit(-1);
	}
	
	float *c = malloc(sizeof(float) * aRow * bCol);
	memset(c,0,sizeof(float));
	thread_ids = (pthread_t *)malloc(sizeof(pthread_t)*threads);
	r = aRow%threads;
	for(t=0; t < threads; t++) {
		args = (struct arg_struct *)malloc(sizeof(struct arg_struct));
		args->my_size = (aRow-r)/threads;
		if (r !=0) {
			args->my_size++;
			r--;
		}
		if (t == 0) {
			args->my_startingIndex = 0;
			args->my_aStartingIndex = 0;
		}
		else {
			args->my_startingIndex = startingCounter;
			args->my_aStartingIndex = aStartingCounter;
		}
		startingCounter += args->my_size*bCol;
		aStartingCounter += args->my_size*aCol;
		args->id = (t+1);
		args->aMatrix = a;
		args->bMatrix = b;
		args->cMatrix = c;
		args->aRow = aRow;
		args->aCol = aCol;
		args->bCol = bCol;
		err = pthread_create(&(thread_ids[t]), NULL, MatrixMultiply, (void *)args);
	}
	for(t=0; t < threads; t++) {
		err = pthread_join(thread_ids[t],NULL);
	//	printf("thread done: %d\n", t+1);
	}
	for (i = 0; i<aRow*bCol; i++)
		printf("%f\n", c[i]);
}
