// A list struct
// Christoforos Kladis 1115201600068

#include <math.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "vector_list.h"
#include "loading.h"

static pLnode init_lnode(pVector pv); // used only from functions of this file
static void print_list2(pList pvl,FILE* stream);

// S T R U C T S

typedef struct lnode{
    long int ID;
    double dist;    // use for nearest neighbor
    pVector vector;
    pLnode next;   // the next vector of list
}lnode;

typedef struct list{
    long int size;
    int dim;        // number of dimensions of vectors
    pLnode temp;   // for function "vector_next"
    pLnode first;
    pLnode last;
}list;

//---------------

/* create an empty vectorList */
pList  create_list(int dim){
    pList pl = malloc(sizeof(list));
    pl->first = NULL;
    pl->last = NULL;
    pl->temp = NULL;
    pl->size = 0;
    pl->dim = dim;
    return pl;
}

/*  Create an empty vectorList from file.
    If there is an error ( if file doesn't exist for example ) return NULL
*/
pList  create_list_file(char* filename,char* message){
    pList pl = NULL;
    long int lines = lines_of_file(filename);
    if(lines < 0){return NULL;}          // if there is no file , return NULL
    FILE* file = fopen(filename,"r");
    pData pd = return_data(file);
    if(message != NULL)
        init_loading(message,lines);
    // init_loading("Create Vector list from file progress : ",lines);
    if(pd != NULL)
        pl = create_list(data_getSize(pd)-1);
    while(pd != NULL){
        new_vector_data(pl,pd);
        free_data(&pd);
        pd = return_data(file);
        loading();
    }
    end_loading();
    fclose(file);
    return pl;
}

/* Initialize lnode ( node of vectorList)*/
static pLnode init_lnode(pVector pv){
    if(pv == NULL){ return NULL;}
    pLnode p0 = malloc(sizeof(lnode));
    p0->ID = -1;
    p0->dist = 0.0;
    p0->vector = pv;
    p0->next = NULL;
    return p0;
}

/* Push vector to vectorList. If success , returns 0, else returns 1 */
int new_vector(pList pl,pVector pv,unsigned long int ID){             // ID for hash tables
    if(pv == NULL || pl == NULL ){ printf("Error! pVectorList or/and  pVector are NULL\n"); return 1;}
    else if(vector_dim(pv) != pl->dim){
        if(vector_dim(pv) == 0){ printf("Error! Vector with no dimensions\n");}
        else{ printf("Error! Vector %s has %d dimensions , not %d\n",vector_id(pv),vector_dim(pv),pl->dim);}
        return 1;
    }
    pLnode pln = init_lnode(pv);
    pln->ID = ID;
    if(pl->first == NULL){
        pl->first = pln;
        pl->last = pln;
    }
    else{
        pl->last->next = pln;
    }
    pl->last = pln;
    pl->size++;
    return 0;
}

int new_vector_sorted(pList pl,pVector pv,long int ID,double pv_dist){
    if(pv == NULL || pl == NULL ){ printf("Error! pVectorList or/and  pVector are NULL\n"); return 1;}
    pLnode pln = init_lnode(pv);
    pln->dist = pv_dist;
    pln->ID = ID;
    pl->size++;
    if(pl->last != NULL){       // if pv_dist is <= from pl->last->dist , then save it and return
        if(strcmp(vector_id(pl->last->vector),vector_id(pv))!=0 && pl->last->dist >= pv_dist){
            pl->last->next = pln;
            pl->last = pln;
            return 0;
        }
    }
    pLnode tmp = pl->first;
    pLnode tmp_prev = NULL;
    while(tmp != NULL){
        if( pv_dist >= tmp->dist ){ break; }
        tmp_prev = tmp;
        tmp = tmp->next;
    }
    if(tmp == NULL){       // first node is NULL
        pl->first = pln;
        pl->last = pln;
    }
    else if(strcmp(vector_id(tmp->vector),vector_id(pv))==0){
        free(pln);
        pl->size--;
    }
    else if(tmp_prev == NULL){       // if dist_pv > first->dist
        pln->next = pl->first;
        pl->first = pln;
    }
    else{
        tmp_prev->next = pln;
        pln->next = tmp;
        if( tmp == NULL)       // if tmp == NULL ( tmp == last->next ) set pln as last
            pl->last = pln;
    }
    return 0;
}


