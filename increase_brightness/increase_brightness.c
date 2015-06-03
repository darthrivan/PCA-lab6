#include <stdio.h>
#include <stdlib.h>
#include "emmintrin.h"

#define N 16000000

typedef struct s_pixel_rgb {
	unsigned char r;
	unsigned char g;
	unsigned char b;
} t_pixel_rgb;


void increase_brightness (t_pixel_rgb *rgb, int len, unsigned char inc, int N_iter)
{
	int i, j, len_chars = 3*len;
	char *rgb_chars = (char *) rgb;
	__m128i a, b, c, d;
	b = _mm_set_epi8(inc, inc, inc, inc, inc, inc, inc, inc, inc, inc, inc, inc, inc, inc, inc, inc);
	for (j=0; j<N_iter; j++) {
		for (i = 0; i < 48*(len_chars/48); i += 48) {
	 		a = _mm_load_si128((__m128i *) &rgb_chars[i]);
	 		c = _mm_load_si128((__m128i *) &rgb_chars[i+16]);
	 		d = _mm_load_si128((__m128i *) &rgb_chars[i+32]);
	 		a = _mm_adds_epu8(a, b);
	 		c = _mm_adds_epu8(c, b);
	 		d = _mm_adds_epu8(d, b);
	 		_mm_store_si128((__m128i *) &rgb_chars[i], a);
	 		_mm_store_si128((__m128i *) &rgb_chars[i+16], c);
	 		_mm_store_si128((__m128i *) &rgb_chars[i+32], d);
		}
		// epilogo
		for (; i < len_chars; ++i) {
			rgb_chars[i] += inc;
			if (rgb_chars[i] > 255) {
				rgb_chars[i] = 255;
			}
		}
	}
}

int main (int argc, char *argv[])
{

  t_pixel_rgb *A;
  int i;
  int n=N; // tamanyo vectores
  int NIt=50; // Num.iters
  unsigned char incr=1;

  if (argc>1) NIt = atoi(argv[1]);
  if (argc>2) n = atoi(argv[2]);
  if (argc>3) incr = (unsigned char) atoi(argv[3]);

  if (n>N) { printf("Maxima longitud vector: %d\n",N); exit(-1); }

  if (posix_memalign((void**)&A, 16, n*sizeof(t_pixel_rgb)) != 0) {
	  fprintf(stderr, "No hay memoria.\n");
	  exit(-1);
  }

   /* Inicialitzacio nombres "aleatoris" */
   for (i=0;i<n;i++) {
	 A[i].r= i^(i-54) & 0x2f;
	 A[i].g= (i+89)^(i-200) & 0x2f;
	 A[i].b= (i+70)^(i+325) &0x2f;
   }

   increase_brightness (A, n, incr, NIt); 

   write(1, A, n*sizeof(t_pixel_rgb));

   return(0);
}
