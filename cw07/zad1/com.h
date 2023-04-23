#ifndef COM
#define COM
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/ipc.h>
#include <sys/sem.h>
#include <sys/shm.h>
#include <signal.h>
#include <time.h>
/*CONFIGURATION BELOW*/

#define M 5 // liczba fryzjerów
#define N 3 // liczba foteli
#define P 10 // liczba miejsc w poczekalni
#define F 3 // liczba dostępnych fryzur

/*END OF CONFIGURATION*/
#define SHM_KEY 12345 
#define SEM_KEY 54321 

void universal_semop(int semid, int sem_num, int sem_op, int sem_flg){
    struct sembuf sops;
    sops.sem_flg=sem_flg;
    sops.sem_num=sem_num;
    sops.sem_op=sem_op;
    if(semop(semid, &sops, 1) == -1) {
        perror("semop");
        exit(1);
    }
}

#endif