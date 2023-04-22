#include "com.h"
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/ipc.h>
#include <sys/sem.h>
#include <sys/shm.h>
#include <signal.h>
#include <time.h>
int glob_id;
int shmid, semid;
int *clients_waiting, *clients_served;
int *barber_status, *chair_status, *client_ids;
void open_semaphores()
{
    semid = semget(SEM_KEY, 5, 0); // pobierz ID semaforów
    if (semid == -1) {
        perror("semget");
        exit(1);
    }
}

void open_shared_memory()
{
    shmid = shmget(SHM_KEY, (M + P + N + M) * sizeof(int), 0); // uzyskaj identyfikator pamięci współdzielonej
    if (shmid == -1) {
        perror("shmget");
        exit(1);
    }

    void *ptr = shmat(shmid, NULL, 0); // uzyskaj wskaźnik do pamięci współdzielonej
    if (ptr == (void *)-1) {
        perror("shmat");
        exit(1);
    }

    // przypisz wskaźniki do zmiennych w pamięci współdzielonej
    clients_waiting = (int *)ptr;
    clients_served = clients_waiting + P;
    barber_status = clients_served + M;
    chair_status = barber_status + M;
    client_ids = chair_status + N;
}


int grab_free_chair(){
    //wait
    struct sembuf sops;
    sops.sem_flg=0;
    sops.sem_num=2;
    sops.sem_op=-1;
    if(semop(semid, &sops, 1) == -1) {
        perror("semop");
        exit(1);
    }

    int to_return = -1;
    for(int i=0; i<N; i++){
        if(chair_status[i]==0){
             to_return = i;
             chair_status[i]=1;
                break;
        }
    }

    //post
    sops.sem_flg=0;
    sops.sem_num=2;
    sops.sem_op=1;
    if(semop(semid, &sops, 1) == -1) {
        perror("semop");
        exit(1);
    }

    return to_return;
}
void raise_sem_waitroom(){
        struct sembuf sops[1];
        sops[0].sem_num = 3;
        sops[0].sem_op = 1;
        sops[0].sem_flg = 0;

        if(semop(semid, sops, 1) == -1){
            perror("semop");
            exit(1);
        }
}

int reserve_place_in_waitroom(){
    //wait
    struct sembuf sops;
    sops.sem_flg=0;
    sops.sem_num=2;
    sops.sem_op=-1;
    if(semop(semid, &sops, 1) == -1) {
        perror("semop");
        exit(1);
    }

    int to_return = -1;
    for(int i=0; i<P; i++){
        if(clients_waiting[i]==-1){
             to_return = i;
             clients_waiting[i]=1;
                break;
        }
    }

    //post
    sops.sem_flg=0;
    sops.sem_num=2;
    sops.sem_op=1;
    if(semop(semid, &sops, 1) == -1) {
        perror("semop");
        exit(1);
    }

    
    raise_sem_waitroom();
    return to_return;
}



void serve_client_from_waitroom(){
    //wait
    struct sembuf sops;
    sops.sem_flg=0;
    sops.sem_num=2;
    sops.sem_op=-1;
    if(semop(semid, &sops, 1) == -1) {
        perror("semop");
        exit(1);
    }

    for(int i=0; i<P; i++){
        if(clients_waiting[i]==1){
             clients_waiting[i]=-1;
             printf("Barber id:%d is serving client from waitroom with waitID:%d\n",glob_id,i);
                break;
        }
    }

    //post
    sops.sem_flg=0;
    sops.sem_num=2;
    sops.sem_op=1;
    if(semop(semid, &sops, 1) == -1) {
        perror("semop");
        exit(1);
    }
}

void clear_chair_space_by_id(int id){
    //wait
    struct sembuf sops;
    sops.sem_flg=0;
    sops.sem_num=2;
    sops.sem_op=-1;
    if(semop(semid, &sops, 1) == -1) {
        perror("semop");
        exit(1);
    }

    chair_status[id]=0;

    //post
    sops.sem_flg=0;
    sops.sem_num=2;
    sops.sem_op=1;
    if(semop(semid, &sops, 1) == -1) {
        perror("semop");
        exit(1);
    }
}

