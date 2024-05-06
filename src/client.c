#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>
#include "struct.h"

#define FIFO_FILE "pipe"
#define MAX_COMMAND 300
#define LOG "tmp/log.txt"



/*
int execute_multi(char* argumentos[], int n){

	int i;
    int p[n-1][2];

    for(i=0; i<n; i++){

        if(i==0){
            pipe(p[0]);
            //no processo filho
            if(fork()==0){

                close(p[i][0]);
                dup2(p[i][1], 1);
                close(p[i][1]);
                int res = exec_command(argumentos[i]);
                _exit(res);

            }
            //no processo pai, ainda tem o descritor de leitura e escrita abertos, o unico que vai escrever é o primeiro processo, que já foi tratado anteriormente
            else{
                close(p[i][1]);
            }
        }

        else if(i==n-1){ //aqui já não é necessário criar um pipe

            if(fork()==0){
                dup2(p[i-1][0],0);
                close(p[i-1][0]);
                int res = exec_command(argumentos[i]);
				_exit(res);
            } 
            else{
                close(p[i-1][0]);
            }

        }

        else{ //i>0 & i<N-1
            pipe(p[i]);

            if(fork()==0){
                close(p[i][0]);
                dup2(p[i-1][0],0); //stdin, do pipe anterior
                close(p[i-1][0]);
                dup2(p[i][1],1); //stdout, do pipe atual
                close(p[i][1]);
                int res = exec_command(argumentos[i]);
				_exit(res);
            }
            else{
                close(p[i-1][0]);
                //o processo de leitura do pipe atual nao pode ser fechado para o processo seguinte poder usa lo
                close(p[i][1]); //mas podemos fechar o de escrita pois mais nenhum vai escrever para ele 
            }
        }
    }

	for(i=0; i<n; i++){
		wait(NULL);
	}

	return 0;
}

*/

int execute_uni(struct Tarefa t) {

    int pipe_fd; 

    pipe_fd = open(FIFO_FILE, O_WRONLY); // Abre o pipe nomeado em modo escrita
    if (pipe_fd == -1) {
        perror("Erro ao abrir.");
        _exit(EXIT_FAILURE);
    }

    ssize_t w = write(pipe_fd, &t, sizeof(struct Tarefa)); // assim????? + perror

    close(pipe_fd); // Fecha o pipe, o servidor depois abre o pipe, lê e executa

    pipe_fd = open(FIFO_FILE, O_RDONLY); // Abre o pipe nomeado em modo leitura para receber a resposta do servidor (ID?????)
    if (pipe_fd == -1) {
        perror("Erro ao abrir.");
        _exit(EXIT_FAILURE);
    }

    char buf[MAX_COMMAND];
    ssize_t r = read(pipe_fd, &buf, MAX_COMMAND); // assim?????? + perror

    close(pipe_fd);

    return 0;
}

