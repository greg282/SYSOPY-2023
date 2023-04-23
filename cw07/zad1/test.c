#include "com.h"
#include <wait.h>
int main(int argc, char *argv[]){
    int times = atoi(argv[1]);
    for(int i=0; i<times; i++){
        pid_t pid = fork();
        if(pid==0){
            execl("./client", "./client", NULL);
            exit(0);
        }
    }
    for(int i=0; i<times; i++){
        wait(NULL);
    }
}