#include "grid.h"
#include "random.h"
#include <stdio.h>
#include <stdlib.h>
#include "vector.h"

typedef struct grid{
    double delta;
    pVector t;
}grid;

pGrid* init_grids_table( double delta, int size, int size_of_vectors){
    if( delta < 0 || size < 1 || size_of_vectors < 1){ return NULL;}
    pGrid* table = malloc(sizeof(pGrid)*(size+1));
    for(int i=0; i<size; i++)
        table[i] = init_grid(delta,size_of_vectors);
    table[size] = NULL;
    return table;
}

void delete_grids_table(pGrid** pGtable){
    if(*pGtable != NULL){
        pGrid p0 = *pGtable[0];
        for(int i=0; p0 != NULL; i++){
            delete_grid(&p0);
            p0 = *pGtable[i+1];
        }
        free(*pGtable);
        *pGtable = NULL;
    }
}

pGrid init_grid(double delta,int size){
    if( delta < 0 || size < 1){ return NULL;}
    pGrid p0 = malloc(sizeof(grid));
    p0->delta = delta;
    float* t = malloc(sizeof(float)*size);
    for(int i=0; i< size; i++)
        t[i] = random_double(0.0, delta);
    p0->t = init_vector("0",size,t);
    return p0;
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
    pVector new_curve = remove_duplicates_curve(snap,2,pudding_number);
    delete_vector(&snap);
    return new_curve;
}


