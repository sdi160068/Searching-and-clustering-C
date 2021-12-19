#include "grid.h"
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
#include "timer.h"
#include "vector.h"

#include <signal.h>         // for breakpoints
#define SIGINT 2

typedef struct lsh_obj{
    pHT* hash_tables;
    g** g_hash;
    int size;
    int L;
}lsh_obj;

pLsh init_lsh(int size,int L, int k, pList pvl){
    if(size < 0 || pvl == NULL){ printf("Error (init_lsh )! size < 0 or list == NULL !\n"); return NULL;}
    pLsh plsh0 = malloc(sizeof(lsh_obj));
    plsh0->hash_tables = malloc(sizeof(pHT)*L);
    plsh0->g_hash = malloc(sizeof(g*)*L);
    plsh0->size = size;
    plsh0->L = L;

    // initialize hash tables anf g hash functions
    for(int i=0; i<L; i++){
        plsh0->hash_tables[i] = init_HT(size,dimensions_of_list(pvl));
        plsh0->g_hash[i] = init_g(k,dimensions_of_list(pvl));
    }
    // put vectors to hash tables
    for(pVector pv = vector_next(pvl); pv != NULL; pv =  vector_next(pvl)){
        for(int i=0; i<L; i++){
            long int index = hash_g(plsh0->g_hash[i],pv,size);
            new_HT_element(plsh0->hash_tables[i],pv,hash_ID(plsh0->g_hash[i],pv),index);
        }
        pv = vector_next(pvl);
        loading();
    }
    end_loading();

    printf("Hash tables for vectors are ready !\n");

    return plsh0;
}

pLsh init_by_grids_lsh(int size,int L, int k,pGrid* grids, pList pvl){
    if(size < 0 || pvl == NULL || grids == NULL){ printf("Error (init_by_grids_lsh )! size < 0 or list == NULL or grids == NULL!\n"); return NULL;}
    pLsh plsh0 = malloc(sizeof(lsh_obj));
    plsh0->hash_tables = malloc(sizeof(pHT)*L);
    plsh0->g_hash = malloc(sizeof(g*)*L);
    plsh0->size = size;
    plsh0->L = L;

    // initialize hash tables anf g hash functions
    for(int i=0; i<L; i++){
        plsh0->hash_tables[i] = init_HT(size,dimensions_of_list(pvl));
        plsh0->g_hash[i] = init_g(k,dimensions_of_list(pvl));
    }

    // declare pudding number
    double pudding_number = 10000000.0;

    // put vectors to hash tables
    pVector curve;
    for(pVector pv = vector_next(pvl); pv != NULL; pv =  vector_next(pvl)){
        for(int i=0; i<L; i++){
            curve = lsh_key_grid(pv,grids[i],pudding_number);
            long int index = hash_g(plsh0->g_hash[i],curve,size);
            new_HT_element(plsh0->hash_tables[i],pv,hash_ID(plsh0->g_hash[i],curve),index);
        }
        pv = vector_next(pvl);
        loading();
    }
    end_loading();

    printf("Hash tables for vectors are ready !\n");

    return plsh0;
}

int delete_lsh(pLsh * plsh0){
    if( *plsh0 == NULL){ printf("Warning (delete_lsh )! plsh0 is NULL!\n"); return 1;}
    for(int i=0; i< (*plsh0)->L; i++){
        delete_HT(&(*plsh0)->hash_tables[i]);
        delete_g(&(*plsh0)->g_hash[i]);
    }
    free((*plsh0)->hash_tables);
    free((*plsh0)->g_hash);
    free(*plsh0);
    *plsh0 = NULL;
    return 0;
}

int delete_frechet_lsh(pLsh * plsh0){
    if( *plsh0 == NULL){ printf("Warning (delete_lsh )! plsh0 is NULL!\n"); return 1;}
    for(int i=0; i< (*plsh0)->L; i++){
        delete_full_HT(&(*plsh0)->hash_tables[i]);
        delete_g(&(*plsh0)->g_hash[i]);
    }
    free((*plsh0)->hash_tables);
    free((*plsh0)->g_hash);
    free(*plsh0);
    *plsh0 = NULL;
    return 0;
}

