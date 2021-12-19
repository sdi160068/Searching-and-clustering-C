#include <stdio.h>
#include "timer.h"

#include <sys/time.h>
 int main(){
    struct timeval stop, start;

    for(int i=0; i< 4; i++){
        gettimeofday(&start, NULL);
        //do stuff
        sleep(2);
        gettimeofday(&stop, NULL);
        printf("took %lu us\n", (stop.tv_sec - start.tv_sec) * 1000000 + stop.tv_usec - start.tv_usec); 

        start_timer();
        sleep(2);
        double tim = stop_timer();
        printf("took %lf us\n",tim);
    }

    return 0;
 }