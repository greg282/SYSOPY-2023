#include "com.h"
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/ipc.h>
#include <sys/sem.h>
#include <sys/shm.h>
#include <signal.h>
#include <time.h>

int shmid, semid;
int *clients_waiting, *clients_served;
int *barber_status, *chair_status, *client_ids;
struct timespec t;

void init_semaphores()
{
    semid = semget(SEM_KEY, 5, IPC_CREAT | 0600); // utwórz 3 semafory
    if (semid == -1) {
        perror("semget");
        exit(1);
    }

    // inicjuj semafory
    semctl(semid, 0, SETVAL, M); // fryzjerzy
    semctl(semid, 1, SETVAL, 0); // bufor dla przychodzacyhc klientow
    semctl(semid, 2, SETVAL, 1); // dostęp do pamięci współdzielonej
    semctl(semid, 3, SETVAL, 0); // dostęp do pamieci poczekalni
    semctl(semid, 4, SETVAL, N); // dostęp do pamieci foteli
}

void init_shared_memory()
{
    shmid = shmget(SHM_KEY, (M + P + N+M+1) * sizeof(int), IPC_CREAT | 0600); // utwórz pamięć współdzieloną
    if (shmid == -1) {
        perror("shmget");
        exit(1);
    }

    // załącz pamięć współdzieloną
    void *ptr = shmat(shmid, NULL, 0);
    if (ptr == (void *)-1) {
        perror("shmat");
        exit(1);
    }

    // zainicjuj zmienne w pamięci współdzielonej
    clients_waiting = ptr;
    clients_served = clients_waiting + P;
    barber_status = clients_served + M;
    chair_status = barber_status + M;
    client_ids = chair_status + N;

    for (int i = 0; i < P; i++) {
        clients_waiting[i] = -1; // klient oczekujący w poczekalni
    }

    for (int i = 0; i < M; i++) {
        barber_status[i] = 0; // fryzjer nieobsługujący klienta
    }

    for (int i = 0; i < N; i++) {
        chair_status[i] = 0; // fotel wolny
    }

    for (int i = 0; i < M; i++) {
        client_ids[i] = -1; // ID klienta obsługiwanego przez fryzjera
    }
}
void remove_semaphores()
{
    if (semctl(semid, 0, IPC_RMID) == -1) {
        perror("semctl");
        exit(1);
    }
}
void remove_shared_memory()
{
    // odłącz pamięć współdzieloną
    if (shmdt(clients_waiting) == -1) {
        perror("shmdt");
    }

    // usuń pamięć współdzieloną
    if (shmctl(shmid, IPC_RMID, NULL) == -1) {
        perror("shmctl");
    }
}
void clear_resources()
{
    remove_semaphores();
    remove_shared_memory();
}
 
int main(int argc, char *argv[]){
    setbuf(stdout, NULL);
    init_semaphores();
    init_shared_memory();
    //print semid
    printf("semid: %d\n", semid);

    //execute m barbers with ids
    for (int i = 0; i < M; i++) {
        if(fork()==0){
            //execute barber with arg i and check for errors
            //parse i to string
            char arg_str[20];
            sprintf(arg_str, "%d", i);
            if(execl("./barber", "./barber", arg_str, NULL)==-1){
                perror("execlp");
                exit(1);
            }
            exit(0);
        }
    }
    while (1)
    {
    
    }
    
    
    /*
    while (1)
    {
        //wait for client to arrive in bufor
        struct sembuf sops[1];
        sops[0].sem_num = 1;
        sops[0].sem_op = 0;
        sops[0].sem_flg = 0;

        if(semop(semid, sops, 1) == -1){
            perror("semop");
            exit(1);
        }
        printf("Client arrived\n");
        
        int tmp = -1;
        for (int i = 0; i < M; i++) {
        if(barber_status[i]==0){
            tmp=i;
            break;}
        }
        if(tmp!=-1){
           //ecexute barber with arg tmp
           execlp("./barber", "barber", tmp, NULL);
        }

        sops[0].sem_num = 1;
        sops[0].sem_op = 1;
        sops[0].sem_flg = 0;
        if(semop(semid, sops, 1) == -1){
            perror("semop");
            exit(1);
        }
        printf("Client was served\n");
        //critical section
        
    }
    
*/


    //clear_resources();
    return 0;
}