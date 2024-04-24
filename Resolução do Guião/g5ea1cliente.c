#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>

#define FIFO_FILE "pipe_servidor"

int main() {
    int pipe_fd;
    int num_to_search;
    int count;

    // Abre o pipe nomeado em modo escrita
    pipe_fd = open(FIFO_FILE, O_WRONLY);
    if (pipe_fd == -1) {
        perror("open");
        exit(EXIT_FAILURE);
    }

    printf("Digite o número a procurar: ");
    scanf("%d", &num_to_search);

    // Escreve o número no pipe
    write(pipe_fd, &num_to_search, sizeof(num_to_search));

    // Fecha o pipe
    close(pipe_fd);

    // Abre o pipe nomeado em modo leitura para receber a resposta do servidor
    pipe_fd = open(FIFO_FILE, O_RDONLY);
    if (pipe_fd == -1) {
        perror("open");
        exit(EXIT_FAILURE);
    }

    // Lê o número de ocorrências do pipe
    read(pipe_fd, &count, sizeof(count));
    printf("Número de ocorrências encontradas: %d\n", count);

    // Fecha o pipe
    close(pipe_fd);

    return 0;
}
