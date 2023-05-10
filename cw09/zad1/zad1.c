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

//Create mutexe
pthread_mutex_t santa_mutex = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t reindeer_mutex = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t elves_mutex = PTHREAD_MUTEX_INITIALIZER;

//Create condition variables
pthread_cond_t santa_cond = PTHREAD_COND_INITIALIZER;
pthread_cond_t reindeer_cond = PTHREAD_COND_INITIALIZER;
pthread_cond_t elves_cond = PTHREAD_COND_INITIALIZER;


int reindeers_returned = 0;

struct thread_data{
    int id;
};



//Create thread function
void *santa_function(void *arg){
    int deliveries = 0;
    while(1){
        //lock mutex
        pthread_mutex_lock(&santa_mutex);
        //wait cond
        printf("Komunikat: Mikołaj: zasypiam\n");

        pthread_cond_wait(&santa_cond, &santa_mutex);
        printf("Komunikat: Mikołaj: budzę się\n");

        // lock mutex reindeer
        pthread_mutex_lock(&reindeer_mutex);
        printf("Reinderss_returned: %d\n", reindeers_returned);
        if(reindeers_returned == REINDERS){
            printf("Komunikat: Mikołaj: dostarczam zabawki\n");
            //sleep for rand 2-4 sec
            int sleep_time = rand() % 3 + 2;
            sleep(sleep_time);
            deliveries++;
            reindeers_returned = 0;
       
        }
        //unlock mutex reinderr
        pthread_mutex_unlock(&reindeer_mutex);

        //unlock mutex
        pthread_mutex_unlock(&santa_mutex);

        if(deliveries == TARGET){
            printf("Komunikat: Mikołaj: kończę pracę\n");
            exit(0);
        }

        pthread_cond_broadcast(&reindeer_cond);

    }    
}

void *elves_function(void *arg){
    int id = *((int*)arg);  

}

void *reindeer_function(void *arg){
    int id = *((int*)arg);   
    while(1)
    {
    //sleep for rand 5-10 sec
    int sleep_time = rand() % 6 + 5;
    printf("Komunikat: Renifer: Jestem na wakacjach przez %ds, ID_Renifera: %d\n",sleep_time,id);
    sleep(sleep_time);

    //lock mutex
    pthread_mutex_lock(&reindeer_mutex);
   

    //increment reindeers_returned
    reindeers_returned++;

    printf("Komunikat: Renifer: czeka %d reniferów na Mikołaja, ID_Renifera:%d\n", reindeers_returned, id);
    if(reindeers_returned == REINDERS){
        //signal santa
        printf("Komunikat: Renifer: wybudzam Mikołaja, ID_Renifera:%d\n", id);
        pthread_cond_broadcast(&santa_cond);
    }
    //wait cond
    pthread_cond_wait(&reindeer_cond, &reindeer_mutex);

    //unlock mutex
    pthread_mutex_unlock(&reindeer_mutex);
    }

    

}




int main(){
    //create santa thread

    pthread_create(&santa, NULL, &santa_function, NULL);
    /*
    //create elves threads
    for(int i = 0; i < ELVES; i++){
    
        //create thread data and pass it to thread
        struct thread_data *td = malloc(sizeof(struct thread_data));
        td->id = i+1;
        pthread_create(&elves[i], NULL, &elves_function, &td->id);
    }
*/
    //create reindeer threads
    for(int i = 0; i < REINDERS; i++){
        struct thread_data *td = malloc(sizeof(struct thread_data));
        td->id = i+1;
        pthread_create(&reindeer[i], NULL, &reindeer_function, &td->id);
    }

    //run threads cleanup
    pthread_join(santa, NULL);
    /*
    for(int i = 0; i < ELVES; i++){
        pthread_join(elves[i], NULL);
    }*/
    for(int i = 0; i < REINDERS; i++){
        pthread_join(reindeer[i], NULL);
    }
    //destroy
    pthread_mutex_destroy(&santa_mutex);
    pthread_mutex_destroy(&reindeer_mutex);
    pthread_mutex_destroy(&elves_mutex);
    return 0;

}