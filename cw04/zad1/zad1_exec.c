#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>
#include <string.h>
#include <sys/wait.h>

void childExec(char *path, char *cmd)
{
    char *args[] = {path, cmd, "child", NULL};
    printf("Exec child\n");
    execvp(path, args);
}

int main(int argc, char *argv[])
{
    if (argc < 2)
    {
        printf("Wrong number of args!\n");
        return 1;
    }
    if (argc == 2)
    {
        char *option = argv[1];
        if (strcmp(option, "ignore") == 0)
        {
            signal(SIGUSR1, SIG_IGN);
            printf("Setting ignore to SIGUSR1\n");
            printf("Raising signal SIGUSR1 in parent process\n");
            raise(SIGUSR1);
            childExec(argv[0], argv[1]);
        }
        else if (strcmp(option, "mask") == 0)
        {
            sigset_t mask, parent_pending;
            sigemptyset(&mask);
            sigaddset(&mask, SIGUSR1);
            printf("Setting mask SIG_BLOCK in parent process for SIGUSR1\n");
            sigprocmask(SIG_BLOCK, &mask, NULL);
            printf("Raising signal SIGUSR1 in parent process\n");
            raise(SIGUSR1);

            sigpending(&parent_pending);

            if (sigismember(&parent_pending, SIGUSR1))
            {
                printf("SIGUSR1 is pending in parent process\n");
            }
            else
            {
                printf("SIGUSR1 is not pending in parent process\n");
            }
            childExec(argv[0], argv[1]);
        }
        else if (strcmp(option, "pending") == 0)
        {
            sigset_t mask, parent_pending;
            sigemptyset(&mask);
            sigaddset(&mask, SIGUSR1);
            printf("Setting mask SIG_BLOCK in parent process for SIGUSR1\n");
            sigprocmask(SIG_BLOCK, &mask, NULL);
            printf("Raising signal SIGUSR1 in parent process\n");
            raise(SIGUSR1);

            sigpending(&parent_pending);

            if (sigismember(&parent_pending, SIGUSR1))
            {
                printf("SIGUSR1 is pending in parent process\n");
            }
            else
            {
                printf("SIGUSR1 is not pending in parent process\n");
            }
            childExec(argv[0], argv[1]);

        }
        return 0;
    }
    else if (argc == 3 && strcmp("child", argv[2]) == 0)
    {
        char *option = argv[1];
        if (strcmp(option, "ignore") == 0)
        {
            printf("Raising signal SIGUSR1 in child process\n");
            raise(SIGUSR1);
        }
        else if (strcmp(option, "mask") == 0)
        {
            sigset_t child_pending;
            printf("Raising signal SIGUSR1 in child process\n");
            raise(SIGUSR1);

            sigpending(&child_pending);

            if (sigismember(&child_pending, SIGUSR1))
            {
                printf("SIGUSR1 is pending in child process\n");
            }
            else
            {
                printf("SIGUSR1 is not pending in child process\n");
            }
        }
        else if (strcmp(option, "pending") == 0)
        {   
            sigset_t child_pending;
            sigpending(&child_pending);

            if (sigismember(&child_pending, SIGUSR1))
            {
                printf("SIGUSR1 is pending in child process\n");
            }
            else
            {
                printf("SIGUSR1 is not pending in child process\n");
            }
        }
        return 0;
    }
}