#include "com.h"


int shmid, semid;
int *clients_waiting, *clients_served;
int *barber_status, *chair_status, *client_ids;
struct timespec t;

void init_semaphores()
{
    semid = semget(SEM_KEY, 5, IPC_CREAT | 0600); 
    if (semid == -1) {
        perror("semget");
        exit(1);
    }

    semctl(semid, 0, SETVAL, M); // fryzjerzy
    semctl(semid, 1, SETVAL, 0); // bufor dla przychodzacyhc klientow
    semctl(semid, 2, SETVAL, 1); // dostęp do pamięci współdzielonej
    semctl(semid, 3, SETVAL, 0); // dostęp do pamieci poczekalni
    semctl(semid, 4, SETVAL, N); // dostęp do pamieci foteli
}

void init_shared_memory()
{
    shmid = shmget(SHM_KEY, (M + P + N+M+1) * sizeof(int), IPC_CREAT | 0600); 
    if (shmid == -1) {
        perror("shmget");
        exit(1);
    }

    void *ptr = shmat(shmid, NULL, 0);
    if (ptr == (void *)-1) {
        perror("shmat");
        exit(1);
    }

    clients_waiting = ptr;
    clients_served = clients_waiting + P;
    barber_status = clients_served + M;
    chair_status = barber_status + M;
    client_ids = chair_status + N;

    for (int i = 0; i < P; i++) {
        clients_waiting[i] = -1; 
    }

    for (int i = 0; i < M; i++) {
        barber_status[i] = 0; 
    }

    for (int i = 0; i < N; i++) {
        chair_status[i] = 0; 
    }

    for (int i = 0; i < M; i++) {
        client_ids[i] = -1; 
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
    if (shmdt(clients_waiting) == -1) {
        perror("shmdt");
    }

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
    atexit(clear_resources);
    setbuf(stdout, NULL);
    init_semaphores();
    init_shared_memory();
    printf("semid: %d\n", semid);

    for (int i = 0; i < M; i++) {
        if(fork()==0){
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
    return 0;
}