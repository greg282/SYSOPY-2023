#include <pthread.h>
#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include <signal.h>
#include <unistd.h>


#define REINDERS 9
#define ELVES 10
#define MAX_ELVES_Q 3
#define TARGET 3

pthread_t santa;
pthread_t reindeer[REINDERS];
pthread_t elves[ELVES];

pthread_mutex_t santa_mutex = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t reindeer_mutex = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t elves_mutex = PTHREAD_MUTEX_INITIALIZER;

pthread_cond_t santa_cond = PTHREAD_COND_INITIALIZER;
pthread_cond_t reindeer_cond = PTHREAD_COND_INITIALIZER;
pthread_cond_t elves_cond = PTHREAD_COND_INITIALIZER;


int reindeers_returned = 0;

int elves_queue[MAX_ELVES_Q];
int elves_waiting = 0;
struct thread_data{
    int id;
};



void *santa_function(void *arg){
    int deliveries = 0;
    while(1){
        pthread_mutex_lock(&santa_mutex);
        printf("Komunikat: Mikołaj: zasypiam\n");

        pthread_cond_wait(&santa_cond, &santa_mutex);
        printf("Komunikat: Mikołaj: budzę się\n");

        pthread_mutex_lock(&elves_mutex);
        if(elves_waiting==MAX_ELVES_Q){
            printf("Komunikat: Mikołaj: rozwiazuje problemy elfów z ID: %d, %d, %d\n", elves_queue[0], elves_queue[1], elves_queue[2]);
            printf("Komunikat: Elf: Mikołaj rozwiązuje problem, ID_Elfa: %d\n", elves_queue[0]);
            printf("Komunikat: Elf: Mikołaj rozwiązuje problem, ID_Elfa: %d\n", elves_queue[1]);
            printf("Komunikat: Elf: Mikołaj rozwiązuje problem, ID_Elfa: %d\n", elves_queue[2]);
            int sleep_time = rand() % 2 + 1;
            sleep(sleep_time);
            elves_waiting = 0;
            for(int i = 0; i < MAX_ELVES_Q; i++){
                elves_queue[i] = 0;
            }
            pthread_cond_broadcast(&elves_cond);
        }
        pthread_mutex_unlock(&elves_mutex);

        pthread_mutex_lock(&reindeer_mutex);
        if(reindeers_returned == REINDERS){
            printf("Komunikat: Mikołaj: dostarczam zabawki\n");
            int sleep_time = rand() % 3 + 2;
            sleep(sleep_time);
            deliveries++;
            reindeers_returned = 0;
            pthread_cond_broadcast(&reindeer_cond);

        }
        pthread_mutex_unlock(&reindeer_mutex);

        pthread_mutex_unlock(&santa_mutex);

        if(deliveries == TARGET){
            printf("Komunikat: Mikołaj: kończę pracę\n");
            exit(0);
        }


    }    
}

void *elves_function(void *arg){
    int id = *((int*)arg);  
    while (1)
    {
        int sleep_time = rand() % 4 + 2;
        printf("Komunikat: Elf : Pracuje przez %ds, ID_Elfa: %d\n", sleep_time, id);

        sleep(sleep_time);


        pthread_mutex_lock(&elves_mutex);
        while (elves_waiting==MAX_ELVES_Q)
        {
            printf("Komunikat: Elf: czeka na powrót elfów, ID_Elfa: %d\n", id);
            pthread_cond_wait(&elves_cond, &elves_mutex);
        }
        pthread_mutex_unlock(&elves_mutex);
        


        pthread_mutex_lock(&elves_mutex);
        if(elves_waiting+1<MAX_ELVES_Q){
            elves_waiting++;
            elves_queue[elves_waiting] = id;
            printf("Komunikat: Elf: czeka %d elfów na Mikołaja, ID_Elfa: %d\n", elves_waiting, id);
            pthread_cond_wait(&elves_cond, &elves_mutex);

        }
        else if (elves_waiting+1 == MAX_ELVES_Q)
        {
            elves_waiting++;
            printf("Komunikat: Elf: wybudzam Mikołaja, ID_Elfa: %d\n", id);
            pthread_cond_broadcast(&santa_cond);
            pthread_cond_wait(&elves_cond, &elves_mutex);

        }
        else{
            printf("Komunikat: Elf: samodzielnie rozwiązuje swój problem, ID_Elfa: %d\n", id);

        }
        pthread_mutex_unlock(&elves_mutex);

        
    }
    

}

void *reindeer_function(void *arg){
    int id = *((int*)arg);   
    while(1)
    {
    int sleep_time = rand() % 6 + 5;
    sleep(sleep_time);

    pthread_mutex_lock(&reindeer_mutex);
   

    reindeers_returned++;

    printf("Komunikat: Renifer: czeka %d reniferów na Mikołaja, ID_Renifera:%d\n", reindeers_returned, id);
    if(reindeers_returned == REINDERS){
        printf("Komunikat: Renifer: wybudzam Mikołaja, ID_Renifera:%d\n", id);
        pthread_cond_broadcast(&santa_cond);
    }
    pthread_cond_wait(&reindeer_cond, &reindeer_mutex);

    pthread_mutex_unlock(&reindeer_mutex);
    }

    

}




int main(){
    //create santa thread

    pthread_create(&santa, NULL, &santa_function, NULL);
    
    //create elves threads
    for(int i = 0; i < ELVES; i++){
    
        //create thread data and pass it to thread
        struct thread_data *td = malloc(sizeof(struct thread_data));
        td->id = i+1;
        pthread_create(&elves[i], NULL, &elves_function, &td->id);
    }

    //create reindeer threads
    for(int i = 0; i < REINDERS; i++){
        struct thread_data *td = malloc(sizeof(struct thread_data));
        td->id = i+1;
        pthread_create(&reindeer[i], NULL, &reindeer_function, &td->id);
    }

    //run threads cleanup
    pthread_join(santa, NULL);
    
    for(int i = 0; i < ELVES; i++){
        pthread_join(elves[i], NULL);
    }
    for(int i = 0; i < REINDERS; i++){
        pthread_join(reindeer[i], NULL);
    }
    //destroy
    pthread_mutex_destroy(&santa_mutex);
    pthread_mutex_destroy(&reindeer_mutex);
    pthread_mutex_destroy(&elves_mutex);
    return 0;

}