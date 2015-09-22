#include<stdio.h>
#include<unistd.h>

//Basic fork() examples

void forkit()
{
	if(fork()==0)
	{
		printf("Child here!\n");
	}
	else
	{
		printf("Parent here!\n");
	}
}

void forkn(int x)
{
	for(int i=0;i<x;i++)
		if(fork())
		{
			printf("Child %d here!\n", i);
			break;	
		}
}

int main(void)
{
	//forkit();
	forkn(10);
	return 0;
}

