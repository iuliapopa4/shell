#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <readline/readline.h>
#include <readline/history.h>
#include <sys/wait.h>
#include <sys/stat.h>
#include <dirent.h>
#include <stdbool.h>
#include <errno.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <ctype.h>

#define MAX_COMMANDS 10
#define MAX_ARGS 10
#define MAX_LEN 100
#define TERMINAL_WIDTH 79


int isDir(char* name){
	struct stat s;
    if(stat(name,&s)==0){
        if(S_ISREG(s.st_mode))
            return 0;
        else if(S_ISDIR(s.st_mode))
            return 1;   
    }
    return 0;
}


void remove_directory(char* directory,bool i,bool v){
	DIR *dir;
	struct dirent *entry;
	dir=opendir(directory);
	if(dir==NULL){
		perror("dir");
		return;
	}
	if(isDir(directory)){
		if(i){
			printf("rm: descend into directory '%s'? ",directory);
			char answer[3];
			scanf("%s",answer);
			if(strcmp(answer,"y")!=0 && strcmp(answer,"Y")!=0)
				return;
		}
	}
	while((entry=readdir(dir))!=NULL){
		if(entry->d_type==DT_REG){
			char file[512];
			sprintf(file,"%s/%s",directory,entry->d_name);
			if(i){
				printf("rm: remove regular file '%s'? ",file);
				char answer[3];
				scanf("%s",answer);
				if(strcmp(answer,"y")!=0 && strcmp(answer,"Y")!=0)
					break;
			}
			else{
				int status=unlink(file);
				if(status==-1){
					printf("rm: cannot remove '%s': No such file or directory\n",file);
					return;
				}
				if(v)
					printf("removed '%s'\n",file);
			}
			int status=unlink(file);
				if(status==-1){
					break;
				}
			if(v)
				printf("removed '%s'\n",file);
		}	
		else if(entry->d_type==DT_DIR){
			char subdir[512];
			if(strcmp(entry->d_name,".")==0 || strcmp(entry->d_name,"..")==0)
				continue;
			sprintf(subdir,"%s/%s",directory,entry->d_name);
			remove_directory(subdir,i,v);
		}
	}
		closedir(dir);
		if(i){
			printf("rm: remove directory '%s'? ",directory);
			char answer[3];
			scanf("%s",answer);
			if(strcmp(answer,"y")!=0 && strcmp(answer,"Y")!=0)
				return;
		}
		if(rmdir(directory)==0)
			if(v)printf("removed directory '%s'\n",directory);
}


void rm2(char* file_name,bool i,bool v,bool r){
	if(isDir(file_name)==1){
		if(r){
			remove_directory(file_name,i,v);
			return;
		}
		else {
			printf("rm: cannot remove '%s': Is a directory\n",file_name);
			return;
		}
	}
	else if(access(file_name, F_OK)==0){
		if(i){
			printf("rm: remove regular file '%s'? ",file_name);
			char answer[3];
			scanf("%s",answer);
			if(strcmp(answer,"y")!=0 && strcmp(answer,"Y")!=0)
				return;
		}
		int status=unlink(file_name);
		if(status==-1){
			printf("rm: cannot remove '%s': No such file or directory\n",file_name);
			return;
		}
		if(v){
			printf("removed '%s'\n",file_name);
		}
	}
	else{
		printf("rm: cannot remove '%s': No such file or directory\n",file_name);
		return;
	}
}


void rm(char* args){
	char* input2=malloc(sizeof(args)+1);
	strcpy(input2,args);
	bool r=false;
	bool i=false;
	bool v=false;
	char* file_name;
	char files[100][100];
	int index=0,j=0;
	char* arg=strtok(args+3," ");
	while(arg!=NULL){
		if(arg[0]=='-'){
			switch(arg[1]){
				case 'r':
					r=true;
					break;
				case 'R':
					r=true;
					break;
				case 'v':
					v=true;
					break;
				case 'i':
					i=true;
					break;
				default:
					system(input2);
					//printf("rm: invalid option\n");
					return;
			}
		}else{
			if(arg!=NULL)
			strcpy(files[index++],arg);
		}
		arg=strtok(NULL," ");				
	}
	for(j=0;j<index;j++){
		file_name=files[j];
		rm2(file_name,i,v,r);	
	}
	free(input2);
}


