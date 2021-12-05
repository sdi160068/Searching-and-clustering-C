#include "hash.h"
#include "HT.h"
#include "loading.h"
#include "lsh.h"
#include <math.h>
#include "mod.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/time.h>
#include "vector.h"
#include "vector_list.h"


#include <signal.h>         // for breakpoints
#define SIGINT 2

pList lsh_n_nearests(pHT* hash_tables,g** g_functions,long int table_size,int L,pVector q,int N);
pList vector_n_nearest(pList pvl,pVector q,int N);
pList lsh_range_search(pHT* hash_tables,g** g_functions,long int table_size,int L,pVector q,double R);
void  lsh_fprintf(FILE* output,pList nn_list,pList nn_brute_list,pList rs_list,pVector q,double cube_timer,double true_timer,double R);

int lsh(int k, int L, char* input_file ,char* output_file, char* query_file){

    printf("k %d\n",k);
    printf("L %d\n",L);

    printf("input_file  %s\n",input_file);
    printf("output_file %s\n",output_file);
    printf("query_file  %s\n",query_file);
    // List with all vectors
    pList pvl = create_list_file(input_file,"Create Vector list from file progress : ");
    printf("List of vectors is ready !\n");

    // size of hash tables
    long int table_size = size_of_list(pvl)/16 ;  // table_size = n/16

    // table with hash tables
    pHT hash_tables[L];    
    // table with g functions for every hash table
    g* g_functions[L];

    // initialize hash tables anf g hash functions
    for(int i=0; i<L; i++){
        hash_tables[i] = init_HT(table_size,dimensions_of_list(pvl));
        g_functions[i] = init_g(k,dimensions_of_list(pvl));
    }

    init_loading("Hash table built progress : ", size_of_list(pvl));    // loading : progress for any loop with limit size

    // start read vectors from list
    vector_next_init(pvl);
    pVector pv = vector_next(pvl);

    // put vectors to hash tables
    while( pv!= NULL){
        for(int i=0; i<L; i++){
            long int index = hash_g(g_functions[i],pv,table_size);
            new_HT_element(hash_tables[i],pv,hash_ID(g_functions[i],pv),index);
        }
        pv = vector_next(pvl);
        loading();
    }
    end_loading();

    printf("Hash tables for vectors are ready !\n");
raise(SIGINT);

    // Open output_file for write
    FILE* output = fopen(output_file,"w");

    pList nearest_neighbors;         // lsh
    pList nearest_neighbors_brute;   // brute search
    pList range_search_list;         // range search
    struct timeval start, stop;
    double lsh_timer = 0;       // time for lsh to complete
    double true_timer = 0;      // time for brute force search to complete

    // query_file_curr -> stores name of current query_file.
    char* query_file_curr = malloc(sizeof(char)*(strlen(query_file)+1) );
    strcpy(query_file_curr,query_file);

    int check = 1;
    while(check){
        // Create Vector List with queries
        pList queries = create_list_file(query_file_curr,"Create querie's list from file progress : ");

        // check if queries list has the same number of dimensions with input list
        if(dimensions_of_list(queries) != dimensions_of_list(pvl)){
            printf("- Error! Query list from %s has different number of dimensions than input list !\n",query_file_curr);
            fclose(output);
            return 1;
        }
        else{
            // find nearest neighbors and range search
            vector_next_init(queries);  // initialize queries->temp = NULL
            pVector query_vector = vector_next(queries);    // return first vector of list "queries"

            init_loading("Find queries nearests neighbors :",size_of_list(queries));

            while( query_vector != NULL){
                // find approximate nearest neighbor via lsh and calculate time to complete
                gettimeofday(&start, NULL);
                nearest_neighbors = lsh_n_nearests(hash_tables,g_functions,table_size,L,query_vector,1);
                gettimeofday(&stop, NULL);
                lsh_timer = (double)(stop.tv_usec - start.tv_usec) / 1000000 + (double)(stop.tv_sec - start.tv_sec);

                // find approximate nearest neighbor via brute force and calculate time to complete
                gettimeofday(&start, NULL);
                nearest_neighbors_brute = vector_n_nearest(pvl,query_vector,1);
                gettimeofday(&stop, NULL);
                true_timer = (double)(stop.tv_usec - start.tv_usec) / 1000000 + (double)(stop.tv_sec - start.tv_sec);
            
                lsh_fprintf(output,nearest_neighbors,nearest_neighbors_brute,NULL,query_vector,lsh_timer,true_timer,0.0);


                // delete lists of vectors for vector query_vector
                delete_list_no_vectors(&nearest_neighbors);
                delete_list_no_vectors(&nearest_neighbors_brute);
                delete_list_no_vectors(&range_search_list);

                // take next vector from list
                query_vector = vector_next(queries);                // get next query
                loading();

            }
            end_loading();
        }

        // free queries list
        delete_list(&queries);

        // end loop
        // check = 0;
        int check_loop = 1;
        printf("Type a file name or exit.\n");
        while(check_loop){
            printf(">>> ");
            pData p0 = scan_full();
            if( data_getSize(p0) > 0  ){
                char* text = data_getWord(p0,0);
                if( strcmp(text,"exit")==0 ){ check_loop = 0; check = 0; }
                else{
                    query_file_curr = malloc(sizeof(char)*(strlen(text)+1));
                    strcpy(query_file_curr,text);
                    FILE* fp = fopen(query_file_curr,"r");
                    if( fp == NULL) { printf("%s doesn't exist !\n",query_file_curr); free(query_file_curr);}
                    else            { fclose(fp); check_loop=0; }
                }
            }
            free_data(&p0);
        }
    }

    // free memory
    for(int i=0; i<L; i++){
        delete_HT(&hash_tables[i]);
        delete_g(&g_functions[i]);
    }

    delete_list(&pvl);
    // free(output_file);

    // close files
    fclose(output);
    return 0;
}

