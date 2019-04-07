#include <iostream>
#include <stdlib.h>
using namespace std;

int file_system_size,block_size;
int no_of_blocks;
typedef struct 
{
	char *b_data;
}block;

typedef struct directory
{
	char *file_name;
	int block_no;
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




int main()
{
	int i;
	cin>>file_system_size>>block_size;
	no_of_blocks = file_system_size/block_size;

	// int fat[no_of_blocks];
	fat* block1;
	block* file_system = (block*)malloc(sizeof(block)*no_of_blocks); 
	for(i=0;i<no_of_blocks;i++)
	{
		file_system[i].b_data = (char*)malloc(sizeof(char)*block_size);
	}

	
	block1 = (fat*)malloc(sizeof(fat));
	block1->arr = (int*)malloc(sizeof(int)*no_of_blocks);
	block1 = (fat*)&(file_system[1]);
	directory *dir = (directory*)malloc(sizeof(directory));
	// creating superblock
	superblock* sb = (superblock*)malloc(sizeof(superblock)+sizeof(bool)*no_of_blocks);
	sb = (superblock*)&file_system[0];
	sb->file_system_size = file_system_size;
	sb->disk_block_size = block_size;
	sb->d = dir;

	return 0;
}