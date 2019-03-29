#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <assert.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/sem.h>
#include <sys/wait.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <sys/msg.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <netdb.h>
#include <signal.h>
#include <ctype.h>
#include <sys/shm.h>

// message queue
typedef struct mymsgbuf {
	long    mtype;
	int pid; // added
	int id;
} mymsgbuf;


int send_message( int pid, int qid, struct mymsgbuf *qbuf )
{
	int     result, length;
	/* The length is essentially the size of the structure minus sizeof(mtype) */
	length = sizeof(struct mymsgbuf) - sizeof(long);
	if ((result = msgsnd( pid, qid, qbuf, length, 0)) == -1)
	{
		perror("Error in sending message");
		exit(1);
	}
	return (result);
}

int read_message( int pid, int qid, long type, struct mymsgbuf *qbuf )
{
	int     result, length;

	/* The length is essentially the size of the structure minus sizeof(mtype) */
	length = sizeof(struct mymsgbuf) - sizeof(long);

	if ((result = msgrcv( pid, qid, qbuf, length, type,  0)) == -1)
	{// type- If msgtyp is greater than 0, then the first message in the queue of type msgtyp is read
		perror("Error in receiving message");
		exit(1);
	}

	return (result);
}


