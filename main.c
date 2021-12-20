#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "vector_list.h"
#include "vector.h"

// #define NAN

int main(){
    pList pl = create_list_file("Datasets/test_set","Progress ");
    pVector p0 = vector_next(pl);

    pl = create_list_file("Datasets/test_set","Progress ");
    pVector p1 = vector_next(pl);

    double dist = dist_frechet(p0,p1);
    printf("%lf\n",dist);

    return 0;
}