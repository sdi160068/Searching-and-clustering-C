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
    
    int complete = 0;
    int method_number = 1;  // classic method
    int max_times = 10;

    // Parameters must have values
    char* method      = NULL;
    char* input_file  = NULL;
    char* conf_file   = NULL;   // configuration file
    char* output_file = NULL;

    int i=0;
    while(i < argc){
        if(*argv[i] == '-'){
            if(strcmp(argv[i],"-complete")==0){       // complete
                complete = 1;
            }
            else if(argv[i][1] == 'w' && (i+1) < argc){       // w
                change_w(atoi(argv[i+1]));
                i++;
            }
            else if(argv[i][1] == 'm' && (i+1) < argc){       // method
                method = malloc(sizeof(char)*(strlen(argv[i+1])+1));
                strcpy(method,argv[i+1]);
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

    // check method
    if(method != NULL){
        if(!strcmp(method,"Classic")){ method_number = 1;}
        else if(!strcmp(method,"LSH")){ method_number = 2;}
        else if(!strcmp(method,"Hypercube")){ method_number = 3;}
        else { printf("- There is no method %s. Starting Classic\n",method);}
        free(method);
    }

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
    printf("K %d\n",K);
    printf("L %d\n",L_lsh);
    printf("k_lsh %d\n",k_lsh);
    printf("k_cube %d\n",k_cube);
    printf("M %d\n",M_cube);
    printf("probes %d\n",p_cube);

    // create list of vectors
    pList pvl = create_list_file(input_file,"Create Vector list from file progress : ");
    printf("List of vectors is ready !\n");

    // Generate via k-means++ a list of Centroids
    pList centroids = k_means_pp(pvl,K);

    // print_list_full(centroids);

    switch (method_number){
        case 1:
            assert(!cluster_Lloyd_method(pvl,centroids,max_times,complete,output_file));    // fix centroids with Lloyd for at least max_times
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