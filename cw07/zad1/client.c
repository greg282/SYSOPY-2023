#include "com.h"


int shmid, semid;
int *clients_waiting, *clients_served;
int *barber_status, *chair_status, *client_ids;
void open_semaphores()
{
    semid = semget(SEM_KEY, 5, 0); 
    if (semid == -1) {
        perror("semget");
        exit(1);
    }
}

void open_shared_memory()
{
    shmid = shmget(SHM_KEY, (M + P + N + M) * sizeof(int), 0); 
    if (shmid == -1) {
        perror("shmget");
        exit(1);
    }

    void *ptr = shmat(shmid, NULL, 0); 
    if (ptr == (void *)-1) {
        perror("shmat");
        exit(1);
    }


    clients_waiting = (int *)ptr;
    clients_served = clients_waiting + P;
    barber_status = clients_served + M;
    chair_status = barber_status + M;
    client_ids = chair_status + N;
}


int main(int argc, char *argv[]){
    open_semaphores();
    open_shared_memory();
    struct sembuf sops;
    sops.sem_flg=0;
    sops.sem_num=0;
    sops.sem_op=-1;
    if(semop(semid, &sops, 1) == -1) {
        perror("semop");
        exit(1);
    }

    sops.sem_num = 1;
    sops.sem_op = 1;
    sops.sem_flg = 0;
    printf("semid: %d\n", semid);

    if (semop(semid, &sops, 1) == -1) {
        perror("semop");
        exit(1);
    }
}