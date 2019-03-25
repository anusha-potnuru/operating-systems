#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <time.h>
#include <fcntl.h>
#include <errno.h>
#include <signal.h>
#include <string.h>
#include <sys/msg.h>
#include <sys/ipc.h>
#include <sys/types.h>
#include <limits.h>
#include <math.h>
#include <sys/shm.h>
#include <sys/sem.h>
#include <sys/stat.h>
#include <sys/wait.h>
#include<iostream>
#include <map>
#include<bits/stdc++.h>

#define PSEND_TYPE 10
#define MMUTOPRO 20
#define INVALID_PAGE_REF -2
#define PAGEFAULT -1
#define PROCESS_OVER -9
#define PAGEFAULT_HANDLED 5
#define TERMINATED 10

using namespace std;
int global_count = 0;
int *pffreq;
FILE *resultf;
int i;

typedef struct {
	int frameno;
	int isvalid;
	int count;
} ptbentry;

typedef struct {
	pid_t pid;
	int m;
	int f_cnt;
	int f_allo;
} pcb;

typedef struct
{
	int current;
	int flist[];
} freelist;

struct msgbuf
{
	long mtype;
	int id;
	int pageno;
};

struct mmutopbuf
{
	long mtype;
	int frameno;
};

struct mmutosch
{
	long mtype;
	char mbuf[1];
};

typedef	map< pair<int,int>,pair<int,int> > tlb;
tlb tlb_table; // tlb variable
tlb::iterator frame_it;

key_t freekey, pagetbkey;
key_t msgq2key, msgq3key;
key_t pcbkey;

int ptbid, freelid;
int msgq2id, msgq3id;
int pcbid;


int m,k;

void insert_into_tlb(int pid, int page_no, int frame_no, int s)
{
	cout<<"MMU: Inserting into tlb"<<endl;
	//If tlb is not yet full
	if(tlb_table.size() < s)
	{
		// Just insert the new pair
		pair< pair<int,int>, pair<int,int> > to_insert(pair<int,int>(pid,page_no),
			pair<int,int>(frame_no, global_count));
		tlb_table.insert(to_insert);
	}
	else{
		//If tlb is full

		auto lru_it = tlb_table.begin();
		int lru_time = -1;

		//Find the oldest entry
		for(auto it = tlb_table.begin(); it != tlb_table.end();it++){
			if( global_count - it->second.second > lru_time) {
				lru_it = it;
				lru_time = global_count - it->second.second;
			}
		}

		//Erase entry from the table
		tlb_table.erase(lru_it);
		//Make a new table entry and insert them into the  tlb
		pair< pair<int,int>, pair<int,int> > to_insert(pair<int,int>(pid,page_no),
			pair<int,int>(frame_no,global_count));
		tlb_table.insert(to_insert);
	}
}

//Remove an entry form the tlb
void remove_from_tlb(int pid,int page_no)
{
	tlb_table.erase(pair<int,int>(pid,page_no));
}

int readReq_from_process(int* id)
{
	struct msgbuf mbuf;
	int length;
	/* The length is essentially the size of the structure minus sizeof(mtype) */
	length = sizeof(struct msgbuf) - sizeof(long);
	memset(&mbuf, 0, sizeof(mbuf));

	int rst = msgrcv(msgq3id, &mbuf, length, PSEND_TYPE, 0);
	if (rst == -1)
	{
		if(errno == EINTR)
			return -1;
		perror("msgrcv");
		exit(EXIT_FAILURE);
	}
	*id = mbuf.id;
	return mbuf.pageno;
}

void sendreply_to_proc(int id, int frameno)
{
	struct mmutopbuf mbuf;
	mbuf.mtype = id + MMUTOPRO;
	mbuf.frameno = frameno;
	int length = sizeof(struct msgbuf) - sizeof(long);
	int rst = msgsnd(msgq3id, &mbuf, length, 0);
	if (rst == -1)
	{
		perror("msgsnd");
		exit(EXIT_FAILURE);
	}
}

void notifySched(int type)
{
	struct mmutosch mbuf;
	mbuf.mtype = type;
	int length = sizeof(struct msgbuf) - sizeof(long);
	int rst = msgsnd(msgq2id, &mbuf, length, 0);
	if (rst == -1)
	{
		perror("msgsnd");
		exit(EXIT_FAILURE);
	}
	//printf("Sent signal to sched = %d\n",type);
}

pcb *pcbptr;
ptbentry *ptbptr;
freelist *freeptr;

int handlePageFault(int id, int pageno)
{
	int i; // ?
	if (freeptr->current == -1 )
	{ // || pcbptr[i].f_cnt <= pcbptr[i].f_allo
		int min = INT_MAX, mini = -1;
		int victim = 0;
		for (i = 0; i < pcbptr[i].m; i++)
		{
			if (ptbptr[id * m + i].isvalid == 1)
			{
				if (ptbptr[id * m + i].count < min)
				{// lru
					min = ptbptr[id * m + i].count;
					victim = ptbptr[id * m + i].frameno;
					mini = i;
				}
			}
		}
		ptbptr[id * m + mini].isvalid = 0;
		return victim;
	}
	else
	{
		int fn = freeptr->flist[freeptr->current];
		freeptr->current -= 1;
		return fn;
	}
}

