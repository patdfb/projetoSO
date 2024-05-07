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


void exec_command(char* arg,int ID, char* output_folder){
    char LOG[30];
    memset(LOG,0,sizeof(LOG));
    strcpy(LOG,output_folder);
    strcat(LOG,"/log.txt");
	char *exec_args[MAX_COMMAND];
    int fd;
	char *string;	
	int i=0;
    int outfd = dup(STDOUT_FILENO);
    char out[30];
    memset(out,'\0',sizeof(out));
    strcpy(out,output_folder);
    strcat(out,"/"); 
    char idstring[10];
    sprintf(idstring, "%d",ID);
    strcat(out,idstring);
    strcat(out,".txt");
    fd = open(out, O_CREAT | O_WRONLY | O_APPEND, 0660);
    dup2(fd,STDOUT_FILENO);
	char* command = strdup(arg);

	string=strtok(command," ");

    char pontoBarra[30];
    strcpy(pontoBarra,"include/");
    strcat(pontoBarra, string);

	while(string!=NULL){
        exec_args[i]=string;
		string=strtok(NULL," ");
		i++;
	}
 
    exec_args[0] = pontoBarra;

	exec_args[i]=NULL;
    pid_t forked = fork();
    if (forked == 0) {

	    execv(exec_args[0],exec_args);

    }
    dup2(outfd,fd);
}

void exec_command_multi(char* arg, int ID, char* output_folder){
    
	char *exec_args[MAX_COMMAND];
	char *string;	
	int i=0;

	char* command = strdup(arg);

	string=strtok(command,"|");


	while(string!=NULL){
        exec_args[i]=string;
		string=strtok(NULL,"|");
		i++;
	}

    for (int j=0;j<i;j++) {
        exec_command(exec_args[j],ID,output_folder);
        wait(NULL);
    }

}    

