//Sharon Levy and Jazarie Thach
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#include <errno.h>
#include <string.h>
#include <unistd.h>
#include <ctype.h>
#include <sys/time.h>

struct arg_struct {
  int id;
  int my_size;
  double *aMatrix, *bMatrix, *cMatrix;
  int my_startingIndex, my_aStartingIndex;
  int aRow, bCol, aCol;
};

double CTimer() {
  struct timeval tm;

  gettimeofday(&tm,NULL);
  return((double)tm.tv_sec + (double)(tm.tv_usec/1000000.0));
}

char *trimW(char *str) {
  char *end;

  while(isspace(*str)) {
    str++;
  }

  if(*str == '\0') {
    return str;
  }
  
  end = str + (strlen(str) - 1);
  while(isspace(*end) && end > str) {
    end--;
  }

  *(end+1) = '\0';

  return str;
}

void *MatrixMultiply(void *arg) {
  struct arg_struct *my_args;
  my_args = (struct arg_struct *)arg;
  double *a = my_args->aMatrix;
  double *b = my_args->bMatrix;
  double *c = my_args->cMatrix;
  int aCounter, bCounter, i, j;
  int startingIndex = my_args->my_startingIndex;
  int aStartingIndex = my_args->my_aStartingIndex;
  int size = my_args->my_size;
  int aR = my_args->aRow;
  int bC = my_args->bCol;
  int aC = my_args->aCol;

  for (i = 0; i< size*bC; i++) {
    c[i+startingIndex] = 0;
    aCounter = aStartingIndex + (i/bC)*aC;
    bCounter = i%bC;
    for (j = 0; j< aC; j++) {
      c[i+startingIndex] += a[aCounter] * b[bCounter];
      aCounter++;
      bCounter +=bC;
    }
  }
  
  free(my_args);
}

int main(int argc, char *argv[]) {
  double startTime = CTimer();
  double d;
  double ret;
  int threads;
  int r, t, err;
  int startingCounter = 0;
  int aStartingCounter = 0;
  int currentRow = 0;
  int aRow, aCol, bRow, bCol, i, j, opt;
  int flag1 = 0, flag2 = 0, flag3 = 0;
  char num[1];
  char *n;
  char *line = malloc(1000);
  char *line2;
  char *ptr;
  char *aFileName;
  char *bFileName;
  FILE *aFile, *bFile;
  pthread_t *thread_ids;
  struct arg_struct *args;

  if (argc != 7) {
    printf("Incorrect number of input arguments\n");
    exit(-1);
  }

  opterr = 0;
  while ((opt = getopt (argc, argv, "a:b:t:")) != -1) {
      switch (opt) {
	case 'a':
	  aFileName = optarg;
	  flag1++;
	  break;
	case 'b':
	  bFileName = optarg;
	  flag2++;
	  break;
	case 't':
	  for (i = 0; i<strlen(optarg); i++) {
	    if (!isdigit(optarg[i])) {
	      printf("Incorrect format for thread value\n");
	      exit(-1);
	    }
	  }
	  threads = atoi(optarg);
	  flag3++;
	  break;
	default:
	  printf("Invalid flag argument\n");
	  exit(-1);
	}
    }

  if (flag1 != 1 || flag2 != 1 || flag3 != 1) {
    printf("Incorrect format for flag arguments\n");
    exit(-1);
  }
	
  aFile = fopen(aFileName, "r");
  if (aFile == NULL) {
    printf("Invalid file\n");
    exit(-1);
  }
  
  bFile = fopen(bFileName, "r");
  if (bFile == NULL) {	
    printf("Invalid file\n");
    exit(-1);
  }
  
  //read in a matrix
  if (fgets(line, 100, aFile) != NULL) {
    n = trimW(strtok(line, " "));
    for (i = 0; i<strlen(n); i++) {
      if (!isdigit(n[i])) {
	printf("Incorrect format for matrix parameter\n");
	exit(-1);
      }
    }

    aRow = atoi(n);
    n = trimW(strtok(NULL, "\n"));
       
    for (i = 0; i<strlen(n); i++) {
      if (!isdigit(n[i])) {
	printf("Incorrect format for matrix parameter\n");
	exit(-1);
      }
    }
    aCol = atoi(n);
  }
  
  double *a = malloc(sizeof(double) * aRow * aCol);
  int aCount = 0;
  while (fgets(line, 1000, aFile) != NULL) {
    if (aCount > aRow*aCol) {
      printf("Invalid number of elements for matrix A\n");
      fflush(stdout);
      exit(-1);
    }
	  
    line2 = trimW(line);
    if (line2[0] != '#' && line2[0] != '\0') {
      if (line2[0] != '-' && !isdigit(line2[0])) {
	printf("Invalid element type 1\n");
	exit(-1);
      }
      ret = strtod(line2, &ptr);
      if (ptr[0] != '\0') {
	printf("Invalid element type 2\n");
	exit(-1);
      }
      a[aCount] = ret;
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
    n = trimW(strtok(line, " "));
    for (i = 0; i<strlen(n); i++) {
      if (!isdigit(n[i])) {
	printf("Incorrect format for matrix parameter\n");
	exit(-1);
      }
    }
    
    bRow = atoi(n);
    n = trimW(strtok(NULL, "\n"));   

    for (i = 0; i<strlen(n); i++) {
      if (!isdigit(n[i])) {
	printf("Incorrect format for matrix parameter\n");
	exit(-1);
      }
    }
    bCol = atoi(n);
  }

  double *b = malloc(sizeof(double) * bRow * bCol);
  int bCount = 0;
  while (fgets(line, 1000, bFile) != NULL) {
    if (bCount > bRow*bCol) {
      printf("Invalid number of elements for matrix B\n");
      fflush(stdout);
      exit(-1);
    }
	  
    line2 = trimW(line);
    if (line2[0] != '#' && line2[0] != '\0') {
      if (line2[0] != '-' && !isdigit(line2[0])) {
	printf("Invalid element type 1\n");
	exit(-1);
      }
      ret = strtod(line2, &ptr);
      if (ptr[0] != '\0') {
	printf("Invalid element type 2\n");
	exit(-1);
      }
      b[bCount] = ret;
      bCount++;
    }
  }

  if (bCount < bRow*bCol) {
    printf("Invalid number of elements for matrix B\n");
    fflush(stdout);
    exit(-1);
  }
       	
  fclose(aFile);
  fclose(bFile);
  
  if (aCol != bRow) {
    printf("Invalid matrix size\n");
    fflush(stdout);
    exit(-1);
  }
	
  double *c = malloc(sizeof(double) * aRow * bCol);
  memset(c,0,sizeof(double));
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
  }

  //printf("Time: %f\n", CTimer() - startTime);
  printf("%d %d\n", aRow, bCol);
  for (i = 0; i<aRow*bCol; i++){
    if(i%bCol == 0){
      printf("# Row %d\n", currentRow);
      currentRow++;
    }	  
    printf("%f\n", c[i]);
  }
  
  //free memory
  free(line);
  free(a);
  free(b);
  free(c);
  free(thread_ids);
  return 0;
}
