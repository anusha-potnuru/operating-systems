#include <iostream>
#include <stdlib.h>
using namespace std;

int file_system_size,block_size;
int no_of_blocks;
typedef struct 
{
	char *b_data;
}block;

typedef struct
{
	int disk_block_size;
	int file_system_size;
	char volume_name[50];
	directory *d;
	bool bit_vector[];
}superblock;

typedef struct
{
	int arr[];
}fat;

typedef struct 
{
	char *file_name;
	int block_no;
	directory *next;
}directory;


int main()
{
	
	cin>>file_system_size>>block_size;
	no_of_blocks = file_system_size/block_size;

	int fat[no_of_blocks];

	block* file_system = malloc(sizeof(block)*no_of_blocks); 
	for(i=0;i<no_of_blocks;i++)
	{
		file_system[i]->b_data = (char*)malloc(sizeof(char)*block_size);
	}

	fat* block1 = malloc(sizeof(fat)+sizeof(int)*no_of_blocks);
	block1 = (fat*)file_system[1];
	directory *dir = malloc(sizeof(directory));
	// creating superblock
	superblock* sb = malloc(sizeof(superblock)+sizeof(bool)*no_of_blocks);
	sb = (superblock*)file_system[0];
	sb->file_system_size = file_system_size;
	sb->disk_block_size = block_size;
	sb->directory = dir;

	return 0;
}