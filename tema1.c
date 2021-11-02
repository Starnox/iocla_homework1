#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_INPUT_LINE_SIZE 300


struct Dir;
struct File;

typedef struct Dir{
	char *name;
	struct Dir* parent;
	struct File* head_children_files;
	struct Dir* head_children_dirs;
	struct Dir* next;
} Dir;

typedef struct File {
	char *name;
	struct Dir* parent;
	struct File* next;
} File;


File* initialiseFile(char *name, Dir* dir);
void freeFile(File **file);
void freeFileList(File **fileList);

Dir* initialiseDir(char *name, Dir* parent);
void freeDir(Dir** dir);
void freeDirList(Dir **dirList);

File* initialiseFile(char *name, Dir* dir)
{
	File *file = (File*) malloc(sizeof(File));
	if(file == NULL)
	{
		printf("Initializarea unui fisier nou nu a avut loc");
		return NULL;
	}
	file->name = (char *) malloc(sizeof(char) * MAX_INPUT_LINE_SIZE);
	strcpy(file->name, name);
	file->parent = dir;
	file->next = NULL;

	return file;
}

// Free the memory of the File object with freeing the memory for its directory
void freeFile(File **file)
{
	free((*file)->name);
	(*file)->name = NULL;
	free(*file);
}

void freeFileList(File **fileList)
{
	File *fileIterator = *fileList;
	while(fileIterator != NULL)
	{
		File *nextFile =fileIterator->next;
		freeFile(&fileIterator);
		fileIterator = nextFile;
	}
}

Dir* initialiseDir(char *name, Dir* parent)
{
	Dir *dir = (Dir*) malloc(sizeof(Dir));
	if(dir == NULL)
	{
		printf("Initializarea unui director nou nu a avut loc");
		return NULL;
	}

	dir->name = (char *) malloc(sizeof(char) * MAX_INPUT_LINE_SIZE);
	strcpy(dir->name, name);

	dir->parent = parent;
	dir->head_children_files = NULL;
	dir->head_children_dirs = NULL;
	dir->next = NULL;

	return dir;
}


void freeDir(Dir** dir)
{
	if(*dir != NULL)
	{
		free((*dir)->name);
		(*dir)->name = NULL;

		// delete all the directory files
		if((*dir)->head_children_dirs != NULL)
		{
			freeDirList(&(*dir)->head_children_dirs);
		}
		// delete all the sub-directories
		if((*dir)->head_children_files != NULL)
		{
			freeFileList(&(*dir)->head_children_files);
		}
	}
	free(*dir);
	*dir = NULL;
}

void freeDirList(Dir **dirList)
{
	Dir *dirIterator = *dirList;
	while(dirIterator != NULL)
	{
		Dir *nextDir = dirIterator->next;
		freeDir(&dirIterator);
		dirIterator = nextDir;
	}
}


void touch (Dir* parent, char* name)
{
	// if it is the first file in the directory
	if(parent->head_children_files == NULL)
		parent->head_children_files = initialiseFile(name, parent);
	else
	{
		// get the end of File and check if the file is already in the directory
		File *fileIterator = parent->head_children_files;

		// check first
		if(strcmp(fileIterator->name, name) == 0)
		{
			printf("File already exists\n");
			return;
		}

		// go through the rest
		while(fileIterator->next != NULL)
		{
			if(strcmp(fileIterator->name, name) == 0)
			{
				printf("File already exists\n");
				return;
			}
			fileIterator = fileIterator->next;
		}

		// initialise file and set it
		fileIterator->next = initialiseFile(name, parent);
	}
}

void mkdir (Dir* parent, char* name)
{
	if(parent->head_children_dirs == NULL)
		parent->head_children_dirs = initialiseDir(name, parent);
	else
	{
		// get the end of the directory list and check if the directory is already in the directory
		Dir *dirIterator = parent->head_children_dirs;

		// check first
		if(strcmp(dirIterator->name, name) == 0)
		{
			printf("Directory already exists\n");
			return;
		}

		while (dirIterator->next != NULL)
		{
			if(strcmp(dirIterator->name, name) == 0)
			{
				printf("Directory already exists\n");
				return;
			}
			dirIterator = dirIterator->next;
		}
		// initialise directory and set it
		dirIterator->next = initialiseDir(name, parent);
	}
}

void ls (Dir* parent)
{

	// print the file list
	File *fileIterator = parent->head_children_files;
	while(fileIterator != NULL)
	{
		printf("%s\n", fileIterator->name);
		fileIterator = fileIterator->next;
	}
	// print the directory list
	Dir *dirIterator = parent->head_children_dirs;
	while(dirIterator != NULL)
	{
		printf("%s\n", dirIterator->name);
		dirIterator = dirIterator->next;
	}

	
}

