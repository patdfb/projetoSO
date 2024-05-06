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

void exec_command_multi(char* arg, int ID){
    
	char *exec_args[MAX_COMMAND];
	char *string;	
	int i=0;

	char* command = strdup(arg);

	string=strtok(command,"|");


	while(string!=NULL){
        exec_args[i]=string;
		string=strtok(NULL,"|");
		i++;
	}

    for (int j=0;j<i;j++) {
        exec_command(exec_args[j],ID);
        wait(NULL);
    }

}    

void exec_command(char* arg,int ID){
    
	char *exec_args[MAX_COMMAND];
    int fd;
	char *string;	
	int exec_ret = 0;
	int i=0;
    int outfd = dup(STDOUT_FILENO);
    char out[50] = "tmp/";
    char idstring[10];
    sprintf(idstring, "%d",ID);
    strcat(out,idstring);
    strcat(out,".txt");
    fd = open(out, O_CREAT | O_WRONLY | O_APPEND, 0660);
    dup2(fd,STDOUT_FILENO);
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
    pid_t forked = fork();
    if (forked == 0) {
	    exec_ret=execv(exec_args[0],exec_args);
    }
    dup2(outfd,fd);
}

void status(pid_t pid) {
    int log_fd = open(LOG, O_RDONLY);
    struct Tarefa completed[MAX_COMMAND];
    struct Tarefa executing[MAX_COMMAND];
    struct Tarefa scheduled[MAX_COMMAND];
    struct Tarefa t;
    char clientID[20];
    for (int i=0;i<20;i++) {
        clientID[i] = '\0';
    }
    sprintf(clientID, "%d", pid);
    char listas[MAX_COMMAND*MAX_COMMAND];
    for (int i=0;i<MAX_COMMAND*MAX_COMMAND;i++) {
        listas[i] = '\0';
    }
    int c0 = 0, c1 = 0, c2 = 0;
    int bytes_read;
    while((bytes_read = read(log_fd, &t, sizeof(struct Tarefa))) > 0){
        int estado = t.estado;

        if(estado == 2){
            completed[c2].estado = t.estado;
            completed[c2].ID = t.ID;
            strcpy(completed[c2].argumento, t.argumento);
            completed[c2].tempoEstimado = t.tempoEstimado;
            completed[c2].tempoReal = t.tempoReal;
            completed[c2].pid = t.pid;
            completed[c2].multi = t.multi;
            c2++;
        } else if(estado == 1){
            executing[c1].estado = t.estado;
            executing[c1].ID = t.ID;
            strcpy(executing[c1].argumento, t.argumento);
            executing[c1].tempoEstimado = t.tempoEstimado;
            executing[c1].tempoReal = t.tempoReal;
            executing[c1].pid = t.pid;
            executing[c1].multi = t.multi;
            c1++;
        } else if(estado == 0){
            scheduled[c0].estado = t.estado;
            scheduled[c0].ID = t.ID;
            strcpy(scheduled[c0].argumento, t.argumento);
            scheduled[c0].tempoEstimado = t.tempoEstimado;
            scheduled[c0].tempoReal = t.tempoReal;
            scheduled[c0].pid = t.pid;
            scheduled[c0].multi = t.multi;
            c0++;
        }
    }
    close(log_fd);

    char tamanho[11] = "Executing\n";
    char tudo[MAX_COMMAND];
    int i;
    for (i=0;i<MAX_COMMAND;i++) {
        tudo[i] = '\0';
    }

    int pipe_fd2 = open(clientID,O_WRONLY); // Abre o pipe nomeado em modo leitura para receber a resposta do servidor (ID?????)
    if (pipe_fd2 == -1) {
        _exit(EXIT_FAILURE);
    }

    strcat(listas,"Executing\n");
    for(i=0; i<c1; i++){
        t = executing[i];
        sprintf(tudo, "%d", t.ID); //transforma int em string
        strcat(tudo, " ");
        if (t.multi == 0) {
            char *comando;
            comando = strtok(t.argumento, " ");
            strcat(tudo, comando);
        } else if (t.multi == 1) {
            char *comandoarray[MAX_COMMAND];
            char *string;	
	        int i=0;

	        char* command = strdup(t.argumento);

	        string=strtok(command,"|");

            while(string!=NULL){
                comandoarray[i]=string;
                string=strtok(NULL,"|");
                i++;
            }

            char *comandofinal;
            for (int j=0;j<i;j++) {
                comandofinal = strtok(comandoarray[j], " ");
                strcat(tudo,comandofinal);
                if (j != i-1) strcat(tudo," | ");
            }
        }
        strcat(tudo, "\n");
        strcat(listas,tudo);
    }

    strcat(listas,"\nScheduled\n");
    for(i=0; i<c0; i++){
        t = scheduled[i];
        sprintf(tudo, "%d", t.ID); //transforma int em string
        strcat(tudo, " ");
        if (t.multi == 0) {
            char *comando;
            comando = strtok(t.argumento, " ");
            strcat(tudo, comando);
        } else if (t.multi == 1) {
            char *comandoarray[MAX_COMMAND];
            char *string;	
	        int i=0;

	        char* command = strdup(t.argumento);

	        string=strtok(command,"|");

            while(string!=NULL){
                comandoarray[i]=string;
                string=strtok(NULL,"|");
                i++;
            }

            char *comandofinal;
            for (int j=0;j<i;j++) {
                comandofinal = strtok(comandoarray[j], " ");
                strcat(tudo,comandofinal);
                if (j != i-1) strcat(tudo," | ");
            }
        }
        strcat(tudo, "\n");
        strcat(listas,tudo);
    }

    strcat(listas,"\nCompleted\n");
    for(i=0; i<c2; i++){
        t = completed[i];
        sprintf(tudo, "%d", t.ID); //transforma int em string
        strcat(tudo, " ");
        if (t.multi == 0) {
            char *comando;
            comando = strtok(t.argumento, " ");
            strcat(tudo, comando);
        } else if (t.multi == 1) {
            char *comandoarray[MAX_COMMAND];
            char *string;	
	        int i=0;

	        char* command = strdup(t.argumento);

	        string=strtok(command,"|");

            while(string!=NULL){
                comandoarray[i]=string;
                string=strtok(NULL,"|");
                i++;
            }

            char *comandofinal;
            for (int j=0;j<i;j++) {
                comandofinal = strtok(comandoarray[j], " ");
                strcat(tudo,comandofinal);
                if (j != i-1) strcat(tudo," | ");
            }
        }
        char Tempo[30];
        sprintf(Tempo, "%d", t.tempoReal);
        strcat(tudo, " ");
        strcat(tudo, Tempo);
        strcat(tudo, " ms\n");
        strcat(listas,tudo);
    }
    write(pipe_fd2, listas, sizeof(listas));
    close(pipe_fd2);
    unlink(clientID);

    _exit(0);
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
        struct Tarefa t;

        if (mkfifo(FIFO_FILE, 0666) == -1) { // Cria o pipe nomeado
            perror("Erro ao criar FIFO.");
            _exit(EXIT_FAILURE);
        }

        int pos = 1;

        while(ligado){
            pipe_fd = open(FIFO_FILE, O_RDONLY); // Abre o pipe nomeado em modo leitura
            if (pipe_fd == -1) {
                perror("Erro ao abrir fifofile.");
                _exit(EXIT_FAILURE);
            }

            read(pipe_fd, &t, sizeof(struct Tarefa));
            if (strcmp(t.argumento,"status") == 0) {
                int forkou = fork();
                if (forkou == 0) {
                   status(t.pid);
                }
            } else {
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
                    if (t.multi == 0) {
                        exec_command(t.argumento,t.ID);
                    } else if (t.multi == 1) {
                        exec_command_multi(t.argumento,t.ID);
                    }
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