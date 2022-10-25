#include <stdio.h>
#include <sys/times.h>
#include <sys/types.h>
#include <time.h>
#define CACHE_MIN (1024) /* smallest cache */
#define CACHE_MAX (16*1024*1024) /* largest cache */
#define SAMPLE 10 /* to get a larger time sample */
#ifndef CLK_TCK
#define CLK_TCK 60 /* number clock ticks per second */
#endif
int x[CACHE_MAX]; /* array going to stride through */
 
double get_seconds() { /* routine to read time */
    struct tms rusage;
    times(&rusage); /* UNIX utility: time in clock ticks */
    return (double) (rusage.tms_utime)/CLK_TCK;
}

int main() {
    int register i, index, stride, limit;
    int temp;
    int steps, tsteps, csize;
    double sec0, sec; /* timing variables */
 
    for (csize=CACHE_MIN; csize <= CACHE_MAX; csize=csize*2) 
        for (stride=1; stride <= 128; stride=stride*2) {
            sec = 0; /* initialize timer */
            limit = csize-stride+1; /* cache size this loop */
     
            steps = 0;
            do { /* repeat until collect 1 second */
                sec0 = get_seconds(); /* start timer */
                for (i=SAMPLE*stride;i!=0;i=i-1) { /* larger sample */
                    for (index=0; index < limit; index=index+stride+4) {
                        x[index+0] = x[index+0] + 1; /* cache access */
			  x[index+1] = x[index+1] + 1; /* cache access */
			  x[index+2] = x[index+2] + 1; /* cache access */
			  x[index+3] = x[index+3] + 1; /* cache access */
			  }
			  
                steps = steps + 1; /* count while loop iterations */
                sec = sec + (get_seconds() - sec0);/* end timer */
				}
            } while (sec < 1.0); /* until collect 1 second */
     
            /* Repeat empty loop to loop subtract overhead */
            tsteps = 0; /* used to match no. while iterations */
            do { /* repeat until same no. iterations as above */
                sec0 = get_seconds(); /* start timer */
                for (i=SAMPLE*stride;i!=0;i=i-1) {/* larger sample */
                    for (index=0; index < limit; index=index+stride+4) {
                        temp = temp + index+0; /* dummy code */
			  			temp = temp + index+1; /* dummy code */
			  			temp = temp + index+2; /* dummy code */
			  			temp = temp + index+3; /* dummy code */
			  			}
			  		
                tsteps = tsteps + 1; /* count while iterations */
                sec = sec - (get_seconds() - sec0);/* - overhead */
                }
            } while (tsteps<steps); /* until = no. iterations */
     
            printf("Size (bytes):%9d, Stride (bytes):%4d, read+write:%6.2f ns\n",
               csize*sizeof(int), stride*sizeof(int),
               (double) sec*1e9/(steps*SAMPLE*stride*((limit-1)/stride+1)));
        }; /* end of both outer for loops */
}