/* Takes a data object to make a Vector object and push it to vectorList. If success , returns 0, else returns 1 */
int new_vector_data(pList pl,pData p0){
    if(p0 == NULL)
        return 1;
    pVector pv = init_vector_pData(p0); 
    if(new_vector(pl,pv,-1)){
        delete_vector(&pv);
        return 1;
    }
    return 0;
}

/* Search for a vector via id . Return vector or else NULL*/
pVector search_vector(pList pl,char* id){
    if(pl == NULL){ return NULL;}
    pLnode p0 = pl->first;
    while(p0!=NULL){
        if(strcmp(vector_id(p0->vector),id) == 0)
            break;
        p0 = p0->next;
    }
    if (p0==NULL){
        printf("- There is no node with id : %s\n",id);
        return NULL;
    }
    return p0->vector;
}

void delete_node_byId(pList pl,char* id){
    pLnode p0 = pl->first;
    pLnode p0_prev;
    while(p0!=NULL){
        if(strcmp(vector_id(p0->vector),id) == 0)
            break;
        p0_prev = p0;
        p0 = p0->next;
    }
    if (p0==NULL){
        printf("- There is no node with id : %s\n",id);
    }
    else{
        if(pl->first == pl->last){
            pl->first = NULL;
            pl->last = NULL;
        }
        else if(pl->first == p0){
            pl->first = p0->next;
        }
        else if(pl->last == p0){
            pl->last = p0_prev;
            pl->last->next = NULL;
        }
        else{
            p0_prev->next = p0->next;
        }
        free(p0);
        p0_prev = NULL;
        p0 = NULL;
        pl->size-=1;
    }
}

void delete_first_lnode(pList pl){
    if(pl == NULL){ return ;}
    pLnode p0 = pl->first;
    if(p0 != NULL){
        if( pl->temp == pl->first)
            pl->temp = pl->first->next;
        pl->first = p0->next;
        free(p0);
    }
    pl->size--;
}

void delete_list(pList* ppl){ // You must give the address of list to work. It returns NULL.
    if(*ppl == NULL){return;}
    pLnode p0 = (*ppl)->first;
    while(p0 != NULL){
        (*ppl)->first = p0->next;
        delete_vector(&p0->vector);
        free(p0);
        p0 = (*ppl)->first;
    }
    free((*ppl));
    *ppl = NULL;
}

void delete_list_no_vectors(pList* ppl){  // Free only pointers of list, no vectors
    if(*ppl == NULL){return;}
    pLnode p0 = (*ppl)->first;
    while(p0 != NULL){
        (*ppl)->first = p0->next;
        free(p0);
        p0 = (*ppl)->first;
    }
    free((*ppl));
    *ppl = NULL;
}

void print_list(pList pvl){
    print_list2(pvl,stdout);
}

void fprint_list(pList pvl,char* text){
    FILE* fp = fopen(text,"a");
    print_list2(pvl,fp);
    fclose(fp);
}

void print_list2(pList pvl,FILE* stream){
    if(pvl == NULL){ fprintf(stream," - Error (print_list2)! Vector List is NULL\n"); return;}
    pLnode pln0 = pvl->first;
    fprintf(stream," List context\n -------------------\n");
    fprintf(stream," - dim  : %d\n",pvl->dim);
    fprintf(stream," - size : %ld\n",pvl->size);
    while(pln0 != NULL){
        fprintf(stream," ");
        // fprint_vector(pln0->vector,stream);
        fprint_vector(pln0->vector,stream);
        fprintf(stream," - dist : %lf\n",sqrt(pln0->dist));
        fprintf(stream,"\n");
        pln0 = pln0->next;
    }
}

