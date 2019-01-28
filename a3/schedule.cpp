#include <iostream>
#include <bits/stdc++.h>

using namespace std;

bool sortbysec(const pair<int, int> &a, const pair<int, int> &b)
{
	return a.second<b.second;
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
			if(p[j].first <= tat)
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



int main()
{
	int n, mean,i,y, temp=0;
	cin>>n;
	double x;
	cout<<"Enter mean:";
	cin>>mean;
	vector< pair<int, int> > p;
	
	for(i=0 ; i<n; i++)
	{
		double l = ((double)rand()/RAND_MAX);
		x = -log(l)/ mean;
		y = (double)rand()/RAND_MAX * 20 +1;
		p.push_back(make_pair( temp ,y));
		temp = temp + x;
		//cout<<l<<" "<<log(l)<< " ";
		cout <<temp << " "<<y<<"\n";
	}
	
	//cout<<fcfs(p);
	cout<<np_sjf(p);
	return 0;
}
