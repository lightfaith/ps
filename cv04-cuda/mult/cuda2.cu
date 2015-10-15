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

// Demo kernel for array elements multiplication.
// Every thread selects one element and multiply it. 
__global__ void kernel_mult( float *pole1, float *pole2, int L)
{
	int l = blockDim.x * blockIdx.x + threadIdx.x;
	// if grid is greater then length of array...
	if (l>=L) return;
	for(int i=0;i<30;i++)
	{
	pole1[l]*=pole2[l];
	pole1[l]/=pole2[l];
	}
	pole1[l] *= pole2[l];
}

void run_mult( float *P1, float *P2, int Length)
{
	cudaError_t cerr;
	int threads = 1024;
	int blocks = ( Length + threads - 1 ) / threads;
	printf("blocks: %d\n", blocks);
	// Memory allocation in GPU device
	float *cudaP1;
	float *cudaP2;
	cerr = cudaMalloc( &cudaP1, Length * sizeof( float ) );
	if ( cerr != cudaSuccess )
		printf( "CUDA Error [%d] - '%s'\n", __LINE__, cudaGetErrorString( cerr ) );	
	
	cerr = cudaMalloc( &cudaP2, Length * sizeof( float ) );
	if ( cerr != cudaSuccess )
		printf( "CUDA Error [%d] - '%s'\n", __LINE__, cudaGetErrorString( cerr ) );	

	// Copy data from PC to GPU device
	cerr = cudaMemcpy( cudaP1, P1, Length * sizeof( float ), cudaMemcpyHostToDevice );
	if ( cerr != cudaSuccess )
		printf( "CUDA Error [%d] - '%s'\n", __LINE__, cudaGetErrorString( cerr ) );	
	cerr = cudaMemcpy( cudaP2, P2, Length * sizeof( float ), cudaMemcpyHostToDevice );
	if ( cerr != cudaSuccess )
		printf( "CUDA Error [%d] - '%s'\n", __LINE__, cudaGetErrorString( cerr ) );	

	// Grid creation
	kernel_mult<<< blocks, threads >>>(cudaP1, cudaP2, Length);

	if ( ( cerr = cudaGetLastError() ) != cudaSuccess )
		printf( "CUDA Error [%d] - '%s'\n", __LINE__, cudaGetErrorString( cerr ) );

	// Copy data from GPU device to PC
	cerr = cudaMemcpy( P1, cudaP1, Length * sizeof( float ), cudaMemcpyDeviceToHost );
	if ( cerr != cudaSuccess )
		printf( "CUDA Error [%d] - '%s'\n", __LINE__, cudaGetErrorString( cerr ) );

	// Free memory
	cudaFree(cudaP1);
	cudaFree(cudaP2);
}
