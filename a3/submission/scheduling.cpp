#include <iostream>
#include <bits/stdc++.h>
#include <unistd.h>
#include <ctime>
using namespace std;

int timet;

//sorting function for sorting queue using cpu burst times
bool sortbysec(const pair< int,pair<int, int> > &a, const pair< int,pair<int, int> > &b)
{
	return a.second.second<b.second.second;
}

// sorting function for highest response ratio using ratios
bool sortbyhrr(const pair< int,pair<int, int> > &a, const pair< int,pair<int, int> > &b)
{
	return double(timet-a.second.first+a.second.second)/a.second.second > (double)(timet-b.second.first+b.second.second)/b.second.second;
}

// printing function for printing process queues
void print(vector< pair< int ,pair<int, int> > > p, int n)
{
	int i;
	if(n<=0)
	{
		return;
	}
	cout<<"Process queue: ";
	for(i=0;i<n;i++)
	{
		cout<<p[i].first<<" ";
	}
	cout<<endl;
	return;
}

// first come first serve
double fcfs(vector< pair< int ,pair<int, int> > > p)
{
	int t=0,i, tat=0;
	// cout<<"In First come First serve\n";
	// t is the current time 
	// tat is the turn around time for processes calculated when they end
	for(i=0 ; i< p.size(); i++)
	{
		t = t + p[i].second.second;
		tat = tat + t - p[i].second.first;
	}
	return (double)tat/p.size();
}

double np_sjf(vector< pair< int ,pair<int, int> > > p) 
{
	int tat=0,j, t=0;
	int n= p.size();
	int i;
	// cout<<"In non pre-emptive shortest job first: \n";
	while(!p.empty())
	{
		//taking count of jobs which have arrival time less than current time
		j=0;
		while(j<p.size())
		{
			if(p[j].second.first <= t) // change
				j++;
			else
				break;
		}
		// sorting the jobs based on cpu bursts
		sort(p.begin(), p.begin()+j, sortbysec);
		// print(p,j);
		t = t + p[0].second.second;
		tat = tat + t - p[0].second.first;
		// deleting the job in queue when completed
		p.erase(p.begin());
		// print(p,j-1);		
		
	}
	return (double)tat/n;	
}

double p_sjf(vector< pair<int, pair<int, int> > > p)
{
	int time=p[0].second.first, next_arr=0, prev_begin=0, j=0;
	double tat=0;
	int n = p.size();
	// cout<<"In pre-emptive shortest job first: \n";
	
	while(!p.empty())
	{
		j=0;
		while(j< p.size())
		{
			if(p[j].second.first <= time)
				j++;
			else
				break;
		}
		if(j< p.size())
			next_arr = p[j].second.first;
		else
			next_arr = INT_MAX;
		//sorting the jobs based on cpu bursts
		sort(p.begin(), p.begin()+j, sortbysec);
		// print(p,j);
		prev_begin = time;
		time = time + p[0].second.second;
		//current time less than the next arriving job 
		while(time <= next_arr && !p.empty())
		{
			tat = tat + time - p[0].second.first;
			p.erase(p.begin());
			// print(p,--j);
			prev_begin = time;
			time = time + p[0].second.second;
		}
		//current time greater than next arrival time forcibly stop the current running job
		if(time > next_arr)
		{
			p[0].second.second -= next_arr - prev_begin;  
		}
		time = next_arr;

	}

	return tat/n;	
}