int lsh(int k, int L, char* input_file ,char* output_file, char* query_file){
    // List with all vectors
    pList pvl = create_list_file(input_file,"Create Vector list from file progress : ");
    printf("List of vectors is ready !\n");

    // size of hash tables
    long int table_size = size_of_list(pvl)/16 ;  // table_size = n/16

    pLsh plsh0 = init_lsh(table_size,L,k,pvl);

    // Open output_file for write
    FILE* output = fopen(output_file,"w");

    pList nearest_neighbors;         // lsh
    pList nearest_neighbors_brute;   // brute search
    double lsh_timer = 0;       // time for lsh to complete
    double true_timer = 0;      // time for brute force search to complete
    double lsh_timer_average = 0;
    double true_timer_average = 0;

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
            init_loading("Find queries nearests neighbors :",size_of_list(queries));

            // find nearest neighbors
            vector_next_init(queries);  // initialize queries->temp = NULL

            for(pVector query_vector = vector_next(queries); query_vector != NULL; query_vector = vector_next(queries)){
                // find approximate nearest neighbor via lsh and calculate time to complete
                start_timer();
                nearest_neighbors = lsh_n_nearests(plsh0, query_vector,1);
                lsh_timer = stop_timer();
                lsh_timer_average += lsh_timer;

                // find approximate nearest neighbor via brute force and calculate time to complete
                start_timer();
                nearest_neighbors_brute = vector_n_nearest(pvl,query_vector,1);
                true_timer = stop_timer();
                true_timer_average += true_timer;
            
                // write results for each query to output file
                lsh_fprintf(output,nearest_neighbors,nearest_neighbors_brute,query_vector,lsh_timer,true_timer);

                // delete lists of vectors for vector query_vector
                delete_list_no_vectors(&nearest_neighbors);
                delete_list_no_vectors(&nearest_neighbors_brute);

                loading();
            }
            end_loading();

            // calculate average time to complete lsh and brute force search and write the results to output file
            lsh_timer_average = lsh_timer_average/size_of_list(queries);
            true_timer_average = true_timer_average/size_of_list(queries);
            fprintf(output,"tApproximateAverage: %lf\n",lsh_timer_average);
            fprintf(output,"tTrueAverage: %lf\n",true_timer_average);
        }

        // free queries list
        delete_list(&queries);
        // free query_file
        free(query_file_curr);

        // end loop
        // check = 0;
        int check_loop = 1;
        printf("Type a file name or exit.\n");
        query_file_curr = loop_new_file(&check_loop);
        if(query_file_curr == NULL){ check = 0;}
    }

    delete_list(&pvl);
    delete_lsh(&plsh0);

    // close files
    fclose(output);
    return 0;
}

