#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#include <omp.h>

//#define DEBUG

void generate_data(float *data, int size);
void generate_data_random(float *data, int size);
int get_data_file(float *data, int z, int nsamples);
void find_msd(float *data, float *msd, int nSamples, int nkern, int nHarm);

int harmonic(float *data, float *mHS, int timeSample, int nHarm, int depth, int shift, int nSamples, int nkern, float pHS){
	if(depth == nHarm){
		return(0);
	}
//	count[0] = count[0] + 1;
	int new_depth;
	int new_shift;
	
	int position = (depth + 1)*timeSample + shift;
	if ( position < nSamples*nkern ){
//		float HS = position; //pHS + data[position];
		float HS = pHS + data[position];
//		if (HS > mHS[depth + timeSample*nHarm]) mHS[depth + timeSample*nHarm] = HS;
		if (HS > mHS[depth]) mHS[depth] = HS;
//		count[0] = count[0] + 1;
//		printf("d:%d; %lf %d\n", depth, pHS, position);
//		printf("d:%d; %lf %lf %d\n", depth, pHS, HS, position);
		new_depth = depth + 1;
		new_shift = shift;
		//left 
		harmonic(data, mHS, timeSample, nHarm, new_depth, new_shift, nSamples, nkern, HS);
		new_depth = depth + 1;
		new_shift = shift + nSamples;
		harmonic(data, mHS, timeSample, nHarm, new_depth, new_shift, nSamples, nkern, HS); //2*nsamples
//		printf("\n");
		//right
		new_depth = depth + 1;
		new_shift = shift + 1;
		harmonic(data, mHS, timeSample, nHarm, new_depth, new_shift, nSamples, nkern, HS);
		new_depth = depth + 1;
		new_shift = shift + 1 + nSamples;
		harmonic(data, mHS, timeSample, nHarm, new_depth, new_shift, nSamples, nkern, HS);  // 2*nsamples
	}

	return(0);
}

float find_max(float *input, size_t size, size_t *max_position){
	float max=0;
        if(size>0){
                max = input[0]; *max_position = 0;
                for(size_t f=0; f<size; f++){
                        if( input[f]>max ) { max = input[f]; *max_position=f;}
                }
        }
        return(max);
}



int find_snr(float *input_data, float *msd, int t, int nHarm, int nSamples, int nkern, float *snr, int *snr_harmonic){

        float *mHS;
        mHS=(float *)malloc(sizeof(float)*nHarm);

        for (int f = 0; f < nHarm; f++) mHS[f] = 0.0f;

        harmonic(input_data, mHS, t, nHarm, 0, 0, nSamples, nkern, 0);

	float temp_SNR = 0.0f;
	size_t temp_SNR_pos = 0;
//        for (int f = 0; f < nHarm; f++) printf("%d %d %lf %lf %lf %lf\n", t, f, mHS[f], msd[2*f], msd[f*2 + 1], (mHS[f]-msd[f*2])/msd[f*2+1]);
        for (int f = 0; f < nHarm; f++) mHS[f] = (mHS[f]-msd[f*2])/msd[f*2 + 1];
		
	temp_SNR = find_max(mHS, nHarm, &temp_SNR_pos);
	*snr = temp_SNR;
	*snr_harmonic = temp_SNR_pos;
//	printf("Max is: %lf %d\n", temp_SNR, (int)temp_SNR_pos);

        free(mHS);
        return 0;
}


