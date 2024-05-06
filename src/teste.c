#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/time.h>
#include <sys/wait.h>
#include "struct.h"
#include <time.h>
#define MAX_COMMAND 300

int exec_command(char* arg){
    
	char *exec_args[MAX_COMMAND];
    int fd;
	char *string;	
	int exec_ret = 0;
	int i=0;
    int outfd = dup(STDOUT_FILENO);

	char* command = strdup(arg);

	string=strtok(command," ");
	
    char pontoBarra[MAX_COMMAND] = "../include/"; 
    strcat(pontoBarra, string);

	while(string!=NULL){
        exec_args[i]=string;
		string=strtok(NULL," ");
		i++;
	}
    exec_args[0] = pontoBarra;
    
	exec_args[i]=NULL;

    pid_t fork3 = fork();
    if (fork3 == 0) {
	    execv(exec_args[0],exec_args);
    }
    
	return 0;
}

int main(){

    pid_t fork2 = fork();

    if (fork2 == 0) {
        struct timeval start,end;
        double elapsedTime;
        gettimeofday(&start,NULL);
        int res = exec_command("void 4");
        int status;
        wait(&status);
        if(WIFEXITED(status)){
            gettimeofday(&end,NULL);
            elapsedTime = (end.tv_sec - start.tv_sec) * 1000.0;
            elapsedTime += (end.tv_usec - start.tv_usec) / 1000.0;
            printf("exit status is %f\n",elapsedTime);
        }
    }/* else {
        int status;
        wait(&status);
        if(WIFEXITED(status)){
            int result = WEXITSTATUS(status);
            printf("exit status is %d\n",result);
        }
    }*/
    
    return 0;
}