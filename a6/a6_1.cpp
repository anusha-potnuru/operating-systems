#include <iostream>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <fcntl.h> 
using namespace std;

int file_system_size,block_size;
int no_of_blocks;
int f=0;
int free_ptr;

typedef struct 
{
	char *b_data;
}block;

typedef struct directory
{
	char file_name[50];
	int block_no;
	int fd;
	struct directory *next;
}directory;

typedef struct
{
	int disk_block_size;
	int file_system_size;
	char volume_name[50];
	directory *d;
	bool bit_vector[];
}superblock;

typedef struct fat
{
	int *arr;
}fat;

superblock* sb;
block* file_system;
fat* block1;

int min(int a, int b)
{
	return a>b?b:a;
}


int find_free_block()
{
	while(1)
	{
		if(sb->bit_vector[free_ptr]==0)
		{
			break;
		}
		else
		{
			free_ptr = (free_ptr+1)%no_of_blocks;
		}
	}
	return free_ptr;
}


int my_open(char* path)
{
	directory *temp = sb->d;
	directory *prev = sb->d;
	// int fd = open(path, O_WRONLY| O_CREAT|O_TRUNC,S_IRWXU);
	while(temp!=NULL)
	{
		if(strcmp(temp->file_name,path)==0)
		{
			return temp->fd;
		}
		prev = temp;
		temp =temp->next;
	} 
	directory *node = (directory*)malloc(sizeof(directory));
	// node->file_name = (char*)malloc(sizeof(char)*)
	strcpy(node->file_name,path);
	// doubt
	node->block_no = -1; 
	node->fd = f;
	// to increase value of fd for next file
	f++;
	node->next=NULL;
	prev->next = node;
	return fd;
}

int my_close(int fd)
{
	directory *temp = sb->d;
	directory *prev = sb->d;
	// int fd = open(path, O_WRONLY| O_CREAT|O_TRUNC,S_IRWXU);
	while(temp!=NULL)
	{
		if(temp->fd==fd)
		{
			break;
		}
		prev = temp;
		temp =temp->next;
	} 
	if(temp!=NULL)
	{
		prev->next = temp->next;
		free(temp);
	}
	return close(fd);
}

int my_read(int fd, char *buf, int size)
{
	directory *temp = sb->d;
	// directory *prev = sb->d;
	// int fd = open(path, O_WRONLY| O_CREAT|O_TRUNC,S_IRWXU);
	while(temp!=NULL)
	{
		if(temp->fd==fd)
		{
			break;
		}
		// prev = temp;
		temp =temp->next;
	} 
	int bn = temp->block_no;
	if(bn==-1)
	{
		return 0;
	}

	int ret = 0;
	while(size>block_size)
	{
		int minsize = min(block_size,strlen(file_system[bn].b_data));
		strncat(buf,file_system[bn].b_data,size%block_size);
		ret += minsize;
		size -= minsize;
		if(block1->arr[bn]==-1)
		{
			break;	
		}
		bn = block1->arr[bn];
	}
	if(size%block_size>0)
	{
		int minsize = min(size%block_size,strlen(file_system[bn].b_data));
		strncat(buf,file_system[bn].b_data,size%block_size);
		ret += minsize;
	}
	return ret;
}

// 1 -append 0-overwrite
int my_write(int fd, char *buf,int size, int flag)
{
	directory *temp = sb->d;
	while(temp!=NULL)
	{
		if(temp->fd==fd)
		{
			break;
		}
		// prev = temp;
		temp =temp->next;
	} 
	int bn = temp->block_no;
	int prev;
	if(flag==1)
	{
		while(bn!=-1)
		{
			prev = bn;
			bn = block1->arr[bn];
		}
		block1->arr[prev] = bn = find_free_block();
	}
	else if(flag==0)
	{
		while(bn!=-1)
		{
			file_system[bn].b_data = '\0';
			bn = block1->arr[bn];
		}
		bn = find_free_block();
		temp->block_no = bn;
	}
	else
	{
		cout<<"invalid flag";
		return -1;
	}
	while(size>block_size)
	{
		strncpy(buf,file_system[bn].b_data,block_size);
		sb->bit_vector[bn]=1;
		bn = find_free_block();
		size -= block_size;
	}
	strncpy(buf,file_system[bn].b_data,size);
	sb->bit_vector[bn]=1;
	return size;
}

int my_copy(int fd)
{
	directory *temp = sb->d;
	char* path;
	strcpy(path, temp->file_name);
	int linux_fd = open(path, O_WRONLY| O_CREAT|O_TRUNC,S_IRWXU);
	while(temp!=NULL)
	{
		if(temp->fd==fd)
		{
			break;
		}
		temp =temp->next;
	} 
	int bn = temp->block_no;
	while(bn!=-1)
	{
		write(linux_fd, file_system[bn].b_data, strlen(file_system[bn].b_data));
		bn = block1->arr[bn];
	}
	return linux_fd;
}

int my_cat(int fd)
{
	directory *temp = sb->d;
	int linux_fd = open(path, O_WRONLY| O_CREAT|O_TRUNC,S_IRWXU);
	while(temp!=NULL)
	{
		if(temp->fd==fd)
		{
			break;
		}
		temp =temp->next;
	} 
	int bn = temp->block_no;
	while(bn!=-1)
	{
		write(linux_fd, file_system[bn].b_data, strlen(file_system[bn].b_data));
		bn = block1->arr[bn];
	}
	return linux_fd;
}

int main()
{
	int i;
	cin>>file_system_size>>block_size;
	no_of_blocks = file_system_size/block_size;

	// int fat[no_of_blocks];
	
	file_system = (block*)malloc(sizeof(block)*no_of_blocks); 
	for(i=0;i<no_of_blocks;i++)
	{
		file_system[i].b_data = (char*)malloc(sizeof(char)*block_size);
	}

	
	block1 = (fat*)malloc(sizeof(fat));
	block1->arr = (int*)malloc(sizeof(int)*no_of_blocks);
	block1 = (fat*)&(file_system[1]);
	directory *dir = (directory*)malloc(sizeof(directory));
	// creating superblock
	sb = (superblock*)malloc(sizeof(superblock)+sizeof(bool)*no_of_blocks);
	sb = (superblock*)&file_system[0];
	sb->file_system_size = file_system_size;
	sb->disk_block_size = block_size;
	sb->d = dir;

	return 0;
}