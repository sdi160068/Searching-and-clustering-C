#include <assert.h>
#include "cluster.h"
#include "hash.h"
#include "HT.h"
#include "loading.h"
#include "lsh.h"
#include "math_custom.h"
#include "random.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "timer.h"
#include "vector.h"

int detect_overflow_plus(double a, double b,double* result);

void cluster_get_conf(char* filename,int* K,int* L_lsh,int* k_lsh,int* k_cube,int* M_cube,int* p_cube){
// void cluster_get_conf(char* filename){
    if( filename == NULL){ return; }
    FILE* fp = fopen(filename,"r");
    if(fp == NULL){ printf("- Error! File '%s' doesn't exist\n",filename); }

    // defaults
    *L_lsh = 3;
    *k_lsh = 4;
    *k_cube = 3;
    *M_cube = 10;
    *p_cube = 2;
    *K = -1;        // for check ! If 

    pData p0 = return_data(fp);
    while( p0 != NULL){
        if( data_getSize(p0) > 1){
            char* text = data_getWord(p0,0);
            if(!strcmp(text,"number_of_clusters:"))
                *K = atoi(data_getWord(p0,1));
            else if(!strcmp(text,"number_of_vector_hash_tables:"))
                *L_lsh = atoi(data_getWord(p0,1));
            else if(!strcmp(text,"number_of_vector_hash_functions:"))
                *k_lsh = atoi(data_getWord(p0,1));
            else if(!strcmp(text,"max_number_M_hypercube:"))
                *M_cube = atoi(data_getWord(p0,1));
            else if(!strcmp(text,"number_of_hypercube_dimensions:"))
                *k_cube = atoi(data_getWord(p0,1));
            else if(!strcmp(text,"number_of_probes:"))
                *p_cube = atoi(data_getWord(p0,1));
        }

        free_data(&p0);
        p0 = return_data(fp);
    }
    fclose(fp);

    // checks
    if(*K < 0){
        *K = 0;
        printf("- Error! In '%s' - K must be given and K > 0\n",filename);
    }

}

pList k_means_pp(pList pvl,int num_clusters, dist_type metric){
    // check for errors
    if( pvl == NULL){ printf("- Error (k_means_pp)! List of vectors is NULL\n"); return NULL;}
    else if( num_clusters <= 0){ printf("- Error (k_means_pp)! Number of clusters must be > 0\n"); return NULL;}
    else if( size_of_list(pvl) == 0){ printf("- Error (k_means_pp)! List of vectors is empty\n"); return NULL;}

    // initialize array of distances
    double distances[size_of_list(pvl)];
    for( int i=0; i< size_of_list(pvl); i++)
        distances[i] = INFINITY;

    // create list of centroids
    pList C_list = create_list(dimensions_of_list(pvl));     // initialize list of centroids

    // find random first centroid
    int index = random_int(0,size_of_list(pvl)-1);  // pick random index 

    pVector centroid = search_vector_by_index(pvl,index);   // find vector by index inside list

    centroid = vector_copy(centroid); 
    new_vector(C_list,centroid,-1);
    
    pVector p0;
    double sum;
    double dist;
    init_loading("k-means++ : generate Centroids : ",num_clusters);
    /* if number of clusters >= size of list -> make every vector of list a centroid 
    */
    if( size_of_list(pvl) <= num_clusters){     
        vector_next_init(pvl);
        for(pVector p0= vector_next(pvl); p0 != NULL;  p0= vector_next(pvl)){
            pVector p0_new = vector_copy(p0);
            new_vector_sorted(C_list,p0_new,-1,0.0);
        }
    }
    else{
        for(int cluster =1; cluster < num_clusters; cluster++){
            sum = 0.0;
            vector_next_init(pvl);
            int i=0;
            for(p0 = vector_next(pvl); p0 != NULL; p0 = vector_next(pvl)){
                dist = distance(metric,p0,centroid);
                if(distances[i] > dist){
                    distances[i] = dist;
                }
                sum += distances[i];
                i++;
            }
            sum = sum * random_int(0,RAND_MAX) / (RAND_MAX-1);
            
            vector_next_init(pvl);
            i=0;
            for(p0 = vector_next(pvl); p0 != NULL; p0 = vector_next(pvl)){
                sum -= distances[i];
                if( sum <= 0){
                    pVector p0_new = vector_copy(p0);
                    new_vector_sorted(C_list,p0_new,-1,distances[i]);
                    centroid = p0;
                    break;
                }
                i++;
            }
            loading();
        }
    }
    end_loading();
    return C_list;
}