void rm (Dir* parent, char* name)
{
	// search for the file, remove it and re-establish the order

	File* fileIterator = parent->head_children_files;
	if(fileIterator == NULL)
	{
		return;
	}

	// if it is the first file in the list
	if(strcmp(fileIterator->name, name) == 0)
	{
		parent->head_children_files = parent->head_children_files->next;
		freeFile(&fileIterator);
		return;
	}

	while(fileIterator->next != NULL)
	{
		if(strcmp(fileIterator->next->name, name) == 0)
		{
			File* fileToDelete = fileIterator->next;
			fileIterator->next = fileIterator->next->next;
			freeFile(&fileToDelete);
			return;
		}
		fileIterator = fileIterator->next;
	}
	printf("Could not find the file\n");
	return;
}

void rmdir (Dir* parent, char* name)
{
	Dir* dirIterator = parent->head_children_dirs;
	if(dirIterator == NULL)
	{
		return;
	}
	if(strcmp(dirIterator->name, name) == 0)
	{
		parent->head_children_dirs = parent->head_children_dirs->next;
		freeDir(&dirIterator);
		return;
	}

	while(dirIterator->next != NULL)
	{
		if(strcmp(dirIterator->next->name, name) == 0)
		{
			Dir* dirToDelete = dirIterator->next;
			dirIterator->next = dirIterator->next->next;
			freeDir(&dirToDelete);
			return;
		}
		dirIterator = dirIterator->next;
	}
	printf("Could not find the dir\n");
}

void cd(Dir** target, char *name)
{
	// go to the parent
	if(strcmp(name, "..") == 0)
	{
		// if is not the root directory
		if((*target)->parent != NULL)
		{
			*target = (*target)->parent;
		}
		return;
	}

	// search for the dir
	Dir* dirIterator = (*target)->head_children_dirs;
	if(dirIterator == NULL)
	{
		return;
	}

	while (dirIterator != NULL)
	{
		// we found the directory
		if(strcmp(dirIterator->name, name) == 0)
		{
			*target = dirIterator;
			return;
		}
		dirIterator = dirIterator->next;
	}

	printf("No directories found!\n");
}

char *pwd (Dir* target)
{

}

void stop (Dir* target)
{
	return;
}

void tree (Dir* target, int level)
{
	if(target == NULL)
		return;

	char *leadingSpaces = (char *) malloc(sizeof(char) * level * 4 + 1);

	for(int i = 0; i < level; ++i)
	{
		strcat(leadingSpaces, "\t");
	}

	Dir *dirIterator = target;
	while(dirIterator != NULL)
	{
		printf("%s%s\n", leadingSpaces,  dirIterator->name);
		tree(dirIterator->head_children_dirs, level+1);
		dirIterator = dirIterator->next;
	}
	File *fileIterator = target->head_children_files;
	while(fileIterator != NULL)
	{
		printf("%s%s\n", leadingSpaces, fileIterator->name);
		fileIterator = fileIterator->next;
	}

	free(leadingSpaces);
	// print the directory list
	
}

void mv(Dir* parent, char *oldname, char *newname) {}

int main () {
	// variables initialisation and allocation
	Dir *root = initialiseDir("home", NULL);
	Dir *currentDirectory = root;



	char *name = (char *) malloc(sizeof(char) * MAX_INPUT_LINE_SIZE);
	// allocate memory for the command variable
	char *command = (char *) malloc(sizeof(char) * MAX_INPUT_LINE_SIZE);
	do
	{
		scanf("%s", command);
		// create a new file
		
		if(strcmp(command, "touch") == 0)
		{
			scanf("%s", name);
			touch(currentDirectory, name);
		}
		else if(strcmp(command, "mkdir") == 0)
		{
			scanf("%s", name);
			mkdir(currentDirectory, name);
		}
		else if(strcmp(command, "ls") == 0)
		{
			ls(currentDirectory);
		}
		else if(strcmp(command, "rm") == 0)
		{
			scanf("%s", name);
			rm(currentDirectory, name);
		}
		else if(strcmp(command, "rmdir") == 0)
		{
			scanf("%s", name);
			rmdir(currentDirectory, name);
		} 
		else if(strcmp(command, "cd") == 0)
		{
			scanf("%s", name);
			cd(&currentDirectory, name);
		}
		else if(strcmp(command, "tree") == 0)
		{
			tree(currentDirectory->head_children_dirs, 0);
		}
		
		/*
		Summary:
			Reads from stdin a string and breaks it down into command and in
			case it needs into a name.
		*/
	} while (strcmp(command, "stop") != 0);


	free(name);
	free(command);
	freeDir(&root);
	return 0;
}
