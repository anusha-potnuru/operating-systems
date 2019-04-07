#include <iostream>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <fcntl.h> 
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
	int bn,next1;
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
		bn = temp->block_no;
		while(bn!=-1)
		{
			sb->bit_vector[bn]=0;
			memset(file_system[bn].b_data,'\0',block_size);
			next1 = bn;
			bn = block1->arr[bn];
			block1->arr[next1]=-1;
		}
		prev->next = temp->next;
		temp->block_no = -1;
		free(temp);
	}
	else
	{
		return -1;
	}
	return 0;
}

int my_read(int fd, char *buf, int size)
{
	directory *temp = sb->d;
	memset(buf ,'\0',size);
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
	if(!temp)
	{
		return 0;
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
		strncat(buf,file_system[bn].b_data,minsize);
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
int my_write(int fd, char *buf,int size, int flag)
{
	cout<<"in my write"<<endl;
	directory *temp = sb->d;

	while(temp!=NULL)
	{
		cout<<temp->fd<<" ";
		if(temp->fd==fd)
		{
			break;
		}
		temp =temp->next;
	} 
	int bn = temp->block_no;
	cout<<"bn = "<<bn;
	int prev;
	if(flag==1)
	{
		while(bn!=-1)
		{
			prev = bn;
			bn = block1->arr[bn];
		}
		
		bn = find_free_block();
		cout<<"free block found "<<bn<<" "<<endl;
		if(temp->block_no==-1)
		{
			temp->block_no=bn;
			cout<<"checking block number = "<<temp->block_no<<endl;
		}
		else
		{
			block1->arr[prev] = bn;
		}
		
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
	cout<<"size  = "<<size<<endl;
	while(size>block_size)
	{
		cout<<"size = "<<size<<endl;
		cout<<"buffer = "<<buf<<endl;
		strncpy(file_system[bn].b_data,buf,block_size);
		cout<<file_system[bn].b_data<<endl;
		buf = buf+block_size;
		cout<<"done"<<endl;
		sb->bit_vector[bn]=1;
		block1->arr[bn] = find_free_block();
		bn = block1->arr[bn];
		cout<<"block number "<<bn<<endl;
		size -= block_size;
	}
	strncpy(file_system[bn].b_data,buf,size);
	cout<<file_system[bn].b_data<<endl;
	sb->bit_vector[bn]=1;
	block1->arr[bn] = -1;
	cout<<"exiting"<<endl;
	return size;
}

int my_copy(int fd)
{
	directory *temp = sb->d;
	char path[150];

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
	if(!temp)
		cout<<"error\n"; 
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
		// cout<<"block number "<<bn<<endl;
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
		// cout<<(int)c<<" ";
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
	
	// cout<<"fd of file is "<<fd;
	char *buffer;
	buffer = (char*)malloc(sizeof(char)*150);
	cout<<"enter buffer"<<endl;

	cin.ignore();
	cin.clear();

	readinput(buffer, 150);

	// scanf("%[^\n]%*c",buffer);
	cout<<"buf="<<buffer<<endl;
	cout<<"done"<<endl;
	int fd = my_open("abc.txt");
	cout<<"fd of file is "<<fd<<endl;
	int size = my_write(fd,buffer,strlen(buffer),1);
	my_cat(fd);
	size = my_read(fd,buffer,60);
	cout<<"after reading\n"<<buffer<<endl;
	size = my_copy(fd);
	size = my_close(fd);
	// cout<<size;
	cout<<"after closing"<<endl;
	size = my_read(fd,buffer,60);
	cout<<size;
	// int fd1 = my_open("file.txt");
	// cout<<"fd of file is "<<fd1<<endl;
	// cout<<"enter buffer"<<endl;

	// // cin.ignore();
	// // cin.clear();

	// readinput(buffer, 150);
	// cout<<buffer<<endl;
	// int size1 = my_write(fd1,buffer,strlen(buffer),1);
	// cout<<"after cating"<<endl;
	// my_cat(fd1);
	// size1 = my_read(fd1,buffer,60);
	// cout<<"after reading\n"<<buffer<<endl;
	// size1 = my_copy(fd1);
	// size1 = my_close(fd1);
	// cout<<size1;
	// cout<<"after closing"<<endl;
	// size1 = my_read(fd1,buffer,60);
	// cout<<size1;
	return 0;
}