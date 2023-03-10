#include <stdio.h>
#include "mylibrary_stats.h"

struct block initBlocks(int max_n_of_blocks){//1
    struct block BLOCK;
    BLOCK.blocks=calloc(max_n_of_blocks,sizeof(char*));
    BLOCK.max_size=max_n_of_blocks;
    BLOCK.current_size=0;

    for(int i=0;i<max_n_of_blocks;i++){
        BLOCK.blocks[i]=calloc(max_n_of_blocks,sizeof(char));
    }
    printf("Block initialized \n");
    return BLOCK;
}
long getFileSize(FILE* file){
    fseek(file,0,SEEK_END);
    return ftell(file);

}

void run_wc(struct block BLOCK,char* filePath){//2
    char* command=calloc(2048,sizeof(char));
    snprintf(command,2048,"wc %s > %s",filePath,"/tmp/tmp.txt");

    int status=system(command);
    if(status!=0){
        printf("%s %d \n","Error Occured during command execution, Code:",status);
    }

    free(command);

    FILE* file = fopen("/tmp/tmp.txt", "r");
    if (file == NULL) 
    {
        printf("FILE NOT FOUND");
    }else{

    int fileSize= getFileSize(file);
    char* buffer = calloc(fileSize,sizeof(char));
    fseek(file, 0, SEEK_SET);
    fread(buffer,sizeof (char ), fileSize, file);

    fclose(file);
    int result=-1;
    for(int i=0;i<BLOCK.max_size;i++){
        if(*BLOCK.blocks[i]==0){
            result=0;
            BLOCK.blocks[i]=buffer;
            BLOCK.current_size+=1;
            break;
        }
    }
    if(result==-1){
        printf("No space in BLOCKS or BLOCK NOT INITIALIZED\n");
    }

    remove("/tmp/tmp.txt");
    }
}

char* getBlockDataByIndex(int index,struct block BLOCK){//3 zwraca wskaznik na tablice char
    if(BLOCK.blocks==NULL){
        printf("BLOCK WAS DESTROYED!\n");
        return NULL;
    }
    if(index>=BLOCK.max_size){
        printf("Wrong index!\n");
        return NULL;
    }
    if(*BLOCK.blocks[index]==0){
        printf("NO DATA ON THIS INDEX\n");
        return NULL;
    }

    return BLOCK.blocks[index];
}

void deleteBlockDataByIndex(int index,struct block BLOCK){//4
    if(index>=BLOCK.max_size){
        printf("Wrong Index !\n");
    }
    else{
        free(BLOCK.blocks[index]);
        BLOCK.blocks[index]= calloc(BLOCK.max_size,sizeof (char ));
    }
}

void freeAllBlocks(struct block BLOCK){//5
    for(int i=0;i<BLOCK.max_size;i++){
        free(BLOCK.blocks[i]);
    }
    free(BLOCK.blocks);

}


