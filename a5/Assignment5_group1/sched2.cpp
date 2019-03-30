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


#define MAX_BUFFER_SIZE 100
#define MAX_PAGES 1000
#define MAX_PROCESS 1000
#define FROMPROCESS 10
#define TOPROCESS 20  // TOPROCESS+id will be used to read msg from sch
// #define TOMMU 10
#define FROMMMU 20

#define PAGEFAULT_HANDLED 5
#define TERMINATED 10

int k; //no. of processes
typedef struct _mmutosch {
	long    mtype;
	char mbuf[1];
} mmutosch;

// message queue
typedef struct mymsgbuf {
	long    mtype;
	int id;
} mymsgbuf;

typedef struct {
	pid_t pid;
	int proc_pid;
	int m;
} pcb;

pcb* pcbptr;


void signal_handler(int signo)
{
	if(signo==SIGUSR1)
	{
		printf("received SIGNAL\n");
		// break from pause
	}
	if(signo==SIGUSR2)
	{
		pause();
	}
}

int send_message( int qid, struct mymsgbuf *qbuf )
{
	int     result, length;
	/* The length is essentially the size of the structure minus sizeof(mtype) */
	length = sizeof(struct mymsgbuf) - sizeof(long);
	if ((result = msgsnd( qid, qbuf, length, 0)) == -1)
	{
		perror("Error in sending message");
		exit(1);
	}
	printf("sch send_message done\n");
	return (result);
}

int read_message( int qid, long type, struct mymsgbuf *qbuf )
{
	int     result, length;

	/* The length is essentially the size of the structure minus sizeof(mtype) */
	length = sizeof(struct mymsgbuf) - sizeof(long);

	if ((result = msgrcv( qid, qbuf, length, type,  0)) == -1)
	{// type- If msgtyp is greater than 0, then the first message in the queue of type msgtyp is read
		perror("Error in receiving message Scheduler read_message");
		exit(1);
	}

	return (result);
}

int read_message_mmu( int qid, long type,mmutosch *qbuf )
{
	int result, length;
	/* The length is essentially the size of the structure minus sizeof(mtype) */
	length = sizeof(mmutosch) - sizeof(long);

	if ((result = msgrcv(qid, qbuf, length, type,  0)) == -1)
	{
		perror("Error in receiving message Scheduler read_message_mmu");
		exit(1);
	}
	return (result);
}

int main(int argc , char * argv[])
{
	int mq1_key, mq2_key, master_pid, pcbid;
	if (argc < 6) {
		printf("Scheduler rkey q2key k mpid\n");
		exit(EXIT_FAILURE);
	}
	mq1_key = atoi(argv[1]);
	mq2_key = atoi(argv[2]);
	k = atoi(argv[3]);
	master_pid = atoi(argv[4]);
	pcbid = atoi(argv[5]);

	pcbptr = (pcb*)(shmat(pcbid, NULL, 0));

	mymsgbuf msg_send, msg_recv;

	int mq1 = msgget(mq1_key, 0666);
	int mq2 = msgget(mq2_key, 0666);
	if (mq1 == -1)
	{
		perror("Message Queue1 creation failed");
		exit(1);
	}
	if (mq2 == -1)
	{
		perror("Message Queue2 creation failed");
		exit(1);
	}
	printf("Total No. of Process received = %d\n", k);

	signal(SIGUSR1, signal_handler);
	signal(SIGUSR2, signal_handler);

	//initialize the variable
	int terminated_process = 0; //to keep track of running process to exit at last

	while (1)
	{
		printf("\n======== SCH LOOP: ========\n");
		// If msgtyp is greater than 0, then the first message in the queue of type msgtyp is read
		read_message(mq1, FROMPROCESS, &msg_recv); // 10
		int curr_id = msg_recv.id;
		printf("sch read message for curr_id: %d\n", curr_id);

		msg_send.mtype = TOPROCESS + curr_id; // 20
		send_message(mq1, &msg_send); // not required after page fault
		printf("sent to mq1: %d\n", TOPROCESS+curr_id);

		int curr_pid = pcbptr[curr_id].proc_pid;
		printf("current pid: %d\n", curr_pid);
		kill(curr_pid, SIGUSR1);

		//recv messages from mmu
		mmutosch mmu_recv;
		read_message_mmu(mq2, 0, &mmu_recv);
		//printf("received %ld\n", mmu_recv.mtype);
		if (mmu_recv.mtype == PAGEFAULT_HANDLED)
		{
			msg_send.mtype = FROMPROCESS;
			msg_send.id=curr_id;
			send_message(mq1, &msg_send); // enqueue current process
		}
		
		else if (mmu_recv.mtype == TERMINATED)
		{
			terminated_process++;
			//printf("Got terminate %d\n", curr_id);
		}
		else
		{
			perror("Wrong message from mmu\n");
			exit(1);
		}
		if (terminated_process == k)
			break;
		//printf("====Term %d====\n", terminated_process);
	}
	//printf("Sending sinal\n");
	kill(master_pid, SIGUSR1);
	pause();
	printf("Scheduler terminating ...\n") ;
	exit(1) ;
}