void move(char* source,char* dest,bool i,bool S,char* suffix,bool b){
	if(S==true && suffix==NULL){
		printf("mv: option requires an argument -- 'S'\n");
		return;
	}
	struct stat path1;
	stat(source,&path1);
	struct stat path2;
	stat(dest,&path2);
	if(access(source,F_OK)==-1 || fopen(source,"r")==NULL){
		printf("mv: cannot stat '%s': No such file or directory\n",source);
		return;
	}
	if(S_ISREG(path1.st_mode) && S_ISREG(path2.st_mode)){
		if(b || S){
			char backup[500];
			strcpy(backup,dest);
			if(S)
				strcat(backup,suffix);
			else
				strcat(backup,"~"); 
			rename(dest,backup);
		}
		if(i){
			printf("mv: overwrite '%s'? ",dest);
			char answer[3];
			scanf("%s",answer);
			if(strcmp(answer,"y")!=0 && strcmp(answer,"Y")!=0)
				return;
		}
		rename(source,dest);
	}
	else if ((S_ISREG(path1.st_mode) && S_ISREG(path2.st_mode)==0)||(S_ISREG(path1.st_mode)==0 && S_ISREG(path2.st_mode)==0)) {
		char new_path[1024];
		snprintf(new_path,sizeof(new_path),"%s/%s",dest,source);
		if(access(new_path, F_OK)==0){
			if(i){
			printf("mv: overwrite '%s'? ",source);
			char answer[3];
			scanf("%s",answer);
			if(strcmp(answer,"y")!=0 && strcmp(answer,"Y")!=0)
				return;
			}
		}
		rename(source,new_path);
	}
		
}


void mv(char* input){
	char* input2=malloc(sizeof(input));
	strcpy(input2,input);
	bool i=false;
	bool S=false;
	bool b=false;
	bool t=false;
	char* target;
	char files[500][500];
	char* suffix=NULL;
	int index=0;
	char* arg=strtok(input+3," ");
	while(arg!=NULL){
		if(arg[0]=='-'){
			switch(arg[1]){
				case 'i':
					i=true;
					break;
				case 'S':
					S=true;
					arg=strtok(NULL," ");
					if(arg!=NULL){
						suffix=malloc(strlen(arg)+1);
						suffix=arg;
					}
					else{
						printf("mv: option requires an argument -- 'S'\n");
						return;
					}
					break;
				case 'b':
					b=true;
					break;
				case 't':
					target=strtok(NULL," ");
					if(target==NULL){
						printf("mv: option requires an argument -- 't'\n");
						return;
					}
					t=true;
					break;
				default:
				system(input2);
				//printf("mv: invalid option\n");
				break;
			}
		}
		else {
			strcpy(files[index++],arg);
		}
		arg=strtok(NULL," ");
	}
	if(t==false){
		if(index==0){
			printf("mv: missing file operand\n");
		}
		else if(index==1){
			printf("mv: missing destination file operand after '%s'\n",files[0]);
		}
		else{
			if(isDir(files[index-1])==0){
				printf("mv: target '%s' is not a directory\n",files[index-1]);
			}
			else{
				char* dest=files[index-1];
				int i;
				for(i=0;i<index-1;i++)
					move(files[i],dest,i,S,suffix,b);
			}
		}
	}
	else{
		if(access(target, F_OK)!=0)
			printf("mv: failed to access '%s': No such file or directory\n",target);
		else if(isDir(target)==0)
			printf("mv: target '%s' is not a directory\n",target);
		else{
			char* dest=target;
			int i;
			for(i=0;i<index;i++){
				move(files[i],dest,i,S,suffix,b);
			}
		}
	}
	free(input2);
}


char* toLowerCase(char* str) {
    int i;
    int len=strlen(str);
    char* newStr=(char*)malloc((len+1)*sizeof(char));
    for(i=0;i<len;i++)
        newStr[i]=tolower(str[i]);
    newStr[len]='\0';
    return newStr;
}


void dir(char* path){
	int i=0,list_size=0,j=0,max=0;
	char dir_list[200][200];
	DIR *dir;
	struct dirent *entry;
	if(path)
		dir=opendir(path);
	else dir=opendir(".");
	if(dir==NULL){
		printf("dir: cannot access '%s': No such file or directory\n",path);
		return;
	}
	while((entry=readdir(dir))!=NULL){
		if(entry->d_name[0]=='.')
			continue;
		strcpy(dir_list[i++],entry->d_name);
		list_size++;
	}
	if(list_size==0){
		return;
	}

    for(i=0;i<list_size;i++)
        if(strlen(dir_list[i])>max)
            max=strlen(dir_list[i]);
      
	char aux[100];	
	for(i=0;i<list_size-1;i++)
		for(j=0;j<list_size-1-i;j++){
			char *first, *second;
			first=toLowerCase(dir_list[j]);
			second=toLowerCase(dir_list[j+1]);
			if(strcmp(first,second)>0){
				strcpy(aux,dir_list[j]);
				strcpy(dir_list[j],dir_list[j+1]);
				strcpy(dir_list[j+1],aux);
			}
		}
	int length[list_size+1],total_length=0;
	for(i=0;i<list_size;i++){
		length[i]=strlen(dir_list[i]);
		total_length+=length[i];
	}
	
	if(total_length+(list_size-1)*2<TERMINAL_WIDTH){
		for(i=0;i<list_size-1;i++)
			printf("%s  ",dir_list[i]);
		printf("%s\n",dir_list[list_size-1]);
	}
	else{
		int ok=1,cols=0,rows=0,found=0,size=0,no=0,k;
		while(ok){
			for(rows=1;found==0;rows++){
				no=0;
				for(k=0;k<rows;k++)
				{
					size=0;
					cols=0;
					for(i=k;i<list_size;i+=rows){
						cols++;
						size+=(length[i])+1;
					}
					if(size>TERMINAL_WIDTH-2*cols+2){
						no=1;
					}
				}
				if(no==0){
					found=1;
					break;	
				}
			}
			ok=0;
			cols=list_size/rows;
			if(list_size%rows)cols+=1;
		}
		int maxi[cols+1];
		k=0;
		for(j=0;j<cols;j++){
			max=0;
			for(i=0;i<rows;i++){
				if(k<list_size && length[k]>max)
					max=length[k];
				k++;
			}
			maxi[j]=max;
		}
		
		k=0;
		int numbers[rows+1][cols+1];
		for(j=0;j<cols;j++)
			for(i=0;i<rows;i++)
				numbers[i][j]=k++;
				
		int space;
		for(i=0;i<rows;i++){
			for(j=0;j<cols;j++){
				printf("%s",dir_list[numbers[i][j]]);
				for(space=0;space<=(maxi[j]-strlen(dir_list[numbers[i][j]]));space++)
					printf(" ");
				if(j!=cols-1)printf(" ");
			}
			printf("\n");
		}
	}
	closedir(dir);
}


