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
                c++;
                if(c==max){
                    wait(NULL);
                    c--;
                }
            }
            close(log_fd);

        }

    }
    
    return 0;
}