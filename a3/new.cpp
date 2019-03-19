#include <iostream>
#include <algorithm>
#include <bits/stdc++.h> 
using namespace std;
int main()
{
	vector<int> p;
	p.push_back(1);
	p.push_back(2);
	p.push_back(3);
	rotate(p.begin(), p.begin()+1, p.end());
	for (int i = 0; i < p.size(); ++i)
	{
		cout<<p[i];
	}

	return 0;
}