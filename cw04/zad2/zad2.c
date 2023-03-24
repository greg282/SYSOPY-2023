#include <stdio.h>
#include <unistd.h>
#include <signal.h>
#include <stdlib.h>
#include <sys/wait.h>

int temp;

void handler(int signum, siginfo_t *info, void *context)
{
    printf("-------------------\n");
    printf("Signal: %d\n", signum);
    printf("PID sending process: %d\n", info->si_pid);
    printf("Value passed in signal: %d\n", info->si_value.sival_int);
    printf("Exit value or signal for process termination: %d\n", info->si_status);
    printf("Address at which fault occurred: %p\n", info->si_addr);
    printf("-------------------\n");
}

void handler_resethand(int signum, siginfo_t *info, void *context)
{
    printf("-------------------\n");
    printf("Signal recieved in handler\n");
    printf("-------------------\n");
}

void handler_nodefer(int signum, siginfo_t *info, void *context)
{
    printf("-------------------\n");
    printf("START: Signal recieved in handler\n");

    ++temp;
    if (temp <3)
    {
        kill(getpid(), SIGUSR1);
    
    }
    printf("END of Handler\n");
    printf("-------------------\n");
}

void test_nodefer()
{
    printf("SA_NODEFER\n");
    struct sigaction sa;
    sigemptyset(&sa.sa_mask);
    sa.sa_sigaction = handler_nodefer;
    sa.sa_flags = SA_NODEFER;
    sigaction(SIGUSR1, &sa, NULL);
    printf("Signal send\n");
    temp = 0;
    kill(getpid(), SIGUSR1);
}

void test_resethand()
{
    printf("SA_RESETHAND\n");
    struct sigaction sa;
    sigemptyset(&sa.sa_mask);
    sa.sa_sigaction = handler_resethand;
    sa.sa_flags = SA_RESETHAND;
    sigaction(SIGUSR1, &sa, NULL);
    printf("First signal send\n");
    kill(getpid(), SIGUSR1);
    printf("Second signal send\n");
    kill(getpid(), SIGUSR1);
}

void test_SIGINFO()
{
    printf("SA_SIGINFO\n");
    struct sigaction sa;
    sigemptyset(&sa.sa_mask);
    sa.sa_sigaction = handler;
    sa.sa_flags = SA_SIGINFO;
    sigaction(SIGUSR1, &sa, NULL);
    printf("Send signal from parent processs\n");
    kill(getpid(), SIGUSR1);

    pid_t pid = fork();
    if (pid == 0)
    {
        printf("Signal send from child process\n");
        kill(getpid(), SIGUSR1);
        exit(0);
    }
    else
    {
        waitpid(pid, NULL, 0);
    }
    sigval_t sigval = {2137};
    printf("Sending signal with extra value\n");
    sigqueue(getpid(), SIGUSR1, sigval);
}

int main()
{
    test_SIGINFO();
    printf("\n");
    test_nodefer();
    printf("\n");

    test_resethand();

    return 0;
}
