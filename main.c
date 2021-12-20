#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "vector_list.h"
#include "vector.h"
#include "timer.h"
#include "grid.h"

// #define NAN

int main(){
    pGrid* grids = init_grids_table(10.0,10,2);
    delete_grids_table(&grids);

    // pGrid p = init_grid(10.0,2);
    // delete_grid(&p);
    return 0;
}