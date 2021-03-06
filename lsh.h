#ifndef __lsh_H__
#define __lsh_H__

typedef struct lsh_obj * pLsh;

#include "grid.h"
#include "vector_list.h"

pLsh init_lsh(int size,int L, int k, pList pvl);

pLsh init_by_grids_lsh(int size,int L, int k,pGrid* grids, pList pvl, double pudding_number);

int delete_lsh(pLsh * lsh_object);

int delete_frechet_lsh(pLsh * plsh0);

int lsh(int k, int L, char* input_file ,char* output_file, char* query_file);

int lsh_frechet(int k, int L, double delta,char* input_file ,char* output_file, char* query_file,dist_type metric);

pList lsh_n_nearests(pLsh plsh0,pVector q,int N,dist_type metric);

pList lsh_frechet_n_nearest(pLsh plsh0,pGrid* grids,pVector q,double pudding_number,int N,dist_type metric);

void lsh_fprintf(FILE* output,pList nn_list,pList nn_brute_list,pVector q,\
        double lsh_timer,double true_timer,double* appr_factor,dist_type metric);

void print_lsh(pLsh plsh0);

pList lsh_clusters_range_search(pVector* centroids,long int centroid_id, pLsh plsh0,int L,double R,pList range_search_list);

long int get_retrieved_items_lsh(pLsh plsh0);

#endif