#include <iostream>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <fcntl.h> 
#include <stdio.h>
#include <bits/stdc++.h>
#include "api.h"

using namespace std;

int main()
{
	char *buffer;
	buffer = (char*)malloc(sizeof(char)*150);
	cout<<"enter buffer"<<endl;

	// cin.ignore();
	// cin.clear();

	readinput(buffer, 150);

	// scanf("%[^\n]%*c",buffer);
	cout<<"entered buffer is = "<<buffer<<endl;
	cout<<strlen(buffer)<<endl;
	int fd = my_open("abc.txt");
	cout<<"fd of file is "<<fd<<endl;
	int size = my_write(fd,buffer,strlen(buffer),0);
	my_cat(fd);
	size = my_read(fd,buffer,60);
	cout<<"reading 60 bytes\n"<<buffer<<endl;
	cout<<strlen(buffer)<<endl;
	size = my_copy(fd);

	int fd2 = my_open("abc.txt");
	cout<<"\nfd of file is "<<fd2<<endl;
	// size = my_close(fd);
	// cout<<"after closing"<<endl;
	size = my_read(fd2,buffer,60);
	cout<<"buffer after second time open\n";
	cout<<buffer<<endl;
	
	int fd1 = my_open("file.txt");
	cout<<"\nfd of file is "<<fd1<<endl;
	cout<<"enter buffer"<<endl;
	readinput(buffer, 150);
	cout<<buffer<<endl;
	int size1 = my_write(fd1,buffer,strlen(buffer),1);
	cout<<"after cat"<<endl;
	my_cat(fd1);
	size1 = my_read(fd1,buffer,60);
	cout<<"reading 60 bytes\n"<<buffer<<endl;
	size1 = my_copy(fd1);
	size1 = my_close(fd1);
	cout<<size1;
	cout<<"after closing"<<endl;
	size1 = my_read(fd1,buffer,60);
	cout<<size1;
	size = my_close(fd2);
	cout<<"after closing"<<endl;
	size = my_close(fd);
	return 0;
}