void print_list_full(pList pvl){
    if(pvl == NULL){ printf(" - Error (print_list_full)! Vector List is NULL\n"); return;}
    pLnode pln0 = pvl->first;
    printf(" List context\n -------------------\n");
    printf(" - dim  : %d\n",pvl->dim);
    printf(" - size : %ld\n",pvl->size);
    while(pln0 != NULL){
        printf(" ");
        print_vector_coords(pln0->vector);
        printf("\n");
        pln0 = pln0->next;
    }
}

void lnode_set_dist(pLnode p0,double dist){
    if(p0 != NULL)
        p0->dist = dist;
}

void vector_next_init(pList pvl){    // initialize temp to point to list->first 
    if(pvl != NULL){
        pvl->temp = NULL;
    }
}

pVector vector_next(pList pvl){  // returns the first vector of list and, in every call, returns the next vector
    if( pvl == NULL){ return NULL;}
    else if( pvl->size == 0){ return NULL;}
    else if(pvl->temp == NULL){ pvl->temp = pvl->first; }
    else { pvl->temp = pvl->temp->next; }

    if(pvl->temp == NULL){ return NULL; }   // if temp is NULL , there is no vector
    return pvl->temp->vector;
}

pVector list_last(pList pvl){
    if( pvl != NULL){ 
        if( pvl->last != NULL)
            return pvl->last->vector;
    }
    return NULL;
}

pVector list_first(pList pvl){
    if( pvl != NULL){ 
        if( pvl->first != NULL)
            return pvl->first->vector;
    }
    return NULL;
}

long int size_of_list(pList pvl){ // Returns size of vectorList. If vectorList is NULL, returns EOF.
    if(pvl == NULL){ return EOF;}
    return pvl->size;
}
int dimensions_of_list(pList pvl){ // Returns number of dimension of vectors of vectorList. If vectorList is NULL, returns EOF.
    if(pvl == NULL){ return EOF;}
    return pvl->dim;
}

double min_dist_list(pList pvl){
    if(pvl == NULL){ return INFINITY; }
    else if(pvl->size == 0){ return INFINITY; }
    return pvl->last->dist;
}

double max_dist_list(pList pvl){
    if(pvl == NULL){ return 0; }
    else if(pvl->size == 0){ return 0; }
    return pvl->first->dist;
}

/* transfer vectors from pvl_transfer to pvl */
int transfer_vectorlist(pList pvl,pList pvl_transfer){
    if(pvl == NULL || pvl_transfer == NULL){ printf(" - Error! In transfer_vectorlist, pvl or/and pvl_transfer are NULLS !\n"); return 1;}
    else if(pvl->dim != pvl_transfer->dim){ printf(" - Error! In transfer_vectorlist, pvl->dim != pvl_transfer->dim !\n"); return 1;}
    if(pvl->first == NULL){
        pvl->first = pvl_transfer->first;
        pvl->last = pvl_transfer->last;
    }
    else if(pvl_transfer->first != NULL){
        pvl->last->next = pvl_transfer->first;
        pvl->last = pvl_transfer->last;
    }
    pvl->temp = pvl->first;
    pvl->size = pvl->size+pvl_transfer->size;
    pvl_transfer->size = 0;
    pvl_transfer->first = NULL;
    pvl_transfer->last  = NULL;
    pvl_transfer->temp  = NULL;
    return 0;
}

void reverse_vectorList(pList pvl){
    if( pvl == NULL){return;}
    else if( pvl->size < 2){ return;}
    pLnode tmp_prev = pvl->first;
    pLnode tmp = tmp_prev->next;
    tmp_prev->next = NULL;
    pLnode tmp_next = NULL;
    while( tmp != NULL){
        tmp_next = tmp->next;
        tmp->next = tmp_prev;
        tmp_prev = tmp;
        tmp = tmp_next;
    }
    pvl->first = tmp_prev;
}

