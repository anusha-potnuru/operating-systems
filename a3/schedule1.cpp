#include <iostream>
#include <bits/stdc++.h>
#include <string>
#include <fstream>
#include <unistd.h>

using namespace std;


int timet;

bool sortbysec(const pair<int, pair<int, int> > &a, const pair<int, pair<int, int> > &b)
{
	return a.second.second<b.second.second;
}

bool sortbyhrr(const pair<int, pair<int, int> > &a, const pair<int, pair<int, int> > &b)
{
	return double(timet-a.second.first+a.second.second)/a.second.second > (double)(timet-b.second.first+b.second.second)/b.second.second;
}

void printprocessqueue(vector<pair<int, pair<int,int> > > p,int n)
{
	int i;
	cout<<"Process queue: ";
	for (i = 0; i < n; ++i)
	{
		cout<<p[i].first<<" ";
	}
	cout<<"\n";
	return;
}

double fcfs(vector< pair<int, pair<int,int> > > p)
{
	int t=0,i, tat=0;
	for(i=0 ; i< p.size(); i++)
	{
		t = t + p[i].second.second;
		tat = tat + t - p[i].second.first;
	}
	return (double)tat/p.size();
}

double np_sjf(vector< pair<int, pair<int, int> > > p) 
{
	int tat=0,j, t=0,i;
	int n= p.size();
	
	while(!p.empty())
	{
		j=0;
		while(j<p.size())
		{
			if(p[j].second.first <= t) // time
				j++;
			else
				break;
		}
		sort(p.begin(), p.begin()+j, sortbysec);
		printprocessqueue(p,j);
		t = t + p[0].second.second;
		tat = tat + t - p[0].second.first;
		p.erase(p.begin());
		printprocessqueue(p,j-1);
	}
	return (double)tat/n;

}

double p_sjf(vector< pair<int, pair<int, int> > > p)
{
	int time=p[0].second.first, next_arr=0, prev_begin=0, j=0;
	double tat=0;
	int n = p.size();
	int t;
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

		sort(p.begin(), p.begin()+j, sortbysec);
		printprocessqueue(p,j);
		prev_begin = time;
		time = time + p[0].second.second;
		
		while(time <= next_arr && !p.empty())
		{
			tat = tat + time - p[0].second.first;
			p.erase(p.begin());
			printprocessqueue(p,--j);
			prev_begin = time;
			time = time + p[0].second.second;
		}

		if(time > next_arr)
		{
			p[0].second.second -= next_arr - prev_begin;  
		}
		time = next_arr;

	}

	return tat/n;	
}


// double roundrobin(vector< pair<int, pair<int, int> > >p)
// {
// 	int time=0, i=0,j=0, delta=2,k,l; 
// 	double tat=0, next_arr;
// 	int n = p.size();
// 	while(!p.empty())
// 	{
// 		j=0;
// 		while(j< p.size())
// 		{
// 			if(p[j].second.first <= time)
// 				j++;
// 			else
// 				break;
// 		}
// 		if(j< p.size())
// 			next_arr = p[j].second.first;
// 		else
// 			next_arr = 10000;

		
// 		cout<<k<<endl;
// 		while(time < next_arr && !p.empty())
// 		{

// 			printprocessqueue(p);
// 			if(p[0].second.second > delta)
// 			{
// 				time = time + delta;
// 				p[0].second.second -= delta;
// 				k=0;
// 				l=0;
// 				while( k<p.size())
// 				{
// 					if(l==0 && p[k].second.first == time)
// 						l=k;
// 					if(p[k].second.first <= time)
// 						k++;
// 					else
// 						break;
// 				}
// 				if(!l)
// 					rotate(p.begin(), p.begin()+1, p.begin()+l);
// 				rotate(p.begin(), p.begin()+l, p.begin()+k);
// 			}

// 			else
// 			{
// 				time += p[0].second.second;
// 				tat += time - p[0].second.first;
// 				p.erase(p.begin());				
// 				j--; // count till where to run round robin
// 			}

// 		}

// 	}
// 	return (double)tat/n;
// }


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
			printprocessqueue(q,q.size());
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

void fcfs_test(vector< pair<int, pair<int, int> > > p)
{
	int lb=0;
	for (int i = 0; i < p.size(); ++i)
		{
			lb += p[i].second.second;
		}	
	double f = fcfs(p);
	cout<<"f="<<f<<endl;
	cout<<"lb="<<lb<<endl;
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

double hrn(vector< pair<int, pair<int, int> > > p)
{
	timet = p[0].second.first;
	int n= p.size(),j=0;
	double tat=0;
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

		sort(p.begin(), p.begin()+j, sortbyhrr);
		printprocessqueue(p,j);
		timet += p[0].second.second;
		tat += timet - p[0].second.first;
		p.erase(p.begin());
		printprocessqueue(p,j-1);
	}

	return tat/n;

}



int main()
{   // first arrival time, second cpu burst
	int n,i,y, temp1=0,temp2=0, j=0, temp=0;
	double mean;
	double x;
	vector< pair<int, pair<int, int> > > p;

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
		p.push_back(make_pair(i+1,make_pair( temp ,y)));
		cout<<temp<<" "<<y<<endl;
		temp = temp + x;
	}

	// ifstream infile;
	// infile.open("input1.txt");
	// temp=0;
	// i=0;
	// while(infile>>temp1>>temp2)
	// {
	// 	// cout<<temp1<<" "<<temp2<<"\n";
	// 	p.push_back(make_pair(i,make_pair(temp, temp2)));
	// 	temp = temp+temp1;
	// 	i++;
	// }

	// cout<<fcfs(p)<<" "<<np_sjf(p)<<" "<<p_sjf(p)<<" "<<roundrobin(p)<<" "<<hrn(p)<<endl;

	cout<<"fcfs: "<<fcfs(p)<<endl;
	cout<<"np sjf: "<<np_sjf(p)<<endl;
	cout<<"p sjf: "<<p_sjf(p)<<endl;
	cout<<"roundrobin "<<roundrobin(p)<<endl;
	fcfs_test(p);
	cout<<"hrn: "<<hrn(p)<<endl;

	return 0;
}