int cluster_LSH_vector_method(pList pvl,pList centroids_list,int L,int k,int complete,char* output){
    if(pvl == NULL || centroids_list == NULL){ printf("- Error! List of data or/and list of centroids are NULL !\n"); return 1;}
    start_timer();     // start count the time !!

    // make centroids table
    pVector centroids[size_of_list(centroids_list)];
    vector_next_init(centroids_list);
    for(int i=0; i<size_of_list(centroids_list); i++){
        pVector p0 = vector_next(centroids_list);
        centroids[i] = p0;
    }
    double min_dist_centroids = INFINITY;
    for(int i=0; i<size_of_list(centroids_list); i++){
        for(int j=i+1; j< size_of_list(centroids_list); j++){
            min_dist_centroids = min(sqrt(dist_L2(centroids[i],centroids[j])), min_dist_centroids);
        }
    }

    // size of hash tables
    long int table_size = size_of_list(pvl)/16 ;  // table_size = n/16

    pLsh plsh0 = init_lsh(table_size,L,k,pvl);

    double acceptable = size_of_list(pvl)*0.05;     // accept chenges upper 0.05 %
    long int retrieved_items = 0;
    double changes = acceptable+1;                  // for the frist loop
    int round = 0;

    // list of clusters
    pList cluster_group[size_of_list(centroids_list)];
    for(int i=0; i< size_of_list(centroids_list); i++)
        cluster_group[i] = NULL;
    
    double R = min_dist_centroids/2.0 + 0.01;   // add 0.01 in case min_dist_centroids is 0

    int max_times = 3;
    while(changes > acceptable || max_times > 0){
        max_times--;
        printf("LSH_Vector round %d\n",round+1);
        
        init_loading("Range-search for each centroid : ",size_of_list(centroids_list));
        for(long int i=0; i< size_of_list(centroids_list); i++){
            cluster_group[i] =  lsh_clusters_range_search(centroids,i,plsh0,L,R,cluster_group[i]);
            loading();
        }
        end_loading();

        // changes
        changes = (get_retrieved_items_lsh(plsh0) - retrieved_items)/size_of_list(pvl);
        retrieved_items = get_retrieved_items_lsh(plsh0);

        init_loading("Calculate new centroids progress : ",size_of_list(pvl));
        for(long int i=0; i<size_of_list(centroids_list); i++){
            centroids[i] = mean_vector(cluster_group[i],centroids[i]);
            loading();
        }
        end_loading();
        init_loading("'Clean' for each centroid the cluster's list : ",size_of_list(centroids_list));
        for(long int i=0; i< size_of_list(centroids_list); i++){
            delete_node_by_ClusterId(cluster_group[i],i);
            loading();
        }
        end_loading();
        round++;    // next round
        R = 2*R;
    }
    printf("LSH made %d rounds !\n",round);
    

    double timer = stop_timer();

    init_loading("Calculate with classic way progress : ",size_of_list(pvl));
    vector_next_init(pvl);
    for( pVector p0 = vector_next_cluster(pvl,-1); p0 != NULL; p0 = vector_next_cluster(pvl,-1)){
        double dist = INFINITY;
        double new_dist;
        long int index=0;
        for(long int i=0; i<size_of_list(centroids_list); i++){
            new_dist = distance(L2,p0,centroids[i]);
            if(dist > new_dist){
                index = i;
                dist = new_dist;
            }
        }
        new_vector(cluster_group[index],p0,-1);
        loading();
    }
    end_loading();

    init_loading("Calculate new centroids progress : ",size_of_list(pvl));
    for(long int i=0; i<size_of_list(centroids_list); i++){
        centroids[i] = mean_vector(cluster_group[i],centroids[i]);
        loading();
    }
    end_loading();

    // write statistics in output
    FILE* fp = fopen(output,"w");
    fprintf(fp,"Algorithm: A3U1\n\n");
    int i=0;
    vector_next_init(centroids_list);
    for(pVector p0 = vector_next(centroids_list); p0 != NULL; p0 = vector_next(centroids_list)){
        fprintf(fp,"CLUSTER-%d  ",i+1);
        fprintf(fp,"{size: %ld, centroid: ",sizeof(cluster_group[i]));
        fprintf(fp,"[");
        for(int j=0; j<dimensions_of_list(centroids_list)-1; j++)
            fprintf(fp,"%lf, ",vector_coord(p0,j));
        fprintf(fp,"%lf",vector_coord(p0,dimensions_of_list(centroids_list)-1));
        fprintf(fp,"]}\n\n");
        i++;
    }
    fprintf(fp,"clustering_time: %lf\n",timer);

    double s_total = 0.0;
    double s_centroid;
    fprintf(fp,"Silhouette: [");
    for(long int index = 0; index < size_of_list(centroids_list); index++){
        s_centroid = 0.0;
        double tmp;
        vector_next_init(centroids_list);
        int i=0;
        for(pVector pi = vector_next(centroids_list); pi !=NULL ;  pi = vector_next(centroids_list)){
            tmp = silhouette(cluster_group,pi,index,centroids_list,L2);
            s_centroid += tmp;
            i++;
        }
        s_total += s_centroid;
        s_centroid = s_centroid/size_of_list(cluster_group[i]);
        fprintf(fp,"%1.3lf, ",s_centroid);

    }
    s_total = s_total/size_of_list(pvl);
    fprintf(fp,"%1.3lf]\n",s_total);

    if(complete){
        vector_next_init(centroids_list);
        i = 0;
        for(pVector p0 = vector_next(centroids_list); p0 != NULL; p0 = vector_next(centroids_list)){
            fprintf(fp,"CLUSTER-%d {centroid",i+1);
            vector_next_init(cluster_group[i]);
            for(pVector p1 = vector_next(cluster_group[i]); p1 != NULL; p1= vector_next( cluster_group[i]))
                fprintf(fp,", %s",vector_id(p1));
            fprintf(fp,"}\n\n");
            i++;
        }

    }
    // free clusters
    for(int i=0; i<size_of_list(centroids_list); i++)
        delete_list_no_vectors(&cluster_group[i]);
    // close file
    fclose(fp);

    return 0;
}