void status(pid_t pid,char* output_folder) {
    char LOG[30];
    memset(LOG,0,sizeof(LOG));
    strcpy(LOG,output_folder);
    strcat(LOG,"/log.txt");
    int log_fd = open(LOG, O_RDONLY);
    struct Tarefa completed[MAX_COMMAND];
    struct Tarefa executing[MAX_COMMAND];
    struct Tarefa scheduled[MAX_COMMAND];
    struct Tarefa t;
    char clientID[20];
    for (int i=0;i<20;i++) {
        clientID[i] = '\0';
    }
    sprintf(clientID, "%d", pid);
    char listas[MAX_COMMAND*MAX_COMMAND];
    for (int i=0;i<MAX_COMMAND*MAX_COMMAND;i++) {
        listas[i] = '\0';
    }
    int c0 = 0, c1 = 0, c2 = 0;
    int bytes_read;
    while((bytes_read = read(log_fd, &t, sizeof(struct Tarefa))) > 0){
        int estado = t.estado;

        if(estado == 2){
            completed[c2].estado = t.estado;
            completed[c2].ID = t.ID;
            strcpy(completed[c2].argumento, t.argumento);
            completed[c2].tempoEstimado = t.tempoEstimado;
            completed[c2].tempoReal = t.tempoReal;
            completed[c2].pid = t.pid;
            completed[c2].multi = t.multi;
            c2++;
        } else if(estado == 1){
            executing[c1].estado = t.estado;
            executing[c1].ID = t.ID;
            strcpy(executing[c1].argumento, t.argumento);
            executing[c1].tempoEstimado = t.tempoEstimado;
            executing[c1].tempoReal = t.tempoReal;
            executing[c1].pid = t.pid;
            executing[c1].multi = t.multi;
            c1++;
        } else if(estado == 0){
            scheduled[c0].estado = t.estado;
            scheduled[c0].ID = t.ID;
            strcpy(scheduled[c0].argumento, t.argumento);
            scheduled[c0].tempoEstimado = t.tempoEstimado;
            scheduled[c0].tempoReal = t.tempoReal;
            scheduled[c0].pid = t.pid;
            scheduled[c0].multi = t.multi;
            c0++;
        }
    }
    close(log_fd);

    char tudo[MAX_COMMAND];
    int i;
    for (i=0;i<MAX_COMMAND;i++) {
        tudo[i] = '\0';
    }

    int pipe_fd2 = open(clientID,O_WRONLY); 
    if (pipe_fd2 == -1) {
        _exit(EXIT_FAILURE);
    }

    strcat(listas,"Executing\n");
    for(i=0; i<c1; i++){
        t = executing[i];
        sprintf(tudo, "%d", t.ID); 
        strcat(tudo, " ");
        if (t.multi == 0) {
            char *comando;
            comando = strtok(t.argumento, " ");
            strcat(tudo, comando);
        } else if (t.multi == 1) {
            char *comandoarray[MAX_COMMAND];
            char *string;	
	        int i=0;

	        char* command = strdup(t.argumento);

	        string=strtok(command,"|");

            while(string!=NULL){
                comandoarray[i]=string;
                string=strtok(NULL,"|");
                i++;
            }

            char *comandofinal;
            for (int j=0;j<i;j++) {
                comandofinal = strtok(comandoarray[j], " ");
                strcat(tudo,comandofinal);
                if (j != i-1) strcat(tudo," | ");
            }
        }
        strcat(tudo, "\n");
        strcat(listas,tudo);
    }

    strcat(listas,"\nScheduled\n");
    for(i=0; i<c0; i++){
        t = scheduled[i];
        sprintf(tudo, "%d", t.ID); 
        strcat(tudo, " ");
        if (t.multi == 0) {
            char *comando;
            comando = strtok(t.argumento, " ");
            strcat(tudo, comando);
        } else if (t.multi == 1) {
            char *comandoarray[MAX_COMMAND];
            char *string;	
	        int i=0;

	        char* command = strdup(t.argumento);

	        string=strtok(command,"|");

            while(string!=NULL){
                comandoarray[i]=string;
                string=strtok(NULL,"|");
                i++;
            }

            char *comandofinal;
            for (int j=0;j<i;j++) {
                comandofinal = strtok(comandoarray[j], " ");
                strcat(tudo,comandofinal);
                if (j != i-1) strcat(tudo," | ");
            }
        }
        strcat(tudo, "\n");
        strcat(listas,tudo);
    }

    strcat(listas,"\nCompleted\n");
    for(i=0; i<c2; i++){
        t = completed[i];
        sprintf(tudo, "%d", t.ID); 
        strcat(tudo, " ");
        if (t.multi == 0) {
            char *comando;
            comando = strtok(t.argumento, " ");
            strcat(tudo, comando);
        } else if (t.multi == 1) {
            char *comandoarray[MAX_COMMAND];
            char *string;	
	        int i=0;

	        char* command = strdup(t.argumento);

	        string=strtok(command,"|");

            while(string!=NULL){
                comandoarray[i]=string;
                string=strtok(NULL,"|");
                i++;
            }

            char *comandofinal;
            for (int j=0;j<i;j++) {
                comandofinal = strtok(comandoarray[j], " ");
                strcat(tudo,comandofinal);
                if (j != i-1) strcat(tudo," | ");
            }
        }
        char Tempo[30];
        sprintf(Tempo, "%d", t.tempoReal);
        strcat(tudo, " ");
        strcat(tudo, Tempo);
        strcat(tudo, " ms\n");
        strcat(listas,tudo);
    }
    write(pipe_fd2, listas, sizeof(listas));
    close(pipe_fd2);
    unlink(clientID);

    _exit(0);
}


