#include <iostream>
#include <bits/stdc++.h>
#include <string>
#include <fstream>
#include <unistd.h>

using namespace std;


int timet;

bool sortbysec(const pair<int, int> &a, const pair<int, int> &b)
{
	return a.second<b.second;
}

bool sortbyhrr(const pair<int, int> &a, const pair<int, int> &b)
{
	return double(timet-a.first+a.second)/a.second > (double)(timet-b.first+b.second)/b.second;
}
void printprocessqueue(vector<pair<int, pair<int,int> > > p)
{
	int i;
	cout<<"Process queue: ";
	for (i = 0; i < p.size(); ++i)
	{
		cout<<p[i].first<<" ";
	}
	cout<<"\n";
	return;
}

double fcfs(vector< pair<int,int> > p)
{
	int t=0,i, tat=0;
	for(i=0 ; i< p.size(); i++)
	{
		t = t + p[i].second;
		tat = tat + t - p[i].first;
	}
	return (double)tat/p.size();
}

double np_sjf(vector< pair<int,int> > p) 
{
	int tat=0,j, t=0;
	int n= p.size();
	
	while(!p.empty())
	{
		j=0;
		while(j<p.size())
		{
			if(p[j].first <= t) // time
				j++;
			else
				break;
		}
		sort(p.begin(), p.begin()+j, sortbysec);
		t = t + p[0].second;
		tat = tat + t - p[0].first;
		p.erase(p.begin());		
		
	}
	return (double)tat/n;

}

double p_sjf(vector< pair<int,int> > p)
{
	int time=p[0].first, next_arr=0, prev_begin=0, j=0;
	double tat=0;
	int n = p.size();
	while(!p.empty())
	{
		j=0;
		while(j< p.size())
		{
			if(p[j].first <= time)
				j++;
			else
				break;
		}

		if(j< p.size())
			next_arr = p[j].first;
		else
			next_arr = 10000;

		sort(p.begin(), p.begin()+j, sortbysec);

		prev_begin = time;
		time = time + p[0].second;
		
		while(time <= next_arr && !p.empty())
		{
			tat = tat + time - p[0].first;
			// cout<<time<<endl;
			// cout<<"tat:"<<time - p[0].first<<endl;
			p.erase(p.begin());
			prev_begin = time;
			time = time + p[0].second;
		}

		if(time > next_arr)
		{
			p[0].second -= next_arr - prev_begin;  
		}
		time = next_arr;

	}

	return tat/n;	
}


double roundrobin(vector< pair<int, pair<int, int> > >p)
{
	int time=0, j=0, k=0,delta=2,count=0; 
	double tat=0, next_arr;
	int n = p.size();
	vector< pair<int, pair<int, int> > > q;
	pair<int, pair<int, int> > temp;
	int g=0;
	j=0;

	while(count<n)
	{
		cout<<"count1="<<count<<endl;
		while(j<n && p[j].second.first <= time)
		{
			q.push_back(p[j]);
			j++;
		}
		if(j<n)
			next_arr = p[j].second.first;
		else
			next_arr = INT_MAX;
		while(time < next_arr && !q.empty())
		{
			printprocessqueue(q);
			if(q[0].second.second > delta)
			{
				time+=delta;
				q[0].second.second-=delta;
				rotate(q.begin(), q.begin()+1, q.end());
				k = n-1;
				g=j;
				while(k>=g)
				{
					if(p[k].second.first <= time)
					{
						cout<<"k= "<<k<<endl;
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
			else
			{
				time+=q[0].second.second;
				q.erase(q.begin());
				tat += time - q[0].second.first;
				count++;
				cout<<"count = "<<count<<endl;
				k = n-1;
				g=j;
				while(k>=g)
				{
					if(p[k].second.first <= time)
					{
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

	}
	return (double)tat/n;
}

double hrn(vector< pair<int,int> > p)
{
	timet = p[0].first;
	int n= p.size(),j=0;
	double tat=0;
	while(!p.empty())
	{
		j=0;
		while(j<p.size())
		{
			if(p[j].first <= timet)
				j++;
			else
				break;
		}

		sort(p.begin(), p.begin()+j, sortbyhrr);
		timet += p[0].second;
		tat += timet - p[0].first;
		// cout<<timet - p[0].first<<endl;	
		p.erase(p.begin());
	}

	return tat/n;

}



int main()
{ // first arrival time, second cpu burst
	int n,i,y, temp1=0,temp2=0, j=0, temp=0;
	double mean;
	double x;
	vector< pair<int, int> > p;
	// mean = 0.5;

	// cout<<"Enter n: ";
	cin>>n;

	// cout<<"Enter mean: ";
	cin>>mean;
	
	srand(time(NULL) * getpid());
	
	
	for(i=0 ; i<n; i++)
	{
		double l = ((double)rand()/RAND_MAX);
		x = -log(l)/ mean;
		y = (double)rand()/RAND_MAX * 20 +1;
		p.push_back(make_pair( temp ,y));
		temp = temp + x;
	}


	ifstream infile;
	infile.open("input1.txt");
	temp=0;
	while(infile>>temp1>>temp2)
	{
		// cout<<temp1<<" "<<temp2<<"\n";
		p.push_back(make_pair(temp, temp2));
		temp = temp+temp1;
	}

	// cout<<fcfs(p)<<" "<<np_sjf(p)<<" "<<p_sjf(p)<<" "<<roundrobin(p)<<" "<<hrn(p)<<endl;

	// cout<<"fcfs: "<<fcfs(p)<<endl;
	// cout<<"np sjf: "<<np_sjf(p)<<endl;
	// cout<<"p sjf: "<<p_sjf(p)<<endl;
	cout<<"roundrobin "<<roundrobin(p)<<endl;
	// cout<<"hrn: "<<hrn(p)<<endl;


	return 0;
}
