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

#define FIFO_FILE "pipe"
#define MAX_COMMAND 300
#define LOG "tmp/log.txt"



void exec_command(char* arg){
    
	char *exec_args[MAX_COMMAND];
    int fd;
	char *string;	
	int exec_ret = 0;
	int i=0;
    int outfd = dup(STDOUT_FILENO);

	char* command = strdup(arg);

	string=strtok(command," ");
	
    char pontoBarra[MAX_COMMAND] = "include/"; 
    strcat(pontoBarra, string);

	while(string!=NULL){
        exec_args[i]=string;
		string=strtok(NULL," ");
		i++;
	}

    exec_args[0] = pontoBarra;
    
	exec_args[i]=NULL;
    fd = open("tmp/out.txt", O_CREAT | O_WRONLY | O_APPEND, 0660);
    dup2(fd,STDOUT_FILENO);
    pid_t forked = fork();
    if (forked == 0) {
	    exec_ret=execv(exec_args[0],exec_args);
    }
    dup2(outfd,fd);

}

int status() {
    return 0;
}


int main(int argc, char* argv[]) {
    if (argc != 4) {
        perror("Argumentos insuficientes");
        _exit(EXIT_FAILURE);
    }
    int max = atoi(argv[2]);

    unlink(FIFO_FILE);

    int ligado = 1;


    int log_fd = open(LOG, O_CREAT , 0640);
    if(log_fd==-1){
        perror("Erro ao abrir o log.");
    }

    close(log_fd);

    pid_t fork1 = fork();
    if(fork1==0){
        int pipe_fd;
        

        if (mkfifo(FIFO_FILE, 0666) == -1) { // Cria o pipe nomeado
            perror("Erro ao criar FIFO.");
            _exit(EXIT_FAILURE);
        }

        int pos = 1;

        while(ligado){
            pipe_fd = open(FIFO_FILE, O_RDONLY); // Abre o pipe nomeado em modo leitura
            if (pipe_fd == -1) {
                perror("Erro ao abrir.");
                _exit(EXIT_FAILURE);
            }
            struct Tarefa t;
            read(pipe_fd, &t, sizeof(struct Tarefa));
            log_fd = open(LOG, O_WRONLY | O_APPEND);
            t.ID = pos;
            t.estado = 0;
            write(log_fd, &t, sizeof(struct Tarefa));
            close(log_fd);

            char taskID[10];
            char clientID[20];
            for (int i=0;i<20;i++) {
                clientID[i] = '\0';
            }
            char printable[50] = "TASK ";

            for (int i=0;i<10;i++) {
                taskID[i] = '\0';
            }
            sprintf(clientID, "%d", t.pid);
            sprintf(taskID, "%d", pos);
            strcat(printable,taskID);
            strcat(printable," Received\n");

            int pipe_fd2 = open(clientID, O_WRONLY); 

            write(pipe_fd2, printable, sizeof(printable));

            close(pipe_fd2);
            unlink(clientID);

            pos++;
        }
        

        //colocar a começar a contar o tempo de execução da tarefa

        // Escreve no log o estado de execução + o tempo demorado

        // ESCALONAMENTO !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
        close(pipe_fd);
        unlink(FIFO_FILE);
        
    } else{
        int c=0,p;
        int tarefados[max];
        __clock_t inicio,fim;
        for (c = 0;c<max;c++){
            tarefados[c]= 0;
        }
        while(1){
            struct Tarefa t;
            int bytes_read;
            log_fd = open(LOG, O_RDONLY);
            if(log_fd==-1){
                perror("Erro ao abrir o log.");
            }
            while((bytes_read = read(log_fd, &t, sizeof(struct Tarefa)))>0) {
                for(c=0;c<max;c++){
                    if(t.ID==tarefados[c] && t.estado==2)tarefados[c] = 0;
                }
            }
            close(log_fd);
            p = -1;
            for(c=0;c<max;c++){
                if(tarefados[c]==0)p = c;
            }
            log_fd = open(LOG, O_RDWR);
            while((bytes_read = read(log_fd, &t, sizeof(struct Tarefa)))>0 && t.estado!=0);
            if(bytes_read != 0 && p != -1 && t.estado == 0){
                t.estado = 1;
                lseek(log_fd, -sizeof(struct Tarefa), SEEK_CUR);
                write(log_fd, &t, sizeof(struct Tarefa));
                close(log_fd);
                tarefados[p] = t.ID;
                pid_t fork2 = fork();
                if(fork2==0){
                    struct timeval start,end;
                    double elapsedTime;
                    gettimeofday(&start,NULL);
                    int tID = t.ID;
                    perror("adeus loren");
                    exec_command(t.argumento);
                    int status;
                    wait(&status);
                    if(WIFEXITED(status)){
                        struct Tarefa t2;
                        int log_fd2 = open(LOG, O_RDWR);
                        while((bytes_read = read(log_fd2, &t2, sizeof(struct Tarefa)))>0 && t2.ID!=tID);
                        perror("merda do pedro");
                        gettimeofday(&end,NULL);
                        elapsedTime = (end.tv_sec - start.tv_sec) * 1000.0;
                        elapsedTime += (end.tv_usec - start.tv_usec) / 1000.0;
                        t2.tempoReal = (int)elapsedTime;
                        t2.estado = 2;
                        lseek(log_fd2, -sizeof(struct Tarefa), SEEK_CUR);
                        write(log_fd2, &t2, sizeof(struct Tarefa));
                        close(log_fd2);
                        perror("ola lorem");
                    }
                }
            
            } else {close(log_fd);}
                    
        }

    }
    
    return 0;
}