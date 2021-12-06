#include "cube.h"
#include "hash.h"
#include "HT.h"
#include "loading.h"
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/time.h>
#include <time.h>
#include "vector.h"
#include "vector_list.h"

long int hamming_distance(long int x,long int y);
pList cube_n_nearests(pHT hash_table,f* f_function,pVector q,int probes,int M,int N);
void cube_fprintf(FILE* output,pList nn_list,pList nn_brute_list,pVector q,double cube_timer,double true_timer);

// int main(int argc, char* argv[]){
int cube(int k, int M, int probes,char* input_file ,char* output_file, char* query_file){
    // create list of vectors
    pList pvl = create_list_file(input_file,"Create Vector list from file progress : ");
    printf("List of vectors is ready !\n");

    // create hash table
    pHT hash_table = init_HT(2<<k,dimensions_of_list(pvl));
    f* f_function = init_f(k,dimensions_of_list(pvl));


    init_loading("Hash table built progress : ", size_of_list(pvl));    // loading : progress for any loop with limit size
    loading();
    vector_next_init(pvl);
    pVector p0 = vector_next(pvl);
    while( p0 != NULL){
        new_HT_element(hash_table,p0,-1,hash_f(f_function,p0));
        p0 = vector_next(pvl);
        loading();
    }
    end_loading();
    printf("Hash table for vectors is ready !\n");

    // Open output_file for write
    FILE* output = fopen(output_file,"w");

    pList nearest_neighbors;         // cube
    pList nearest_neighbors_brute;   // brute search
    struct timeval start, stop;
    double cube_timer = 0;       // time for cube to complete
    double true_timer = 0;      // time for brute force search to complete
    double cube_timer_average = 0;
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
            // find nearest neighbors and range search
            vector_next_init(queries);  // initialize queries->temp = NULL
            pVector query_vector = vector_next(queries);    // return first vector of list "queries"

            init_loading("Find queries nearests neighbors :",size_of_list(queries));
            while( query_vector != NULL){
                // find nearests neighbors via cube and calculate time to complete
                gettimeofday(&start, NULL);
                nearest_neighbors = cube_n_nearests(hash_table,f_function,query_vector,probes,M,1);
                gettimeofday(&stop, NULL);
                cube_timer = (double)(stop.tv_usec - start.tv_usec) / 1000000 + (double)(stop.tv_sec - start.tv_sec);
                cube_timer_average += cube_timer;

                // find nearests neighbors via brute force and calculate time to complete
                gettimeofday(&start, NULL);
                nearest_neighbors_brute = vector_n_nearest(pvl,query_vector,1);
                gettimeofday(&stop, NULL);
                true_timer = (double)(stop.tv_usec - start.tv_usec) / 1000000 + (double)(stop.tv_sec - start.tv_sec);
                true_timer_average += true_timer;
            
                // write results for each query to output file
                cube_fprintf(output,nearest_neighbors,nearest_neighbors_brute,query_vector,cube_timer,true_timer);


                // delete lists of vectors for vector query_vector
                delete_list_no_vectors(&nearest_neighbors);
                delete_list_no_vectors(&nearest_neighbors_brute);

                // take next vector from list
                query_vector = vector_next(queries);                // get next query
                loading();

            }
            end_loading();

            // calculate average time to complete lsh and brute force search and write the results to output file
            cube_timer_average = cube_timer_average/size_of_list(queries);
            true_timer_average = true_timer_average/size_of_list(queries);
            fprintf(output,"tApproximateAverage: %lf\n",cube_timer_average);
            fprintf(output,"tTrueAverage: %lf\n",true_timer_average);
        }

        // free queries list
        delete_list(&queries);
        // free query_file_curr
        free(query_file_curr);

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
    delete_HT(&hash_table);
    delete_f(&f_function);
    delete_list(&pvl);

    //close output
    fclose(output);
    return 0;
}

pList cube_n_nearests(pHT hash_table,f* f_function,pVector q,int probes,int M,int N){
    if(hash_table == NULL || f_function == NULL || q == NULL){ return NULL;}
    long int index = hash_f(f_function,q);
    int new_M = M;
    long int new_index = index;
    int num_of_retrieved_items = 0;
    long int i=0;
    int ham_dist = 1;
    pList bucket, best_N = NULL;
    for(int j=0; j<probes && new_M != 0; j++){
        bucket = HT_bucket(hash_table,new_index);
        best_N = vector_n_nearest_max(bucket,best_N,q,new_M,N,&num_of_retrieved_items);
        for( ; i < size_HT(hash_table); i++){
            if(hamming_distance(index,i) == ham_dist){
                new_index = i;
                i++;
                break;
            }
        }
        if(i >= size_HT(hash_table)){
            i=0;
            ham_dist++;
        }
        new_M -= num_of_retrieved_items;
    }
    return best_N;
}

void cube_fprintf(FILE* output,pList nn_list,pList nn_brute_list,pVector q,double cube_timer,double true_timer){
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
    fprintf(output,"Algorithm: Hypercube\n");
    
    // write Nearests neighbors and distances
    int neighbor_number = 1 ;

    while( tmp1 != NULL && tmp2 != NULL){
        fprintf(output,"Approximate Nearest neighbor: %s\n",vector_id(tmp1));
        fprintf(output,"True Nearest neighbor: %s\n",vector_id(tmp2));
        fprintf(output,"distanceApproximate: %lf\n",sqrt( distance('2',q,tmp1) ) );
        fprintf(output,"distanceTrue: %lf\n",sqrt( distance('2',q,tmp2) ) );
        tmp1 = vector_next(nn_list);
        tmp2 = vector_next(nn_brute_list);
        neighbor_number++;
    }

    // write times 
    fprintf(output,"\ntApproximate: %lf\n",cube_timer);
    fprintf(output,"tTrue: %lf\n\n",true_timer);
}

long int hamming_distance(long int x,long int y){
    long int xor = x^y;
    long int distance = 0;
    while( xor != 0){
        if( (xor&1) == 1)
            distance++;
        xor = xor >> 1;
    }
    return distance;
}

//https://algorithms.tutorialhorizon.com/hamming-distance-between-two-given-integers/