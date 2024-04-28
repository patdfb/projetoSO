#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h> // legal???????

#define FIFO_FILE "pipe_servidor"
#define MAX_COMMAND 300

int execute_uni(char* argumentos[]) {
    char* comando_dup = strdup(argumentos);
    char* comando = strtok(comando_dup, " ");
    char* args[32];
    int i = 0;
    while(comando != NULL){
        args[i] = comando;
        comando = strtok(NULL," ");
        i++;
        }
    arg[i] == NULL;

    int pipe_fd;
    pipe_fd = open(FIFO_FILE, O_WRONLY); // Abre o pipe nomeado em modo escrita
    if (pipe_fd == -1) {
        perror("Erro ao abrir.");
        _exit(EXIT_FAILURE);
    }

    char buf[MAX_COMMAND];
    ssize_t w = write(pipe_fd, buf, MAX_COMMAND); // assim????? + perror

    close(pipe_fd); // Fecha o pipe, o servidor depois abre o pipe, lê e executa

    pipe_fd = open(FIFO_FILE, O_RDONLY); // Abre o pipe nomeado em modo leitura para receber a resposta do servidor (ID?????)
    if (pipe_fd == -1) {
        perror("Erro ao abrir.");
        _exit(EXIT_FAILURE);
    }

    ssize_t r = read(pipe_fd, buf, MAX_COMMAND); // assim?????? + perror

    close(pipe_fd);

    return 0;
}

int execute_multi(char* argumentos[]) {

    return 0;
}

int main(int argc, char* argv[]) {
    if (strcmp(argv[1], "execute") == 0 && strcmp(argv[3], "-u")) { // execute time -u "prog-a [args]"
        // execute_uni
    }
    else if (strcmp(argv[1], "execute") == 0 && strcmp(argv[3], "-p")) {
        // execute_multi
    }
    else if (strcmp(argv[1], "status") == 0 ) {
        // vai buscar ao log (temos de fazer o log!!!!!!!!!!!!) que está no servidor
    }

    return 0;
}