// in tat, calculated only at end, but it should be at every quantum
double roundrobin(vector< pair<int, pair<int, int> > >p)
{
	int time=0, j=0, k=0,delta=2,count=0; 
	double tat=0, next_arr;
	int n = p.size();
	// cout<<"In round robin scheduling with delta =2:"<<endl;
	vector< pair<int, pair<int, int> > > q;
	pair<int, pair<int, int> > temp;
	int g=0;
	j=0;

	while(count<n)
	{
		// cout<<"count1="<<count<<endl;
		while(j<n && p[j].second.first <= time)
		{
			q.push_back(p[j]);
			j++;
		}
		if(j<n)
			next_arr = p[j].second.first;
		else
			next_arr = INT_MAX;
		//while current time less than next arrival time we rotate the job order and run each for delta time
		// if any process ends before time=delta the cpu clock is reset
		while(time < next_arr && !q.empty())
		{
			// print(q,q.size());
			if(q[0].second.second > delta)
			{
				time+=delta;
				q[0].second.second-=delta;
				rotate(q.begin(), q.begin()+1, q.end());
			}
			else
			{
				time+=q[0].second.second;
				q.erase(q.begin());
				tat += time - q[0].second.first;
				count++;
				// cout<<"count = "<<count<<endl;
			}
			k = n-1;
			g=j;
			// pushing jobs in queue at the starting having arrival time 
			// less than current time i.e time+delta
			while(k>=g)
			{
				if(p[k].second.first <= time)
				{
					// cout<<"k= "<<k<<endl;
					q.insert(q.begin(),p[k]);
					j++;
				}
				k--;
			}
			if(j<n)
				next_arr = p[j].second.first;
			else
				next_arr = INT_MAX;
		}

	}
	return (double)tat/n;
}

double hrn(vector< pair< int ,pair<int, int> > > p)
{
	timet = p[0].second.first;
	int n= p.size(),j=0,i=0;
	double tat=0;
	// cout<<"In Highest response-ratio scheduling: \n";
	// timet is declared as variable cause it is used in sorting function to calcuate the response ratio
	while(!p.empty())
	{
		j=0;
		while(j<p.size())
		{
			if(p[j].second.first <= timet)
				j++;
			else
				break;
		}
		// print(p,j);
		// sorting the jobs in the queue based on the response ratio
		sort(p.begin(), p.begin()+j, sortbyhrr);
		timet += p[0].second.second;
		tat += timet - p[0].second.first;
		p.erase(p.begin());
	}

	return tat/n;

}

// lower bound test based on the fact that lower bound of turn around time occurs when all the processes get executed as they arrive
// without having to wait for other jobs to complete
// hence theoretical lower bound is taken as the sum of all cpu burst for a given job queue
void fcfs_lower_bound_test(vector< pair<int, pair<int, int> > > p)
{
	int lb=0;
	for (int i = 0; i < p.size(); ++i)
		{
			lb += p[i].second.second;
		}	
	double f = fcfs(p);
	cout<<"turn around time ="<<f*p.size()<<endl;
	cout<<"theoretical lower bound="<<lb<<endl;
	if((f*p.size())>=lb)
	{
		cout<<"theoretical lower bound satisified"<<endl;
	}
	else
	{
		cout<<"theoretical lower bound not satisfied"<<endl;
	}
	return ;
}

int main()
{
	int n,i,y, temp=0;
	double mean;
	cin>>n;
	double x;
	// cout<<"Enter mean:";
	cin>>mean;
	vector< pair< int ,pair<int, int> > > p;
	srand(time(NULL) + getpid()); //
	
	// cout<<"Values:\n";
	for(i=0 ; i<n; i++)
	{
		double l = ((double)rand()/RAND_MAX);
		x = -1.0*log(l)/ mean;
		y = (double)rand()/RAND_MAX * 20 +1;
		p.push_back(make_pair(i+1 , make_pair( temp ,y)));
		temp = temp + x;
		// cout <<p[i].second.first << " "<<p[i].second.second<<"\n";
	}
	// cout<<endl;
	// cout<<"first  come first serve: "<<fcfs(p)<<endl<<endl;
	// cout<<"non pre-emptive shortest job first: "<<np_sjf(p)<<endl<<endl;
	// cout<<"pre-emptive shortest job first: "<<p_sjf(p)<<endl<<endl;
	// cout<<"round robin method: "<<roundrobin(p)<<endl<<endl;
	// cout<<"Highest response ratio next: "<<hrn(p)<<endl<<endl;
	// cout<<"First come first server theoretial lower bound test:\n";
	// fcfs_lower_bound_test(p);
	// cout<<endl;
	// cout<<fcfs(p)<<" "<<np_sjf(p)<<" "<<p_sjf(p)<<" "<<roundrobin(p)<<" "<<hrn(p);
	fcfs_lower_bound_test(p);

	return 0;
}