#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <unistd.h>
#include <fcntl.h> 
#include <errno.h> 
#include <sys/wait.h>
extern int errno; 
#define MAX_ARGS 50

int main()
{    
	while(1)
	{	
		char *args[MAX_ARGS];
    	char **next = args;
        char c[1024];
    	int cnt=0;
    	int j=0,i, bk=0, pipe_process=0;	
    	// taking a line with spaces as input-  
		// the regex implies to take a charcater sring as input until encountered by a new line
		scanf("%[^\n]%*c",c);
        char *copy = strdup(c); 

		// exit loop when encountered with quit
		if(strcmp(c,"quit")==0)
		{
			break;
		}

        int in_redirect=0;
        int out_redirect=0,pipecount=0;
        char infile[20],outfile[20];
        int in=0, out=0,fd,fd1,count=0;
        int saved_stdout,saved_stdin;
        saved_stdout = dup(1);
        saved_stdin = dup(0);

        for (i = 0; i <= strlen(c); ++i)
        {
            if(c[i] == '|')
            {
                pipecount++;
                pipe_process=1;
            }
            if(c[i] == '&')
            {
                bk =1;
            }
        }
        int p[pipecount][2];
        for (i = 0; i < pipecount; ++i)
        {
            if(pipe(p[i])<0)
                perror("pipe not");
        }

        if(pipe_process==1)
        {
            char *end_str;
            char* token = strtok_r(c, "|&", &end_str);
            char de[] = " ><";
            
            while(token!=NULL)
            {
                char *args[MAX_ARGS];
                char **next = args;                                

                char* end_token;
                char* copy1 = strdup(token);
                char* subtoken = strtok_r(token, de, &token);                
                
                int j=0;
                while(strchr(" ", copy1[j])!=NULL)
                    j++;
                while (subtoken != NULL)
                { 
                    //fprintf(stderr, "file output redirec\n");
                    if(in_redirect==1)
                    {
                        fd = open(subtoken,O_RDONLY );
                        if (fd ==-1) 
                        {
                            printf("Error Number % d\n", errno);  
                            perror("Program");                  
                        }
                        dup2(fd, 0);
                        in_redirect=0;
                    }                    
                    else if(out_redirect==1)
                    {
                        // //fprintf(stderr, "file output redirec\n");
                        fd1 = open(subtoken, O_WRONLY| O_CREAT|O_TRUNC,S_IRWXU);
                        if (fd1 ==-1) 
                        {
                            printf("Error Number % d\n", errno);  
                            perror("Program");                  
                        }
                        dup2(fd1, 1);
                        out_redirect=0;
                    }
                    else
                    {
                        // //fprintf(stderr, "%s\n", subtoken);
                        *next++ = subtoken;
                    }

                    j = j+ strlen(subtoken); 
                    //fprintf(stderr, "j,copy1: %d %d\n", j, strlen(copy1));
                    while(j<strlen(copy1) && strchr(de,copy1[j])!=NULL)
                    {
                        //fprintf(stderr, "%d\n", j);
                        if(copy1[j]=='<')
                        {
                            //fprintf(stderr, "in redirect\n");
                            in_redirect=1;
                        }
                        else if(copy1[j]=='>')
                        {
                            //fprintf(stderr, "out redirect\n");
                            out_redirect=1;
                        }
                        j++;
                    }        
                    
                    subtoken = strtok_r(NULL, de, &token);
                } 

                *next = NULL;
                int pid;

                if((pid=fork())==0)
                {
                    if(count==0)
                    {
                        dup2(p[0][1], 1);
                        close(p[0][1]); 
                        
                    }
                    else if(count==pipecount)
                    {                     
                        dup2(p[pipecount-1][0], 0); 
                        close(p[pipecount-1][0]);                        
                    }
                    else
                    {                     
                        dup2(p[count-1][0], 0);
                        dup2(p[count][1], 1); 
                        close(p[count-1][1]);                                                
                        close(p[count][0]);
                    }

                    if(execvp(args[0], args)<0)
                    {
                        perror("exec error\n");
                        exit(1);
                    }
                    exit(0);
                }
                else 
                {
                    if(!bk)
                    {                        
                        // perror("execvp"); 
                        waitpid(pid, NULL, 0); 
                        if(count!=pipecount)
                        {                   
                            close(p[count][1]);                         
                        }
                    }
                    if(bk)
                    {
                        waitpid(pid, NULL, WNOHANG);
                        if(count!=pipecount)
                        {                   
                            close(p[count][1]);                         
                        }
                    }                   
                }
                count++;          
                token = strtok_r(NULL, "|&", &end_str);
            }

            close(fd);
            close(fd1);
            dup2(saved_stdout, 1);
            close(saved_stdout);
            dup2(saved_stdin, 0);
            close(saved_stdin); 
        }

        else
        {
    	    //loop for seperating the string into executable file name and arguments
        	char* token = strtok(c, " ><|&"); 
            while (token != NULL)
            { 
                if(in_redirect==1)
                {
                    fd = open(token,O_RDONLY );
                    if (fd ==-1) 
                    {
                        printf("Error Number % d\n", errno);  
                        perror("Program");                  
                    }
                    dup2(fd, 0);
                    in_redirect=0;
                }                
                else if(out_redirect==1)
                {
                    fd1 = open(token, O_WRONLY| O_CREAT|O_TRUNC, S_IRWXU);
                    if (fd1 ==-1) 
                    {
                        printf("Error Number % d\n", errno);  
                        perror("Program");                  
                    }
                    dup2(fd1, 1);
                    out_redirect=0;
                }
                else        
                {
                    *next++ = token;
                }
                int j = token-c+strlen(token);
                while(j<strlen(copy) && strchr(" ><|&",copy[j])!=NULL)
                {

                    if(copy[j]=='<')
                    {
                        in_redirect=1;
                        break;
                    }
                    if(copy[j]=='>')
                    {
                        out_redirect=1;
                        break;
                    }
                    if(copy[j]=='&')
                    {
                        bk=1;
                        break;
                    }
                    j++;
                }
                
                token = strtok(NULL, " ><|&");
            } 
    	    *next = NULL;
            int pid;
            pid = fork();
    	    //calling execvp to execute the respective executable
            if(pid<0)
            {
                perror("fork error");
                return 1;
            }
    		if(pid==0)
    		{	
    			if(execvp(args[0], args)<0)
    			{
    				perror("error\n");
    				exit(1);
    			}
                
                exit(0);
    		}
            else if(!bk)
            {
                wait(NULL);
            }
            close(fd);
            close(fd1);
            dup2(saved_stdout, 1);
            close(saved_stdout);
            dup2(saved_stdin, 0);
            close(saved_stdin);            
        }
        
	}
	return 0;
}