#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>
#include <string.h>
#include <sys/wait.h>

void handle_sigusr1(int sig)
{
    printf("Received signal SIGUSR1: %d\n", sig);
}

void test_ignore()
{
    signal(SIGUSR1, SIG_IGN);
    printf("Setting ignore to SIGUSR1\n");
    printf("Raising signal SIGUSR1 in parent process\n");
    raise(SIGUSR1);
    if (fork() == 0)
    {
        printf("Raising signal SIGUSR1 in child process\n");
        raise(SIGUSR1);
        exit(0);
    }
}

void test_handler()
{
    struct sigaction sa;
    sa.sa_handler = handle_sigusr1;
    sigaction(SIGUSR1, &sa, NULL);
    printf("Handler installed in parent process, raising signal\n");
    raise(SIGUSR1);
    printf("Fork\n");
    if (fork() == 0)
    {
        printf("Raising signal in child process\n");
        raise(SIGUSR1);
        exit(0);
    }
}

void test_pending()
{
    sigset_t mask, parent_pending, child_pending;
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

    if (fork() == 0)
    {
        sigpending(&child_pending);

        if (sigismember(&child_pending, SIGUSR1))
        {
            printf("SIGUSR1 is pending in child process\n");
        }
        else
        {
            printf("SIGUSR1 is not pending in child process\n");
        }
        exit(0);
    }
}

void test_mask()
{
    sigset_t mask, parent_pending, child_pending;
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

    pid_t pid = fork();
    if (pid == 0)
    {
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
        exit(0);
    }
    else
    {
        waitpid(pid, NULL, 0);
    }
}

int main(int argc, char *argv[])
{
    if (argc != 2)
    {
        printf("Wrong number of args!\n");
        return 1;
    }
    char *option = argv[1];
    if (strcmp(option, "ignore") == 0)
    {
        test_ignore();
    }
    else if (strcmp(option, "handler") == 0)
    {
        test_handler();
    }
    else if (strcmp(option, "mask") == 0)
    {
        test_mask();
    }
    else if (strcmp(option, "pending") == 0)
    {
        test_pending();
    }
    return 0;
}
