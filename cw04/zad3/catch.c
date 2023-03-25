#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>
#include <time.h>

int count = 0;

void print_time()
{
    time_t t;
    struct tm *tim;

    t = time(NULL);
    tim = localtime(&t);

    printf("Current time: %02d:%02d:%02d\n", tim->tm_hour, tim->tm_min, tim->tm_sec);
}

void handler(int sig, siginfo_t *info, void *ucontext)
{

    int mode = info->si_value.sival_int;

    sigval_t sigval;
    sigval.sival_int = mode;

    printf("Catcher: received signal SIGUSR1 with value %d\n", mode);
    sigqueue(info->si_pid, SIGUSR1, sigval);

    if (mode == 1)
    {
        for (int i = 1; i <= 100; i++)
        {
            printf("%d\n", i);
        }
    }
    else if (mode == 2)
    {
        print_time();
    }
    else if (mode == 3)
    {
        printf("Current change mode count:%d\n", count);
    }
    else if (mode == 4)
    {
        sigset_t tmp;
        while (1)
        {
            print_time();
            sleep(1);
            sigpending(&tmp);
            if (sigismember(&tmp, SIGUSR1))
            {
                break;
            }
        }
    }
    else if (mode == 5)
    {
        printf("Catcher: exiting\n");
        exit(0);
    }

    count++;
}

int main(int argc, char *argv[])
{
    pid_t pid = getpid();
    printf("Catcher PID: %d\n", pid);

    struct sigaction sa;
    sa.sa_sigaction = handler;
    sa.sa_flags = SA_SIGINFO;

    if (sigaction(SIGUSR1, &sa, NULL) == -1)
    {
        perror("sigaction");
        exit(1);
    }

    while (1)
    {
        pause();
    }
}
