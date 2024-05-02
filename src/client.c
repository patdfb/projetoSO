#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h> // legal???????

#define FIFO_FILE "pipe"
#define MAX_COMMAND 300



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

int execute_uni(char* argumentos[]) {

    int pipe_fd; 

    pipe_fd = open(FIFO_FILE, O_WRONLY); // Abre o pipe nomeado em modo escrita
    if (pipe_fd == -1) {
        perror("Erro ao abrir.");
        _exit(EXIT_FAILURE);
    }

    ssize_t w = write(pipe_fd, &argumentos, MAX_COMMAND); // assim????? + perror

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

    if (strcmp(argv[1], "execute") == 0 && strcmp(argv[3], "-u") == 0) { // execute time -u "prog-a [args]"
        int uni = execute_uni(argv[4]); 
        if (uni == -1) {
            perror("Erro.");
            _exit(EXIT_FAILURE);
        }
    }
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
    else if (strcmp(argv[1], "status") == 0 ) {
        int fifo_fd = open("fifo", O_WRONLY);
        if (fifo_fd == -1){
            perror("Erro.");
            _exit(EXIT_FAILURE); //será que o problema não é por ter aqui este _exit????
        }

        int bytes_written = write(fifo_fd, argv[1], strlen(argv[1])); //MAX_COMMAND ou strlen(argv[1]) ??? porque ao mandar só o argv[1] sabemso o tamanho dele
        close(fifo_fd);

        int status_fd = open("fifostatus", O_RDONLY); //criar fifo só para o status? HUGO PORQUÊ?
        if(status_fd == -1){
            perror("Erro.");
            _exit(EXIT_FAILURE);
        }

        int bytes_read;
        while(bytes_read = read(status_fd, buff, MAX_COMMAND) > 0){
            write(1, buff, bytes_read);
        }

        close(status_fd);
        // vai buscar ao log (o log abre no servidor mas ainda nada é colocado nele, apenas está criado)
    }

    return 0;
}
