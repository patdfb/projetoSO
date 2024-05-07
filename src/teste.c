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


int main() {
    int max = 3;
    int c;
    char *contador = "../tmp/contador.txt";
    int cont_fd;
    cont_fd = open(contador,O_CREAT | O_WRONLY,0640);
    int zerou = 0;
    for (c=0;c<max;c++) {
        write(cont_fd,&zerou,sizeof(zerou));
    }
    int lido;
    lseek(cont_fd,0,SEEK_SET);
    for (c=0;c<max;c++) {
        read(cont_fd,&lido,sizeof(lido));
        printf("digito é %d\n",lido);
    }
}