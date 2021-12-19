#include <stdio.h>
#include "timer.h"
#include <sys/time.h>

struct timeval stop, start;
static int ok = 1;

int start_timer(){
    if(ok){
        ok = 0;
        gettimeofday(&start, NULL);
        return 0;
    }
    return 1;
}

double stop_timer(){
    if(!ok){
        gettimeofday(&stop, NULL);
        ok = 1;
        return ((stop.tv_sec - start.tv_sec) * 1000000 + stop.tv_usec - start.tv_usec) / 1000000.0;
    }
    return -1;
}

void stop_timer_print(){
    printf("time : %lf \n",stop_timer());
}