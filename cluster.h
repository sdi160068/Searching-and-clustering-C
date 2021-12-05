#ifndef __cluster_H__
#define __cluster_H__

#include "vector_list.h"
#include <math.h>

void cluster_get_conf(char* filename,int* K,int* L_lsh,int* k_lsh,int* k_cube,int* M_cube,int* p_cube);
pList k_means_pp(pList pvl,int num_clusters);
int cluster_Lloyd_method(pList pvl,pList centroids,int max_times,int complete,char* output);

#endif