pList vector_n_nearest_ID(pList pvl,pList nearests,pVector q,long int ID,int N){
    if(pvl == NULL || q == NULL){ printf(" - Error! In vector_nearest, pvl or/and p0 are NULLS !\n");  return NULL;}
    pList nearests_new;
    if( nearests == NULL ){ nearests_new = create_list(pvl->dim); }
    else                  { nearests_new = nearests; }
    int count_ID = 0;
    pLnode p0 = pvl->first;
    // find and add to list the ID vectors
    while(p0!= NULL && ID != -1){
        if(p0->ID == ID){
            new_vector_sorted(nearests_new,p0->vector,ID,dist_L2(q,p0->vector));
            if(size_of_list(nearests_new) > N)
                        delete_first_lnode(nearests_new);
            count_ID++;
        }
        p0 = p0->next;
    }

    if( count_ID < N){  // if count_ID < N then search tree again
        double worse_dist;
        p0 = pvl->first;
        worse_dist = INFINITY;
        while( p0 != NULL){
            if( p0->ID != ID || ID < 0){
                double dist_qp = dist_L2(q,p0->vector);
                if( dist_qp < worse_dist || size_of_list(nearests_new) < N){
                    new_vector_sorted(nearests_new,p0->vector,p0->ID,dist_qp);
                    if(size_of_list(nearests_new) > N)
                        delete_first_lnode(nearests_new);
                    worse_dist = max_dist_list(nearests_new);
                }
            }
            p0 = p0->next;
        }
    }
    return nearests_new;
}

pList vector_n_nearest_max(pList pvl,pList nearests,pVector q,double M,int N,int *num_retrieved_items){
    *num_retrieved_items = 0;
    if(pvl == NULL || q == NULL){ printf(" - Error! In vector_nearest, pvl or/and p0 are NULLS !\n");  return NULL;}
     
    pList nearests_new;
    if( nearests == NULL ){ nearests_new = create_list(pvl->dim); }
    else                  { nearests_new = nearests; }

    if( size_of_list(pvl) == 0){ return nearests_new; }
    double MAX;
    pLnode p0 = pvl->first;
    if(M < 0 ){ MAX = INFINITY ; }      // if M <0 turn it to INFINITY
    else{       MAX = M; }

    double worse_dist;
    worse_dist = INFINITY;

    long int count_items = 0;
    p0 = pvl->first;
    while( p0 != NULL && count_items < MAX){
        double dist_qp = dist_L2(q,p0->vector);
        if( dist_qp < worse_dist || size_of_list(nearests_new) < N){
            new_vector_sorted(nearests_new,p0->vector,p0->ID,dist_qp);
            if(size_of_list(nearests_new) > N)
                delete_first_lnode(nearests_new);
            worse_dist = max_dist_list(nearests_new);
        }
        p0 = p0->next;
        count_items++;
    }
    if(count_items != pvl->size && count_items !=(long int) M){
        printf("count items : %ld, size of list : %ld, M is %ld\n",count_items,pvl->size,(long int) M);
    }
    *num_retrieved_items = count_items;
    return nearests_new;
}

pList vector_n_nearest(pList pvl,pVector q,int N){
    if(q == NULL || pvl == NULL){ return NULL; }
    int num_of_retrieved_items = 0;
    pList best_N = vector_n_nearest_max(pvl,NULL,q,INFINITY,N,&num_of_retrieved_items);
    return best_N;
}

long int vector_nearest_number(pList pvl,pVector q){
    if(q == NULL || pvl == NULL){ return -1; printf("- Error (vector_nearest_number)! Vector or/and List are NULL\n"); }
    long int index = 0;
    long int i = 0;
    double dist, dist_min = INFINITY;
    for(pLnode p0 = pvl->first; p0 != NULL; p0 = p0->next){
        dist = dist_L2(p0->vector,q);
        if( dist_min > dist){
            dist_min = dist;
            index = i;
        }
        i++;
    }
    return index;
}

