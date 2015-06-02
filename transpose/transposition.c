#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <time.h>
#include "xmmintrin.h"

#define BLOCK_SIZE 4

#define TRANSPOSE_BLOCK(I,J,DIM,SRC,DST)       \
        a = _mm_load_ps(&SRC[I*DIM + J]);     \
        b = _mm_load_ps(&SRC[(I+1)*DIM + J]); \
        c = _mm_load_ps(&SRC[(I+2)*DIM + J]); \
        d = _mm_load_ps(&SRC[(I+3)*DIM + J]); \
        _MM_TRANSPOSE4_PS(a, b, c, d);         \
        _mm_store_ps(&DST[I*DIM + J], a);     \
        _mm_store_ps(&DST[(I+1)*DIM + J], b); \
        _mm_store_ps(&DST[(I+2)*DIM + J], c); \
        _mm_store_ps(&DST[(I+3)*DIM + J], d);

#define MOVE_BLOCK(I,J,DIM,SRC,DST)            \
        a = _mm_load_ps(&SRC[I*DIM + J]);     \
        b = _mm_load_ps(&SRC[(I+1)*DIM + J]); \
        c = _mm_load_ps(&SRC[(I+2)*DIM + J]); \
        d = _mm_load_ps(&SRC[(I+3)*DIM + J]); \
        _mm_store_ps(&DST[I*DIM + J], a);     \
        _mm_store_ps(&DST[(I+1)*DIM + J], b); \
        _mm_store_ps(&DST[(I+2)*DIM + J], c); \
        _mm_store_ps(&DST[(I+3)*DIM + J], d);

void transpose(float *dst, float *src, int dim) {
    int i, j, ii, jj;
    float aux;
    __m128 a, b, c, d;
    for (i = 0; i < dim; i += BLOCK_SIZE) {
        // Transposicion de la diagonal
        TRANSPOSE_BLOCK(i,i,dim,src,dst)
        for(j = i+BLOCK_SIZE; j < dim; j += BLOCK_SIZE) {
            // Transposicion de no diagonal
            TRANSPOSE_BLOCK(i,j,dim,src,dst)
            TRANSPOSE_BLOCK(j,i,dim,src,dst)
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

    float *src = 16 + ((float *) ((uintptr_t) malloc(dim*dim*sizeof(float)+16) & ~15));
    float *dst = 16 + ((float *) ((uintptr_t) malloc(dim*dim*sizeof(float)+16) & ~15));

    srand(0);
    for (i = 0; i < dim*dim; ++i)
        src[i] = (float) rand();


    // Start timing
    transpose(dst, src, dim);
    // End timing

    // Check result

    return 0;
}
