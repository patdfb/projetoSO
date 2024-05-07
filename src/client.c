#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/time.h>
#include <sys/wait.h>
#include "struct.h"
#include <time.h>

#define FIFO_FILE "pipe"
#define MAX_COMMAND 300
#define LOG "tmp/log.txt"


int execute_uni(struct Tarefa t) {

    int pipe_fd,pipe_fd2;
    char clientID[20];
    for (int i=0;i<20;i++) {
        clientID[i] = '\0';
    }
    sprintf(clientID, "%d", t.pid);

    if (mkfifo(clientID, 0666) == -1) { 
        perror("Erro ao criar FIFO.");
        _exit(EXIT_FAILURE);
    }

    pipe_fd = open(FIFO_FILE, O_WRONLY); // Abre o pipe nomeado em modo escrita
    if (pipe_fd == -1) {
        perror("Erro ao abrir.");
        _exit(EXIT_FAILURE);
    }

    write(pipe_fd, &t, sizeof(struct Tarefa)); // assim????? + perror

    close(pipe_fd); // Fecha o pipe, o servidor depois abre o pipe, lê e executa

    pipe_fd2 = open(clientID,O_RDONLY); // Abre o pipe nomeado em modo leitura para receber a resposta do servidor (ID?????)
    if (pipe_fd2 == -1) {
        perror("Erro ao abrir.");
        _exit(EXIT_FAILURE);
    }

    char buf[50];
    read(pipe_fd2, &buf, sizeof(buf)); // assim?????? + perror

    write(1,&buf,sizeof(buf));

    close(pipe_fd2);
    unlink(clientID);

    return 0;
}

int main(int argc, char* argv[]) {

    struct Tarefa t;

    if (strcmp(argv[1], "execute") == 0 && strcmp(argv[3], "-u") == 0) { // execute time -u "prog-a [args]"
        strcpy(t.argumento, argv[4]);
        t.tempoEstimado = atoi(argv[2]);
        t.tempoReal = 0;
        t.ID = 0;
        t.estado = 0;
        t.multi = 0;
        t.pid = getpid();
        int uni = execute_uni(t); 
        if (uni == -1) {
            perror("Erro.");
            _exit(EXIT_FAILURE);
        }
    }
    else if (strcmp(argv[1], "execute") == 0 && strcmp(argv[3], "-p") == 0) {
        strcpy(t.argumento, argv[4]);
        t.tempoEstimado = atoi(argv[2]);
        t.tempoReal = 0;
        t.ID = 0;
        t.estado = 0;
        t.multi = 1;
        t.pid = getpid();
        int uni = execute_uni(t); 
        if (uni == -1) {
            perror("Erro.");
            _exit(EXIT_FAILURE);
        }
    }
    else if (strcmp(argv[1], "status") == 0 ) {
        char clientID[20];
        int pipe_fd,pipe_fd2;
        struct Tarefa t;

        strcpy(t.argumento,"status");
        t.pid = getpid();

        for (int i=0;i<20;i++) {
            clientID[i] = '\0';
        }
        sprintf(clientID, "%d", t.pid);
        if (mkfifo(clientID, 0666) == -1) { 
            perror("Erro ao criar FIFO.");
            //_exit(EXIT_FAILURE);
        }

        pipe_fd = open(FIFO_FILE, O_WRONLY); // Abre o pipe nomeado em modo escrita
        if (pipe_fd == -1) {
            perror("Erro ao abrir fifo11.");
            _exit(EXIT_FAILURE);
        }

        write(pipe_fd, &t, sizeof(struct Tarefa)); // assim????? + perror

        close(pipe_fd); // Fecha o pipe, o servidor depois abre o pipe, lê e executa

        pipe_fd2 = open(clientID,O_RDONLY); // Abre o pipe nomeado em modo leitura para receber a resposta do servidor (ID?????)
        if (pipe_fd2 == -1) {
            perror("Erro ao abrir fifo2.");
            _exit(EXIT_FAILURE);
        }

        char linha[MAX_COMMAND];
        for (int i=0;i<MAX_COMMAND;i++) {
            linha[i] = '\0';
        }

        read(pipe_fd2, linha, sizeof(linha));
        write(1,linha,sizeof(linha));

        
        close(pipe_fd2);
        unlink(clientID);
    } else if (strcmp(argv[1], "shutdown") == 0 ) {
        struct Tarefa t;

        strcpy(t.argumento,"shutdown");

        int pipe_fd = open(FIFO_FILE, O_WRONLY); // Abre o pipe nomeado em modo escrita
        if (pipe_fd == -1) {
            perror("Erro ao abrir fifo11.");
            _exit(EXIT_FAILURE);
        }

        write(pipe_fd, &t, sizeof(struct Tarefa)); // assim????? + perror

        close(pipe_fd); // Fecha o pipe, o servidor depois abre o pipe, lê e executa

    }
    return 0;
}