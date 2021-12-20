#include "grid.h"
#include "random.h"
#include <stdio.h>
#include <stdlib.h>
#include "vector.h"

typedef struct grid{
    double delta;
    pVector t;
}grid;

pGrid* init_grids_table( double delta, int num_of_dimensions, int size_of_vectors){
    if( delta < 0 || num_of_dimensions < 1 || size_of_vectors < 1){ return NULL;}
    pGrid* table = malloc(sizeof(pGrid)*(num_of_dimensions+1));
    for(int i=0; i<num_of_dimensions; i++)
        table[i] = init_grid(delta,size_of_vectors);
    table[num_of_dimensions] = NULL;
    return table;
}

void delete_grids_table(pGrid** pg_table){
    if(*pg_table != NULL){
        pGrid* pGtable = *pg_table;
        pGrid p0 = pGtable[0];
        for(int i=1; p0 != NULL; i++){
            delete_grid(&p0);
            p0 = pGtable[i];
        }
        free(*pg_table);
        *pg_table = NULL;
    }
}

pGrid init_grid(double delta,int num_of_dimensions){
    if( delta <= 0 || num_of_dimensions < 1){ return NULL;}
    pGrid p0 = malloc(sizeof(grid));
    p0->delta = delta;
    float t[num_of_dimensions];
    for(int i=0; i< num_of_dimensions; i++){
        t[i] = random_double(0.0, delta);
    }
    p0->t = init_vector("0",num_of_dimensions,t);
    return p0;
}

void print_grid(pGrid p0){
    if(p0 == NULL){ printf("grid p0 == NULL\n");return;}
    printf("delta : %lf\n",p0->delta);
    print_vector_coords(p0->t);
    printf("---------------\n");
}

void delete_grid(pGrid* pp0){
    if(*pp0 != NULL){
        delete_vector(&(*pp0)->t);
        free(*pp0);
        *pp0 = NULL;
    }
}

pVector lsh_key_grid(pVector curve, pGrid pg, double pudding_number){
    if( curve == NULL || pg == NULL){ printf("Error (edit_curve_grid)! Return NULL");return NULL;}
    pVector snap = snapping_curve(curve,pg->t,pg->delta);
    pVector new_curve = remove_duplicates_curve(snap,pudding_number);
    delete_vector(&snap);
    return new_curve;
}


pVector t_grid(pGrid pg){
    if(pg == NULL){ return NULL;}
    return pg->t;
}

