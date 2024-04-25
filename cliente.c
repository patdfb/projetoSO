#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h> // legal???????

#define FIFO_FILE "pipe_servidor"

int rodar(char* programa[], char* argumentos[]) {
    int pipe_fd;
    pipe_fd = open(FIFO_FILE, O_WRONLY); // Abre o pipe nomeado em modo escrita
    if (pipe_fd == -1) {
        perror("Erro ao abrir.");
        _exit(EXIT_FAILURE);
    }

    // Escreve o comando no pipe usando write

    close(pipe_fd); // Fecha o pipe

    pipe_fd = open(FIFO_FILE, O_RDONLY); // Abre o pipe nomeado em modo leitura para receber a resposta do servidor (ID?????)
    if (pipe_fd == -1) {
        perror("Erro ao abrir.");
        _exit(EXIT_FAILURE);
    }

    // Lê a resposta do pipe usando read

    close(pipe_fd);

    return 0;
}

int main(int argc, char* argv[]) {
    if (strcmp(argv[0], "execute") && strcmp(argv[2], "-u")) {
        // rodar
    }
    else if (strcmp(argv[0], "execute") && strcmp(argv[2], "-p")) {
        // rodar
    }
    else if (strcmp(argv[0], "status")) {
        // outra coisa
    }

    return 0;
}