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

void recursivePwd(Dir *target, char *s)
{
	if(target == NULL)
		return;

	recursivePwd(target->parent, s);
	strcat(s, "/");
	strcat(s, target->name);
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

	// print the directory list
	Dir *dirIterator = parent->head_children_dirs;
	while(dirIterator != NULL)
	{
		printf("%s\n", dirIterator->name);
		dirIterator = dirIterator->next;
	}
	// print the file list
	File *fileIterator = parent->head_children_files;
	while(fileIterator != NULL)
	{
		printf("%s\n", fileIterator->name);
		fileIterator = fileIterator->next;
	}
	

	
}

void rm (Dir* parent, char* name)
{
	// search for the file, remove it and re-establish the order

	File* fileIterator = parent->head_children_files;
	if(fileIterator == NULL)
	{
		printf("Could not find the file\n");
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
		printf("Could not find the dir\n");
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
	char *res = (char *) malloc(sizeof(char) * MAX_INPUT_LINE_SIZE);
	recursivePwd(target, res);
	return res;
}


void tree (Dir* target, int level)
{
	if(target == NULL)
		return;

	char *leadingSpaces = (char *) malloc(sizeof(char) * level * 4 + 1);

	for(int i = 0; i < level * 4; ++i)
	{
		strcat(leadingSpaces, " ");
	}

	Dir *dirIterator = target->head_children_dirs;
	while(dirIterator != NULL)
	{
		printf("%s%s\n", leadingSpaces,  dirIterator->name);
		tree(dirIterator, level+1);
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

void mvHelperDir(Dir* parent, char *oldname, char *newname)
{
	Dir* dirIterator = parent->head_children_dirs;
	if(dirIterator == NULL)
		return;

	Dir* oldDir;
	if(strcmp(dirIterator->name, oldname) == 0)
	{
		oldDir = dirIterator;
		parent->head_children_dirs = parent->head_children_dirs->next;
	}

	while(dirIterator->next != NULL)
	{
		if(strcmp(dirIterator->next->name, oldname) == 0)
		{
			oldDir = dirIterator->next;
			dirIterator->next = dirIterator->next->next;
		}
		dirIterator = dirIterator->next;
	}

	strcpy(oldDir->name, newname);
	oldDir->next = NULL;
	if(parent->head_children_dirs == NULL)
		parent->head_children_dirs = oldDir;
	else
		dirIterator->next = oldDir;
}

void mvHelperFile(Dir* parent, char *oldname, char *newname)
{
	File* fileIterator = parent->head_children_files;
	if(fileIterator == NULL)
		return;

	File* oldFile;

	// initial check
	if(strcmp(fileIterator->name, oldname) == 0)
	{
		oldFile = fileIterator;
		parent->head_children_files = parent->head_children_files->next;

	}

	// look in all of the list and go to the end
	while(fileIterator->next != NULL)
	{
		if(strcmp(fileIterator->next->name, oldname) == 0)
		{
			oldFile = fileIterator->next;
			fileIterator->next = fileIterator->next->next;
		}
		fileIterator = fileIterator->next;
	}
	// go to the end of the list
	strcpy(oldFile->name, newname);
	oldFile->next = NULL;
	if(parent->head_children_files == NULL)
		parent->head_children_files = oldFile;
	else
		fileIterator->next = oldFile;
}


void mv(Dir* parent, char *oldname, char *newname)
{
	int alreadyExistsFile = 0, alreadyExistsDir = 0, foundFile = 0, foundDir = 0;
	// check conditions
	Dir *dirIterator = parent->head_children_dirs;
	while(dirIterator != NULL)
	{
		// if we find a directory with that name
		if(strcmp(dirIterator->name, oldname) == 0)
		{
			foundDir = 1;
		}
		if(strcmp(dirIterator->name, newname) == 0)
		{
			alreadyExistsDir = 1;
			break;
		}
		dirIterator = dirIterator->next;
	}

	// look for file with that name
	File *fileIterator = parent->head_children_files;
	while(fileIterator != NULL)
	{
		if(strcmp(fileIterator->name, oldname) == 0)
		{
			foundFile = 1;
		}
		if(strcmp(fileIterator->name, newname) == 0)
		{
			alreadyExistsFile = 1;
			break;
		}
		fileIterator = fileIterator->next;
	}	

	if(alreadyExistsDir || alreadyExistsFile)
		printf("File/Director already exists\n");
	else if(!foundFile && !foundDir)
		printf("File/Director not found\n");
	else if(foundDir == 1 && alreadyExistsDir == 0)
		mvHelperDir(parent, oldname, newname);
	else
		mvHelperFile(parent, oldname, newname);

}

void stop(Dir *target)
{
	freeDir(&target);
	exit(0);
}

int main () {
	// variables initialisation and allocation
	Dir *root = initialiseDir("home", NULL);
	Dir *currentDirectory = root;

	char *name = (char *) malloc(sizeof(char) * MAX_INPUT_LINE_SIZE);
	char *name2 = (char *) malloc(sizeof(char) * MAX_INPUT_LINE_SIZE);

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
			tree(currentDirectory, 0);
			printf("\n");
		}
		else if(strcmp(command, "pwd") == 0)
		{
			char* res = pwd(currentDirectory);
			printf("%s\n", res);
			free(res);
		}
		else if(strcmp(command, "mv") == 0)
		{
			scanf("%s %s", name, name2);
			mv(currentDirectory, name, name2);
		}
		else if(strcmp(command, "stop") == 0)
		{
			free(name);
			free(name2);
			free(command);
			stop(root);
		}
	} while (1);

	return 0;
}
