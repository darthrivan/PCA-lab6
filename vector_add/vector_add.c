#include <stdio.h>
#include <stdlib.h>
#include <emmintrin.h>

#define N 8192 /* Tindrem tot el vector a la cache */


#if defined(INT)
#define TIPUS int
#define INTR(a, b) _mm_add_epi32(a,b)
#elif defined(SHORT)
#define TIPUS short
#define INTR(a, b) _mm_add_epi16(a,b)
#else
#define TIPUS char
#define INTR(a, b) _mm_add_epi8(a,b)
#endif

#ifndef VECTORIAL
void vector_add(TIPUS *A, TIPUS *B, TIPUS *C, int len, int N_iter)
{
    int i,j;

    for (j=0; j<N_iter; j++)
        for (i=0;i<len;i++)
            C[i] = A[i] + B[i];
}
#else
void vector_add(TIPUS *A, TIPUS *B, TIPUS *C, int len, int N_iter)
{
    int i,j;

    for (j=0; j<N_iter; j++)
        for (i=0; i<len; i=i+(sizeof(__m128i)/sizeof(TIPUS))) {
            __m128i *pa, *pb, *pc;

            pa = (__m128i*) &A[i];
            pb = (__m128i*) &B[i];
            pc = (__m128i*) &C[i];
            *pc = INTR(*pa, *pb);
        }
}

/*
// Alternative equivalent
void vector_add(TIPUS *A, TIPUS *B, TIPUS *C, int len, int N_iter)
{
    int i,j;

    for (j=0; j<N_iter; j++)
        for (i=0; i<len; i=i+(sizeof(__m128i)/sizeof(TIPUS))) {
            __m128i pa, pb, pc;

            pa = _mm_load_si128((__m128i*) &A[i]);
            pb = _mm_load_si128((__m128i*) &B[i]);
            pc = INTR(pa, pb);
            _mm_store_si128((__m128i*) &C[i],pc);
        }
}

// Alternative unaligned
void vector_add(TIPUS *A, TIPUS *B, TIPUS *C, int len, int N_iter)
{
    int i,j;

    for (j=0; j<N_iter; j++)
        for (i=0; i<len; i=i+(sizeof(__m128i)/sizeof(TIPUS))) {
            __m128i pa, pb, pc;

            pa = _mm_loadu_si128((__m128i*) &A[i]);
            pb = _mm_loadu_si128((__m128i*) &B[i]);
            pc = INTR(pa, pb);
            _mm_storeu_si128((__m128i*) &C[i],pc);
        }
}
*/
#endif

int main (int argc, char *argv[])
{

  TIPUS *A, *B, *C; /* Vectores para malloc */

  int i;
  int n=N; /* Number of elements */
  int NIt=500000; /* Number of iterations*/

  if (argc>1) NIt = atoi(argv[1]);
  if (argc>2) n = atoi(argv[2]);
  if (n>N) { printf("Maxima longitud vector: %d\n",N); exit(-1); }

#ifdef VECTORIAL
  if ((posix_memalign((void**)&A, 16, n)!=0) || (posix_memalign((void**)&B, 16, n)!=0) || (posix_memalign((void**)&C, 16, n)!=0) )
  {
    printf("No memory.\n");
    exit(-1);
  }
#else
  A= (TIPUS *)malloc(sizeof(TIPUS)*n);
  if (A==NULL) { printf("No memory.\n"); exit(-1);}
  B= (TIPUS *)malloc(sizeof(TIPUS)*n);
  if (B==NULL) { printf("No memory.\n"); exit(-1);}
  C= (TIPUS *)malloc(sizeof(TIPUS)*n);
  if (C==NULL) { printf("No memory.\n"); exit(-1);}
#endif

  n = n/sizeof(TIPUS);
  NIt = NIt*sizeof(TIPUS); /* Per igualar el nombre d'operacions add */

  /* Inicialització amb nombres "aleatoris" */
  for (i=0;i<n;i++) {
     A[i]=i^(i<<6)^i;
     B[i]=(i-5)^(i<<4)^(i+3);
  }

  vector_add (A,B,C,n,NIt);

  write(1, C, n*sizeof(TIPUS));

  return(0);
}
