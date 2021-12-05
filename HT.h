#ifndef HT_H
#define HT_H

#include "vector_list.h"
#include "random.h"

/*
How to use :  
- init_HT   | pHT <name of pointer> = init_HT( <number of buckets> );
- print_HT  | print_HT( <name of pointer> );
- delete_HT  | delete( & <name of pointer> ); ( Do not forget "&" before <name of pointer> )
*/
typedef struct hash_table* pHT;
typedef struct bucket* pBucket;

pHT init_HT(long int number,int dim);   // initialize hash table struct

int new_HT_element(pHT ph, pVector p0,unsigned long int ID,long int index);  // return 0 for success, 1 for error

void print_HT(pHT p0);

void print_HT_lite(pHT p0);      // print hash table contex - only numbers of vectors in buckets

void fprint_HT_lite(pHT p0,long int data_size, FILE* fp);      // print hash table contex - only numbers of vectors in buckets

void delete_HT(pHT* pp0);

long int size_HT(pHT p0);

pList HT_bucket(pHT p0,long int bucket_id);

#endif
