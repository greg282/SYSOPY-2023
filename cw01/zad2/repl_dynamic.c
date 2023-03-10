#include <dlfcn.h>
#include <stdio.h>
#include "../zad1/mylibrary_stats.h"
#include <string.h>
#include <time.h>
#include <sys/times.h>
#define MAX_RESPONSE_SIZE 1024
#define MAX_COMMAND_LEN 64
#define MAX_FILE_PATH_LEN 512

void printTimer(clock_t clock_start,clock_t clock_end,struct tms tms_start,struct tms tms_end,char* command){
    double realtime=(double) (clock_end-clock_start) / CLOCKS_PER_SEC;;
    double systime=(double ) (tms_end.tms_stime-tms_start.tms_stime)/CLOCKS_PER_SEC;
    double usertime=(double ) (tms_end.tms_utime-tms_start.tms_utime)/CLOCKS_PER_SEC;

    printf("Type: %s. Time in seconds: Real: %.20f System: %.20f User: %.20f\n",command,realtime,systime,usertime);
}

void repl() {
    char response[MAX_RESPONSE_SIZE];
    char command[MAX_COMMAND_LEN];
    char file[MAX_FILE_PATH_LEN];
    int size;
    int char_readed;
    int index;
    struct block BLOCK;

    clock_t clockStart, clockEnd;
    struct tms tms_Start, tms_End;

    while ( fgets(response, MAX_RESPONSE_SIZE, stdin)) {

        sscanf(response,"%s %n",command,&char_readed);

        if(strcmp(command,"init")==0){
            sscanf(response+char_readed,"%d",&size);

            clockStart=times(&tms_Start);
            BLOCK=initBlocks(size);
            clockEnd=times(&tms_End);

            printTimer(clockStart,clockEnd,tms_Start,tms_End,"init");

        }
        else if(strcmp(command,"count")==0){
            sscanf(response+char_readed,"%s",file);
            clockStart=times(&tms_Start);

            run_wc(BLOCK,file);

            clockEnd=times(&tms_End);

            printTimer(clockStart,clockEnd,tms_Start,tms_End,"count");
        }
        else if(strcmp(command,"show")==0){
            sscanf(response+char_readed,"%d",&index);
            clockStart=times(&tms_Start);

            char* tmp=getBlockDataByIndex(index,BLOCK);
            if(tmp!=NULL){
                printf("%s\n", getBlockDataByIndex(index,BLOCK));

            }

            clockEnd=times(&tms_End);

            printTimer(clockStart,clockEnd,tms_Start,tms_End,"show");
        }
        else if(strcmp(command,"delete")==0){
            sscanf(response+char_readed,"%d",&index);
            clockStart=times(&tms_Start);


            deleteBlockDataByIndex(index,BLOCK);

            clockEnd=times(&tms_End);

            printTimer(clockStart,clockEnd,tms_Start,tms_End,"delete");
        }
        else if(strcmp(command,"destroy")==0){
            clockStart=times(&tms_Start);

            freeAllBlocks(BLOCK);
            BLOCK.blocks=NULL;

            clockEnd=times(&tms_End);

            printTimer(clockStart,clockEnd,tms_Start,tms_End,"destroy");
        } else{
            printf("COMMAND NOT FOUND!\n");
        }


    }
}
int main(){
    void* handle = dlopen("./mylibrary_stats.so", RTLD_LAZY);
    if(!handle){printf("Error during loading library\n");}

    struct block (*_initBlocks)(int)= dlsym(handle,"initBlocks");
    void (*_run_wc)(struct block,char*)= dlsym(handle,"run_wc");
    char* (*_getBlockDataByIndex)(int,struct block)= dlsym(handle,"getBlockDataByIndex");
    void (*_deleteBlockDataByIndex)(int,struct block)= dlsym(handle,"deleteBlockDataByIndex");
    void (*_freeAllBlocks)(struct block)= dlsym(handle,"freeAllBlocks");
    repl();


    dlclose(handle);
}