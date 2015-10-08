#include<stdio.h>
#include<unistd.h>
#include<stdlib.h>
#include<sys/mman.h>
#include<fcntl.h>
#include<string.h>
#include<sys/types.h>
#include<sys/stat.h>
#include<sys/wait.h>
#include<time.h>
#include<stdarg.h>
#include<sys/time.h>

/*
This program generates X random integer numbers and bubblesorts them in multithreaded fashion using Y fork() children.
X and Y must be passed as parameters to the main() function.

Usage: ./sort <elements> <processes> <parallel>
*/

int ARRLEN; //number of numbers
int CHILDCOUNT; //number of sorters (or parts)
int ARRSIZE; //total size of array (ARRLEN*sizeof(int))
int INTERVAL; //number of elements for each fork() child

void die(const char *format, ...)
//print and die
{
	va_list args;
	va_start(args, format);
	vprintf(format, args);
	va_end(args);
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
		die("Process %d has an incorrect interval (%d - %d) defined!\n", id, start, end);
	
	//printf("Process %d will sort items %d through %d\n", id, start+1, end);
	for(int i=start; i<end-1; i++) //for every number try find maximum value and move to the top
	{
		for(int j=start; j<end+start-i-1; j++) //for every yet unsorted number
		{
			if(numbers[j+1]<numbers[j])
				swap(numbers+j+1, numbers+j);
		}
	}
	//if(ARRLEN<=100) printf("\nresult %d: ", id);
	//printarr(numbers, start, end);
}


int *preparememory()
//generate numbers and prepare for sharing
{
	int *map = (int*)mmap(0, ARRSIZE, PROT_READ | PROT_WRITE, MAP_SHARED | MAP_ANONYMOUS, -1, 0);
	if (map == MAP_FAILED)
	{
		die("Error while mapping...");
	}
	//generate random values
	for(int i=0; i<ARRLEN; i++)
		map[i]=rand()%(ARRLEN*100);
	//-----	

	return map;
}

void unleashmemory(int* numbers)
//free mmap and delete file
{
		munmap(numbers, ARRSIZE);
}

void mergeparts(int *numbers)
//array now contains sorted intervals, like [[6789], [1239], [2478]], merge them!
{
	int pointerinc[CHILDCOUNT]={0}; //how many times has each childcount pointer moved
	
	int counter=0; //number of elements already sorted
	int sorted[ARRLEN]={0}; //new array of sorted elements
	
	
	//if(ARRLEN<=100)	printf("Merging array:\n");
	//printarr(numbers, 0, ARRLEN);
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
		pointerinc[plowest]++;
		counter++;
	}
	memcpy(numbers, sorted, ARRSIZE); //replace partially sorted array with the fully sorted one
}

void startstopwatch(timeval* starttimer)
{
	gettimeofday(starttimer, NULL);
	//printf("starting at %ld\n\n", starttimer->tv_sec);
}

float endstopwatch(timeval* starttimer)
{
	struct timeval endtimer;
	gettimeofday(&endtimer, NULL);
	//printf("stopping at %ld\n\n", endtimer.tv_sec);
	long seconds = endtimer.tv_sec - starttimer->tv_sec;
	long useconds = endtimer.tv_usec - starttimer->tv_usec;
	long result = ((seconds)*1000+useconds/1000.0)+0.5;
	return result;
}

int main(int argc, char **argv)
{
	if(argc!=4)
		die("Usage: %s <items> <processes> (0|1)\n", argv[0]);
	
	struct timeval total, fraction; //start values, end will be calcullated locally in endstopwatch()
	long totaltime, fractiontime;
	bool goparallel;
	
	startstopwatch(&total);
	//initialize properties
	ARRLEN=atoi(argv[1]);
	CHILDCOUNT=atoi(argv[2]);
	ARRSIZE = ARRLEN*sizeof(int);
	INTERVAL = ARRLEN/CHILDCOUNT; 
	goparallel=(strcmp(argv[3], "1")==0)?true:false;

	// if ARRLEN%CHILDCOUNT=0 then INTERVAL is accurate
	//	else last child will take care of the remainders
	// if ARRLEN<CHILDCOUNT then Last child to sort'em all

	printf("Sorting %d elements in %d parts %s\n.", ARRLEN, CHILDCOUNT, goparallel?"in parallel":"sequentially");
	
	//randomize, prepare shared array of elements
	srand((unsigned)time(NULL));
	int* numbers = preparememory();


	startstopwatch(&fraction);
	if(!goparallel)//sort parts sequentially
	{
		for(int i=0; i<CHILDCOUNT; i++)
		{
			bubblesort(numbers, i); //sort numbers as parts (nobody knows we are not child processes ;) )
		}
	}
	else //sort in parallel
	{	
		for(int i=0; i<CHILDCOUNT; i++)
		{
			int forkresult=fork();
			if(forkresult==0) //child
			{
				bubblesort(numbers, i);
				exit(0);
			}
		}

		//kill all the zombies
		int status=0;
		for(int i=0;i<CHILDCOUNT; i++)
			wait(&status);
	}
	fractiontime = endstopwatch(&fraction);

	
	//merge semi-sorted intervals
	mergeparts(numbers);
	//destroy shared array
	unleashmemory(numbers);

	//end of parallel sorting

	totaltime = endstopwatch(&total);
	printf("Parallelizable: %ld ms.\n", fractiontime);
	printf("Total run time: %ld ms.\n\n", totaltime);
	return 0;
}