void freepages(int i)
{

	int k = 0;
	for (k = 0; k < pcbptr[i].m; i++)
	{
		if (ptbptr[i * m + k].isvalid == 1)
		{
			freeptr->flist[freeptr->current + 1] = ptbptr[i * m + k].frameno;
			freeptr->current += 1;
		}
	}
}

int serviceMRequest()
{
	pcbptr = (pcb*)(shmat(pcbid, NULL, 0));

	ptbptr = (ptbentry*)(shmat(ptbid, NULL, 0));

	freeptr = (freelist*)(shmat(freelid, NULL, 0));

	int id = -1, pageno;
	pageno = readReq_from_process(&id);
	if(pageno == -1 && id == -1)
	{
		return 0;
	}
	int i = id;
	if (pageno == PROCESS_OVER)
	{ // -9
		freepages(id);
		notifySched(TERMINATED); // type 2 message
		return 0;
	}
	int fno;
	global_count ++;
	printf("Page reference : (%d,%d,%d)\n",global_count,id,pageno);
	fprintf(resultf,"Page reference : (%d,%d,%d)\n",global_count,id,pageno);
	if (pcbptr[id].m < pageno || pageno < 0)
	{
		printf("Invalid Page Reference : (%d %d)\n",id,pageno);
		fprintf(resultf,"Invalid Page Reference : (%d %d)\n",id,pageno);
		sendreply_to_proc(id, INVALID_PAGE_REF);
		printf("Process %d: TRYING TO ACCESS INVALID PAGE REFERENCE %d\n", id, pageno);
		freepages(id);
		notifySched(TERMINATED);
		//Invalid reference

	}
	else
	{
		if(tlb_table.find(pair<int, int>(id,pageno)) != tlb_table.end())
		{//  in tlb
			int frame = tlb_table.find(pair<int, int>(id,pageno))->second.second;
			sendreply_to_proc(id , frame );
			ptbptr[i * m + pageno].count = global_count;
		} 
		else
		{// not in tlb

			if (ptbptr[i * m + pageno].isvalid == 0)
			{// not in page table
				//PAGE FAULT
				printf("Page Fault : (%d, %d)\n",id,pageno);
				fprintf(resultf,"Page Fault : (%d, %d)\n",id,pageno);
				pffreq[id] += 1;
				sendreply_to_proc(id, -1);
				fno = handlePageFault(id, pageno);
				ptbptr[i * m + pageno].isvalid = 1;
				ptbptr[i * m + pageno].count = global_count;
				ptbptr[i * m + pageno].frameno = fno;
				
				notifySched(PAGEFAULT_HANDLED);
			}
			else
			{
				sendreply_to_proc(id, ptbptr[i * m + pageno].frameno);
				ptbptr[i * m + pageno].count = global_count;
				//FRAME FOUND
			}
			insert_into_tlb(id,pageno,fno,100);
		}
	}
	if(shmdt(pcbptr) == -1)
	{
		perror("pcbptr-shmdt");
		exit(EXIT_FAILURE);
	}
	if(shmdt(ptbptr) == -1)
	{
		perror("ptbptr-shmdt");
		exit(EXIT_FAILURE);
	}
	if(shmdt(freeptr) == -1)
	{
		perror("freel-shmdt");
		exit(EXIT_FAILURE);
	}
	return 0;
}

int flag = 1;
void handletgerm(int sig)
{
	//printf("I am a god kill me\n");
	flag = 0;
}

int main(int argc, char const *argv[])
{
	if (argc < 4)
	{
		printf("mmu m2key m3key ptbkey fkey pcbkey m k\n");
		exit(EXIT_FAILURE);
	}
	msgq2id = atoi(argv[1]);
	msgq3id = atoi(argv[2]);
	ptbid = atoi(argv[3]);
	freelid = atoi(argv[4]);
	pcbid = atoi(argv[5]);
	m = atoi(argv[6]);
	k = atoi(argv[7]);
	signal(SIGUSR2, handletgerm);

	pffreq = (int *)malloc(k*sizeof(int));
	for(i=0;i<k;i++)
	{
		pffreq[i] = 0;
	} 
	resultf = fopen("result.txt","w");
	while(flag)
	{
		serviceMRequest();
	}
	printf("Page fault Count for each Process:\n");	
	fprintf(resultf,"Page fault Count for each Process:\n");
	printf("Process-Id\t Page fault Freq\n");
	fprintf(resultf,"Process Id\tFreq\n");
	for(i = 0;i<k;i++)
	{
		printf("%d\t\t\t%d\n",i,pffreq[i]);
		fprintf(resultf,"%d\t\t\t%d\n",i,pffreq[i]);
	}
	fclose(resultf);
	return 0;
}
