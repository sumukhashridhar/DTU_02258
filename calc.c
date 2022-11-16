#include <stdio.h>
#include <time.h>
<<<<<<< HEAD
#include <sys/times.h>
#include <sys/types.h>

#ifndef CLK_TCK
#define CLK_TCK 60 /* number clock ticks per second */
#endif

#define INTERVALS 10000000
#define CLOCK_RATE_GHZ 2.3e9

double get_seconds() { /* routine to read time */
    struct tms rusage;
    times(&rusage); /* UNIX utility: time in clock ticks */
    return (double) (rusage.tms_utime)/CLK_TCK;
}

static __inline__ unsigned long long RDTSC(void) {
    unsigned hi,lo;
    __asm__ volatile("rdtsc" : "=a"(lo),"=d"(hi));
    return ((unsigned long long) lo)| (((unsigned long long)hi) << 32);
}
=======

#define INTERVALS 100000
>>>>>>> task2

double a[INTERVALS], b[INTERVALS];

int main(int argc, char **argv)
{
  double time2;
  time_t time1 = clock();
  double *to = b;
  double *from = a;
  int    time_steps = 100;
<<<<<<< HEAD
  double cycles0, cycles; /* timing variables */
=======
>>>>>>> task2

  /* Set up initial and boundary conditions. */
  from[0] = 1.0;
  from[INTERVALS - 1] = 0.0;
  to[0] = from[0];
  to[INTERVALS - 1] = from[INTERVALS - 1];
  
  for(long i = 1; i < INTERVALS; i++)
   from[i] = 0.0;
  
  printf("Number of intervals: %ld. Number of time steps: %d\n", INTERVALS, time_steps);

<<<<<<< HEAD
  //cycles = 0; /* initialize timer */
  
  /* Apply stencil iteratively. */
  
  while(time_steps-- > 0)
  {
  // cycles0 = get_seconds(); /* start timer */
       unsigned long long cycles = RDTSC();

=======
  /* Apply stencil iteratively. */
  while(time_steps-- > 0)
  {
>>>>>>> task2
   for(long i = 1; i < (INTERVALS - 1); i++)
    to[i] = from[i] + 0.1*(from[i - 1] - 2*from[i] + from[i + 1]);

   {
    double* tmp = from;
    from = to;
    to = tmp;
   }
<<<<<<< HEAD
   cycles = (get_seconds() - cycles0);/* end timer */
   cycles = RDTSC()-cycles;
    
    double microseconds = cycles/CLOCK_RATE_GHZ*1e6;
   printf("elapsed time is %6.2f ns\n", cycles);   
=======
>>>>>>> task2
  }

  time2 = (clock() - time1) / (double) CLOCKS_PER_SEC;

  printf("Elapsed time (s) = %f\n", time2);

<<<<<<< HEAD
  for(long i = 2; i < 30; i += 2)
=======
  for(long i = 2; i < 12; i += 2)
>>>>>>> task2
   printf("Interval %ld: %f\n", i, to[i]);
  
  return 0;
}                
<<<<<<< HEAD

=======
>>>>>>> task2
