#include <stdio.h>
#include <stdlib.h>
#include <math.h>

//#define DEBUG

void print_data(float *data, int size, int size_x, int nSamples){
	int h,w;
	for(h = 0; h < size_x; h++){
		for(w = 0; w < size; w++){
			printf("%lf ", data[h*nSamples + w]);
		}
		printf("\n");
	}
}

void simple_msd(float *data, int nSamples, int size, int size_x, float *mean){

	double sum;
	sum = 0;
	for(int s = 0; s < size; s++){
		for (int i = 0; i < size_x; i++){
			sum += data[i*nSamples + s];
		}
	}
//	printf("%lf %d\n", sum, size*size);
	sum /= size*size_x;
	*mean = sum;
}


void simple_sd(float *data, int nSamples, int size, int size_x, float mean, float *sd){
	double sum;
	sum = 0;
	float temp;
	double a;

	for(int s = 0; s < size; s++){
                for (int i = 0; i < size_x; i++){
                        temp = (data[i*nSamples + s] - mean);
			a = temp*temp;
			sum += a;
                }
        }
	sum = sqrt(sum/size/size_x);
	*sd = sum;

}

int boxcar(float *data, float *old_data, int width, int size_x, int size){
	int s, w, h, ds;

	float temp;
	for(ds = 0; ds < size_x - width + 1; ds++){
		for(s = 0; s < size - width + 1; s++){
			temp = 0.0f;
			for(h = 0; h < width; h++){
				for(w = 0; w < width; w++){
					temp += old_data[s + w + (ds + h)*size];
//					printf("%d", s+w+(ds+h)*size);
				}
			}
//			printf("\n");
			data[ds*size + s] = temp;
		}
	}

	return(width - 1);
}

void find_msd(float *data, float *msd, int size, int nkern, int nHarm){

	float mean = 0.0f;
	float sd = 0.0f;
	float *temporary;
	temporary = (float *)malloc(sizeof(float)*size*nkern);

	simple_msd(data, size, size, nkern, &mean);
	simple_sd(data, size, size, nkern, mean, &sd);
	msd[0] = mean;  
	msd[1] = sd;

	//higher harmonics
	for (int h = 2; h <=nHarm; h++){
		int remainder; 
		remainder = boxcar(temporary, data, h, nkern, size);
		#ifdef DEBUG
			printf("\nBoxcar #%d\n", h);
			print_data(temporary, size - remainder, nkern, size);
		#endif
		simple_msd(temporary, size, size - remainder, nkern, &mean);
		simple_sd(temporary, size, size - remainder, nkern, mean, &sd);
		msd[2*(h-1)] = mean;
		msd[2*(h-1) + 1] = sd;
//		printf("pos: %d\n", 2*(h-1));
	}

	free(temporary);
}
