#include <assert.h>
#include "cluster.h"
#include "data.h"
#include "hash.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/time.h>
#include "vector_list.h"

int main(int argc, char* argv[]){
    // C h e c k   f o r   p a r a m e t e r s
    
    int complete      = 0;
    int max_times     = 10;
    int update        = 1;  // update method
    int assignment    = 1;  // assignment method

    // Parameters must have values
    char* input_file  = NULL;
    char* conf_file   = NULL;   // configuration file
    char* output_file = NULL;

    int i=0;
    while(i < argc){
        if(*argv[i] == '-'){
            if(strcmp(argv[i],"-complete")==0){       // complete
                complete = 1;
            }
            else if(strcmp(argv[i],"-update")==0){       // complete
                if(strcmp(argv[i+1],"Frechet") == 0)
                    update = 2;
                else if(strcmp(argv[i+1],"Vector") == 0)
                    update = 1;
                else
                    update = 0;
            }
            else if(strcmp(argv[i],"-assignment")==0){       // complete
                if(strcmp(argv[i+1],"Classic") == 0)
                    assignment = 1;
                else if(strcmp(argv[i+1],"LSH_Frechet") == 0)
                    assignment = 2;
                else if(strcmp(argv[i+1],"LSH") == 0)
                    assignment = 3;
                else if(strcmp(argv[i+1],"Hypercube") == 0)
                    assignment = 4;
                else
                    assignment = 0;
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
            else if(argv[i][1] == 'c' && (i+1) < argc){       // configuration file
                conf_file = malloc(sizeof(char)*(strlen(argv[i+1])+1));
                strcpy(conf_file,argv[i+1]);
                i++;
            }
            else if(argv[i][1] == 'o' && (i+1) < argc){       // output file
                output_file = malloc(sizeof(char)*(strlen(argv[i+1])+1));
                strcpy(output_file,argv[i+1]);
                i++;
            }
            else if(argv[i][1] == 't' && (i+1) < argc){           // times of iterations for any method
                max_times = atoi(argv[i+1]);
                i++;
            }
        }
        i++;
    }
    FILE* file;
    int check =0;
    if(input_file  == NULL){ printf(" - Error! You must give an input   file\n"); check = 1; }
    else if(!(file = fopen(input_file,"r"))){ printf(" - Error! %s doesn't exist\n",input_file); check = 1; }
    else { fclose(file); }
    if(output_file == NULL){ printf(" - Error! You must give an output  file\n"); check = 1; }
    if(conf_file  == NULL){  printf(" - Error! You must give a  configuration file\n"); check = 1; }
    else if(!(file = fopen(conf_file,"r"))){ printf(" - Error! %s doesn't exist\n",conf_file); check = 1; }
    else { fclose(file); }

    if(update == 0){        printf(" - Error! You must give 'Frechet' or 'Vector' as update method\n"); check = 1; }
    if(assignment == 0){    printf(" - Error! You must give 'Classic' or 'LSH' or 'Hypercube' or 'LSH_Frechet' as assignment method\n"); check = 1; }


    if(check){
        free(input_file);
        free(output_file);
        free(conf_file);
        return 1;           // if there are any errors, return 1
    }
    if(max_times < 0){
        max_times = 1;
    }

    // C h e c k   f o r   p a r a m e t e r s


    int K,L_lsh,k_lsh,k_cube,M_cube,p_cube;
    cluster_get_conf(conf_file,&K,&L_lsh,&k_lsh,&k_cube,&M_cube,&p_cube);
    printf("+--------------------------------------------------\n");
    printf("| K             %d\n",K);
    printf("| L             %d\n",L_lsh);
    printf("| k_lsh         %d\n",k_lsh);
    printf("| k_cube        %d\n",k_cube);
    printf("| M             %d\n",M_cube);
    printf("| probes        %d\n",p_cube);
    printf("+--------------------------------------------------\n");

    // create list of vectors
    pList pvl = create_list_file(input_file,"Create Vector list from file progress : ");
    printf("List of vectors is ready !\n");

    // Generate via k-means++ a list of Centroids
    pList centroids = k_means_pp(pvl,K,L2);

    // print_list_full(centroids);
    int choise = assignment*10+update;
    switch (choise){
        case 11:
            assert(!cluster_Lloyd_method(pvl,centroids,max_times,complete,output_file,L2));    // fix centroids with Lloyd for at least max_times
            break;
        case 31:
            assert(!cluster_LSH_vector_method(pvl,centroids,L_lsh,k_lsh,complete,output_file)); // fix centroids with LSH of vectors for at least max_times
            break;
    }

    // print_list_full(centroids);

    // free memory
    delete_list(&centroids);
    delete_list(&pvl);

    // free file_names
    free(input_file);
    free(output_file);
    free(conf_file);
    printf("End of program\n");
    return 0;
}