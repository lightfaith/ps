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

#include <cuda_runtime.h>
#include <stdio.h>

//#define swap(float *p1, float *p2){	float tmp = *p1;	*p1=*p2;	*p2=tmp;}

// Demo kernel for array elements multiplication.
// Every thread selects one element and multiply it. 
__global__ void kernel_mult( float *pole, int L)
{
	// No 2 swapping in one kernel (collisions), rather run 2 kernels in loop below...
	int l = blockDim.x * blockIdx.x + threadIdx.x;
	if(l%2==1)
		return;
	// if grid is greater then length of array...
	if (l>=L-1) return;
	float tmp;
	if(pole[l]>pole[l+1])
	{
		tmp=pole[l];
		pole[l]=pole[l+1];
		pole[l+1]=tmp;
	}
	if(l>=L-2) return;
	if(pole[l+1]>pole[l+2])
	{
		tmp=pole[l+1];
		pole[l+1]=pole[l+2];
		pole[l+2]=tmp;
	}
}

void bsort( float *P, int Length)
{
	cudaError_t cerr;
	int threads = 1024;
	int blocks = ( Length + threads - 1 ) / threads;
	printf("blocks: %d\n", blocks);
	// Memory allocation in GPU device
	float *cudaP;
	cerr = cudaMalloc( &cudaP, Length * sizeof( float ) );
	if ( cerr != cudaSuccess )
		printf( "CUDA Error [%d] - '%s'\n", __LINE__, cudaGetErrorString( cerr ) );	
	

	// Copy data from PC to GPU device
	cerr = cudaMemcpy( cudaP, P, Length * sizeof( float ), cudaMemcpyHostToDevice );
	if ( cerr != cudaSuccess )
		printf( "CUDA Error [%d] - '%s'\n", __LINE__, cudaGetErrorString( cerr ) );	

	// Grid creation
	for(int i=0;i<Length; i++)
	// here run kernel twice
		kernel_mult<<< blocks, threads >>>(cudaP, Length);

	if ( ( cerr = cudaGetLastError() ) != cudaSuccess )
		printf( "CUDA Error [%d] - '%s'\n", __LINE__, cudaGetErrorString( cerr ) );

	// Copy data from GPU device to PC
	cerr = cudaMemcpy( P, cudaP, Length * sizeof( float ), cudaMemcpyDeviceToHost );
	if ( cerr != cudaSuccess )
		printf( "CUDA Error [%d] - '%s'\n", __LINE__, cudaGetErrorString( cerr ) );

	// Free memory
	cudaFree(cudaP);
}
