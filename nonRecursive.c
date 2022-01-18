#include <stdio.h> /* For printf, fprintf */ 
#include <string.h> /* For strcmp */ 
#include <ctype.h> /* For isdigit */ 
#include <fcntl.h> /* For O_RDONLY */
#include <unistd.h>
#include <sys/syscall.h>
#include <dirent.h> /* For getdents */ 
#include <sys/stat.h> /* For IS macros */ 
#include <sys/types.h> /* For modet */ 
#include <time.h> /* For localtime, asctime */
#include <stdlib.h>
#define MAX_FILES 100 
#define MAX_FILENAME 257 
#define NOT_FOUND -1 
#define FOREVER -1 
#define DEFAULT_DELAY_TIME 10
#define DEFAULT_LOOP_COUNT FOREVER

struct file{ // linked list to be sorted
	//char fileName[MAX_FILENAME];
	char *fileName;
	struct stat stats;
	struct file * ptr;
};
struct file *f_head=NULL;
int add_file(char *name,struct stat * stats){
	struct file* file_list=(struct file*) malloc(sizeof(struct file));
	printf("MADE FILE_LIST\n");
	//strdup(file_list->fileName,name);//formally strcpy
	//file_list->fileName=strdup(name);
	//file_list->fileName=malloc(strlen(name)+1);
	//file_list->fileName=malloc(sizeof(char) * (strlen(name)+1));
	//strcpy(file_list->fileName,name);
	file_list->fileName=name;
	printf("%s %s \n",file_list->fileName,name);
	//file_list->fileName=name;
	file_list->stats=*stats; // error here
	file_list->ptr=f_head;
	f_head=file_list;
}
struct dir{
	//char dirname[MAX_FILENAME];
	char *dirname;
	struct dir * ptr; // we'll just make each dir go thru
};
struct dir *d_head=NULL; // lol
struct dir *d_current=NULL;
struct dir *d_PREV=NULL;
struct dir *d_TEMP=NULL;
struct dir* add_dir(char *name){
	struct dir* dir_list=(struct dir*) malloc(sizeof(struct dir));
	printf("MADE DIR_LIST\n");
        //strdup(dir_list->dirname,name);//formally strcpy
	//dir_list->dirname=strdup(name);
	//dir_list->dirname=malloc(strlen(name));// +1 inside malloc
	//malloc(sizeof(char) * (strlen(tribe_name) + 1));
	//dir_list->dirname=malloc(sizeof(char) * (strlen(name)+1));
	//strcpy(dir_list->dirname,name);
	dir_list->dirname=name;
	printf("%s %s \n",dir_list->dirname,name);
	//dir_list->dirname=name;
	//dir_list->ptr=d_head;
	dir_list->ptr=NULL;
	//dir_list->ptr=d_PREV; // HERE
	d_head=dir_list;
	return dir_list;
}

int monitorFile(char *fileName){
	struct stat statBuf; 
	mode_t mode; 
	int result; 
	result = lstat(fileName, &statBuf); /* LSTAT SHOULD BE USED HERE Obtain file status */ 
	if ( result == -1 ) { /* Status was not available */ 
		fprintf( stderr, "Cannot stat %s \n", fileName ); 
		return 0; 
	} 
	mode = statBuf.st_mode;  /* Mode of file */  
	if ( S_ISDIR( mode ) ){ /* Directory */ 
		//processDirectory( fileName );
		printf("FOUND DIR %s \n",fileName);
		d_TEMP=add_dir(fileName);
		//printf("|| %s \n",d_TEMP->dirname);
		d_PREV->ptr=d_TEMP; // i think that's what I want to do
		printf("my last node %s points to %s \n",d_PREV->dirname,d_TEMP->dirname);
		d_PREV=d_TEMP;
		printf("d_PREV IS NOW :: %s\n",d_TEMP->dirname);
	}else if(S_ISREG(mode)){
	       	printf("FOUND FILE\n");	
		//updateStat( fileName, &statBuf );
		add_file(fileName, &statBuf);
	}
}

int traverse(){
	struct dir *d_h_ptr=d_head;
	struct file *f_h_ptr=f_head;
	
	DIR *dir;
	struct dirent *dirEntry;
	//char fileName[MAX_FILENAME];
	char *fileName=NULL;
	dir=opendir(d_h_ptr->dirname); /* Open for reading */
	d_current=d_head;
	while(1) { /* Read all directory entries */
		dirEntry=readdir(dir);
		printf("WHERE IS D_CURRENT NOW :: %s\n",d_current->dirname);
		if(d_current->ptr==NULL){
			printf("PTR NULLZIEZ\n");
		} else {
			printf("PTR @ %s\n",d_current->ptr->dirname);
		}
		printf("\n----\n");
		struct dir *n=d_current;
		while (n != NULL) {
        		printf(" %s ", n->dirname);
        		n = n->ptr;
    		}
		printf("\n----\n");
		if(dirEntry==NULL){
			if(d_current->ptr==NULL){
				break;
			}
			printf("ptr is not null, dirEntry is null .. changing dir\n");
			//closedir(d_current->dirname);
			//closedir(dir);
			d_current=d_current->ptr;
			printf("%s \n",d_current->dirname);
			dir=opendir(d_current->dirname);
			continue;
			//dirEntry=readdir(dir);
		}
		if ( strcmp(dirEntry->d_name, ".") != 0 && strcmp( dirEntry->d_name, "..") != 0) {
			//if(d_current->ptr!=NULL){printf("BEFORE :: %s \n",d_current->ptr->dirname);}
			if(d_current->ptr!=NULL){
				printf("BEFORE :: %s \n",d_current->ptr->dirname);
				//snprintf( fileName, sizeof(fileName),"%s/%s",d_current->dirname,dirEntry->d_name );
				fileName=malloc(strlen(d_current->dirname)+strlen(dirEntry->d_name)+strlen("/")+1);
				sprintf(fileName,"%s/%s",d_current->dirname,dirEntry->d_name);
				//strcat(strcat(strcpy(fileName,d_current->dirname),"/"),dirEntry->d_name);
                        	printf("AFTER :: %s \n",d_current->ptr->dirname);
			} else {
				//strcat(strcat(strcpy(fileName,d_current->dirname),"/"),dirEntry->d_name);
				fileName=malloc(strlen(d_current->dirname)+strlen(dirEntry->d_name)+strlen("/")+1);
                                sprintf(fileName,"%s/%s",d_current->dirname,dirEntry->d_name);
				//snprintf( fileName, sizeof(fileName),"%s/%s",d_current->dirname,dirEntry->d_name );
			}//printf("AFTER :: %s \n",d_current->ptr->dirname);
			printf("%s \n",fileName);
			monitorFile( fileName );
		}
	}
	closedir(dir);
}

int main(int argc, char * argv[]){

	d_PREV=add_dir(argv[1]); // passing dir
	traverse();
	struct file * fh=f_head;
	printf("%s\n",fh->fileName);
	return 0;
}

// end
