#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <emmintrin.h> 
#include <immintrin.h>
#include <stdint.h>
#include <string.h>

 /* where intrinsics are defined */

#define CLOCK_RATE_GHZ 2.3e9

/* Time stamp counter from Lecture 2/17 */
static __inline__ unsigned long long RDTSC(void) {
    unsigned hi,lo;
    __asm__ volatile("rdtsc" : "=a"(lo),"=d"(hi));
    return ((unsigned long long) lo)| (((unsigned long long)hi) << 32);
}

int sum_naive( int n, int *a )
{
    int sum = 0;
    for( int i = 0; i < n; i++ )
        sum += a[i];
    return sum;
}

int sum_unrolled( int n, int *a )
{
    int sum = 0;

    /* do the body of the work in a faster unrolled loop */
    for( int i = 0; i < n/4*4; i += 4 )
    {
        sum += a[i+0];
        sum += a[i+1];
        sum += a[i+2];
        sum += a[i+3];
    }

    /* handle the small tail in a usual way */
    for( int i = n/4*4; i < n; i++ )   
        sum += a[i];

    return sum;
}

int sum_vectorized( int n, int *a )
{

	__m128i vecSum = _mm_setzero_si128();
	int sum = 0;
	int i = 0;
    int u_count = 4;

	for (; i < n-1; i+=u_count) {
        vecSum = _mm_add_epi32(vecSum, _mm_loadu_si128((__m128i *)(a + i)));
	}
	
	vecSum = _mm_hadd_epi32(vecSum, vecSum);
	vecSum = _mm_hadd_epi32(vecSum, vecSum);
	sum = _mm_extract_epi32(vecSum, 0);

	i = n - (n % u_count);

	for(; i < n; i++)
    {
        sum += a[i];
    }

    return sum;
}

int sum_vectorized_unrolled( int n, int *a )
{
	__m128i vecSum = _mm_setzero_si128();
	int sum = 0;
	int i = 0;
    int u_count = 16;
    int u_count_inc = u_count/4; // 4 unrolled loops, if you do more divide by that number

	for (; i < n-1; i+=u_count) {
	
        vecSum = _mm_add_epi32(vecSum, _mm_loadu_si128((__m128i *)(a + i + 0)));
        vecSum = _mm_add_epi32(vecSum, _mm_loadu_si128((__m128i *)(a + i + u_count_inc)));
        vecSum = _mm_add_epi32(vecSum, _mm_loadu_si128((__m128i *)(a + i + 2*u_count_inc)));
        vecSum = _mm_add_epi32(vecSum, _mm_loadu_si128((__m128i *)(a + i + 3*u_count_inc)));

	}
	vecSum = _mm_hadd_epi32(vecSum, vecSum);
	vecSum = _mm_hadd_epi32(vecSum, vecSum);
	sum = _mm_extract_epi32(vecSum, 0);

	i = n - (n % u_count);

	for(; i < n; i++)
    {
        sum += a[i];
    }

    return sum;
        return 0;
}

void benchmark( int n, int *a, int (*computeSum)(int,int*), char *name )
{
    /* warm up */
    int sum = computeSum( n, a );

    /* measure */
    unsigned long long cycles = RDTSC();
    sum += computeSum( n, a );
    cycles = RDTSC()-cycles;
    
    double microseconds = cycles/CLOCK_RATE_GHZ*1e6;
    
    /* report */
    printf( "%20s: ", name );
    if( sum == 2*sum_naive(n,a) ) printf( "%.2f microseconds\n", microseconds );
    else	                  printf( "ERROR!\n" );
}

int main( int argc, char **argv )
{
    const int n = 7777; /* small enough to fit in cache */
    
    /* init the array */
    srand48( time( NULL ) );
    int a[n] __attribute__ ((aligned (16))); /* align the array in memory by 16 bytes */
    for( int i = 0; i < n; i++ ) a[i] = lrand48( );
    /* benchmark series of codes */
    benchmark( n, a, sum_naive, "naive" );
    benchmark( n, a, sum_unrolled, "unrolled" );
    benchmark( n, a, sum_vectorized, "vectorized" );
    benchmark( n, a, sum_vectorized_unrolled, "vectorized unrolled" );

    return 0;
}

