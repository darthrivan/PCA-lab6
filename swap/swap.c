#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include "emmintrin.h"
#include "tmmintrin.h"

#define BUFFER_SIZE (1<<20)

int main(int argc, char *argv[])
{
    __m128i a, b;
    int n1, n2, i;
    char buffer[BUFFER_SIZE];
 
    b = _mm_set_epi8(14,15,12,13,10,11,8,9,6,7,4,5,2,3,0,1);
    while ((n1=read(0, buffer, BUFFER_SIZE*sizeof(char))) > 0)
    {
		for (i = 0; i < 16*(n1/16); i += 16) {
			a = _mm_load_si128((__m128i *) &buffer[i]);
			a = _mm_shuffle_epi8(a, b);
			_mm_store_si128((__m128i *) &buffer[i], a);
		}
		// epilogo
		for (; i < n1-1; i += 2) {
			buffer[i]   = buffer[i] ^ buffer[i+1];
			buffer[i+1] = buffer[i] ^ buffer[i+1];
			buffer[i]   = buffer[i] ^ buffer[i+1];
		}
		write(1, buffer, n1);
    }
 
    return 0;
}
