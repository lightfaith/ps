// ***********************************************************************
//
// Demo program for education in subject
// Computer Architectures and Paralel Systems
// Petr Olivka, dep. of Computer Science, FEI, VSB-TU Ostrava
// email:petr.olivka@vsb.cz
//
// Example of CUDA Technology Usage
// Multiplication of elements in float array
//
// ***********************************************************************

#include <stdio.h>
#include<stdlib.h>
#include<unistd.h>
#include<string.h>

// Function prototype from .cu file
void run_mult( float *arr1, float *arr2, int L);


void run(float *arr1, float *arr2, unsigned int L)
{
	for(unsigned int i=0; i<L; i++)
	{
		for(int j=0; j<30; j++)
		{
			arr1[i]*=arr2[i];
			arr1[i]/=arr2[i];
		}
		arr1[i]*=arr2[i];
	}
}

int main(int argc, char **argv)
{
	if(argc<2)
	{
		printf("Usage: %s <elements> [gpu] \n", argv[0]);
		exit(1);
	}

	unsigned int N = atoi(argv[1]);
	// Array initialization
	float * prvky;
	prvky = new float[N];
	//OCfloat prvky[ N ];
	for ( unsigned int i = 0; i < N; i++ )
		prvky[ i ] = ( float ) i/1000.0;

	if(argc==2)
	{
		printf("Running program on CPU!\n");
		run(prvky, prvky, N);
	}
	else if(argc==3 && strcmp(argv[2], "gpu")==0)
	{
		printf("Running program on GPU!\n");
		run_mult( prvky, prvky, N);
	}
	else
	{
		printf("Usage: %s <elements> [gpu] \n", argv[0]);
		exit(1);
	}

	// Print result
	//for ( int i = N-10; i < N; i++ )
	//	printf( "%8.2f\n", prvky[ i ] );
	printf("Done!\n");
	return 0;
}

