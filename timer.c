#include <stdio.h>
#include "timer.h"
#include <time.h>

static clock_t t;
static int ok = 1;

int start_timer(){
    if(ok){
        ok = 0;
        t = clock();
        return 0;
    }
    return 1;
}

double stop_timer(){
    if(!ok){
        t = clock() - t;
        return ((double)t)/CLOCKS_PER_SEC;
    }
    return -1;
}

void stop_timer_print(){
    printf("time : %lf \n",stop_timer());
}