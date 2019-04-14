#include <iostream>
#include <string.h>
#include <bits/stdc++.h>
#include <unistd.h>
#include <fcntl.h> 

using namespace std;
#define SIZE 64

int file_system_size = 2048, block_size = 64;
int no_of_blocks = (int)file_system_size/block_size;

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
	block* dir_ptr[5];
	block* single_ptr;
	block* double_ptr;
	bool type; //0-file, 1-direc
	// file size = -1 inode is empty
	int file_size;
}inode;
// 16 bytes

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
vector<block*> freeptr; 
open_file* open_file_list;
block *file_system;
inode *inode_list;

directory root_direc;
vector<directory*> dir_list;


int find_free_inode()
{
	for(int i=0; i<2*block_size/sizeof(inode) ;i++)
	{
		if(inode_list[i].file_size==-2)
		{
			cout<<"inode in free list"<<i<<endl;
			for(int j=0; j<5; j++)
				inode_list[i].dir_ptr[j] = NULL;
			inode_list[i].single_ptr = NULL;
			inode_list[i].double_ptr = NULL;
			return i;
		}
	}
	return -1;
}

block* find_free_block()
{
	cout<<"in find free blocks"<<endl;
	block* temp;
	while(1)
	{
		if(freeptr.empty()==true)
		{
			printf("No free blocks\n");
			return 0;
		}
		else
		{
			cout<<"here\n";
			temp = freeptr.front();
			freeptr.erase(freeptr.begin());
			// cout<<temp->id;
			return temp;
		}
	}
	// cout<<"free block returned"<<endl;
	// return free_ptr;
}