pList lsh_n_nearests(pHT* hash_tables,g** g_functions,long int table_size,int L,pVector q,int N){
    if(q == NULL || hash_tables == NULL || g_functions == NULL){ return NULL; }
    pList best_N = NULL;
    long int ID ,index;
    pList bucket;
    for(int l=0; l<L; l++){
        ID = hash_ID(g_functions[l],q);
        index = mod(ID,table_size);     // g(p) = ID(p) mod table_size
        bucket = HT_bucket(hash_tables[l],index);
        best_N = vector_n_nearest_ID(bucket,best_N,q,ID,N);
    }
    return best_N;
}

pList lsh_range_search(pHT* hash_tables,g** g_functions,long int table_size,int L,pVector q,double R){
    if(hash_tables == NULL || g_functions == NULL){ return NULL; }
    pList range_search_list = NULL;
    long int ID ,index;
    pList bucket;
    for(int l=0; l<L; l++){
        ID = hash_ID(g_functions[l],q);
        index = mod(ID,table_size);     // g(p) = ID(p) mod table_size
        bucket = HT_bucket(hash_tables[l],index);
        int num;
        range_search_list = vector_range_search(bucket,range_search_list,q,INFINITY,R,&num);
    }
    return range_search_list;
}

void lsh_fprintf(FILE* output,pList nn_list,pList nn_brute_list,pList rs_list,pVector q,double lsh_timer,double true_timer,double R){
    if(output == NULL || nn_list == NULL || nn_brute_list == NULL || q == NULL){
        printf("- Error! output,nn_list, nn_brute_list or q are NULL !\n");
        return;
    }
    // list first -> N neighbor  , so with a reversed list, first-> 1 neighbor 
    reverse_vectorList(nn_list);
    reverse_vectorList(nn_brute_list);

    // fprintfs for output_file
    vector_next_init(nn_list);
    vector_next_init(nn_brute_list);
    pVector tmp1 = vector_next(nn_list);
    pVector tmp2 = vector_next(nn_brute_list);
    fprintf(output,"Query: %s\n",vector_id(q));
    
    // write Nearests neighbors and distances
    int neighbor_number = 1 ;

    while( tmp1 != NULL && tmp2 != NULL){
        fprintf(output,"Nearest neighbor-%d: %s\n",neighbor_number,vector_id(tmp1));
        fprintf(output,"distanceLSH: %lf\n",sqrt( dist_L2(q,tmp1) ) );
        fprintf(output,"distanceTrue: %lf\n",sqrt( dist_L2(q,tmp2) ) );
        tmp1 = vector_next(nn_list);
        tmp2 = vector_next(nn_brute_list);
        neighbor_number++;
    }

    // write times 
    fprintf(output,"\ntLSH: %lf\n",lsh_timer);
    fprintf(output,"tTrue: %lf\n\n",true_timer);

    if(rs_list != NULL){
        // write vector's id of range search list
        vector_next_init(rs_list);
        fprintf(output,"%lf-near neighbors:\n",R);
        while( (tmp1 = vector_next(rs_list)) != NULL){
            fprintf(output,"%s\n",vector_id(tmp1));
        }
    }
    fprintf(output,"\n");
}