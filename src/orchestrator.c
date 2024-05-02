#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/time.h>

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
    if (argc != 4) perror("Não!\n");
    unlink("pipe");

    int log_fd = open("log.txt", O_WRONLY | O_CREAT , 0640);
    if(log_fd==-1){
        perror("Erro ao abrir o log.");
    }

    perror("não abortei :D");

    int pipe_fd;
    if (mkfifo(FIFO_FILE, 0666) == -1) { // Cria o pipe nomeado
        perror("Erro ao criar FIFO.");
        _exit(EXIT_FAILURE);
    }
    
    perror("não abortei :D");

    pipe_fd = open(FIFO_FILE, O_RDONLY); // Abre o pipe nomeado em modo leitura
    if (pipe_fd == -1) {
        perror("Erro ao abrir.");
        _exit(EXIT_FAILURE);
    }

    perror("não abortei :D");

    char* comando[MAX_COMMAND];
    int bytes_read;

    while((bytes_read = read(pipe_fd, &comando, MAX_COMMAND)) > 0){
        //int bytes_written = write(log_fd, &comando, bytes_read);
        perror("ABRI E LI");
    } // Lê o comando

    perror("não abortei depois de ler");

    pid_t pid = fork();
        if (pid == 0) {
            // Faz execvp em processo filho
            exec_command(comando); //execl???????
            _exit(-1);
        }
        else {
            // Retorna no processo pai o identificador único como em mysystem() DEPENDE DO ESCALONAMENTO

        }

    //colocar a começar a contar o tempo de execução da tarefa

    /*
    for(int i = 0; buf[i] != NULL; i++) { //isto era debugging, pode sair depois
        printf("%s ", buf[i]);
    }
    */
    write(1, comando, MAX_COMMAND);
    perror("não abortei aqui");
    // Escreve no log o estado de execução + o tempo demorado

    // ESCALONAMENTO !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!

    close(pipe_fd);
    perror("abortei aqui? AH NÃO!");
    unlink(FIFO_FILE);
    perror("e aqui? tambem não");
    close(log_fd);
    perror("nao nao nao!");

    return 0;
}