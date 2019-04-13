#include <iostream>
#include <string.h>
#include <bits/stdc++.h>

using namespace std;
#define SIZE 64
int file_system_size,block_size;
int no_of_blocks = file_system_size/block_size;
char cwd[50] = "root";
// block* freeblockptr;
int fd=0;

typedef struct 
{
	int id;
	char* b_data;
}block;

typedef struct 
{
	vector<block*> *freeblockptr;
	// block *first_block;
}superblock;

typedef struct 
{
	// block* data[block_size/sizeof(int)];
	int sptr[SIZE/sizeof(int)];
}singletable;

typedef struct 
{
	singletable dptr[SIZE/sizeof(int)];
}doubletable;


typedef struct 
{
	block* dir_ptr[5];
	block* single_ptr;
	block* double_ptr;
	bool type; //0-file, 1-direc
	// file size = -1 inode is empty
	int file_size;
}inode;
//16 bytes

typedef struct
{
	short int inode_no;
	char file_name[14];
}record;

typedef struct directory
{
	// int inode_no;
	// . - current .. - parent
	int d_id;
	char *d_name;
	struct directory* dot;
	struct directory* ddot;
	record* r; //14bytes file name
}directory;

typedef struct open_file
{
	char path[50];
	int inode_no;
	int fd;
	struct open_file *next;
}open_file;


vector<block*> *freeblockptr;
superblock *sb;
vector<block*> &freeptr = *(sb->freeblockptr);
open_file* open_file_list;

inode *inode_list;

directory root_direc;
vector<directory*> dir_list;

int find_free_inode()
{
	for(int i=0; i<2*block_size/sizeof(inode) ;i++)
	{
		if(inode_list[i].file_size==-2)
			return i;
	}
	return -1;
}

directory* check_valid_directory(char* name)
{
	for(int i=0; i<dir_list.size();i++)
	{
		if(strcmp(dir_list[i]->d_name, name) ==0)
		{
			return dir_list[i];		
		}
	}
	return NULL;
}

block* find_free_block()
{
	block* temp;
	while(1)
	{
		if(sb->freeblockptr->empty()==true)
		{//empty
			printf("No free blocks\n");
			return 0;
		}
		else
		{
			temp = freeptr.front();
			freeptr.erase(freeptr.begin());
			return temp;
		}
	}
	// cout<<"free block returned"<<endl;
	// return free_ptr;
}

directory* get_parent(char* path)
{
	char *file;
	char *word = strtok(path, "/");
	char *current;
	strcpy(current, cwd);
	char *parent;
	strcpy(parent, cwd);
	int count=0;

	while(word!=NULL)
	{
		if(count==0)
		{
			file = word;
		}
		else if(count==1)
		{
			strcat(current,"/");
			strcat(current,file);
			if(!check_valid_directory(current))
			{
				printf("invalid path\n");
				return NULL;
			}
			file = word;
		}
		else 
		{
			strcpy(parent, current);
			strcat(current,"/");
			strcat(current,file);
			if(!check_valid_directory(current))
			{
				printf("invalid path\n");
				return NULL;
			}
			file = word;
		}
		count++;
		word = strtok(NULL,"/");
	}

	for(int j=0; j<dir_list.size(); j++)
	{
		if( strcmp(dir_list[j]->d_name , current)==0)
		{
			return dir_list[j];
		}
	}
	return NULL;
}