int main(int argc, char* argv[]) {

    char buff[MAX_COMMAND];
    struct Tarefa t;

    if (strcmp(argv[1], "execute") == 0 && strcmp(argv[3], "-u") == 0) { // execute time -u "prog-a [args]"
        strcpy(t.argumento, argv[4]);
        t.tempo = atoi(argv[2]);
        t.ID = 0;
        t.estado = 0;
        t.pid = getpid();
        int uni = execute_uni(t); 
        if (uni == -1) {
            perror("Erro.");
            _exit(EXIT_FAILURE);
        }
    }/*
    else if (strcmp(argv[1], "execute") == 0 && strcmp(argv[3], "-p") == 0) {
        char* argumentos_dup = strdup(argv[4]);
        char* comando = strtok(argumentos_dup, "|");
        char* args[32];
        int i = 0;
        while(comando != NULL){
            args[i] = comando;
            comando = strtok(NULL,"|");
            i++;
        }

        int multi = execute_multi(args, i); 
        if (multi == -1) {
            perror("Erro.");
            _exit(EXIT_FAILURE);
        }
    }
    */
    else if (strcmp(argv[1], "status") == 0 ) {
        int log_fd = open(LOG, O_RDONLY);
        struct Tarefa completed[MAX_COMMAND];
        struct Tarefa executing[MAX_COMMAND];
        struct Tarefa scheduled[MAX_COMMAND];
        struct Tarefa t;
        
        int c0 = 0, c1 = 0, c2 = 0;
        int bytes_read;
        while((bytes_read = read(log_fd, &t, sizeof(struct Tarefa))) > 0){
            int estado = t.estado;

            if(estado == 2){
                completed[c2].estado = t.estado;
                completed[c2].ID = t.ID;
<<<<<<< HEAD
                completed[c2].tempoEstimado = t.tempoEstimado;
                completed[c2].tempoReal = t.tempoReal;
=======
                strcpy(completed[c2].argumento, t.argumento);
                completed[c2].tempo = t.tempo;
>>>>>>> parent of d0ed313 (tenho medo do computador crashar)
                completed[c2].pid = t.pid;
                strcpy(completed[c2].argumento, t.argumento);
                c2++;
            } else if(estado == 1){
                executing[c1].estado = t.estado;
                executing[c1].ID = t.ID;
<<<<<<< HEAD
                executing[c1].tempoEstimado = t.tempoEstimado;
                completed[c1].tempoReal = t.tempoReal;
                completed[c1].pid = t.pid;
                strcpy(executing[c1].argumento, t.argumento);
=======
                strcpy(executing[c1].argumento, t.argumento);
                executing[c1].tempo = t.tempo;
                executing[c1].pid = t.pid;
>>>>>>> parent of d0ed313 (tenho medo do computador crashar)
                c1++;
            } else if(estado == 0){
                scheduled[c0].estado = t.estado;
                scheduled[c0].ID = t.ID;
<<<<<<< HEAD
                scheduled[c0].tempoEstimado = t.tempoEstimado;
                completed[c0].tempoReal = t.tempoReal;
                completed[c0].pid = t.pid;
                strcpy(scheduled[c0].argumento, t.argumento);
=======
                strcpy(scheduled[c0].argumento, t.argumento);
                scheduled[c0].tempo = t.tempo;
                scheduled[c0].pid = t.pid;
>>>>>>> parent of d0ed313 (tenho medo do computador crashar)
                c0++;
            }
        }
        close(log_fd);
        char tamanho[11] = "Executing\n";
        char tudo[MAX_COMMAND];
        write(1, "Executing\n", sizeof(tamanho));
        for(int i=0; i<c1; i++){
            t = executing[i];
            sprintf(tudo, "%d", t.ID); //transforma int em string
            strcat(tudo, " ");
            char *comando;
            comando = strtok(t.argumento, " ");
            strcat(tudo, comando);
            strcat(tudo, "\n");
            write(1, tudo, sizeof(tudo));
        }

        write(1, "\nScheduled\n", sizeof(tamanho));
        for(int i=0; i<c0; i++){
            t = scheduled[i];
            sprintf(tudo, "%d", t.ID); //transforma int em string
            strcat(tudo, " ");
            char *comando;
            comando = strtok(t.argumento, " ");
            strcat(tudo, comando);
            strcat(tudo, "\n");
            write(1, tudo, sizeof(tudo));
        }

        write(1, "\nCompleted\n", sizeof(tamanho));
        for(int i=0; i<c2; i++){
            t = completed[i];
            sprintf(tudo, "%d", t.ID); //transforma int em string
            strcat(tudo, " ");
            char *comando;
            comando = strtok(t.argumento, " ");
            strcat(tudo, comando);
            char Tempo[30];
            sprintf(Tempo, "%d", t.tempo);
            strcat(tudo, " ");
            strcat(tudo, Tempo);
            strcat(tudo, "\n");
            write(1, tudo, sizeof(tudo));
        }

    }

    return 0;
}
