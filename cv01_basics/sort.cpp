#include<stdio.h>
#include<unistd.h>
#include<stdlib.h>
#include<sys/mman.h>
#include<fcntl.h>
#include<string.h>
#include<sys/types.h>
#include<sys/stat.h>
#include<sys/wait.h> //ESSENTIAL FOR WAIT & WAITPID!!!!!!!!!!!!!!!!
#include<time.h>
#define MEMFILE "memfile" //shared file for virtual memory map

/*
This program generates X random integer numbers and bubblesorts them in multithreaded fashion using Y fork() children.
X and Y must be passed as parameters to the main() function.

Usage: ./sort <elements> <processes>
*/

int ARRLEN; //number of numbers
int CHILDCOUNT; //number of sorters
int ARRSIZE; //total size of array (ARRLEN*sizeof(int))
int INTERVAL; //number of elements for each fork() child
int f=0; //MEMFILE handler

void die(const char *msg) 
//print and die
{
	printf(msg);
	exit(1);
}

void printarr(int* numbers, int start, int end) 
//print defined part of array
{
	if(ARRLEN>100)
		return;
	for(int i=start; i<end; i++)
		printf("%d ", numbers[i]);
	printf("\n");
}

void swap(int* p1, int* p2)
//swaps two numbers (for bubblesort)
{
	int tmp=*p1;
	*p1=*p2;
	*p2=tmp;
}

void bubblesort(int* numbers, int id)
//bubblesort defined range of numbers[] (different for each child process)
{
	int start=INTERVAL*id;
	int end=(id==CHILDCOUNT-1)?ARRLEN:INTERVAL*(id+1);
	if(start>end)
	{
		printf("Process %d has an incorrect interval (%d - %d) defined!\n", id, start, end);
		exit(1);
	}
	
	printf("Process %d will sort items %d through %d\n", id, start+1, end);
	for(int i=start; i<end-1; i++) //for every number try find maximum value and move to the top
	{
		for(int j=start; j<end+start-i-1; j++) //for every yet unsorted number
		{
			if(numbers[j+1]<numbers[j])
				swap(numbers+j+1, numbers+j);
		}
	}
	if(ARRLEN<=100) printf("\nresult %d: ", id);
	printarr(numbers, start, end);
}


int *preparememory()
//generate numbers and prepare for sharing
{
	/*
	//----- mmap with file
	int generated[ARRLEN]={1,1,1,1,1,0};
	f = open(MEMFILE, O_RDWR | O_CREAT | O_TRUNC, (mode_t)0600);
	for(int i=0; i<ARRLEN; i++)
		generated[i]=rand()%10000;
	write(f, generated, ARRSIZE);
	int *map = (int*)mmap(0, ARRSIZE, PROT_READ | PROT_WRITE, MAP_SHARED, f, 0);
	//-----
	*/

	
	//----- mmap memory directly
	int *map = (int*)mmap(0, ARRSIZE, PROT_READ | PROT_WRITE, MAP_SHARED | MAP_ANONYMOUS, -1, 0);
	if (map == MAP_FAILED)
	{
		close(f);
		die("Error while mapping...");
	}
	//generate random values
	for(int i=0; i<ARRLEN; i++)
		map[i]=rand()%10000;
	//-----	

	return map;
}

void unleashmemory(int* numbers)
//free mmap and delete file
{
	if(f!=0)
	{
		munmap(numbers, ARRSIZE);
		close(f);
		remove(MEMFILE);
	}
}

void mergeparts(int *numbers)
//array now contains sorted intervals, like [[6789], [1239], [2478]], merge them!
{
	
	int pointerinc[CHILDCOUNT]={0}; //how many times has each childcount pointer moved
	
	int counter=0; //number of elements already sorted
	int sorted[ARRLEN]={0}; //new array of sorted elements
	
	
	printf("Merging array:\n");
	printarr(numbers, 0, ARRLEN);
	//finally merge them...
	while(1)//or while(counter<=ARRLEN)
	{
		//find pointer with lowest value
		int plowest=-1;
		for(int i=0; i<CHILDCOUNT; i++)
		{
			if((i<CHILDCOUNT-1 && pointerinc[i]>=INTERVAL) || pointerinc[i]>=ARRLEN-(CHILDCOUNT-1)*INTERVAL)
			//this interval already exhausted
				continue;
			if(plowest==-1 || numbers[i*INTERVAL+pointerinc[i]]<numbers[plowest*INTERVAL+pointerinc[plowest]])
			//first interval in iteration? pointer to lower value than detected so far? 
				plowest=i;
		}
		if(plowest==-1) //everything in place
			break;

		//add the lowest element into sorted array
		sorted[counter]=numbers[plowest*INTERVAL+pointerinc[plowest]];
		if(sorted[counter]==0)
		{
			printf("ZERO! counter=%d, plowest=%d, interval=%d, pointerinc[plowest]=%d\n", counter, plowest, INTERVAL, pointerinc[plowest]);
			printf("numbers[plowest*INTERVAL+pointerinc[plowest]]=%d\n", numbers[plowest*INTERVAL+pointerinc[plowest]]);
		}
		pointerinc[plowest]++;
		counter++;
	}
	memcpy(numbers, sorted, ARRSIZE); //replace partially sorted array with the fully sorted one
}

int main(int argc, char **argv)
{
	if(argc!=3)
		die("Usage: ./sort <items> <processes>\n");
	
	//initialize properties
	ARRLEN=atoi(argv[1]);
	CHILDCOUNT=atoi(argv[2]);
	ARRSIZE = ARRLEN*sizeof(int);
	INTERVAL = ARRLEN/CHILDCOUNT; 
	// if ARRLEN%CHILDCOUNT=0 then INTERVAL is accurate
	//	else last child will take care of the remainders
	// if ARRLEN<CHILDCOUNT then Last child to sort'em all


	//randomize, prepare shared array of elements
	srand((unsigned)time(NULL));
	int* numbers = preparememory();

	//array for PID of all child processes (unnecessary right now)
	int pids[CHILDCOUNT];
	
	//print array (if small), fork() for glory and sort intervals like a boss
	printarr(numbers, 0, ARRLEN);
	for(int i=0; i<CHILDCOUNT; i++)
	{
		int forkresult=fork();
		if(forkresult==0) //child
		{
			bubblesort(numbers, i);
			exit(0);
		}
		else //parent
		{
			pids[i]=forkresult;
		}
	}

	//kill all the zombies
	int status=0;
	for(int i=0;i<CHILDCOUNT; i++)
		wait(&status);
		//waitpid(-1, &status, 0); //like wait()
		//waitpid(pids[i], &status, 0); //wait for specific PID

	//merge semi-sorted intervals
	mergeparts(numbers);
	//print array (if small)
	printarr(numbers, 0, ARRLEN);
	//destroy shared array
	unleashmemory(numbers);
	return 0;
}

