#include <math.h>
#include "math_custom.h"
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

double* point_curve(pVector c0,int index, double* point){
    if(c0 == NULL ){ printf("Error! Return NULL");return NULL;}
    else if(vector_dim(c0)%2 != 0 ){ printf("Error! Curves-vectors must have even number of coordinates\n"); return NULL;}
    else if(index*2 >vector_dim(c0) ){printf("Error! Index out of range !\n"); return NULL;}
    int index_t = index*2;
    point[0] = vector_coord(c0,index_t);
    point[1] = vector_coord(c0,index_t+1);
    return point;
}

void print_point(double* p){
    printf("(%lf,%lf)\n",p[0],p[1]);
}

int delete_point_curve(double** point){
    if(*point == NULL){ return 1;}
    free(*point);
    *point = NULL;
    return 0;
}

double dist_frechet(pVector p0, pVector p1){
    int size0 = vector_dim(p0)/2;
    int size1 = vector_dim(p1)/2;
    double c[size0][size1];
    int n=1,m=1;
    double point0[2] , point1[2];

    c[0][0] =  norm_2(point_curve(p0,0,point0),point_curve(p1,0,point1));

    for(int i=1; i<size0; i++){
        c[i][0] = max(c[i-1][0], norm_2(point_curve(p0,i,point0) , point_curve(p1,0,point1)));
    }
    for(int i=1; i<size1; i++){
        c[0][i] = max(c[0][i-1], norm_2(point_curve(p0,0,point0) , point_curve(p1,i,point1)));
    }

    n = 1; m = 1;
    while( n < size0 && m < size1){
        for(int i=n; i<size0; i++)
            c[i][m] = max(min(min(c[i-1][m],c[i][m-1]),c[i-1][m-1]), norm_2(point_curve(p0,i,point0),point_curve(p1,m,point1)));
        for(int i=m; i<size1; i++)
            c[n][i] = max(min(min(c[n-1][i],c[n][i-1]),c[n-1][i-1]), norm_2(point_curve(p0,n,point0),point_curve(p1,i,point1)));
        n++;
        m++;
    }
    return c[size0-1][size1-1];
}

pVector snapping_curve(pVector p0,pVector t,double delta){
    if(p0 == NULL || t == NULL){ printf(" - Error! p0 or t are NULL !\n"); return NULL; }
    float coords[vector_dim(p0)];
    int num = vector_dim(p0);
    for(int i=0; i< num; i=i+2){
        for( int j=0; j< vector_dim(t); j++){
            coords[i+j] = floor((vector_coord(p0,i+j) - vector_coord(t,j) )/delta+0.5)*delta + vector_coord(t,j);
        }
    }
    return init_vector(vector_id(p0),vector_dim(p0),coords);
}

pVector remove_duplicates_curve(pVector p0,int d,double padding_number){
    if(p0 == NULL){ printf(" - Error! p0 is NULL !\n"); return NULL; }
    float coords[vector_dim(p0)];
    
    float tmp_x = vector_coord(p0,0);
    float tmp_y = vector_coord(p0,1);
    int counter = d;
    coords[0] = vector_coord(p0,0);
    coords[1] = vector_coord(p0,1);


    for( int i=d; i<vector_dim(p0); i+=d){
        if(tmp_x == vector_coord(p0,i) && tmp_y == vector_coord(p0,i+1)){
            printf("same !!\n");
            continue;
        }
        else{
            // copy to new array
            coords[i] = vector_coord(p0,i);
            coords[i+1] = vector_coord(p0,i+1);

            // set tmp_x, tmp_y
            tmp_x = vector_coord(p0,i);
            tmp_y = vector_coord(p0,i+1);
        }
    }
    if( counter < vector_dim(p0)){
        for(int i=counter; i< vector_dim(p0); i++)
            coords[i] = padding_number;
    }

    return init_vector(vector_id(p0),vector_dim(p0),coords);
}
