#include <bits/stdc++.h>
#include <cstdlib>
#include <unistd.h>
using namespace std;

#define M 300

int *buffer;
int *status;
int bcount;
int in,out;
int n,totp,curr_thr;

void signal_sleep(int signum){
	// signal(SIGUSR1,signal_sleep);	// Signal to put a thread to sleep
	pause();
}

void signal_wake(int signum){
	// signal(SIGUSR2,signal_wake);		// Signal to wake up a thread
}

void* produce(void* param)
{
	long curr_thread_ind;
	curr_thread_ind = (long)param;
	status[curr_thread_ind] = 0;
	signal(SIGUSR1,signal_sleep); //installing signal handler
	signal(SIGUSR2,signal_wake); //installing signal handler

	pthread_kill(pthread_self(),SIGUSR1);

	for (int i = 0; i < 1000; ++i)
	{
		while(bcount == M);
		buffer[in] = rand()%100+1;
		in = (in+1)%M;
		bcount++;
	}

	totp--;
	status[curr_thread_ind] = -1;	// change status of thread to exit
	pthread_exit(NULL);

}

void* consume(void* param)
{

	long curr_thread_ind;
	curr_thread_ind = (long)param;
	status[curr_thread_ind] = 0;
	signal(SIGUSR1,signal_sleep);
	signal(SIGUSR2,signal_wake);
	pthread_kill(pthread_self(),SIGUSR1);

	while(1)
	{
		while(bcount==0);
		out = (out+1)%M;
		bcount--;
	}
	status[curr_thread_ind] = -1; // change status of thread to exit
	pthread_exit(NULL);

}


/*
	Scheduler thread
*/
void *scheduler(void *param){	
	int flag = 0;		
	time_t start_time,rawtime;
	
	time(&rawtime);
	printf("Scheduler thread started at time %s\n",ctime(&rawtime));
	
	while(totp || bcount)
	{													// While total waiting threads < n
		usleep(20);
		if(status[curr_thr]!=-1)
		{			
			pthread_kill(*((pthread_t *)param+curr_thr), SIGUSR2);		// wake up the current thread

			status[curr_thr] = 1;										// change status of current thread
						
			start_time = time(NULL);
			while( time(NULL)-start_time < 1 ) 
			{													
				if(status[curr_thr]==-1)
				{							
					flag = 1;								//	Allow current thread to execute for 1 second
					break;								
				}							
			}
			
			if(flag==0)
			{
				pthread_kill(*((pthread_t *)param+curr_thr),SIGUSR1);	// if thread has not terminated then put the thread to sleep
				status[curr_thr] = 0;
				
			}
			else 
				flag = 0;

			// cout<<"Number of elements in BUFFER: "<<bcount<<endl;
			// cout<<"in: "<<in<<" "<<"out: "<<out<<endl;
		}
		
		curr_thr = (curr_thr+1)%n;											// switch to next thread
	}

	time(&rawtime);
	printf("\nScheduler thread exiting at time %s\n",ctime(&rawtime));
	pthread_exit(NULL);
}

void *reporter(void *param)
{
	time_t rawtime;
	int old_status[n],flag_terminated,flag_suspended,flag_resume,resumed_thread,suspended_thread,terminated_thread;
	for(int i=0;i<n;i++)
		old_status[i] = 0;
		
	flag_terminated = 0;
	flag_suspended = 0;
	flag_resume = 0;
	while(totp || bcount)
	{															// While total waiting threads < n
		for(int i=0;i<n;i++)
		{
			if(status[i]!=old_status[i])
			{// If status of any thread has changed

				if(status[i]==-1 && old_status[i]==1)
				{
					terminated_thread = i;
					cout<<"Thread "<<terminated_thread<<" has finished"<<endl;	// keep record of terminated thread
					flag_terminated = 1;
				}
				else{
					if(status[i]==0 && old_status[i]==1)
					{
						suspended_thread = i;								// keep record of suspended thread
						flag_suspended = 1;
					}
					else{
						if(status[i]==1 && old_status[i]==0){
							if((flag_suspended==1 || flag_terminated==1)){
								resumed_thread = i;							// keep record of resumed thread
								flag_resume = 1;
							}
						}
					}
				}
				
				old_status[i] = status[i];									// update old status
			}
		}

		// Printing the context switch information
		if(flag_suspended==1 && flag_resume==1){
			time(&rawtime);
			cout<<"Thread "<<suspended_thread<<" suspended and Thread "<<resumed_thread<<" resumed at time "<<ctime(&rawtime)<<endl;
			flag_suspended = 0;
			flag_resume = 0;

			cout<<"Number of elements in BUFFER: "<<bcount<<endl;
			cout<<"in: "<<in<<" "<<"out: "<<out<<endl;
		}
		
		// Printing thread termination information
		if(flag_terminated==1 && flag_resume==1){
			time(&rawtime);
			cout<<"Thread "<<resumed_thread<<" resumed at time "<<ctime(&rawtime)<<endl;	
			flag_terminated = 0;
			flag_resume = 0;

			cout<<"Number of elements in BUFFER: "<<bcount<<endl;
			cout<<"in: "<<in<<" "<<"out: "<<out<<endl;
		}
		
	}

	for(int i=0;i<n;i++)
	{
		if(status[i]==-1 && (old_status[i]==1 || old_status[i]==0))
		{
			cout<<"Thread "<<i<<" has finished";

			cout<<"Number of elements in BUFFER: "<<bcount<<endl;
			cout<<"in: "<<in<<" "<<"out: "<<out<<endl;
			break;
		}

	}

	time(&rawtime);
	cout<<"\nReporter thread exiting at time "<<ctime(&rawtime)<<endl;
	pthread_exit(NULL);
}

int main()
{
	srand(time(NULL));

	cout << "Enter number of threads n: ";
	cin>>n;

	int x;
	totp = 0;
	curr_thr = 0;
	bcount=0;
	in =0;
	out=0;
	status = new int[n];
	buffer = new int[M];
	
	pthread_t worker_threads[n], scheduler_thread, reporter_thread;	// Declare all the threads
	pthread_attr_t attr;
	pthread_attr_init(&attr);
	cout<<"Producer threads: ";
	for(int i=0;i<n;i++)
	{
		x = rand() %2;
		if(x==1)
		{
			cout<<i<<" ";
			totp++;
			pthread_create(&worker_threads[i], &attr ,produce,(void *)(size_t)i);	
		}	
		else
			pthread_create(&worker_threads[i], &attr, consume, (void*)(size_t)i);
	}
	cout<<endl;
		
	pthread_create(&scheduler_thread,&attr,scheduler,(void *)worker_threads);	
	pthread_create(&reporter_thread,&attr,reporter,NULL);						
	
	pthread_join(scheduler_thread, NULL);
	pthread_join(reporter_thread, NULL);
	
	return 0;
}