pList vector_range_search(pList pvl,pList range_search_list,pVector p0,double M,double R,int *num_retrieved_items){
    if(pvl == NULL || p0 == NULL){        printf(" - Error! In vector_range_search, pvl or/and p0 are NULLS !\n");  return NULL;}
    else if(pvl->dim != vector_dim(p0) ){ printf(" - Error! In vector_range_search, pvl->dim != p0->dim !\n");      return NULL;}
    if(range_search_list == NULL)
        range_search_list = create_list(pvl->dim);
    
    if(pvl->size == 0){ return range_search_list; }
    double MAX;
    if(M < 0 ){ MAX = INFINITY ; }      // if M <0 turn it to INFINITY
    else{       MAX = M; }

    pLnode plv_node = pvl->first;
    *num_retrieved_items = 0;
    long int count = 0;
    while( plv_node != NULL && count < MAX){     // search only MAX vectors
        pVector pv = plv_node->vector;
        double dist = dist_L2(p0,pv);
        if(  dist <= R )
            new_vector_sorted(range_search_list,pv,-1,dist);    // we dont care about ID, so ID = -1
        plv_node = plv_node->next;
        count++;
    }
    return range_search_list;
}

pList vector_range_search_cluster(pList pvl,pList range_search_list,pVector* centroids,pVector p0,double M,double R,long int cluster,int *num_retrieved_items){
    if(pvl == NULL || p0 == NULL){        printf(" - Error! In vector_range_search_cluster, pvl or/and p0 are NULLS !\n");  return NULL;}
    else if(pvl->dim != vector_dim(p0) ){ printf(" - Error! In vector_range_search_cluster, pvl->dim != p0->dim !\n");      return NULL;}
    if(range_search_list == NULL)
        range_search_list = create_list(pvl->dim);
    
    if(pvl->size == 0){ return range_search_list; }
    double MAX;
    if(M < 0 ){ MAX = INFINITY ; }      // if M <0 turn it to INFINITY
    else{       MAX = M; }

    pLnode plv_node = pvl->first;
    *num_retrieved_items = 0;
    long int count = 0;
    while( plv_node != NULL && count < MAX){     // search only MAX vectors
        long int vectors_cluster = vector_cluster(plv_node->vector);
        if(vectors_cluster > 0){
            if(vectors_cluster != cluster){
                /*
                    if the distance of vector from the new cluster (clusrter)
                     is better than vector's cluster (vectors_cluster), do vector->cluster = cluster
                */
                if(dist_L2(p0,centroids[cluster]) < dist_L2(p0,centroids[vectors_cluster])){
                    vector_set_cluster(p0,cluster);
                }
            }
        }
        else{
            double dist = dist_L2(p0,plv_node->vector);
            if(  dist <= R ){
                new_vector_sorted(range_search_list,plv_node->vector,-1,dist);    // we dont care about ID, so ID = -1
                vector_set_cluster(plv_node->vector,cluster);
            }
        }
        plv_node = plv_node->next;
        count++;
    }
    return range_search_list;
}

double silhouette(pList* items_clusters,pVector pi,long int cluster,pList clusters_list){
    if(items_clusters == NULL || pi == NULL || clusters_list == NULL || cluster < 0){ return -10.0;}
    double average_a = 0.0;
    double num = 0.0;
    for(pLnode p0 = items_clusters[cluster]->first; p0 != NULL; p0 = p0->next){
        average_a += dist_L2(p0->vector,pi);
        num += 1.0;
    }
    if( num != 0.0)
        average_a = average_a/num;
    long int new_index = 0;
    double dist = INFINITY;
    long int i=0;
    for(pLnode p0 = items_clusters[cluster]->first; p0 != NULL; p0 = p0->next){
        if( vector_cluster(p0->vector) != cluster){
            double new_dist = dist_L2(p0->vector,pi);
            if( new_dist < dist){
                dist = new_dist;
                new_index = i;
            }
        }
        i++;
    }
    num = 0.0;
    double average_b = 0.0;
    for(pLnode p0 = items_clusters[new_index]->first; p0 != NULL; p0 = p0->next){
        average_b += dist_L2(p0->vector,pi);
        num += 1.0;
    }
    if( num != 0.0)
        average_b = average_b/num;
    if(average_a < average_b){ return 1.0-average_a/average_b;}
    else if( average_a == average_b){ return 0.0;}
    else{ return (average_b/average_a) - 1.0; }
}