void init()
{
	// cout<<sizeof(inode)<<endl;

	file_system = (block*)malloc(sizeof(block)*no_of_blocks);

	sb = (superblock*)malloc(sizeof(superblock));
	sb = (superblock*)&file_system[0];
	inode_list = (inode*)malloc(sizeof(inode)*(2*block_size/sizeof(inode)));
	int i;
	for(i=0;i<2*block_size/sizeof(inode) ;i++)
	{
		inode_list[i].file_size = -2; // empty
	}
	// inode_list = (inode*)&file_system[1];  	
	sb->freeblockptr= new vector<block*>;
	for(i=0; i<no_of_blocks; i++)
	{
		sb->freeblockptr->push_back(&file_system[i]);
	}
	freeptr = *(sb->freeblockptr);
	// create root direc
	root_direc.d_name = (char*)malloc(sizeof(char)*50);
	strcpy(root_direc.d_name, "root");
	root_direc.dot = &root_direc;
	root_direc.ddot = NULL;
	// i = find_free_inode();
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


int my_read(int fd, char *buf, int size)
{
	cout<<"in my read"<<endl;
	open_file *temp;
	temp = open_file_list;
	int flag=0;
	while(temp!=NULL)
	{
		if(temp->fd ==fd)
		{
			flag = 1;
			break;
		}
		temp = temp->next;
	}
	if(flag == 0)
	{
		cout<<"error file doesnt exist"<<endl;
		return -1;
	}
	cout<<"got open file"<<endl;
	int i_no = temp->inode_no;
	cout<<"inode no = "<<i_no<<endl;
	cout<<"file size = "<<inode_list[i_no].file_size<<endl;
	int j=0;
	int ret = 0;
	while(j < 5 && size >= block_size)
	{
		cout<<"here"<<endl;
		if(inode_list[i_no].dir_ptr[j]==NULL)
		{
			return ret;
		}
		strncat(buf,inode_list[i_no].dir_ptr[j]->b_data,block_size);
		cout<<buf<<endl;
		j++;
		size = size - block_size;
		ret += block_size;
	}
	if(j<5 && size < block_size)
	{
		cout<<"buf in here "<<endl;
		cout<<inode_list[i_no].dir_ptr[j]->b_data<<endl;
		strncat(buf,inode_list[i_no].dir_ptr[j]->b_data,size);
		cout<<buf<<endl;
		ret += size;
	}
	if(j>=5 && size)
	{
		j = 0;
		block** sptr;
		if(inode_list[i_no].single_ptr==NULL)
		{
			return ret;
		}
		sptr = (block**)inode_list[i_no].single_ptr->b_data;
		while(j< block_size/sizeof(int) && size>=block_size)
		{
			if(sptr[j]==NULL)
			{
				return ret;
			}
			strncat(buf,sptr[j]->b_data,block_size);
			j++;
			size = size - block_size;
			ret += block_size;
		}
		if(j<block_size/sizeof(int) && size<block_size)
		{
			strncat(buf,sptr[j]->b_data,size);
			ret += size;
		}
	}
	if(j>=block_size/sizeof(int) && size)
	{
		j = 0;
		int k = 0;
		block** dptr;
		block** dsptr;
		if(inode_list[i_no].double_ptr==NULL)
		{
			return ret;
		}
		while(j<block_size/sizeof(int) && size>=block_size)
		{
			dptr = (block**)inode_list[i_no].double_ptr->b_data;	
			if(dptr[j]==NULL)
			{
				return ret;
			}
			dsptr = (block**)dptr[j]->b_data;
			while(k<block_size/sizeof(int) && size>=block_size)
			{
				if(dsptr[k]==NULL)
				{
					return ret;
				}
				strncat(buf,dsptr[k]->b_data,block_size);
				j++;
				size = size - block_size;
				ret += block_size;
			}

		}
		if(j<block_size/sizeof(int) && k< block_size/sizeof(int) && size<block_size)
		{
			strncat(buf,dsptr[k]->b_data,size);
			ret += size;
		}
	}
	return ret;

}

int my_copy(int fd,char *path)
{
	open_file *temp;
	temp = open_file_list;
	int flag=0;
	while(temp!=NULL)
	{
		if(temp->fd ==fd)
		{
			flag = 1;
			break;
		}
		temp = temp->next;
	}
	if(flag == 0)
	{
		cout<<"error file doesnt exist"<<endl;
		return -1;
	}
	int i_no = temp->inode_no;
	int file_size = inode_list[i_no].file_size;
	char *buf;
	buf = (char*)malloc(sizeof(char)*file_size);
	my_read(fd,buf,file_size);
	// char file_name[150];
	// strcpy(file_name,cwd);
	// strcpy()
	// strcat(file_name,path);
	int linux_fd = open(path, O_WRONLY| O_CREAT|O_TRUNC,S_IRWXU);
	write(linux_fd, buf, file_size);
	return linux_fd;

}


int my_cat(int fd)
{
	cout<<"my_cat"<<endl;
	open_file *temp;
	temp = open_file_list;
	int flag=0;
	while(temp!=NULL)
	{
		if(temp->fd ==fd)
		{
			flag = 1;
			break;
		}
		temp = temp->next;
	}
	if(flag == 0)
	{
		cout<<"error file doesnt exist"<<endl;
		return -1;
	}
	int i_no = temp->inode_no;
	int file_size = inode_list[i_no].file_size;
	char *buf;
	buf = (char*)malloc(sizeof(char)*file_size);
	my_read(fd,buf,file_size);
	cout<<buf<<endl;
	return 1;
}



directory* check_valid_directory( char* name)
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


directory* get_parent( char* path)
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
	cout<<"in my open"<<endl;
	char *file = (char*)malloc(sizeof(char)*50);
	// char *path = (char*)path1;
	int i;
	int fl = 0;
	for(i=0;i<strlen(path);i++)
	{
		if(path[i]=='/')
		{
			fl = 1;
			break;
		}
	}
	char *word;
	if(fl == 1)
	{
		word = strtok(path, "/");
	}
	else
	{
		word = NULL;
	}
	strcpy(file,path);
	char *current = (char*)malloc(sizeof(char)*200);
	strcpy(current, cwd);
	char *parent= (char*)malloc(sizeof(char)*200);
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

int my_write(int fd, char* buffer, int size)
{
	cout<<"in my write"<<endl;
	int i_no = -1, di=0, si=0, ddi=0, dsi=0;
	open_file* temp = open_file_list;
	while(temp!=NULL)
	{
		if(temp->fd ==fd)
		{
			i_no = temp->inode_no;
			break;
		}
		temp=temp->next;
	}
	cout<<"inode_no "<<i_no<<endl;
	while(size > 0)
	{
		if(di==5)
			break;
		if(inode_list[i_no].dir_ptr[di] == NULL)
		{
			// inode_list[i_no].dir_ptr[di] = (block*)malloc(sizeof(block));
			cout<<"here\n";
			inode_list[i_no].dir_ptr[di] = find_free_block();			
			inode_list[i_no].dir_ptr[di]->b_data = (char*)malloc(sizeof(char)*block_size);
			cout<<"here\n";
			if(size >= block_size)
			{
				strncpy(inode_list[i_no].dir_ptr[di]->b_data, buffer, block_size);
				cout<<"length stored "<<strlen(inode_list[i_no].dir_ptr[di]->b_data)<<endl;
				cout<<"in write direct pointer "<<inode_list[i_no].dir_ptr[di]->b_data<<endl;
				buffer+=block_size;
				size-=block_size;
				inode_list[i_no].file_size+=block_size;
			}
			else if( size>0 && size<block_size )
			{
				strncpy(inode_list[i_no].dir_ptr[di]->b_data, buffer, size);
				cout<<"in write direct pointer ending"<<inode_list[i_no].dir_ptr[di]->b_data<<endl;
				inode_list[i_no].file_size += size;
				size=0;
				return 1;
			}
			di++;
		}	
	}
	cout<<"direct ptr done"<<endl;
	if(di==5)
	{//single ptr
		block** sptr;
		while(size>0)
		{
			if(inode_list[i_no].single_ptr == NULL)
			{
				inode_list[i_no].single_ptr = find_free_block();
				inode_list[i_no].single_ptr->b_data = (char*)malloc(sizeof(char)*block_size);
				sptr = (block**)inode_list[i_no].single_ptr->b_data;
				// for(int k=0 ; k<sizeof(char)*block_size/ sizeof(block*) ; k++)
				// {
				// 	sptr[k] = NULL;
				// }
			}
			else
			{
				if(si == sizeof(char)*block_size/ sizeof(block*))
					break;
				if(sptr[si] == NULL)
				{
					sptr[si] = find_free_block();
					sptr[si]->b_data = (char*)malloc(sizeof(char)*block_size);

					if(size >= block_size)
					{
						strncpy(sptr[si]->b_data, buffer, block_size);
						cout<<"in write single pointer "<<sptr[si]->b_data<<endl;
						buffer+=block_size;
						size -= block_size;
						inode_list[i_no].file_size+=block_size;
					}
					else if(size>0 && size<block_size)
					{
						strncpy(sptr[si]->b_data, buffer, size);
						cout<<"in write single pointer ending"<<sptr[si]->b_data<<endl;
						buffer+=size;
						inode_list[i_no].file_size+=size;
						size -= size;
						
						return 1;
					}
					si++;
				}

			}
		}		
	}

	if(si == sizeof(char)*block_size/ sizeof(block*))
	{
		block** dptr;
		block** dsptr;
		while(size>= block_size)
		{
			if(inode_list[i_no].double_ptr==NULL)
			{
				inode_list[i_no].double_ptr = find_free_block();
				inode_list[i_no].double_ptr->b_data = (char*)malloc(sizeof(char)*block_size);
				dptr = (block**)inode_list[i_no].double_ptr->b_data;		
				// for(int k=0 ; k<sizeof(char)*block_size/ sizeof(block*) ; k++)
				// { // for safety
				// 	inode_list[i_no].double_ptr->b_data[k] = NULL;
				// }
			}
			else
			{
				if(ddi == sizeof(char)*block_size/ sizeof(block*))
				{
					break;
				}
				if(dsi == sizeof(char)*block_size/ sizeof(block*))
				{
					dsi=0;
					ddi++;
				}

				if(dsi ==0 )
				{ // create single ptr table
					dptr[ddi] = find_free_block();
					dptr[ddi]->b_data = (char*)malloc(sizeof(char)*block_size);

					dsptr = (block**)dptr[ddi]->b_data;

					dsptr[dsi] = find_free_block();

					// copy data
					dsptr[dsi]->b_data = (char*)malloc(sizeof(char)*block_size);

					if(size>=block_size)
					{
						strncpy(dsptr[dsi]->b_data, buffer, block_size);
						cout<<"in write double pointer "<<dsptr[dsi]->b_data<<endl;
						buffer+=block_size;
						size-=block_size;
						inode_list[i_no].file_size+=block_size;
					}
					else if(size>0 && size<block_size)
					{
						strncpy(dsptr[dsi]->b_data, buffer, size);
						cout<<"in write double pointer ending"<<dsptr[dsi]->b_data<<endl;
						buffer+=size;
						size-=size;
						inode_list[i_no].file_size+=size;
						return 1;
					}
					dsi++;
				}
				else
				{
					// single ptr exist, only copy data
					dsptr[dsi]->b_data = (char*)malloc(sizeof(char)*block_size);
					if(size >= block_size)
					{
						strncpy(dsptr[dsi]->b_data, buffer, block_size);
						cout<<"in write double pointer "<<dsptr[dsi]->b_data<<endl;
						buffer+=block_size;
						size-=block_size;
						inode_list[i_no].file_size+=block_size;
					}
					else if(size>0 && size<block_size)
					{
						strncpy(dsptr[dsi]->b_data, buffer, size);
						cout<<"in write double pointer ending "<<dsptr[dsi]->b_data<<endl;
						buffer+=size;
						size-=size;
						inode_list[i_no].file_size+=size;
						return 1;
					}
					
					dsi++;
				}

			}
		}
	}

	return -1;
}



int my_mkdir( char* path)
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

int my_chdir( char* path)
{//full path
	if(check_valid_directory(path))
	{
		strcpy(cwd, path);
		return 1;
	}
	else
		return 0;
}

int my_rmdir( char* path)
{
	vector<directory*>::iterator it;
	if(check_valid_directory(path))
	{
		for(it= dir_list.begin() ; it!=dir_list.end() ; it++)
		{
			if( strcmp( (*it)->d_name, path)==0)
			{
				// remove all the files in direc

				for(int j=0 ; j< sizeof(block_size)/16 ; j++)
				{
					if( inode_list[(*it)->r[j].inode_no].file_size==-2)
					{ 
						break;
					}
					else if( inode_list[(*it)->r[j].inode_no].file_size==-1)
					{
						if(my_rmdir((*it)->r[j].file_name)==-1)
						{
							return -1;
						}
					}
					else
					{
						int i_no = (*it)->r[j].inode_no;
						int size =  inode_list[(*it)->r[j].inode_no].file_size;
						open_file *temp,*prev;
						temp = open_file_list;
						prev = open_file_list;
						int flag=0;
						while(temp!=NULL)
						{
							if(temp->inode_no == i_no)
							{
								prev->next = temp->next;
								free(temp);
								temp = prev;
							}
							prev = temp;
							temp = temp->next;
						}
						while(j < 5 && size>=block_size)
						{
							if(inode_list[i_no].dir_ptr[j]==NULL)
							{
								break;
							}
							memset(inode_list[i_no].dir_ptr[j]->b_data,'\0',block_size);
							j++;
							size = size - block_size;
						}
						if(j<5 && size < block_size)
						{
							memset(inode_list[i_no].dir_ptr[j]->b_data,'\0',block_size);
						}
						if(j>=5 && size)
						{
							j = 0;
							block** sptr;
							if(inode_list[i_no].single_ptr==NULL)
							{
								continue;
							}
							sptr = (block**)inode_list[i_no].single_ptr->b_data;
							while(j< block_size/sizeof(int) && size>=block_size)
							{
								if(sptr[j]==NULL)
								{
									continue;
								}
								memset(sptr[j]->b_data,'\0',block_size);
								j++;
								size = size - block_size;
							}
							if(j<block_size/sizeof(int) && size<block_size)
							{
								memset(sptr[j]->b_data,'\0',size);
							}
						}
						if(j>=block_size/sizeof(int) && size)
						{
							j = 0;
							int k = 0;
							block** dptr;
							block** dsptr;
							if(inode_list[i_no].double_ptr==NULL)
							{
								continue;
							}
							while(j<block_size/sizeof(int) && size>=block_size)
							{
								dptr = (block**)inode_list[i_no].double_ptr->b_data;	
								if(dptr[j]==NULL)
								{
									continue;
								}
								dsptr = (block**)dptr[j]->b_data;
								while(k<block_size/sizeof(int) && size>=block_size)
								{
									if(dsptr[k]==NULL)
									{
										continue;
									}
									memset(dsptr[k]->b_data,'\0',block_size);
									j++;
									size = size - block_size;
								}

							}
							if(j<block_size/sizeof(int) && k< block_size/sizeof(int) && size<block_size)
							{
								memset(dsptr[k]->b_data,'\0',size);
							}
						}
						inode_list[i_no].file_size = -2;
					}
				}
				dir_list.erase(it);
				break;
			}
		}		
	}
	return -1;
}

int my_close(int fd)
{
	open_file *temp,*prev;
	temp = open_file_list;
	prev = open_file_list;
	int flag=0;
	int i_no;
	while(temp!=NULL)
	{
		if(temp->fd == fd)
		{
			i_no = temp->inode_no;
			prev->next = temp->next;
			free(temp);
			flag = 1;
			break;
		}
		prev = temp;
		temp = temp->next;
	}
	if(flag==-1)
	{
		return -1;
	}
	return 1;
}


void readinput(char* buffer, int len)
{
	char c;
	int i=0;
	while(i<len-1)
	{
		scanf("%c", &c);
		// cout<<(int)c<<" ";
		// cout<<c;
		if(c=='\n')
		{
			break;
		}
		buffer[i++]=c;
	}	
	buffer[i]='\0';
	// cin.ignore();
	// cin.clear();
	cin.sync();
	// cout<<"returned"<<endl;
	return;
}

int main()
{
	cout<<"enter filesize and block_size"<<endl;
	// cin>>file_system_size>>block_size;
	file_system_size = INT_MAX;
	block_size = 64;

	// how to typecast inode list to two blocks

	cout<<"in main"<<endl;
	char *buffer;
	buffer = (char*)malloc(sizeof(char)*600);
	cout<<"enter buffer"<<endl;
	// scanf("%s",buffer);
	// readinput(buffer, 150);
	// // scanf("%[^\n]%*c",buffer);
	strcpy(buffer, "The data blocks of a file are maintained using index nodes or i-nodes. Each i-node will contain information about the data blocks, and will include 5 direct pointers, 1 singly indirect pointer, and 1 doubly indirect pointer. Each pointer will be 32 bits in size, and will indicate a block number. It will also store a type field indicating whether the file is a regular file or a directory, and file size in bytes. The i-nodes will be stored in Block-1 and Block-2, in increasing order of their numbers (i.e. i-node-0 first, followed by i-node-1, and so on).");
	// cout<<"entered buffer is = "<<buffer<<endl;
	init();
	cout<<"buffer length: "<<strlen(buffer)<<endl;
	int fd = my_open("abc.txt");
	cout<<"fd of file is "<<fd<<endl;
	int size = my_write(fd,buffer,strlen(buffer));
	memset(buffer,'\0',size);
	// size = my_read(fd,buffer,560);
	// cout<<buffer<<endl;
	// cout<<size<<endl;
	// my_cat(fd);
	// my_copy(fd,"abc.txt");
	// size = my_read(fd,buffer,60);
	// cout<<"reading 60 bytes\n"<<buffer<<endl;
	// cout<<strlen(buffer)<<endl;
	// size = my_copy(fd);
	return 0;

}