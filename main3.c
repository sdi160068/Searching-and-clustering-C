#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <time.h>

double fRand(double fMin, double fMax)
{
    double f = (double)rand() / RAND_MAX;
    return fMin + f * (fMax - fMin);
}

int main(){
    srand(time(0));
    
}