int count_pipes(const char* string){
	int p=0,i;
	for(i=0;i<strlen(string);i++)
		if(string[i]=='|')p++;
	return p;		
}


void pipes_command(char* input){
	char* commands[MAX_COMMANDS];
	int num_commands=0;
	char* p=strtok(input,"|");
	while(p!=NULL){
		commands[num_commands++]=p;
		p=strtok(NULL,"|");
	}
	int fd[2],n,input_fd=0,i;
	pid_t child_pids[MAX_COMMANDS];
	char* args[MAX_ARGS];
	int num_args;
	for(i=0; i<num_commands;i++){
		pipe(fd);
		child_pids[i]=fork();
		if(child_pids[i]==0){
		    dup2(input_fd,STDIN_FILENO);
		    if(i<num_commands-1)
		        dup2(fd[1],STDOUT_FILENO);
		    close(fd[0]);
		    close(fd[1]);
		    num_args=0;
		    char* arg=strtok(commands[i]," ");
		    while(arg!=NULL){
		        args[num_args++]=arg;
		        arg=strtok(NULL," ");
		    }
		    args[num_args]=NULL;
		    execvp(args[0],args);
		    return ;
		} 
		else{
		    close(fd[1]);
		    input_fd=fd[0];
		}
	}
	for(i=0;i<num_commands;i++)
		waitpid(child_pids[i],&n,0);
}


int main(){
	char previous[300];
	while(1){
		char* input=readline(">");
		char* input2=malloc(sizeof(input)+1);
		if(strcmp(previous,input)){
			add_history(input);
			strcpy(previous,input);
		}
		strcpy(input2,input);
		if(strcmp(input,"exit")==0)return 0;
		
		if(strchr(input,'>')){
			char *arg;
			char *args[MAX_LEN];
			int i, out, append;
			FILE *out_file;
			out=0;
		    append=0;
		    arg=strtok(input," ");
		    i=0;
		    while(arg!=NULL){
		        if(strcmp(arg,">")==0)
		            out=1;
		        else if(strcmp(arg,">>")==0)
		            append=1;
		        else if(out || append){
		            if(out){
		                out_file=fopen(arg,"w");
		                dup2(fileno(out_file),1);
		            }else if(append){
		                out_file=fopen(arg,"a");
		                dup2(fileno(out_file),1);
		            }
		            out=0;
		            append=0;
		        }else{
		            args[i]=arg;
		            i++;
		        }
		        arg=strtok(NULL," ");
		    }
		    args[i]=NULL;
		    if(fork()==0){
		        execvp(args[0],args);
		        exit(0);
		    }else{
		        wait(NULL);
		        if(out_file)
		            fclose(out_file);
		    }
		    return 0;
		}
		
		if(count_pipes(input))
			pipes_command(input);
		else{
			char* command=strtok(input," \n");
			if(strcmp(command,"rm")==0){
				if(strcmp(command,input2)==0)
					printf("rm: missing operand\n");
				else
					rm(input);
			}	
			else if(strcmp(command,"mv")==0){
				if(strcmp(command,input2)==0){
					printf("mv: missing file operand\n");
				}
				else
					mv(input);
			}		
			else if(strcmp(command,"dir")==0){
				char* path=strtok(NULL," ");
				if(isDir(path)==0 && access(path, F_OK)==0)
					printf("%s\n",path);
				else {
					char* path2=strtok(NULL," ");
					if(path2){
						printf("%s:\n",path);
						dir(path);
						while(path2!=NULL){
							printf("\n");
							printf("%s:\n",path2);
							dir(path2);
							path2=strtok(NULL," ");
						}	
					}
					else
						dir(path);	
				}
			}
			else if(strcmp(input2,"help")==0){
				printf("Available commands:\n");
				printf("rm [-i] [-v] [-r] [-R]\n");
				printf("mv [-i] [-t] [-S] [-b]\n");
				printf("dir\n");
			}
			else{
				system(input2);	
			} 
		}
		free(input2);
		free(input);		
	}
	return 0;
}
