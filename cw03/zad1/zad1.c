#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>

int main(int argc, char *argv[]) {
 

    int num_child_processes = atoi(argv[1]);

    for (int i = 0; i < num_child_processes; i++) {
        pid_t pid = fork();
        if (pid == 0) {
            printf("Parent process ID: %d, Child process ID: %d\n", getppid(), getpid());
            return 0;
        }
        else{
            waitpid(pid,NULL,0);
        }
    }

    printf("%s\n", argv[1]);

    return 0;
}
