#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "vector_list.h"
#include "curve.h"

// #define NAN

int main(){
    pList l = create_list_file("Datasets/test4_set","Progress");
    pVector p0 = vector_next(l);
    pVector p1 = vector_next(l);

    double dist = dist_discrete_frechet(p0,p1);
    printf("dist is : %lf\n",dist);

    delete_list(&l);
    return 0;
}