#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/stat.h>
#include <sys/types.h>

#define FIFO_FILE "pipe"
#define MAX_COMMAND 300

int main(int argc, char* argv[]) {
    if (argc != 4) perror("Não!\n");

    int pipe_fd;
    if (mkfifo(FIFO_FILE, 0666) == -1) { // Cria o pipe nomeado
        perror("Erro ao criar FIFO.");
        _exit(EXIT_FAILURE);
    }
    
    pipe_fd = open(FIFO_FILE, O_RDONLY); // Abre o pipe nomeado em modo leitura
    if (pipe_fd == -1) {
        perror("Erro ao abrir.");
        _exit(EXIT_FAILURE);
    }

    char* comando[32];
    ssize_t rr = read(pipe_fd, &comando, MAX_COMMAND); // Lê o comando

    /*
    for(int i = 0; buf[i] != NULL; i++) { //isto era debugging, pode sair depois
        printf("%s ", buf[i]);
    }
    */
    
    pid_t pid = fork();
    if (pid == 0) {
        // Faz execvp em processo filho
        execl(comando, comando, NULL); //execl???????
        _exit(-1);
    }
    else {
        // Retorna no processo pai o identificador único como em mysystem() DEPENDE DO ESCALONAMENTO

    }

    // Escreve no log o estado de execução + o tempo demorado

    // ESCALONAMENTO !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!

    close(pipe_fd);
    unlink(FIFO_FILE);

    return 0;
}