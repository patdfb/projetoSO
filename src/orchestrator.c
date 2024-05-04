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


int exec_command(char* arg){

	//Estamos a assumir numero maximo de argumentos
	char *exec_args[MAX_COMMAND];

	char *string;	
	int exec_ret = 0;
	int i=0;

	char* command = strdup(arg);

	string=strtok(command," ");
	
	while(string!=NULL){
		exec_args[i]=string;
		string=strtok(NULL," ");
		i++;
	}

	exec_args[i]=NULL;
	
	exec_ret=execvp(exec_args[0],exec_args);
	
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


    int log_fd = open("log.txt", O_CREAT , 0640);
        if(log_fd==-1){
            perror("Erro ao abrir o log.");
    }

    close(log_fd);


    if(fork()==0){
        int pipe_fd;

        if (mkfifo(FIFO_FILE, 0666) == -1) { // Cria o pipe nomeado
            perror("Erro ao criar FIFO.");
            _exit(EXIT_FAILURE);
        }
        int pos = 0;
        while(ligado){
            pipe_fd = open(FIFO_FILE, O_RDONLY); // Abre o pipe nomeado em modo leitura
            if (pipe_fd == -1) {
                perror("Erro ao abrir.");
                _exit(EXIT_FAILURE);
            }

            read(pipe_fd, &t, sizeof(struct Tarefa));
            log_fd = open("log.txt", O_WRONLY | O_APPEND);
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
            if(fork()==0){
                int bytes_read;
                log_fd = open("log.txt", O_RDWR);
                while((bytes_read = read(log_fd, &t, sizeof(struct Tarefa)))>0 && t.estado!=0);

                if(bytes_read != 0){
                    int res = exec_command(t.argumento);
                    t.estado = 1;
                    lseek(log_fd, -sizeof(struct Tarefa), SEEK_CUR);
                    write(log_fd, &t, sizeof(struct Tarefa));
                    _exit(res);
                    t.estado = 2;
                    lseek(log_fd, -sizeof(struct Tarefa), SEEK_CUR);
                    write(log_fd, &t, sizeof(struct Tarefa));
                }
                close(log_fd);
            }
            c++;
            if(c==max){
                wait(NULL);
                c--;
            }
        }

    }



    return 0;
}