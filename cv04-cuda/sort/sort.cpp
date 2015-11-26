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
//#define DEBUG
// Function prototype from .cu file
void bsort( float *arr1, int L);


int main(int argc, char **argv)
{
	if(argc<2)
	{
		printf("Usage: %s <elements> \n", argv[0]);
		exit(1);
	}

	unsigned int N = atoi(argv[1]);
	// Array initialization
	float * prvky;
	prvky = new float[N];
	//OCfloat prvky[ N ];
	for ( unsigned int i = 0; i < N; i++ )
		prvky[ i ] = ( float ) ((N-i)/1000.0);

	#ifdef DEBUG
	for ( int i = 0; i < N; i++ )
		printf( "%8.3f\n", prvky[ i ] );
	printf("--------------------------\nSorting...\n");
	#endif
	
	bsort(prvky, N);
	
	// Print result
	#ifdef DEBUG
	for ( int i = 0; i < N; i++ )
		printf( "%8.3f\n", prvky[ i ] );
	#endif

	printf("Done!\n");
	return 0;
}

