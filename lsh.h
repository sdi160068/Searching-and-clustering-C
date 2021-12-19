#ifndef __lsh_H__
#define __lsh_H__

typedef struct lsh_obj * pLsh;

#include "vector_list.h"

pLsh init_lsh(int size,int L, int k, pList pvl);

int delete_lsh(pLsh * lsh_object);

int delete_frechet_lsh(pLsh * plsh0);

int lsh(int k, int L, char* input_file ,char* output_file, char* query_file);

pList lsh_n_nearests(pLsh lsh_object,pVector q,int N);

void  lsh_fprintf(FILE* output,pList nn_list,pList nn_brute_list,pVector q,double cube_timer,double true_timer);

// pList lsh_clusters_range_search(pVector* centroids,long int centroid_id, pHT* hash_tables,g** g_functions,long int table_size,int L,double R);

#endif