int main(int argc, char* argv[]) {
    if (argc != 4) {
        perror("Argumentos insuficientes");
        _exit(EXIT_FAILURE);
    }
    int politica = atoi(argv[3]);//1=FCFS 2=STF
    int max = atoi(argv[2]);
    char output_folder[30];
    strcpy(output_folder,argv[1]);
    if (mkdir(output_folder,S_IRWXU | S_IRWXG | S_IRWXO)==-1) {//READ| WRITE |EXECUTE
        perror("Erro a criar pasta");
        _exit(EXIT_FAILURE);
    }

    char LOG[30];
    memset(LOG,0,sizeof(LOG));
    strcpy(LOG,output_folder);
    strcat(LOG,"/log.txt");

    char Completed[50];
    strcpy(Completed,output_folder);
    strcat(Completed,"/completed.txt");

    unlink(FIFO_FILE);

    int ligado = 1;


    int log_fd = open(LOG, O_CREAT , 0640);
    if(log_fd==-1){
        perror("Erro ao criar o log.");
    }

    close(log_fd);

    int cmp_fd = open(Completed, O_CREAT , 0640);
    if(cmp_fd==-1){
        perror("Erro ao criar o cmp.");
    }

    close(cmp_fd);

    pid_t fork1 = fork();
    if(fork1==0){
        int pipe_fd;
        struct Tarefa t;

        if (mkfifo(FIFO_FILE, 0666) == -1) { 
            perror("Erro ao criar FIFO.");
            _exit(EXIT_FAILURE);
        }

        int pos = 1;

        while(ligado){
            pipe_fd = open(FIFO_FILE, O_RDONLY); 
            if (pipe_fd == -1) {
                perror("Erro ao abrir fifofile.");
                _exit(EXIT_FAILURE);
            }

            read(pipe_fd, &t, sizeof(struct Tarefa));
            if (strcmp(t.argumento,"status") == 0) {
                int forkou = fork();
                if (forkou == 0) {
                   status(t.pid,output_folder);
                }
            } else if (strcmp(t.argumento,"shutdown") == 0) {
                ligado = 0;
                log_fd = open(LOG, O_WRONLY | O_APPEND);
                t.ID = pos;
                t.estado = 0;
                t.tempoEstimado = -1;
                write(log_fd, &t, sizeof(struct Tarefa));
                close(log_fd);
            } else {
                log_fd = open(LOG, O_WRONLY | O_APPEND);
                t.ID = pos;
                t.estado = 0;
                write(log_fd, &t, sizeof(struct Tarefa));
                close(log_fd);

                char taskID[10];
                char clientID[20];
                for (int i=0;i<20;i++) {
                    clientID[i] = '\0';
                }
                char printable[50] = "TASK ";

                for (int i=0;i<10;i++) {
                    taskID[i] = '\0';
                }
                sprintf(clientID, "%d", t.pid);
                sprintf(taskID, "%d", pos);
                strcat(printable,taskID);
                strcat(printable," Received\n");

                int pipe_fd2 = open(clientID, O_WRONLY); 

                write(pipe_fd2, printable, sizeof(printable));

                close(pipe_fd2);
                unlink(clientID);
                pos++;
            }
        }
        
        close(pipe_fd);
        unlink(FIFO_FILE);
        
    } else{
        int c=0;
        int ligado = 1;
        char *contador = "tmp/contador.txt";
        int cont_fd;
        cont_fd = open(contador,O_CREAT | O_WRONLY,0640);
        int zerou = 0;
        for (c=0;c<max;c++) {
            write(cont_fd,&zerou,sizeof(zerou));
        }
        close(cont_fd);
        while(ligado){
            struct Tarefa t;
            struct Tarefa tmenor;
            int menortempo = 30000;
            int bytes_read,conts_read,lido,contando=0;

            cont_fd = open(contador,O_RDONLY);
            while((conts_read = read(cont_fd,&lido,sizeof(lido)))>0) {
                if (lido != 0) {
                    contando++;
                }
            }
            close(cont_fd);
            if (contando == max) {
                wait(NULL);
            }

            log_fd = open(LOG, O_RDWR);
            if (politica == 1) {
                while((bytes_read = read(log_fd, &t, sizeof(struct Tarefa)))>0 && t.estado!=0);
            } else if (politica == 2) {
                while((bytes_read = read(log_fd, &t, sizeof(struct Tarefa)))>0) {
                    if (t.estado == 0 && t.tempoEstimado < menortempo) {
                        menortempo = t.tempoEstimado;
                        tmenor.estado = t.estado;
                        tmenor.ID = t.ID;
                        strcpy(tmenor.argumento,t.argumento);
                        tmenor.tempoEstimado = t.tempoEstimado;
                        tmenor.tempoReal = t.tempoReal;
                        tmenor.multi = t.multi;
                        tmenor.pid = t.pid;
                    }
                }
                lseek(log_fd,0,SEEK_SET);
                while((bytes_read = read(log_fd, &t, sizeof(struct Tarefa)))>0 && t.ID!=tmenor.ID);
            }
            if (strcmp(t.argumento,"shutdown")==0) {
                ligado = 0;
                t.estado = 2;
            }
            if(bytes_read != 0 && t.estado == 0){
                cont_fd = open(contador,O_RDWR);
                while((conts_read = read(cont_fd,&lido,sizeof(lido)))>0 && lido != 0);
                if (lido == 0) {
                    lseek(cont_fd,-sizeof(lido),SEEK_CUR);
                    write(cont_fd,&t.ID,sizeof(lido));
                }
                close(cont_fd);
                t.estado = 1;
                lseek(log_fd, -sizeof(struct Tarefa), SEEK_CUR);
                write(log_fd, &t, sizeof(struct Tarefa));
                close(log_fd);
                pid_t fork2 = fork();
                if(fork2==0){
                    struct timeval start,end;
                    double elapsedTime;
                    gettimeofday(&start,NULL);
                    int tID = t.ID;
                    if (t.multi == 0) {
                        exec_command(t.argumento,t.ID,output_folder);
                    } else if (t.multi == 1) {
                        exec_command_multi(t.argumento,t.ID,output_folder);
                    }
                    int status;
                    wait(&status);
                    if(WIFEXITED(status)){
                        cont_fd = open(contador,O_RDWR);
                        while((conts_read = read(cont_fd,&lido,sizeof(lido)))>0 && lido != tID);
                        int zerou2 = 0;
                        if (lido == tID) {
                            lseek(cont_fd,-sizeof(lido),SEEK_CUR);
                            write(cont_fd,&zerou2,sizeof(zerou2));
                        }
                        close(cont_fd);
                        struct Tarefa t2;
                        int log_fd2 = open(LOG, O_RDWR);
                        while((bytes_read = read(log_fd2, &t2, sizeof(struct Tarefa)))>0 && t2.ID!=tID);
                        gettimeofday(&end,NULL);
                        elapsedTime = (end.tv_sec - start.tv_sec) * 1000.0;
                        elapsedTime += (end.tv_usec - start.tv_usec) / 1000.0;
                        t2.tempoReal = (int)elapsedTime;
                        t2.estado = 2;
                        lseek(log_fd2, -sizeof(struct Tarefa), SEEK_CUR);
                        write(log_fd2, &t2, sizeof(struct Tarefa));
                        close(log_fd2);
                        int comp_fd;
                        comp_fd = open(Completed,O_WRONLY | O_APPEND);
                        char stringfinal[50];
                        sprintf(stringfinal,"%d",tID);
                        strcat(stringfinal, " ");
                        char tempostr[30];
                        sprintf(tempostr,"%d",(int)elapsedTime);
                        strcat(tempostr," ms\n");
                        strcat(stringfinal,tempostr);
                        write(comp_fd,stringfinal,sizeof(stringfinal));
                        close(comp_fd);
                    }
                }
            
            } else {close(log_fd);}
                    
        }

    }
    
    return 0;
}