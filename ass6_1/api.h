#include <iostream>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <fcntl.h> 
#include <stdio.h>
#include <bits/stdc++.h>

using namespace std;

int my_open(const char* path);
int my_close(int fd);
int my_read(int fd, char *buf, int size);
int my_write(int fd, char *buf,int size, int flag);
int my_copy(int fd);
void my_cat(int fd);
void readinput(char* buffer, int len);