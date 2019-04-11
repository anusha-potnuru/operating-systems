#include <iostream>

using namespace std;


int file_system_size,block_size;
int no_of_blocks = file_system_size/block_size;
char cwd[50] = "root";
// block* freeblockptr;

typedef struct 
{
	int id;
	char *b_data;
}block;

typedef struct 
{
	block* freeblockptr;
	// block *first_block;
}superblock;

typedef struct 
{
	// block* data[block_size/sizeof(int)];
	int sptr[block_size/sizeof(int)];
}singletable;

typedef struct 
{
	singletable dptr[block_size/sizeof(int)];
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

typedef struct 
{
	// int inode_no;
	// . - current .. - parent
	int d_id;
	char *d_name;
	directory* dot;
	directory* ddot; 
	record* r; //14bytes file name
}directory;

int my_open(char* path)
{
	int i_no = find_free_inode();
	char *file;
	char *word = strtok(path,'\\');
	char *current = cwd;
	char *parent = cwd;
	int count = 0;
	while(word!=NULL)
	{
		if(count==0)
		{
			file = word;
		}
		else if(count==1)
		{
			current = file;
			file = word;
		}
		else 
		{
			parent = curent;
			curent = file;
			file = words;
		}
		count++;
		word = strtok(NULL,"/");
	}
	
	
}

int main()
{
	block *file_system = (block*)malloc(sizeof(blocks)*no_of_blocks);

	superblock *sb = (superblock*)malloc(sizeof(superblock));
	sb = (superblock*)&file_system[0];

	inode *in = (inode*)malloc(sizeof(inode)*(2*block_size/sizeof(inode)));
	// how to typecast inode list to two blocks

}