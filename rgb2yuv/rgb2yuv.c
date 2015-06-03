#include <stdio.h>
#include <stdlib.h>
#include <malloc.h>
#include "emmintrin.h"
#include "xmmintrin.h"

#define N 4000000
/* #define DEBUG 0 */

typedef struct s_pixel_rgb {
	float r;
	float g;
	float b;
} t_pixel_rgb;

typedef struct s_pixel_yuv {
	float y;
	float u;
	float v;
} t_pixel_yuv;

// typedef struct
// {
//   float *r;
//   float *g;
//   float *b;
// } soa_pixel_rgb;

// typedef struct
// {
//   float *y;
//   float *u;
//   float *v;
// } soa_pixel_yuv;

// void aos2soa(t_pixel_rgb *aos, soa_pixel_rgb *soa) {
//   int i;
//   for (i = 0; i < 4*(size/4); i += 4) {
//     soa.r[i] = _mm_set_ps(aos[i].r, aos[i+1].r, aos[i+2].r, aos[i+3].r);
//     soa.g[i] = _mm_set_ps(aos[i].g, aos[i+1].g, aos[i+2].g, aos[i+3].g);
//     soa.b[i] = _mm_set_ps(aos[i].b, aos[i+1].b, aos[i+2].b, aos[i+3].b);
//   }
//   for (; i < size; ++i) {
//     soa.r[i] = aos[i].r;
//     soa.g[i] = aos[i].g;
//     soa.b[i] = aos[i].b;
//   }
// }

// void soa2aos(soa_pixel_yuv *soa, t_pixel_yuv *aos) {
//   int i;
//   for (i = 0; i < 4*(size/4); i += 4) {
//     aos[i].r = _mm_set_ps(soa.r[i], soa.r[i+1], soa.r[i+2], soa.r[i+3]);
//     aos[i].g = _mm_set_ps(soa.g[i], soa.g[i+1], soa.g[i+2], soa.g[i+3]);
//     aos[i].b = _mm_set_ps(soa.b[i], soa.b[i+1], soa.b[i+2], soa.b[i+3]);
//   }
//   for (; i < size; ++i) {
//     aos[i].r = soa.r[i];
//     aos[i].g = soa.g[i];
//     aos[i].b = soa.b[i];
//   }
// }

void rgb_to_yuv(t_pixel_rgb *rgb, t_pixel_yuv *yuv, int len, int N_iter)
{
	int i,j;
  // soa_pixel_rgb *soa_rgb;
  // soa_pixel_yuv *soa_yuv;
  __m128 a, b, c, d, e, f;
  // if (posix_memalign((void **)&soa_rgb, 16, len*sizeof(soa_pixel_rgb)) != 0){
  //   fprintf(stderr, "No memory.\n");
  //   exit(-1);
  // }
  // if (posix_memalign((void **)&soa_yuv, 16, len*sizeof(soa_pixel_yuv)) != 0){
  //   fprintf(stderr, "No memory.\n");
  //   exit(-1);
  // }
  b = _mm_set_ps(0.299f, -0.147f, 0.615f, 1.0f);
  c = _mm_set_ps( 0.587f, -0.289f, -0.515f, 1.0f);
  d = _mm_set_ps( 0.114f, 0.436f, -0.100f, 1.0f);
  // aos2soa(rgb, soa_rgb);
  for (j=0; j<N_iter; j++) {
    for (i = 0; i < len; i++) {
      a = _mm_load_ps1(&rgb[i].r);
      a = _mm_mul_ps(a, b);
      e = _mm_load_ps1(&rgb[i].g);
      e = _mm_mul_ps(e, c);
      a = _mm_add_ps(a, e);
      f = _mm_load_ps1(&rgb[i].b);
      f = _mm_mul_ps(f, d);
      a = _mm_add_ps(a, f);
      _mm_store_ps(&yuv[i], a);
    }
  }

 //     for (j=0; j<N_iter; j++)
	// for (i=0; i<len; i++) {
 //    yuv[i].y = 0.299 * rgb[i].r + 0.587 * rgb[i].g + 0.114 * rgb[i].b;
 //    yuv[i].u = -0.147 * rgb[i].r - 0.289 * rgb[i].g + 0.436 * rgb[i].b;
	// 	yuv[i].v = 0.615 * rgb[i].r - 0.515 * rgb[i].g - 0.100 * rgb[i].b;
    // yuv[i].u = 0.492 * (rgb[i].b - yuv[i].y);
		// yuv[i].v = 0.877 * (rgb[i].r - yuv[i].y);
}

int main (int argc, char *argv[])
{
  t_pixel_rgb *rgb; t_pixel_yuv *yuv; // Vectores para malloc

  int i;
  int n=N; // tamanyo vectores
  int NIt=50; // Num.iters

  if (argc>1) NIt = atoi(argv[1]);
  if (argc>2) n = atoi(argv[2]);

  if (n>N) { printf("Maxima longitud vector: %d\n",N); exit(-1); }

  if (posix_memalign((void**)&rgb, 16, n*sizeof(t_pixel_rgb)) !=0) {
      fprintf(stderr, "No memory.\n");
      exit(-1);
  }
  if (posix_memalign((void**)&yuv, 16, n*sizeof(t_pixel_yuv)) !=0) {
      fprintf(stderr, "No memory.\n");
      exit(-1);
  }
 
  for (i=0;i<n;i++) {
     rgb[i].r=(float) ((i<<26) ^ ((i + 340) << 22) ^ i ^ 0xf215fabc);
     rgb[i].g=(float) (((i+450)<<27) ^ ((i + 567) <<20) ^ i ^ 0xb152e8ca);
     rgb[i].b=(float) (((i+7823454) << 25) ^0xbad51cde);
  }

  rgb_to_yuv(rgb, yuv, n, NIt);

  write(1, yuv, n*sizeof(t_pixel_yuv));
  
  return(0);
}
