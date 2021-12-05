#ifndef __hash_H__
#define __hash_H__

#include "vector.h"

typedef struct h h;
typedef struct g g;
typedef struct f f;


h* init_h(int dim);                     // initialize hash function h_i

g* init_g(int k,int dim);               // initialize hash function g_i

f* init_f(int d,int dim);               // d -> d' , dim -> #dimensions of vectors

int hash_h(h* ph,pVector p, int* check);

long int hash_f(f* pf,pVector p);   // for cube

unsigned long int hash_ID(g* pg,pVector p);

unsigned long int hash_g(g* pg,pVector p,unsigned long int table_size);

void delete_h(h** pph);         // delete hash function h

void delete_f(f** ppf);         // delete hash function f

void delete_g(g** ppg);         // delete hash function g

void change_w(long int ww);     // delete

#endif