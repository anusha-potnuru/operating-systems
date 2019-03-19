/*
Assignment 1
P.V.S.L Hari chandana 16CS30026
Potnuru Anusha 16CS30027
*/

#include <stdio.h>
#include <stdlib.h>

#include <sys/types.h>
#include <unistd.h>
#include<time.h> 

int compare(const void*a, const void*b)
{
	return (*(int*)a -*(int*)b );
}


void merge(int *c, int a[], int b[], int m, int n)
{

	int i=0,j=0, k=0;
	while(i<m && j<n)
	{
		if(a[i]<b[j])
			c[k++] = a[i++];
		else
			c[k++] = b[j++];
	}
	while(i<m)
		c[k++] = a[i++];
	while(j<n)
		c[k++] = b[j++];

}


int main()
{
	printf("The main process id: %d\n\n", getpid() );
	srand(time(NULL)^getpid());
	int a[50], b[50], c[50];
	int p[4][2];

	for (int i = 0; i < 4; ++i)
	{
		if(pipe(p[i])<0)
		{
			exit(1);
		}
	}


	if (fork() ==0)
	{
		if(fork()==0)
		{
			if(fork()==0)
			{
				printf("Process id of A: %d\n", getpid() );
				printf("Random numbers created in A:\n");
				for (int i = 0; i < 50; ++i)
				{

					a[i] = 1+ rand()%300;
					printf("%d ",a[i] );
				}
				printf("\n\n");
				qsort(a, 50, sizeof(int), compare);
				write(p[0][1], a, sizeof(a));
			}
			else
			{
				printf("Process id of B: %d\n", getpid() );
				srand(time(NULL)^getpid());
				printf("Random numbers created in B: \n");
				for (int i = 0; i < 50; ++i)
				{
					/* code */
					b[i] = 1 + rand()%300;
					printf("%d ", b[i]);
				}
				printf("\n\n");
				qsort(b, 50, sizeof(int), compare);
				write(p[1][1], b, sizeof(b));
			}
		}
		else
		{
			printf("Process id of C: %d\n", getpid() );
			printf("Random numbers created in C:\n");
			srand(time(NULL)^getpid());
			for (int i = 0; i < 50; ++i)
			{
				c[i]= 1 + rand()% 300;
				printf("%d ",c[i] );
			}
			printf("\n\n");
			qsort(c, 50, sizeof(int), compare );

			write(p[2][1], c, sizeof(c));
		}

	}
	else
	{
		
		if(fork()==0)
		{	
			printf("Process id of D: %d\n", getpid() );
			int d[100];		
			read(p[0][0], a , sizeof(a));
			read(p[1][0], b, sizeof(b));
			merge(d,a,b, 50, 50);
			printf("Merged A and B sent to D: \n");
			for (int i = 0; i < 100; ++i)
			{
				/* code */
				printf("%d ", d[i]);
			}
			printf("\n\n");
			write(p[3][1], d, sizeof(d) );


		}
		else
		{
			printf("Process id of E: %d\n", getpid() );
			int e[150];
			int d[100];
			read(p[3][0], d, sizeof(d));
			read(p[2][0], a, sizeof(a));
			printf("Sorted C sent to E: \n");
			for (int i = 0; i < 50; ++i)
			{
				printf("%d ", a[i] );
			}
			printf("\n\n");

			merge(e,d,a,100,50);
			printf("All numbers recieved in E: \n");
			for (int i = 0; i < 150; ++i)
			{
				/* code */
				printf("%d ", e[i]);
			}

		}

	}
	printf("\n");

	return 0;
}