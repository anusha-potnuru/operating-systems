#include <iostream>
#include <stdlib.h>
using namespace std;

int file_system_size,block_size;
int no_of_blocks;
typedef struct 
{
	char b_data[];
}block;

typedef struct
{
	int disk_block_size;
	int file_system_size;
	char volume_name[50];
	bool bit_vector[];
}superblock;

int main()
{
	
	cin>>file_system_size>>block_size;
	no_of_blocks = file_system_size/block_size;
	int fat[no_of_blocks];
	block* file_system = malloc(sizeof(block)+sizeof(char)*block_size);
	// creating superblock
	superblock* sb = malloc(sizeof(superblock)+sizeof(bool)*no_of_blocks);
	sb = (superblock*)file_system;
	sb->file_system_size = file_system_size;
	sb->disk_block_size = block_size;
	return 0;
}