#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "vector_list.h"
#include "curve.h"
#include "timer.h"

// #define NAN

int main(){
    pList l = create_list_file("Datasets/query_2.csv","Progress");
    //pList l = create_list_file("Datasets/test4_set","Progress");
    pVector p0 = vector_next(l);
    pVector p1 = vector_next(l);


    start_timer();
    double dist = dist_discrete_frechet(p0,p1);
    stop_timer_print();

    printf("dist is : %lf\n",dist);

    start_timer();
    double dist_simple = dist_frechet(p0,p1);
    stop_timer_print();
    printf("dist is : %lf\n",dist_simple);
    delete_list(&l);
    return 0;
}