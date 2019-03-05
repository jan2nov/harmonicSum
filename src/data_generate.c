#include <stdio.h>
#include <stdlib.h>
#include <time.h>

void generate_data(float *data, int size){
	for(int i = 0; i < size; i++){
		for(int j = 0; j < size; j++){
			data[i*size + j] = (float)(i/10.0) + 0.1;
		}
	}
			
}

void generate_data_random(float *data, int size){
	
	srand( time(NULL) );
	
        for(int i = 0; i < size; i++)
                for(int j = 0; j < size; j++)
                        data[i*size + j] = rand() / (float)(RAND_MAX); 
}


int get_data_file(float *data, int z, int nsamples){

	FILE *fptr;
	fptr = fopen("acc_0.000000.dat","r");

	if(fptr == NULL){
		printf("Error!! Cannot open file \n" );
		exit(0);
	}
	else{
		for(int i = 0; i < z*nsamples; i++){
			fscanf(fptr,"%f",&data[i]);
//		        printf("%f\n",data[i]);      
		}
	}
	

	fclose(fptr);
	return 0;
}
