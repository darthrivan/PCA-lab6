#include <stdio.h>
#include <stdlib.h>
#include <time.h>

void transpose(float *dst, float *src, int dim) {
    int i, j, ii, jj;
    for (i = 0; i < dim; i ++) {
        for(j = 0; j < dim; j ++) {
           dst[j * dim + i] = src[i * dim + j];
        }
    }
}

int main(int argc, char *argv[]) {
    int i;

    if (argc != 2) {
        printf("Usage: ./exec N\n");
        exit(0);
    }
    
    int dim = atoi(argv[1]);

    float *src = malloc(dim*dim*sizeof(int));
    float *dst = malloc(dim*dim*sizeof(int));

    srand(0);
    for (i = 0; i < dim*dim; ++i)
        src[i] = (float) rand();


    // Start timing
    transpose(dst, src, dim);
    // End timing

    // Check result

    return 0;
}
