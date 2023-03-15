#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>

int main(int argc, char *argv[]) {        
    setbuf(stdout, NULL);

    printf("Nazwa programu: %s ", argv[0]);

  
    execl("/bin/ls","ls","-l",argv[1],NULL);
    printf("Błąd uruchamiania /bin/ls ls -l !\n");


    return 0;
}