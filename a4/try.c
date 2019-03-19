#include<stdio.h> 
// #include<wait.h> 
#include <unistd.h>
#include <stdlib.h>
#include<signal.h> 

pid_t pid; 
int counter = 0; 
void handler1(int sig) 
{ 
	counter++; 
	printf("counter1 = %d\n", counter); 
	/* Flushes the printed string to stdout */
	fflush(stdout); 
	kill(pid, SIGUSR1); 
} 
void handler2(int sig) 
{ 
	counter += 3; 
	printf("counter2 = %d\n", counter); 
	exit(0); 
} 

int main() 
{ 
	pid_t p; 
	int status; 
	signal(SIGUSR1, handler1); 
	if ((pid = fork()) == 0) 
	{ 
		signal(SIGUSR1, handler2); 
		kill(getppid(), SIGUSR1); 
		// fflush(stdout); 
		printf("here\n");
		while(1) ; 
	} 
	if ((p = wait(&status)) > 0) 
	{ 
		counter += 4; 
		printf("counter3 = %d\n", counter); 
	} 
} 