int main(int argc, char* argv[]){

	char *pEnd;
	int nTimeSamples;
	int nHarmonics;
	int nkern;
//	int *count; 
	float *input_data, *MSD, *SNR;
	int *SNR_width;
//	count = (int *)malloc(sizeof(int));
//	count[0] = 0;

	if (argc < 4) {
		printf("Argument error!\n");
		printf("1) nTimesamples\n");
		printf("2) nkern\n");
		printf("3) nHarmonics\n");
		return(1);
	}

	if (argc>=4) {
		nTimeSamples = strtol(argv[1],&pEnd,10);
		nkern = strtol(argv[2],&pEnd,10);
		nHarmonics = strtol(argv[3],&pEnd,10);
	}		

	printf("\tSelected number of Timesamples: %d\n", nTimeSamples);
	printf("\tSelected # harmonic: %d\n\n", nHarmonics);

	size_t input_size = ( (size_t)nTimeSamples*(size_t)nkern );
	size_t MSD_size = ( (size_t)nHarmonics*2 );
	size_t SNR_size = ( (size_t)nTimeSamples*nkern );
	input_data = (float *)malloc(sizeof(float)*input_size);
	MSD = (float *)malloc(sizeof(float)*MSD_size);
	SNR = (float *)malloc(sizeof(float)*SNR_size);
	SNR_width = (int *)malloc(sizeof(int)*SNR_size);

	double time_start, execution_time;

	//generate data	
	time_start = omp_get_wtime();
//	generate_data(input_data,nTimeSamples);
//	generate_data_random(input_data,nTimeSamples);
	get_data_file(input_data, nkern, nTimeSamples);
	execution_time = omp_get_wtime() - time_start;
	printf("Data generated: %f s;\n", execution_time);

	//compute msd for each harmonics layer

#ifdef DEBUG
	printf("\nInput data matrix:\n");
	for(int i = 0; i < nkern; i++){
		for(int j = 0; j < nTimeSamples; j++)
			printf("%lf ", input_data[i*nTimeSamples + j]);
		printf("\n");
	}
#endif

	time_start = omp_get_wtime();
	find_msd(input_data, MSD, nTimeSamples, nkern, nHarmonics);

	for(int di = 0; di < nkern; di++){
		for(int i = 0; i < nTimeSamples; i++){
//				printf("\t\t%d: \n", i+di*nTimeSamples);
			float t_SNR = 0.0f;
			int t_SNR_harmonic = 0;
//				harmonic(input_data, SNR, MSD, i + di*nTimeSamples, nHarmonics, 0, 0, nTimeSamples, count, 0);
			find_snr(input_data, MSD, i + di*nTimeSamples, nHarmonics, nTimeSamples, nkern, &t_SNR, &t_SNR_harmonic);
			SNR[i + di*nTimeSamples] = t_SNR;
			SNR_width[i + di*nTimeSamples] = t_SNR_harmonic;
		}
	}
	execution_time = omp_get_wtime() - time_start;
	printf("2dEHRMS time: %f s;\n", execution_time);

	printf("\nMSD stat:\n");
	for(int i = 0; i < nHarmonics; i++)
		printf("H%d: %lf %lf\n", i,MSD[2*i], MSD[2*i+1]);

#ifdef DEBUG
	printf("\nResults of harmonics for each time sample");
#endif

	FILE *fp_out;
        char filename[200];
        sprintf(filename, "output.dat");
                if ((fp_out=fopen(filename, "w")) == NULL) {
                        fprintf(stderr, "Error opening results output data file!\n");
                        exit(0);
                }


	for(int i = 0; i < nkern; i++){
		for(int j = 0; j < nTimeSamples; j++){
			if(SNR[i*nTimeSamples + j] > 10){
			fprintf(fp_out, "%d %d %+lf %d\n", i, j, SNR[i*nTimeSamples + j], SNR_width[i*nTimeSamples + j]);
			#ifdef DEBUG
			printf("%d %d %+lf %d\n", i, j, SNR[i*nTimeSamples + j], SNR_width[i*nTimeSamples + j]);
			#endif
			}
		}
	}
	fclose(fp_out);

	free(input_data);
	free(MSD);
	free(SNR);
	free(SNR_width);

        printf("\n\t**********************************\n");
        printf( "\t        #That's all folks!        \n");
        printf( "\t**********************************\n");

	return 0;
}
