#ifndef _MYLIB_STATS_
#define _MYLIB_STATS_

#include <stdlib.h>
#include <stdio.h>

struct block {  
    char** blocks;
    int current_size;
    int max_size;
};
typedef struct block block;

struct block initBlocks(int max_n_of_blocks);//1

void run_wc(struct block BLOCK,char* filePath);//2

char* getBlockDataByIndex(int index,struct block BLOCK);//3

void deleteBlockDataByIndex(int index,struct block BLOCK);//4

void freeAllBlocks(struct block BLOCK);//5

#endif