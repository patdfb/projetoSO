#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/time.h>
#include "struct.h"

#define FIFO_FILE "pipe"
#define MAX_COMMAND 300
#define LOG "tmp/log.txt"



int exec_command(char* arg){
    
	char *exec_args[MAX_COMMAND];

	char *string;	
	int exec_ret = 0;
	int i=0;

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

	exec_ret=execv(exec_args[0],exec_args);

	return exec_ret;
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
    struct Tarefa t;


    int log_fd = open(LOG, O_CREAT , 0640);
    if(log_fd==-1){
        perror("Erro ao abrir o log.");
    }

    perror("758");
    close(log_fd);

    pid_t fork1 = fork();
    perror("1");
    if(fork1==0){
        int pipe_fd;
        
        perror("2");

        if (mkfifo(FIFO_FILE, 0666) == -1) { // Cria o pipe nomeado
            perror("Erro ao criar FIFO.");
            _exit(EXIT_FAILURE);
        }
        int pos = 1;
        perror("3");
        while(ligado){
            pipe_fd = open(FIFO_FILE, O_RDONLY); // Abre o pipe nomeado em modo leitura
            if (pipe_fd == -1) {
                perror("Erro ao abrir.");
                _exit(EXIT_FAILURE);
            }

            read(pipe_fd, &t, sizeof(struct Tarefa));
            log_fd = open(LOG, O_WRONLY | O_APPEND);
            t.ID = pos++;
            t.estado = 0;
            write(log_fd, &t, sizeof(struct Tarefa));
            close(log_fd);
            
        }
        

        //colocar a começar a contar o tempo de execução da tarefa

        // Escreve no log o estado de execução + o tempo demorado

        // ESCALONAMENTO !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
        close(pipe_fd);
        unlink(FIFO_FILE);
        
    } else{
<<<<<<< HEAD
<<<<<<< HEAD
        int c=0,i=0,p,maxp =0;
        struct Tarefa lista[1024];
        int tarefados[max];
        __clock_t inicio,fim;
        for (c = 0;c<max;c++){
            tarefados[c]= 0;
        }
        while(1){
            int bytes_read;
            struct Tarefa t;
            log_fd = open(LOG, O_RDONLY);
            if(log_fd==-1){
                perror("Erro ao abrir o log.");
            }
            while((bytes_read = read(log_fd, &t, sizeof(struct Tarefa)))>0){
                if(t.ID>maxp){
                    lista[t.ID].ID = t.ID;
                    lista[t.ID].tempoEstimado = t.tempoEstimado;
                    lista[t.ID].pid = t.pid;
                    lista[t.ID].estado = 0;
                    lista[t.ID].tempoReal = 0;
                    strcpy(lista[t.ID].argumento,t.argumento);
                    maxp = t.ID;
                }
            }
            close(log_fd);

            for(i = 0; i<maxp;i++){
                for(c=0;c<max;c++){
                    if(lista[i].ID==tarefados[c] && lista[i].estado==2)tarefados[c] = 0;
                }
            }
            p = -1;
            for(c=0;c<max;c++){
                if(tarefados[c]==0){
                    p = c;
                }
            }

            for(i = 0; i<maxp && lista[i].estado !=0;i++);

            if(p != -1 && lista[i].estado == 0){
                lista[i].estado = 1;

                tarefados[p] = lista[i].ID;
                pid_t fork2 = fork();
                if(fork2==0){
                    struct timeval start,end;
                    double elapsedTime;
                    gettimeofday(&start,NULL);
                    perror("adeus loren");
                    exec_command(lista[i].argumento);
                    int status;
                    wait(&status);
                    if(WIFEXITED(status)){
                        perror("merda do pedro");
                        gettimeofday(&end,NULL);
                        elapsedTime = (end.tv_sec - start.tv_sec) * 1000.0;
                        elapsedTime += (end.tv_usec - start.tv_usec) / 1000.0;
                        lista[i].tempoReal = (int)elapsedTime;
                        lista[i].estado = 2;
                        perror("ola lorem");
                    }
                }
            
            }

            log_fd = open(LOG, O_RDWR);
            if(log_fd==-1){
                perror("Erro ao abrir o log.");
            }
            while((bytes_read = read(log_fd, &t, sizeof(struct Tarefa)))>0 && t.ID >= maxp){
                if(t.tempoEstimado != lista[t.ID].tempoEstimado || t.estado != lista[t.ID].estado){
                    lseek(log_fd,-sizeof(struct Tarefa),SEEK_CUR);
                    write(log_fd,&t,sizeof(struct Tarefa));
                }
            }
            close(log_fd);
                    
=======
        int c=0;
        while(1){
            int bytes_read;
            log_fd = open(LOG, O_RDWR);
            if(log_fd==-1){
                perror("Erro ao abrir o log.");
            }
            while((bytes_read = read(log_fd, &t, sizeof(struct Tarefa)))>0 && t.estado!=0);
            if(bytes_read != 0){
                pid_t fork2 = fork();
                if(fork2==0){
                    perror("5");
                    t.estado = 1;
                    lseek(log_fd, -sizeof(struct Tarefa), SEEK_CUR);
                    perror("6");
                    write(log_fd, &t, sizeof(struct Tarefa));
                    perror("7");
                    write(1, t.argumento, sizeof(t.argumento));
                    int res = exec_command(t.argumento);
                    perror("8");
                    close(log_fd);
                    _exit(t.ID);
                    
                } else{
                    perror("9");
                    int status;
                    wait(&status);
                    if(WIFEXITED(status)){
                        perror("10");
                        int bytes_read;
                        log_fd = open(LOG, O_RDWR);
                            if(log_fd==-1){
                            perror("Erro ao abrir o log.");
                        }
                        int result = WEXITSTATUS(status);
                        while((bytes_read = read(log_fd, &t, sizeof(struct Tarefa)))>0 && t.ID!=result);
                        t.estado = 2;
                        lseek(log_fd, -sizeof(struct Tarefa), SEEK_CUR);
                        write(log_fd, &t, sizeof(struct Tarefa));
                        close(log_fd);
                    }
                }
=======
        int c=0;
        while(1){
            int bytes_read;
            log_fd = open(LOG, O_RDWR);
            if(log_fd==-1){
                perror("Erro ao abrir o log.");
            }
            while((bytes_read = read(log_fd, &t, sizeof(struct Tarefa)))>0 && t.estado!=0);
            if(bytes_read != 0){
                pid_t fork2 = fork();
                if(fork2==0){
                    perror("5");
                    t.estado = 1;
                    lseek(log_fd, -sizeof(struct Tarefa), SEEK_CUR);
                    perror("6");
                    write(log_fd, &t, sizeof(struct Tarefa));
                    perror("7");
                    write(1, t.argumento, sizeof(t.argumento));
                    int res = exec_command(t.argumento);
                    perror("8");
                    close(log_fd);
                    _exit(t.ID);
                    
                } else{
                    perror("9");
                    int status;
                    wait(&status);
                    if(WIFEXITED(status)){
                        perror("10");
                        int bytes_read;
                        log_fd = open(LOG, O_RDWR);
                            if(log_fd==-1){
                            perror("Erro ao abrir o log.");
                        }
                        int result = WEXITSTATUS(status);
                        while((bytes_read = read(log_fd, &t, sizeof(struct Tarefa)))>0 && t.ID!=result);
                        t.estado = 2;
                        lseek(log_fd, -sizeof(struct Tarefa), SEEK_CUR);
                        write(log_fd, &t, sizeof(struct Tarefa));
                        close(log_fd);
                    }
                }
>>>>>>> parent of d0ed313 (tenho medo do computador crashar)
                c++;
                if(c==max){
                    wait(NULL);
                    c--;
                }
            }
            close(log_fd);

<<<<<<< HEAD
>>>>>>> parent of d0ed313 (tenho medo do computador crashar)
=======
>>>>>>> parent of d0ed313 (tenho medo do computador crashar)
        }

    }
    
    return 0;
}