int cluster_Lloyd_method(pList pvl,pList centroids,int max_times,int complete,char* output,dist_type metric){
    if(pvl == NULL || centroids == NULL){ printf("- Error! List of data or/and list of centroids are NULL !\n"); return 1;}
    start_timer();     // start count the time !!

    long int num_centroids = size_of_list(centroids);
    int num_dimensions = dimensions_of_list(centroids);

    double coords[num_centroids][num_dimensions];   // coordinates for centroids
    
    long int num_items[num_centroids];

    long int cluster_index[size_of_list(pvl)];   // for every vector of pvl list to save cluster's index
    for(long int i=0; i<size_of_list(pvl); i++)
        cluster_index[i] = -1;

    int max_times_new;
    if(max_times < 1){ 
        max_times_new = 1;
    }
    else
        max_times_new = max_times;
    double acceptable = ((double) size_of_list(pvl))/1000.0;

    int changes = (int) (acceptable+1 );
    int round = 0;

    while(changes > acceptable && round < max_times_new){
        printf("Lloyd round %d/%d\n",round+1,max_times);
        changes = 0;
        for(int i=0; i<num_centroids; i++){
            for(int j=0; j<num_dimensions; j++)
                coords[i][j] = 0.0;
            num_items[i] = 0;
        }

        init_loading("Find for each point the nearest cluster progress : ",size_of_list(pvl));
        int vector_index = 0;
        vector_next_init(pvl);
        for(pVector p0 = vector_next(pvl); p0 != NULL; p0 = vector_next(pvl)){
            long int index = vector_nearest_number(centroids,p0,metric);
            if( cluster_index[vector_index] != index){
                changes++;
                cluster_index[vector_index] = index;
            }
            for(int i=0; i<num_dimensions; i++){
                double result;
                assert(detect_overflow_plus(coords[index][i],vector_coord(p0,i),&result));
                coords[index][i] = result;
            }
            num_items[index]++;
            vector_index++;
            loading();
        }
        end_loading();

        vector_next_init(centroids);
        int i=0;
        init_loading("Calculate new centroids progress : ",size_of_list(pvl));
        for(pVector p0 = vector_next(centroids); p0 != NULL; p0 = vector_next(centroids)){
            if(num_items[i] != 0 ){
                for(int j=0; j < num_dimensions; j++){
                    coords[i][j] = coords[i][j]/(double) num_items[i];
                }
                assert(!vector_update_coords(p0,coords[i]));
            }
            i++;
            loading();
        }
        end_loading();
        round++;    // next round
    }
    double timer = stop_timer();

    // write statistics in output
    FILE* fp = fopen(output,"w");
    fprintf(fp,"Algorithm: A1U");
    if(metric == L2)
        fprintf(fp,"1\n\n");
    else
        fprintf(fp,"2\n\n");
    int i=0;
    vector_next_init(centroids);
    for(pVector p0 = vector_next(centroids); p0 != NULL; p0 = vector_next(centroids)){
        fprintf(fp,"CLUSTER-%d ",i+1);
        fprintf(fp,"{size: %ld, centroid: ",num_items[i]);
        fprintf(fp,"- [");
        for(int j=0; j<num_dimensions-1; j++)
            fprintf(fp,"%lf, ",vector_coord(p0,j));
        fprintf(fp,"%lf",vector_coord(p0,num_dimensions-1));
        fprintf(fp,"]}\n\n");
        i++;
    }
    fprintf(fp,"clustering_time: %lf\n",timer);


    /*
    Lists of vectors for every cluster
    */
    pList* cluster_group = malloc(sizeof(pList)*num_centroids);
    for(int i=0; i<num_centroids; i++)
        cluster_group[i] = create_list(num_dimensions);
    
    int vector_index = 0;
    vector_next_init(pvl);
    for(pVector p0 = vector_next(pvl); p0 != NULL ; p0 = vector_next(pvl)){
        long int index = cluster_index[vector_index];
        new_vector(cluster_group[index],p0,-1);
        vector_index++;
    }

    double s_total = 0.0;
    double s_centroid;
    fprintf(fp,"Silhouette: [");
    for(long int index = 0; index < num_centroids; index++){
        s_centroid = 0.0;
        double tmp;
        vector_next_init(centroids);
        int i=0;
        for(pVector pi = vector_next(centroids); pi !=NULL ;  pi = vector_next(centroids)){
            tmp = silhouette(cluster_group,pi,index,centroids,metric);
            s_centroid += tmp;
            i++;
        }
        s_total += s_centroid;
        s_centroid = s_centroid/num_items[index];
        fprintf(fp,"%1.3lf, ",s_centroid);

    }
    s_total = s_total/size_of_list(pvl);
    fprintf(fp,"%1.3lf]\n",s_total);

    if(complete){
        vector_next_init(centroids);
        i = 0;
        for(pVector p0 = vector_next(centroids); p0 != NULL; p0 = vector_next(centroids)){
            fprintf(fp,"CLUSTER-%d {centroid",i+1);
            vector_next_init(cluster_group[i]);
            for(pVector p1 = vector_next(cluster_group[i]); p1 != NULL; p1= vector_next( cluster_group[i]))
                fprintf(fp,", %s",vector_id(p1));
            fprintf(fp,"}\n\n");
            i++;
        }

    }
    // free clusters
    for(int i=0; i<num_centroids; i++)
        delete_list_no_vectors(&cluster_group[i]);
    free(cluster_group);
    // close file
    fclose(fp);

    return 0;
}


int detect_overflow_plus(double a, double b,double* result){
    if( a >= 0 && b>=0){
        *result = a+b;

        if(*result < 0 ){
            printf("E R R O R !! overflow detected !!!\n");
            return 0;
        }
    }
    else if( a<=0 && b<=0){

        *result = a+b;
    
        if(*result > 0 ){
            printf("E R R O R !! overflow detected !!!\n");
            return 0;
        }
    }
    else if( (a<0 && b>0) || (a>0 && b<0) ){
        *result = a+b;
        printf("a*b <0  (._.) ? \n");
    }

    return 1;
}

int update_centroids_cluster(pVector* centroids,pList* cluster_group,long int size){
    if(centroids == NULL || cluster_group == NULL || size < 1){
        printf("Error (update_centroids_cluster)! centroids are NULL or cluster_group are NULL or size < 1 !\n");
        return 1;
    }
    for(int i=0; i<size; i++){
        assert(!update_centroid_vector(cluster_group[i],centroids[i],i));
    }
    return 0;
}


// https://rosettacode.org/wiki/K-means%2B%2B_clustering
