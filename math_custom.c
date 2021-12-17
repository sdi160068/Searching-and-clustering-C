#include <stdio.h>
#include <math.h>
#include "math_custom.h"

inline double max(double a1, double a2){
    if( a1 <= a2){ return a2; }
    return a1;
}

inline double min(double a1, double a2){
    if( a1 <= a2){ return a1; }
    return a2;
}

inline double norm_2(double* point1, double* point2){
    return sqrt(pow(point1[0]-point2[0],2.0) + pow(point1[1]-point2[1],2.0));
}