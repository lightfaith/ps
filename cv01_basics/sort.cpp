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

int ARRLEN; //number of numbers
int CHILDCOUNT; //number of sorters
int ARRSIZE;
int INTERVAL;
int f=0; //MEMFILE handler

void die(const char *msg) //print and die
{
	printf(msg);
	exit(1);
}

void printarr(int* numbers, int start, int end) //print part of array
{
	if(ARRLEN>100)
		return;
	for(int i=start; i<end; i++)
		printf("%d ", numbers[i]);
	printf("\n");
}

void swap(int* p1, int* p2)
{
	int tmp=*p1;
	*p1=*p2;
	*p2=tmp;
}

void bubblesort(int* numbers, int id)
{
	char fl[]={",.'_"};
	char tr[]={";:\"-"};
	int start=INTERVAL*id;
	int end=INTERVAL*(id+1);
	
	printf("Process %d will sort items %d through %d\n", id, start+1, end);
	for(int i=start; i<end-1; i++)
	{
		for(int j=start; j<end+start-i-1; j++)
		{
			if(numbers[j+1]<numbers[j])
			{
				swap(numbers+j+1, numbers+j);
				if(ARRLEN<=100) printf("%c", tr[id]);
			}
			else
				if(ARRLEN<=100) printf("%c", fl[id]);
		}
	}
	if(ARRLEN<=100) printf("\nresult %d: ", id);
	printarr(numbers, start, end);
}


int *preparememory() //generate numbers and prepare for sharing
{
	/*//mmap with file
	int generated[ARRLEN]={1,1,1,1,1,0};
	f = open(MEMFILE, O_RDWR | O_CREAT | O_TRUNC, (mode_t)0600);
	for(int i=0; i<ARRLEN; i++)
		generated[i]=rand()%ARRLEN;
	write(f, generated, ARRSIZE);
	int *map = (int*)mmap(0, ARRSIZE, PROT_READ | PROT_WRITE, MAP_SHARED, f, 0);
	*/

	
	//mmap memory directly
	int *map = (int*)mmap(0, ARRSIZE, PROT_READ | PROT_WRITE, MAP_SHARED | MAP_ANONYMOUS, -1, 0);
	if (map == MAP_FAILED)
	{
		close(f);
		die("Error while mapping...");
	}
	for(int i=0; i<ARRLEN; i++)
		map[i]=rand()%ARRLEN;
	

	
	return map;
}

void unleashmemory(int* numbers) //free mmap and delete file
{
	if(f!=0)
	{
		munmap(numbers, ARRSIZE);
		close(f);
		remove(MEMFILE);
	}
}

void mergeparts(int * numbers)
{
	
	int pointerinc[CHILDCOUNT]={0}; //how many times has each childcount pointer moved
	
	int counter=0;
	int sorted[ARRLEN]={0};
	//finally merge them...
	while(counter<ARRLEN)
	{
		//find pointer with lowest value
		int plowest=-1;
		for(int i=0; i<CHILDCOUNT; i++)
		{	
			if(pointerinc[i]>=INTERVAL)
				continue;
			if(plowest==-1 || numbers[i*INTERVAL+pointerinc[i]]<numbers[plowest*INTERVAL+pointerinc[plowest]])
				plowest=i;
		}
		if(plowest==-1)
			break;
		sorted[counter]=numbers[plowest*INTERVAL+pointerinc[plowest]];
		pointerinc[plowest]++;
		counter++;
	}
	memcpy(numbers, sorted, ARRSIZE);
}

int main(int argc, char **argv)
{
	if(argc<3)
		die("Usage: ./sort <items> <processes>\n");
	
	//initialize properties
	ARRLEN=atoi(argv[1]);
	CHILDCOUNT=atoi(argv[2]);
	ARRSIZE = ARRLEN*sizeof(int);
	INTERVAL = ARRLEN/CHILDCOUNT;

	
	srand((unsigned)time(NULL));
	int* numbers = preparememory();
	int pids[CHILDCOUNT];
	
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
	int status=0;
	for(int i=0;i<CHILDCOUNT; i++)
		wait(&status);
		//waitpid(-1, &status, 0); //like wait()
		//waitpid(pids[i], &status, 0); //wait for specific PID

	mergeparts(numbers);
	printarr(numbers, 0, ARRLEN);
	unleashmemory(numbers);
	return 0;
}

