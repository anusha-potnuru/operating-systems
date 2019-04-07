#include <iostream>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <fcntl.h> 
#include <string>
#include <stdio.h> 
using namespace std;

int file_system_size,block_size;
int no_of_blocks;
int f=0;
int free_ptr=0;

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
	// cout<<"free block returned"<<endl;
	return free_ptr;
}


int my_open(const char *path)
{
	directory *temp = sb->d;
	directory *prev = sb->d;
	cout<<"in my open"<<endl;
	directory *node = (directory*)malloc(sizeof(directory));
	// int fd = open(path, O_WRONLY| O_CREAT|O_TRUNC,S_IRWXU);
	if(prev == NULL)
	{
		cout<<"head node"<<endl;
		strcpy(node->file_name,path);
		node->block_no = -1; 
		node->fd = f;
		f++;
		node->next=NULL;
		sb->d = node;
	}
	else
	{
		while(temp!=NULL)
		{
			if(strcmp(temp->file_name,path)==0)
			{
				return temp->fd;
			}
			prev = temp;
			temp =temp->next;
		} 
		// directory *node = (directory*)malloc(sizeof(directory));
		// node->file_name = (char*)malloc(sizeof(char)*)
		strcpy(node->file_name,path);
		// doubt
		node->block_no = -1; 
		node->fd = f;
		// to increase value of fd for next file
		f++;
		node->next=NULL;
		prev->next = node;
	}
	return node->fd;
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
	memset(buf, 0, sizeof(buf));
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
		strncat(buf,file_system[bn].b_data, minsize);
		// buf = buf+minsize;
		ret += minsize;
		size -= minsize;
		if(block1->arr[bn]==-1)
		{
			return ret;
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
int my_write(int fd, char *buf, int size, int flag)
{
	cout<<"in my write"<<endl;
	directory *temp = sb->d;

	while(temp!=NULL)
	{		
		if(temp->fd==fd)
		{
			break;
		}
		temp =temp->next;
	}
	cout<<temp->fd<<" "<<endl;
	if(!temp)
		cout<<"error temp is null"<<endl; 
	int bn = temp->block_no;
	int prev;
	if(flag==1)
	{
		while(bn!=-1)
		{
			prev = bn;
			bn = block1->arr[bn];
		}
		
		bn = find_free_block();
		cout<<"free block found"<<endl;
		cout<<"prev is :"<<prev<<endl;		
		if(temp->block_no==-1)
		{
			temp->block_no=bn;
		}
		else
		{
			block1->arr[prev] = bn;
		}
		cout<<"free block found "<<bn<<" ";

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
		cout<<"free block found "<<bn<<" ";
	}
	else
	{
		cout<<"invalid flag";
		return -1;
	}
	cout<<"block number "<<bn<<endl;
	while(size>block_size)
	{
		cout<<"size = "<<size<<endl;
		cout<<"buffer = "<<buf<<endl;
		strncpy(file_system[bn].b_data,buf,block_size);
		cout<<file_system[bn].b_data;
		buf = buf+block_size;
		cout<<"done"<<endl;

		sb->bit_vector[bn]=1;
		block1->arr[bn] = find_free_block();
		bn = block1->arr[bn];
		cout<<"block number "<<bn<<endl;
		size -= block_size;
	}
	strncpy(file_system[bn].b_data,buf,size);
	sb->bit_vector[bn]=1;
	block1->arr[bn]=-1;
	cout<<"exiting"<<endl;
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

void my_cat(int fd)
{
	directory *temp = sb->d;
	while(temp!=NULL)
	{
		if(temp->fd==fd)
		{
			break;
		}
		temp =temp->next;
		cout<<temp->fd<<" ";
	} 
	int bn = temp->block_no;
	cout<<"block number "<<bn<<endl;
	while(bn!=-1)
	{
		cout<<file_system[bn].b_data;
		bn = block1->arr[bn];
	}
	cout<<endl;
	return;
}


void readinput(char* buffer, int len)
{
	char c;
	int i=0;
	while(i<len-1)
	{
		scanf("%c", &c);
		cout<<(int)c<<" ";
		cout<<c;
		if(c=='\n')
		{
			break;
		}
		buffer[i++]=c;
	}	
	buffer[i]='\0';
	cout<<"returned"<<endl;
	return;
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
	// directory *dir = (directory*)malloc(sizeof(directory));
	// creating superblock
	sb = (superblock*)malloc(sizeof(superblock)+sizeof(bool)*no_of_blocks);
	sb = (superblock*)&file_system[0];
	sb->file_system_size = file_system_size;
	sb->disk_block_size = block_size;
	sb->d = NULL;

	sb->bit_vector[0]=-1;
	sb->bit_vector[1]=-1;

	int fd = my_open("abc.txt");
	cout<<"fd of file is "<<fd;
	char *buffer;
	buffer = (char*)malloc(sizeof(char)*150);
	cout<<"enter buffer"<<endl;

	cin.ignore();
	cin.clear();

	readinput(buffer, 150);
	// cin>>buffer;

	// string str;
	// getline(cin, str);
	// strcpy(buffer, str.c_str());

	// gets(buffer);
	// scanf("%[^\n]*c",buffer);

	cout<<"BUFFER:"<<endl;
	cout<<buffer<<endl;
	cout<<"string length of buffer"<<strlen(buffer)<<endl;
	int size = my_write(fd,buffer,strlen(buffer),1);
	my_cat(fd);
	my_read(fd, buffer, 70);

	cout<<"printing buffer"<<endl<<buffer<<endl;
	return 0;
}