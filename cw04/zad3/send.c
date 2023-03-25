#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>
#include <time.h>



void handler(int sig, siginfo_t *info, void *ucontext) {
    printf("Signal recieved\n");
}

int main(int argc, char *argv[]) {
    struct sigaction sa;
    sa.sa_sigaction = handler;
    sa.sa_flags = SA_SIGINFO;
    sigaction(SIGUSR1, &sa, NULL);

    pid_t catcher_pid = atoi(argv[1]);

    for (int i = 2; i < argc; i++) {
        int mode = atoi(argv[i]);
        printf("Sending SIGUSR1 with value %d\n", mode);
        sigqueue(catcher_pid, SIGUSR1, (union sigval)mode);
        sigset_t set;
        sigpending(&set);
        sigsuspend(&set);
    }


    printf("All finished,exit\n");
    exit(0);
}
