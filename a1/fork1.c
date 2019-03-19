#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <unistd.h>

#define MAX_ARGS 50
int main()
{
	
	while(1)
	{	

		char c[1024];

		char *args[MAX_ARGS];
    	char **next = args;
    
    	int cnt=0;
    	int j=0,i;	
		scanf("%[^\n]%*c",c);
		// printf("%s",c);
		if(strcmp(c,"exit")==0)
		{
			break;
		}
	    char b[20][20];

    	for(i=0;i<=(strlen(c));i++)
    	{
        
        	if(c[i]==' '||c[i]=='\0')
        	{
            	b[cnt][j]='\0';
            	*next++ = b[cnt];
            	cnt++;  
            	j=0;    
        	}
        	else
        	{
            	b[cnt][j]=c[i];
            	j++;
        	}
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
	}
	return 0;
}