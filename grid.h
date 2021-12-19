#ifndef __grid_H__
#define __grid_H__

#include "vector.h"

typedef struct grid * pGrid;

pGrid* init_grids_table(double delta, int size, int size_of_vectors);

void delete_grids_table(pGrid** pGtable);

pGrid init_grid(double delta,int size);

void delete_grid(pGrid* pp0);

pVector lsh_key_grid(pVector curve, pGrid pg, double pudding_number);

#endif