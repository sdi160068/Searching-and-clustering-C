#include "HT.h"
#include <stdio.h>
#include <stdlib.h>

// HT == H-ash T-able
//pHT == p-ointer for H-ash T-able

/*----- S T U C T -----*/
typedef struct hash_table{
    long int size;          // size == number of buckets
    pList* hTable;   // hash table
}hash_table;
/*-----------------------*/


pHT init_HT(long int size,int dim){   // initialize hash table struct
    hash_table* p0 = malloc(sizeof(hash_table));
    p0->size = size;
    p0->hTable = malloc(sizeof(pList)*size);
    for(long int i=0; i < size; i++)
        p0->hTable[i] = create_list(dim);
    return p0;
}

int new_HT_element(pHT ph, pVector p0,unsigned long int ID,long int index){
    if(index < 0 || index >= ph->size){ 
        printf(" - Error! index == %ld ,but hashTable->size == %ld !\n",index,ph->size);
        return 1;
    }
    new_vector(ph->hTable[index],p0,ID);
    return 0;
}

void print_HT(pHT p0){      // print hash table contex
    printf("Hash Table Buckets :\n\n");
    for(long int i = 0; i< p0->size; i++){
        printf("------\nBucket %ld : \n------\n\n",i);
        print_list(p0->hTable[i]);
    }
}

void print_HT_lite(pHT p0){      // print hash table contex - only numbers of vectors in buckets
    printf("Hash Table Buckets :\n\n");
    for(long int i = 0; i< p0->size; i++){
        printf("------\nBucket %ld : \n------\n\n",i);
        printf(" - size : %ld\n - dim  : %d\n",size_of_list(p0->hTable[i]),dimensions_of_list(p0->hTable[i]));
    }
}

void fprint_HT_lite(pHT p0,long int data_size, FILE* fp){      // print hash table contex - only numbers of vectors in buckets
    fprintf(fp,"Hash Table stats :\n\n");
    long int perfect = data_size/p0->size;
    long int zone = perfect/3;
    long int low = 0;
    long int high = 0;
    long int zeros = 0;
    long int in_zone = 0;
    int tmp;
    for(long int i = 0; i< p0->size; i++){
        tmp = size_of_list(p0->hTable[i]);
        if(tmp == 0)
            zeros++;
        else if(tmp <= perfect+zone && tmp >= perfect-zone)
            in_zone++;
        else if(tmp < perfect)
            low++;
        else
            high++;
    }
    fprintf(fp,"perfect size of list per bucket : %ld/%ld\n",perfect,data_size);
    fprintf(fp,"buckets with size in (%3ld,%3ld)  : %3.1lf%%\n",perfect-zone,perfect+zone,(double)(in_zone*100)/(double)p0->size);
    fprintf(fp,"buckets with size < perfect     : %3.1lf%%\n",(double)(low*100)/(double)p0->size);
    fprintf(fp,"buckets with size > perfect     : %3.1lf%%\n",(double)(high*100)/(double)p0->size);
    fprintf(fp,"buckets with size 0             : %3.1lf%%\n",(double)(zeros*100)/(double)p0->size);
}

void delete_HT(pHT* pp0){   // delete hash table - pointer will point to NULL
    for(long int i = 0; i< (*pp0)->size; i++){
        delete_list_no_vectors(&(*pp0)->hTable[i]);
        (*pp0)->hTable[i] = NULL;
    }
    free((*pp0)->hTable);
    free(*pp0);
    *pp0 = NULL;
}

void delete_full_HT(pHT* pp0){   // delete hash table - pointer will point to NULL
    for(long int i = 0; i< (*pp0)->size; i++){
        delete_list(&(*pp0)->hTable[i]);
        (*pp0)->hTable[i] = NULL;
    }
    free((*pp0)->hTable);
    free(*pp0);
    *pp0 = NULL;
}

long int size_HT(pHT p0){
    if(p0 == NULL){ return -1;}     // returns -1 if hash table doesn't exist
    return p0->size;
}

pList HT_bucket(pHT p0,long int bucket_id){
    if(p0 == NULL){printf(" - Error! Hash table is NULL\n"); return NULL;}
    else if(p0->size == 0){ printf(" - Error! Hash table has size == 0\n"); return NULL;}
    else if(bucket_id < 0 || bucket_id >= p0->size){
        printf(" - Error! bucket_id %ld is < or >= of hash table size (%ld)\n",bucket_id,p0->size);
        return NULL;
    }
    return p0->hTable[bucket_id];
}