int my_open(char* path)
{
	char *file;
	char *word = strtok(path, "/");
	char *current;
	strcpy(current, cwd);
	char *parent;
	strcpy(parent, cwd);
	int count = 0;
	while(word!=NULL)
	{
		if(count==0)
		{
			file = word;
		}
		else if(count==1)
		{
			strcat(current,"/");
			strcat(current,file);
			if(!check_valid_directory(current))
			{
				printf("invalid path\n");
				return -1;
			}
			file = word;
		}
		else
		{
			// strcat(parent,"/");
			// strcat(parent,current);
			strcpy(parent, current);
			strcat(current,"/");
			strcat(current,file);
			if(!check_valid_directory(current))
			{
				printf("invalid path\n");
				return -1;
			}
			file = word;
		}
		count++;
		word = strtok(NULL,"/");
	}

	// keep file in open dir
	int flag=0;
	char* temp;
	temp = strdup(cwd);
	open_file* tempnode, *prev;
	
	open_file* node = new open_file;
	// node->inode_no = i_no;
	strcpy(node->path , strcat(temp,path)); //temp has full path now
	node->fd = fd++;
	node->next = NULL;
	if(open_file_list==NULL)
	{
		node->inode_no = find_free_inode();
		open_file_list = node;
	}
	else
	{
		tempnode = open_file_list;
		while(tempnode != NULL)
		{
			if(strcmp(tempnode->path, temp) ==0)
			{
				node->inode_no = tempnode->inode_no;
				flag=1;
			}
			prev = tempnode;
			tempnode = tempnode->next;
		}
		prev->next = node;
	}

	int i_no = node->inode_no;

	record r;
	r.inode_no = i_no;
	strcpy(r.file_name, file);

	// create inode
	inode_list[i_no].type=0;
	// strcpy(inode_list[i_no].file_name, file);
	inode_list[i_no].file_size=0;

	int k, flag1=0;
	// keep record in direc
	directory* cur_dir = check_valid_directory(current);
	if(flag==0)
	{
		for(k=0 ; k<block_size/16 ; k++)
		{
			if(cur_dir->r[k].inode_no==-1)
			{
				flag1=1;
				break;
			}
		}
		if(flag1==1)
			cur_dir->r[k] = r;
		else
			cout<<"no free record\n";
	}
	return fd-1;	
}



int mkdir(char* path)
{
	directory dir;
	char* fpath;
	fpath = strdup(cwd);
	strcpy(dir.d_name,  strcat(fpath, path));

	dir.dot = &dir;
	dir.ddot = get_parent(path);

	if(dir.ddot==NULL)
		return -1;

	int i = find_free_inode();
	inode_list[i].type = 1;
	inode_list[i].single_ptr = find_free_block();// given 1 block, give size, using only single ptr to store records
	inode_list[i].file_size=-1;

	dir.r = (record*)(inode_list[i].single_ptr);
	record* temp = dir.r;
	int k, n=1;
	for(k=0;k< n*block_size/16; k++)
	{
		temp[k].inode_no = -1;
	}
	dir_list.push_back(&dir);
	return 1;

}

int chdir(char* path)
{//full path
	if(check_valid_directory(path))
	{
		strcpy(cwd, path);
		return 1;
	}
	else
		return 0;
}

int my_rmdir(char* path)
{
	// vector<directory*> iterator it;
	if(check_valid_directory(path))
	{
		for(auto it= dir_list.begin() ; it<dir_list.end() ; it++)
		{
			if( strcmp(*it->d_name, path)==0)
			{
				// remove all the files in direc

				for(int j=0 ; j< sizeof(block_size)/16 ; j++)
				{
					if(*it->r.inode_no!=-2)
					{ // remove all those blocks and add freeblockptr
						

						*it->r.inode_no=-2;
					}
				}
				

				dir_list.erase(it);
				break;
			}
		}		
	}
	return -1;
}


void init()
{
	block *file_system = (block*)malloc(sizeof(block)*no_of_blocks);

	sb = (superblock*)malloc(sizeof(superblock));
	sb = (superblock*)&file_system[0];

	inode_list = (inode*)malloc(sizeof(inode)*(2*block_size/sizeof(inode)));
	for(int i=0;i<2*block_size/sizeof(inode) ;i++)
	{
		inode_list[i].file_size=-2; // empty
	}

	sb->freeblockptr= new vector<block*>;
	block* temp;
	for(int i=0; i<no_of_blocks; i++)
	{
		sb->freeblockptr->push_back(&file_system[i]);
	}

	// create root direc
	strcpy(root_direc.d_name, "root");
	root_direc.dot = &root_direc;
	root_direc.ddot = NULL;
	int i = find_free_inode();
	inode_list[i].type = 1;
	inode_list[i].single_ptr = find_free_block();// given 1 block, give size, using only single ptr to store records
	inode_list[i].file_size=-1; 

	root_direc.r = (record*)(inode_list[i].single_ptr);
	record* rtemp = root_direc.r;
	int k, n=1;
	for(k=0;k< n*block_size/16; k++)
	{
		rtemp[k].inode_no = -1;
	}
	dir_list.push_back(&root_direc);

}

int main()
{

	// how to typecast inode list to two blocks

}