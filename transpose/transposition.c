#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#define BLOCK_SIZE 250

void transpose(float *dst, float *src, int dim) {
    int i, j, ii, jj;
    float aux;
    for (i = 0; i < dim; i += BLOCK_SIZE) {
        // Transposicion de la diagonal
        for (ii = i; ii < i+BLOCK_SIZE; ++ii) {
            for (jj = i; jj < i+BLOCK_SIZE; ++jj) {
                dst[jj * dim + ii] = src[ii * dim + jj];
            }
        }
        for(j = i+BLOCK_SIZE; j < dim; j += BLOCK_SIZE) {
            // Transposicion de no diagonal
            for (ii = i; ii < i+BLOCK_SIZE; ++ii) {
                for (jj = ii+1; jj < j+BLOCK_SIZE; ++jj) {
                    // TODO arreglar accesos
                    aux = src[ii * dim + jj];
                    // src[ii * dim + jj] = src[jj * dim + ii];
                    src[ii * dim + jj] = src[(ii+jj-j) * dim + (ii-i)];
                    src[jj * dim + ii] = aux;
                }
            }
            for (ii = j; ii < j+BLOCK_SIZE; ++ii) {
                for (jj = ii+1; jj < i+BLOCK_SIZE; ++jj) {
                    aux = src[ii * dim + jj];
                    // src[ii * dim + jj] = src[jj * dim + ii];
                    src[ii * dim + jj] = src[(ii+jj-j) * dim + (ii-i)];
                    src[jj * dim + ii] = aux;
                }
            }
            for (ii = i; ii < i+BLOCK_SIZE; ++ii) {
                for (jj = j; jj < j+BLOCK_SIZE; ++jj) {
                    dst[jj * dim + ii] = src[ii * dim + jj];
                    dst[ii * dim + jj] = src[jj * dim + ii];
                }
            }
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

    float *src = malloc(dim*dim*sizeof(float));
    float *dst = malloc(dim*dim*sizeof(float));

    srand(0);
    for (i = 0; i < dim*dim; ++i)
        src[i] = (float) rand();


    // Start timing
    transpose(dst, src, dim);
    // End timing

    // Check result

    return 0;
}
