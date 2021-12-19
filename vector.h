#ifndef __vector__
#define __vector__

#include "data.h"

typedef enum dist_type{L2, discrete_frechet, continuous_frechet}dist_type;

typedef struct vector * pVector;

pVector init_vector_pData(pData p0);
pVector init_vector(char* id,int dim,float* coords);

pVector vector_from_file(FILE* file);

int vector_dim(pVector p0);
double vector_coord(pVector p0,int i);
char* vector_id(pVector p0);
long int vector_cluster(pVector p0);    // get vector's cluster ( use for clustering ONLY )

pVector vector_copy(pVector p0);
int vector_update_coords(pVector p0,double* coords);
int vector_set_cluster(pVector p0,long int cluster);    // set vector to have a cluster ( use for clustering ONLY )

void delete_vector(pVector* pv);

void print_vector(pVector p0); // prints "node of list with id" if the node exists, else it prints "there is no node"
void fprint_vector(pVector p0,FILE* fp);
void print_vector_coords(pVector p0);

double dist_L2(pVector p0, pVector p1);     // distance^2

double* point_curve(pVector c0,int index, double* point);

void print_point(double* p);

int delete_point_curve(double** point);

double dist_discrete_frechet(pVector p0, pVector p1);
double dist_frechet(pVector p0, pVector p1);

pVector snapping_curve(pVector p0,pVector t,double delta);     // snapping : floor((x-t)/delta + 1/2)*delta +t
pVector remove_duplicates_curve(pVector p0,int d,double padding_number);     // remove duplicates and add pudding number 

#endif