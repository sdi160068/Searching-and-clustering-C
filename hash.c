#include <stdio.h>
#include <stdlib.h>
#include "hash.h"
#include <math.h>
#include "mod.h"
#include "random.h"

static int f_return_bit(f* pf,int index,int h);

const int r_min = 1;
const int r_max = __INT32_MAX__;
static int w = 6;
const long int M = 4294967291;  // 2^32-5

/*  S t r u c t s  */
typedef struct h{
    int dim;
    double t;
    double* v;
}h;

typedef struct f{
    int d;
    int* size_of_table;
    h** hash;
    int*** memory;
}f;


typedef struct g{
    int k;
    h** hash;
    int* r;
}g;
/*-----------------*/

h* init_h(int dim){             // initialize hash function h_i
    if( dim < 1){ return NULL; }
    h* p0 = malloc(sizeof(h));
    p0->dim = dim;
    p0->t = random_double(0,w);
    p0->v = malloc(sizeof(double)*dim);
    for(int i=0; i<dim; i++)    // generate coordinates for v vector via normal distribution ( normal_generator )
        p0->v[i] = normal_generator();
    return p0;
}

f* init_f(int d,int dim){   // d -> d' , dim -> #dimensions of vectors
	f* p0 = malloc(sizeof(f));
    p0->d = d;
	p0->size_of_table = malloc(sizeof(int)*d);
	p0->memory = malloc(sizeof(int**)*d);
    p0->hash = malloc(sizeof(h*)*d);
    for(int i=0; i<d; i++){
        p0->size_of_table[i] = 0;
        p0->memory[i] = NULL;
        p0->hash[i] = init_h(dim);
    }
	return p0;
}

long int hash_f(f* pf,pVector p){
    long int result = 0;
    int check = 0;
    for(int i=0; i<pf->d; i++){
        result += f_return_bit(pf,i,hash_h(pf->hash[i],p,&check) ) * pow((double) 2,(double) i);    // bit * 2^i 
    }
	return result;
}

int f_return_bit(f* pf,int index,int h){
    if(pf == NULL){return -1;}
    if( pf->memory[index] == NULL){
        pf->memory[index] = malloc(sizeof(int*));
        pf->memory[index][0] = malloc(sizeof(int)*2);
        pf->memory[index][0][0] = h;
        pf->memory[index][0][1] = random_int(0,1);
        pf->size_of_table[index]++;
        return pf->memory[index][0][1];
    }
    int** memory = pf->memory[index];
    int size = pf->size_of_table[index];
    for(int i=0; i< size; i++){
        if( memory[i][0] == h)
            return memory[i][1];
    }
    pf->size_of_table[index]++;
    pf->memory[index] = realloc(memory,sizeof(int*)*pf->size_of_table[index]);
    memory = pf->memory[index];
    memory[size] = malloc(sizeof(int)*2);
    memory[size][0] = h;
    memory[size][1] = random_int(0,1);
    return memory[size][1];
}


g* init_g(int k,int dim){             // initialize hash function g_i
    static int check = 1;
    if(check){ printf("w is %d\n",w); check = 0;}
    if( dim < 1){ return NULL; }
    g* p0 = malloc(sizeof(g));
    p0->k = k;
    p0->hash = malloc(sizeof(h*)*k);
    p0->r = malloc(sizeof(int)*k);
    for(int i =0; i<k; i++){
        p0->hash[i] = init_h(dim);
        p0->r[i] = random_int(r_min,r_max);
    }
    return p0;
}

void delete_h(h** pph){         // delete hash function h
    if( (*pph) != NULL){
        free((*pph)->v);
        free((*pph));
        *pph = NULL;
    }
}

void delete_f(f** ppf){         // delete hash function g
    if( (*ppf) != NULL){
        for(int i=0; i<(*ppf)->d; i++){
            delete_h(&(*ppf)->hash[i]);
            for(int j=0; j<(*ppf)->size_of_table[i]; j++){
                if( (*ppf)->memory[i] != NULL)
                    free((*ppf)->memory[i][j]);
            }
            free((*ppf)->memory[i]);
        }
        free((*ppf)->memory);
        free((*ppf)->size_of_table);
        free((*ppf)->hash);
        free((*ppf));
        *ppf = NULL;
    }
}

void delete_g(g** ppg){         // delete hash function g
    if( (*ppg) != NULL){
        for(int i=0; i<(*ppg)->k; i++)
            delete_h(&(*ppg)->hash[i]);
        free((*ppg)->hash);
        free((*ppg)->r);
        free((*ppg));
        *ppg = NULL;
    }
}

int hash_h(h* ph,pVector p, int* check){
    if( ph == NULL){printf(" - Error! In function hash_h, h* is NULL\n"); *check = 1; return -1;}
    else if( p == NULL ){printf(" - Error! In function hash_h, p is NULL\n"); *check = 1; return -1;}
    int dim_p = vector_dim(p);
    if(dim_p != ph->dim){ printf(" - Error! dim of hash function (%d) != dim of vector p (%d)!\n",ph->dim,dim_p); *check = 1; return -1;}
	int pv = 0;
	for(int i=0; i<dim_p; i++)
		pv += vector_coord(p,i)*ph->v[i];
    *check = 0;
	return (pv+ph->t)/w;
}

unsigned long int hash_g(g* pg,pVector p,unsigned long int table_size){
    unsigned long int result = hash_ID(pg,p);
    return mod(result,table_size);
}

unsigned long int hash_ID(g* pg,pVector p){
    if(pg == NULL){ printf(" - Error! g* is NULL\n"); return -1;}
    else if( p == NULL ){printf(" - Error! In function hash_ID, p is NULL\n"); return -1;}
    long int result = 0;
    int check;
    for(int i=0; i< pg->k; i++){
        result += mod(pg->r[i]*hash_h(pg->hash[i],p,&check),M);    // (r_i*h_i) mod M
    }
    return mod(result,M);
}


void change_w(long int ww){     // delete
    w = ww; 
}