void raise_seat_sem(){
    struct sembuf sops[1];
    sops[0].sem_num = 4;
    sops[0].sem_op = 1;
    sops[0].sem_flg = 0;

    if(semop(semid, sops, 1) == -1){
        perror("semop");
        exit(1);
    }
}

void lower_seat_sem(){
    struct sembuf sops[1];
    sops[0].sem_num = 4;
    sops[0].sem_op = -1;
    sops[0].sem_flg = 0;

    if(semop(semid, sops, 1) == -1){
        perror("semop");
        exit(1);
    }
}
void wait_for_seat_to_free(){
    lower_seat_sem();
}
int main(int argc, char *argv[]){
    setbuf(stdout, NULL);
    open_semaphores();
    open_shared_memory();
    //print int from argv
    int id = atoi(argv[1]);
    glob_id = id;
    printf("Barber id: %d\n", id);

    int haircut[F]=HAIRCUTS;

    while (1)
    {
        //wait for client to arrive in bufor
        struct sembuf sops[1];
        sops[0].sem_num = 3;//obsługa poczekalni
        sops[0].sem_op = -1;
        sops[0].sem_flg = IPC_NOWAIT;
        if(semop(semid, sops, 1) != -1){
            //grab chair
            int chair_id = grab_free_chair();
            if(chair_id==-1){
                printf("Barber id:%d is waiting for chair for client from waitroom\n", id);
                wait_for_seat_to_free();
            }

            serve_client_from_waitroom();

            int rand_num = rand() % 3;
            printf("Barber id:%d is cutting hair for %d seconds\n", id, haircut[rand_num]);
            sleep(haircut[rand_num]);
            raise_seat_sem();
            sops[0].sem_num = 0;
            sops[0].sem_op = 1;
            sops[0].sem_flg = 0;

            if(semop(semid, sops, 1) == -1){
                perror("semop");
                exit(1);
            }

            printf("Client served by barber id:%d\n", id);
            continue;
        }

        sops[0].sem_num = 1;//oblsuga klientow przychozacych
        sops[0].sem_op = -1;
        sops[0].sem_flg = 0;

        if(semop(semid, sops, 1) == -1){
            perror("semop");
            exit(1);
        }
        printf("Client arrived for barber id:%d\n", id);

        int chair_nr=grab_free_chair();
        if(chair_nr==-1){
            printf("No free chairs for barber id:%d\n", id);
            int place_nr=reserve_place_in_waitroom();
            if (place_nr==-1)
            {
                printf("No free places in waitroom client leaves, barber id:%d\n", id);
                sops[0].sem_num = 0;
                sops[0].sem_op = 1;
                sops[0].sem_flg = 0;
                if(semop(semid, sops, 1) == -1){
                    perror("semop");
                    exit(1);
                }
                continue;
            }
            else{
                printf("Client is waiting in waitroom with waitID:%d\n", place_nr);
                sops[0].sem_num = 0;
                sops[0].sem_op = 1;
                sops[0].sem_flg = 0;
                if(semop(semid, sops, 1) == -1){
                    perror("semop");
                    exit(1);
                }
                continue;
            }
            
        }
        printf("Client is sitting in chair nr:%d\n", chair_nr);
        lower_seat_sem();
        srand ( time(NULL) );
        int rand_num = rand() % 3;
        printf("Barber id:%d is cutting hair for %d seconds\n", id, haircut[rand_num]);
        sleep(haircut[rand_num]);
        clear_chair_space_by_id(chair_nr);
        raise_seat_sem();
        sops[0].sem_num = 0;
        sops[0].sem_op = 1;
        sops[0].sem_flg = 0;

        if(semop(semid, sops, 1) == -1){
            perror("semop");
            exit(1);
        }

        printf("Client served by barber id:%d\n", id);
    }
    
    
}