#include <iostream>
#include <bits/stdc++.h>

using namespace std;
#define SIZE 64
int file_system_size,block_size;
int no_of_blocks = file_system_size/block_size;

// block* freeblockptr;


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
	struct directory* dot;
	struct directory* ddot; 
	record* r; //14bytes file name
}directory;

vector<block*> *freeblockptr;
superblock *sb;
vector<block*> &freeptr = *(sb->freeblockptr);

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


void init()
{

	block *file_system = (block*)malloc(sizeof(block)*no_of_blocks);

	sb = (superblock*)malloc(sizeof(superblock));
	sb = (superblock*)&file_system[0];

	inode *in = (inode*)malloc(sizeof(inode)*(2*block_size/sizeof(inode)));

	sb->freeblockptr= new vector<block*>;
	block* temp;
	for(int i=0; i<no_of_blocks; i++)
	{
		sb->freeblockptr->push_back(&file_system[i]);
	}

}
int main()
{

	// how to typecast inode list to two blocks

}