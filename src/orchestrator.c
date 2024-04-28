#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/stat.h>
#include <sys/types.h>

#define FIFO_FILE "pipe_servidor"
#define MAX_COMMAND 300

int main() {
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

    // Lê o comando do pipe usando read

    // Faz exec em processo filho e retorna no processo pai o identificador único como em mysystem()

    // Escreve no log o estado de execução + o tempo demorado

    // ESCALONAMENTO !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!

    close(pipe_fd);

    return 0;
}