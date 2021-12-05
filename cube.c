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
pList cube_range_search(pHT hash_table,f* f_function,pVector q,int probes,int M,double R);
void cube_fprintf(FILE* output,pList nn_list,pList nn_brute_list,pList rs_list,pVector q,double cube_timer,double true_timer,double R);

int main(int argc, char* argv[]){
    // C h e c k   f o r   p a r a m e t e r s
    
    // Parameters with default values
    int k = 14;     // d'
    int M = 10;
    int probes = 2;
    int N = 1;
    double R = 10000.0;

    // Parameters must have values
    char* input_file  = NULL;
    char* query_file  = NULL;
    char* output_file = NULL;

    int i=0;
    while(i < argc){
        if(*argv[i] == '-'){
            if(argv[i][1] == 'k' && (i+1) < argc){           // k
                k = atoi(argv[i+1]);
                i++;
            }
            else if(strcmp(argv[i],"-probes")==0 && (i+1) < argc){       // probes
                probes = atoi(argv[i+1]);
                i++;
            }
            else if(argv[i][1] == 'M' && (i+1) < argc){       // M
                M = atoi(argv[i+1]);
                i++;
            }
            else if(argv[i][1] == 'N' && (i+1) < argc){       // N
                N = atoi(argv[i+1]);
                i++;
            }
            else if(argv[i][1] == 'R' && (i+1) < argc){       // R
                R = atof(argv[i+1]);
                i++;
            }
            else if(argv[i][1] == 'w' && (i+1) < argc){       // w
                change_w(atoi(argv[i+1]));
                i++;
            }
            else if(argv[i][1] == 'i' && (i+1) < argc){       // input file
                input_file = malloc(sizeof(char)*(strlen(argv[i+1])+1));
                strcpy(input_file,argv[i+1]);
                i++;
            }
            else if(argv[i][1] == 'q' && (i+1) < argc){       // query file
                query_file = malloc(sizeof(char)*(strlen(argv[i+1])+1));
                strcpy(query_file,argv[i+1]);
                i++;
            }
            else if(argv[i][1] == 'o' && (i+1) < argc){       // output file
                output_file = malloc(sizeof(char)*(strlen(argv[i+1])+1));
                strcpy(output_file,argv[i+1]);
                i++;
            }
        }
        i++;
    }
    FILE* file;
    int check =0;
    if(input_file  == NULL){ printf(" - Error! You must give an input  file\n"); check = 1; }
    else if(!(file = fopen(input_file,"r"))){ printf(" - Error! %s doesn't exist\n",input_file); check = 1; }
    else { fclose(file); }
    if(output_file == NULL){ printf(" - Error! You must give an output file\n"); check = 1; }
    if(query_file  == NULL){ printf(" - Error! You must give a  query  file\n"); check = 1; }
    else if(!(file = fopen(query_file,"r"))){ printf(" - Error! %s doesn't exist\n",query_file); check = 1; }
    else { fclose(file); }

    if(check){ 
        free(input_file);
        free(output_file);
        free(query_file);
        return 1;           // if there are any errors, return 1
    }

    // C h e c k   f o r   p a r a m e t e r s

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
    pList range_search_list;         // range search
    struct timeval start, stop;
    double cube_timer = 0;       // time for cube to complete
    double true_timer = 0;      // time for brute force search to complete

    check = 1;
    while(check){
        // Create Vector List with queries
        pList queries = create_list_file(query_file,"Create querie's list from file progress : ");

        // check if queries list has the same number of dimensions with input list
        if(dimensions_of_list(queries) != dimensions_of_list(pvl)){
            printf("- Error! Query list from %s has different number of dimensions than input list !\n",query_file);
        }
        else{
            // find nearest neighbors and range search
            vector_next_init(queries);  // initialize queries->temp = NULL
            pVector query_vector = vector_next(queries);    // return first vector of list "queries"

            init_loading("Find queries nearests neighbors :",size_of_list(queries));
            while( query_vector != NULL){
                // find nearests neighbors via cube and calculate time to complete
                gettimeofday(&start, NULL);
                nearest_neighbors = cube_n_nearests(hash_table,f_function,query_vector,probes,M,N);
                gettimeofday(&stop, NULL);
                cube_timer = (double)(stop.tv_usec - start.tv_usec) / 1000000 + (double)(stop.tv_sec - start.tv_sec);

                // find nearests neighbors via brute force and calculate time to complete
                gettimeofday(&start, NULL);
                nearest_neighbors_brute = vector_n_nearest(pvl,query_vector,N);
                gettimeofday(&stop, NULL);
                true_timer = (double)(stop.tv_usec - start.tv_usec) / 1000000 + (double)(stop.tv_sec - start.tv_sec);

                // range search
                range_search_list =  cube_range_search(hash_table,f_function,query_vector,probes,M,R*R);
            
                cube_fprintf(output,nearest_neighbors,nearest_neighbors_brute,range_search_list,query_vector,cube_timer,true_timer,R);


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
        // free query_file
        free(query_file);

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
                    query_file = malloc(sizeof(char)*(strlen(text)+1));
                    strcpy(query_file,text);
                    FILE* fp = fopen(query_file,"r");
                    if( fp == NULL) { printf("%s doesn't exist !\n",query_file); free(query_file);}
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

    // free files
    free(input_file);
    free(output_file);

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

pList cube_range_search(pHT hash_table,f* f_function,pVector q,int probes,int M,double R){
   if(hash_table == NULL || f_function == NULL || q == NULL){ return NULL;}
    long int index = hash_f(f_function,q);
    int new_M = M;
    long int new_index = index;
    int num_of_retrieved_items = 0;
    long int i=0;
    int ham_dist = 1;
    pList bucket, range_search_list = NULL;
    for(int j=0; j<probes && new_M != 0; j++){
        bucket = HT_bucket(hash_table,new_index);
        range_search_list = vector_range_search(bucket,range_search_list,q,(double) M,R,&num_of_retrieved_items);
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
    return range_search_list;
}

void cube_fprintf(FILE* output,pList nn_list,pList nn_brute_list,pList rs_list,pVector q,double cube_timer,double true_timer,double R){
    if(output == NULL || nn_list == NULL || nn_brute_list == NULL || rs_list == NULL || q == NULL){
        printf("- Error! output,nn_list, nn_brute_list,rs_list or q are NULL !\n");
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
        fprintf(output,"distancecube: %lf\n",sqrt( dist_L2(q,tmp1) ) );
        fprintf(output,"distanceTrue: %lf\n",sqrt( dist_L2(q,tmp2) ) );
        tmp1 = vector_next(nn_list);
        tmp2 = vector_next(nn_brute_list);
        neighbor_number++;
    }

    // write times 
    fprintf(output,"\ntcube: %lf\n",cube_timer);
    fprintf(output,"tTrue: %lf\n\n",true_timer);

    // write vector's id of range search list
    vector_next_init(rs_list);
    fprintf(output,"%lf-near neighbors:\n",R);
    while( (tmp1 = vector_next(rs_list)) != NULL){
        fprintf(output,"%s\n",vector_id(tmp1));
    }
    fprintf(output,"\n");
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