int lsh_discrete_frechet(int k, int L, double delta,char* input_file ,char* output_file, char* query_file){
    // List with all vectors
    pList pvl = create_list_file(input_file,"Create Vector list from file progress : ");
    printf("List of vectors is ready !\n");

    // size of hash tables
    long int table_size = size_of_list(pvl)/16 ;  // table_size = n/16

    pGrid* grids = init_grids_table(delta, L, dimensions_of_list(pvl) );
    pLsh plsh0 = init_by_grids_lsh(table_size,L,k,grids,pvl);

    // Open output_file for write
    FILE* output = fopen(output_file,"w");

    pList nearest_neighbors;         // lsh
    pList nearest_neighbors_brute;   // brute search
    double lsh_timer = 0;       // time for lsh to complete
    double true_timer = 0;      // time for brute force search to complete
    double lsh_timer_average = 0;
    double true_timer_average = 0;

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
            init_loading("Find queries nearests neighbors :",size_of_list(queries));

            // find nearest neighbors
            vector_next_init(queries);  // initialize queries->temp = NULL

            for(pVector query_vector = vector_next(queries); query_vector != NULL; query_vector = vector_next(queries)){
                // find approximate nearest neighbor via lsh and calculate time to complete
                start_timer();
                nearest_neighbors = lsh_n_nearests(plsh0, query_vector,1);
                lsh_timer = stop_timer();
                lsh_timer_average += lsh_timer;

                // find approximate nearest neighbor via brute force and calculate time to complete
                start_timer();
                nearest_neighbors_brute = vector_n_nearest(pvl,query_vector,1);
                true_timer = stop_timer();
                true_timer_average += true_timer;
            
                // write results for each query to output file
                lsh_fprintf(output,nearest_neighbors,nearest_neighbors_brute,query_vector,lsh_timer,true_timer);

                // delete lists of vectors for vector query_vector
                delete_list_no_vectors(&nearest_neighbors);
                delete_list_no_vectors(&nearest_neighbors_brute);

                loading();
            }
            end_loading();

            // calculate average time to complete lsh and brute force search and write the results to output file
            lsh_timer_average = lsh_timer_average/size_of_list(queries);
            true_timer_average = true_timer_average/size_of_list(queries);
            fprintf(output,"tApproximateAverage: %lf\n",lsh_timer_average);
            fprintf(output,"tTrueAverage: %lf\n",true_timer_average);
        }

        // free queries list
        delete_list(&queries);
        // free query_file
        free(query_file_curr);

        // end loop
        // check = 0;
        int check_loop = 1;
        printf("Type a file name or exit.\n");
        query_file_curr = loop_new_file(&check_loop);
        if(query_file_curr == NULL){ check = 0;}
    }

    delete_list(&pvl);
    delete_frechet_lsh(&plsh0);

    // close files
    fclose(output);
    return 0;
}

pList lsh_n_nearests(pLsh plsh0,pVector q,int N){
    if(q == NULL || plsh0 == NULL){ return NULL; }
    pList best_N = NULL;
    long int ID ,index;
    pList bucket;
    for(int l=0; l<plsh0->L; l++){
        ID = hash_ID(plsh0->g_hash[l],q);
        index = mod(ID,plsh0->size);     // g(p) = ID(p) mod table_size
        bucket = HT_bucket(plsh0->hash_tables[l],index);
        best_N = vector_n_nearest_ID(bucket,best_N,q,ID,N);
    }
    return best_N;
}

void lsh_fprintf(FILE* output,pList nn_list,pList nn_brute_list,pVector q,double lsh_timer,double true_timer){
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
    fprintf(output,"Algorithm: LSH_Vector\n");
    
    // write Nearests neighbors and distances
    int neighbor_number = 1 ;

    while( tmp1 != NULL && tmp2 != NULL){
        fprintf(output,"Approximate Nearest neighbor: %s\n",vector_id(tmp1));
        fprintf(output,"True Nearest neighbor: %s\n",vector_id(tmp2));
        fprintf(output,"distanceApproximate: %lf\n",sqrt( dist_L2(q,tmp1) ) );
        fprintf(output,"distanceTrue: %lf\n",sqrt( dist_L2(q,tmp2) ) );
        tmp1 = vector_next(nn_list);
        tmp2 = vector_next(nn_brute_list);
        neighbor_number++;
    }

    // write times 
    fprintf(output,"\ntApproximate: %lf\n",lsh_timer);
    fprintf(output,"tTrue: %lf\n\n",true_timer);
    fprintf(output,"\n");
}

// pList lsh_clusters_range_search(pVector* centroids,long int centroid_id, pHT* hash_tables,g** g_hash,long int table_size,int L,double R){
//     if(hash_tables == NULL || g_hash == NULL || centroids == NULL){ return NULL; }
//     pList range_search_list = NULL;
//     long int ID ,index;
//     pList bucket;
//     for(int l=0; l<L; l++){
//         ID = hash_ID(g_hash[l],centroids);
//         index = mod(ID,table_size);     // g(p) = ID(p) mod table_size
//         bucket = HT_bucket(hash_tables[l],index);
//         int num;
//         // range_search_list = vector_range_search(bucket,range_search_list,q,INFINITY,R,&num);
//         // range_search_list = vector_range_search_cluster(range_search_list,)
//     }
//     return range_search_list;
// }