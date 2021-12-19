// A list struct
// Christoforos Kladis 1115201600068

#ifndef __vector_list__
#define __vector_list__

#include "data.h"
#include "vector.h"

typedef struct list * pList;
typedef struct lnode * pLnode;

// returns an empty vectorList or NULL for errors
pList  create_list(int dim);

// returns a vectorList initialized from a file or NULL for errors
pList  create_list_file(char* filename,char* message);

int new_vector(pList pl,pVector pv,unsigned long int ID); /* Push vector to vectorList.
                                                                    If success , returns 0, else returns 1 . ID for hash tables */
int new_vector_data(pList pl,pData p0);  /*  Takes a data object to make a Vector object and 
                                                    push it to vectorList. If success , returns 0, else returns 1 */
int new_vector_sorted(pList pl,pVector pv,long int ID,double pv_dist);   // push vector and sort list (via dist)

pVector search_vector(pList pl,char* id); // Search for a vector via id . Returns vector or else NULL.

/* Search for a vector by index . Return vector or else NULL. If index > list.size , return last vector */
pVector search_vector_by_index(pList pl,long int index);

void delete_vector_byId(pList pl,char* id);  // Deletes vector by id.
void delete_node_by_ClusterId(pList pl,long int ClusterId);     // remove node by Cluster id
void delete_first_lnode(pList pl);

void delete_list(pList* ppl); // You must give the address of list to work. It returns NULL.
void delete_list_no_vectors(pList* ppl);  // Free only pointers of list, no vectors

void print_list(pList pvl);
void fprint_list(pList pvl,char* text);
void print_list_full(pList pvl);

void lnode_set_dist(pLnode p0,double dist);

pList vector_n_nearest_ID(pList pvl,pList nearests,pVector q,long int ID,int N,dist_type metric);
pList vector_n_nearest_max(pList pvl,pList nearests,pVector q,double M,int N,int *num_retrieved_items,dist_type metric);
pList vector_n_nearest(pList pvl,pVector q,int N,dist_type metric);
long int vector_nearest_number(pList pvl,pVector q,dist_type metric);

pList vector_range_search(pList pvl,pList range_search_list,pVector p0,double M,double R,int *num_retrieved_items,dist_type metric);

/* use ONLY for cluster!! p0 is the searching cluster */
pList vector_range_search_cluster(pList pvl,pList range_search_list,pVector* centroids,double M,double R,long int cluster_id,int *num_retrieved_items,dist_type metric);
void vector_next_init(pList pvl);    // initialize temp to point to list->first 
pVector vector_next(pList pvl);  // returns the first vector of list and, in every call, return the next vector

pVector list_last(pList pvl);
pVector list_first(pList pvl);

long int size_of_list(pList pvl); // Returns size of vectorList. If vectorList is NULL, returns EOF.
int dimensions_of_list(pList pvl); /* Returns number of dimension of vectors of vectorList.
                                                If vectorList is NULL, returns EOF. */
double min_dist_list(pList pvl);
double max_dist_list(pList pvl);

int transfer_vectorlist(pList pvl,pList pvl_transfer);    /* transfer vectors from pvl_transfer to pvl */
void reverse_vectorList(pList pvl);

pVector* list_to_array_vector(pList pvl);

double silhouette(pList* items_clusters,pVector pi,long int cluster,pList clusters_list);

pVector mean_vector(pList pvl, char * id);

int update_centroid_vector(pList pvl, pVector centroid,long int cluster_id);

#endif
