#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <unistd.h>

#define MAX_ARGS 50
int main()
{
	char* temp= NULL;
	char line[1024];
	// char c[200];
	// char **next = args;
	char *args[MAX_ARGS];
    char **next = args;
	while(1)
	{		
		scanf("%[^\n]%*c",line);
		// printf("%s",line);
		if(strcmp(line,"exit")==0)
		{
			break;
		}
    	temp = strtok(line, " ");
	   	while (temp != NULL)
	    {
	        *next++ = temp;
	        printf("%s ", temp);
	        temp = strtok(NULL, " ");
	    }
	    printf("\n\n");
	    *next = NULL;
		if(fork()==0)
		{	
			if(execvp(args[0], args)<0)
			{
				printf("error\n");
				exit(1);
			}
		}
		// printf("\n");
	}
	return 0;
}