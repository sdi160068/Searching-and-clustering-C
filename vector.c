#include <math.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include "vector.h"

typedef struct vector{
    long int cluster;   // cluster index ( for clustering )
    char* id;    // id of vector
    int dim;        // dimensions of vector
    float* coords;    // a table of coordinates of vector
}vector;

pVector init_vector_pData(pData p0){
    pVector pln = malloc(sizeof(vector));
    pln->coords = malloc(sizeof(int)*(data_getSize(p0)-1));
    pln->dim = data_getSize(p0)-1;
    pln->cluster = -1;
    char* id = data_getWord(p0,0);
    pln->id = malloc((strlen(id)+1)*sizeof(char));
    strcpy(pln->id,id);
    for(int i=1; i<data_getSize(p0); i++ ){
        pln->coords[i-1] = atof(data_getWord(p0,i));
    }
    return pln;
}

pVector init_vector(char* id,int dim,float* coords){
    pVector pln = malloc(sizeof(vector));
    pln->coords = malloc(sizeof(int)*dim);
    pln->dim = dim;
    pln->cluster = -1;
    pln->id = malloc((strlen(id)+1)*sizeof(char));
    strcpy(pln->id,id);
    for(int i=0; i<dim; i++ ){
        pln->coords[i] = coords[i];
    }
    return pln;
}

pVector vector_from_file(FILE* file){
    pData p0 = return_data(file);
    if(p0 == NULL){ return NULL;}
    pVector pv = init_vector_pData(p0);
    free_data(&p0);
    return pv;
}

int vector_dim(pVector p0){
    if( p0 == NULL) { return EOF;}
    return p0->dim;
}

double vector_coord(pVector p0,int i){
    if( p0 == NULL) { return EOF;}
    else if( p0->dim < i+1){ return EOF;}
    return p0->coords[i];
}

long int vector_cluster(pVector p0){    // get vector's cluster ( use for clustering ONLY )
    if(p0 == NULL){ return -1;}
    return p0->cluster;
}

pVector vector_copy(pVector p0){
    if(p0 == NULL){ return NULL; }
    return init_vector(p0->id,p0->dim,p0->coords);
}

int vector_update_coords(pVector p0,double* coords){
    if(p0 == NULL){return 1;}
    for(int i=0; i<p0->dim; i++){
        p0->coords[i] = (float) coords[i];
    }
    return 0;
}

int vector_set_cluster(pVector p0,long int cluster){    // set vector to have a cluster ( use for clustering ONLY )
    if(p0 == NULL || cluster < 0 ){ return 1;}
    p0->cluster = cluster;
    return 0;
}


char* vector_id(pVector p0){
    if( p0 == NULL) { return NULL;}
    return p0->id;
}

void delete_vector(pVector* ppv){
    if((*ppv) != NULL){
        free((*ppv)->coords);
        free((*ppv)->id);
        free(*ppv);
        *ppv=NULL;
    }
}

void print_vector(pVector p0){
    if(p0 == NULL)
        printf("- There is no node\n");
    else
        printf("- Node of list with id : %s\n",p0->id);
}

void fprint_vector(pVector p0,FILE* fp){
    if(p0 == NULL)
        fprintf(fp,"- There is no node\n");
    else
        fprintf(fp,"- Node of list with id : %s\n",p0->id);
}

void print_vector_coords(pVector p0){
    if(p0 == NULL)
        printf("- There is no node\n");
    else{
        printf("- Vector id : %s\n  dim : %d\n",p0->id,p0->dim);
        int j =0;
        for(int i=0; i<p0->dim; i++){
            printf(" [%3d] = %1.3f ",i+1,p0->coords[i]);
            j++;
            if(j == 8 || i == p0->dim-1){printf("\n");j=0;}
        }
    }

}

double dist_L2(pVector p0, pVector p1){     // distance^2
    if(p0 == NULL || p1 == NULL){ printf(" - Error! p0 or p1 are NULL !\n"); return EOF; }
    else if(p0->dim != p1->dim){  printf(" - Error! p0->dim=%d and p1->dim=%d ! Different dimensions\n",p0->dim,p1->dim); return EOF; }
    else if( p0 == p1 ){ return 0.0;}
    double dist = 0;
    for(int i=0; i<p0->dim; i++){
        dist += pow(p0->coords[i]-p1->coords[i],2.0); // (x-y)^2
    }
    return dist;
}