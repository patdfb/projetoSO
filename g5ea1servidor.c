#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/stat.h>
#include <sys/types.h>

#define FIFO_FILE "pipe_servidor"
#define MAX_PHRASE 50

int main() {
    int pipe_fd;
    int data_fd;
    int num_to_search;
    int count = 0;
    int number;

    // Cria o pipe nomeado
    if (mkfifo(FIFO_FILE, 0666) == -1) {
        perror("mkfifo");
        _exit(EXIT_FAILURE);
    }

    printf("Servidor a correr...\n");

    // Abre o pipe nomeado em modo leitura
    pipe_fd = open(FIFO_FILE, O_RDONLY);
    if (pipe_fd == -1) {
        perror("open");
        _exit(EXIT_FAILURE);
    }

    // Lê o número a ser procurado do pipe
    read(pipe_fd, &num_to_search, sizeof(num_to_search));
    printf("Servidor recebeu o número: %d\n", num_to_search);

    //Abre log
    int log_fd = open("log.txt", O_CREAT | O_WRONLY, 0666);
    if (log_fd == -1) {
        perror("open");
        _exit(EXIT_FAILURE);
    }

    // Abre o arquivo de dados
    data_fd = open("data.txt", O_RDONLY);
    if (data_fd == -1) {
        perror("open");
        _exit(EXIT_FAILURE);
    }

    // Procura o número no arquivo de dados
    ssize_t n_read;
    char log_phrase[MAX_PHRASE];
    while (n_read = read(data_fd, &number, sizeof(number)) > 0) {
        if (number == num_to_search) {
            count++;
        }
        snprintf(log_phrase, sizeof(log_phrase), "%d apareceu %d vezes\n", number, count);
        write(log_fd, log_phrase, strlen(log_phrase));
    }

    // Fecha o arquivo de dados , log e pipe
    close(data_fd);
    close(log_fd);
    close(pipe_fd);
    unlink(FIFO_FILE); // Remove o pipe nomeado

    // Envia o número de ocorrências para o cliente
    pipe_fd = open(FIFO_FILE, O_WRONLY);
    if (pipe_fd == -1) {
        perror("open");
        _exit(EXIT_FAILURE);
    }
    write(pipe_fd, &count, sizeof(count));

    // Fecha o pipe nomeado
    close(pipe_fd);

    return 0;
}

// Como fazer com que os clientes não fiquem bloqueados?