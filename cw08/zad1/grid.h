#pragma once
#include <stdbool.h>
#include <pthread.h>
#include <signal.h>
#include <unistd.h>
char *create_grid();
void destroy_grid(char *grid);
void draw_grid(char *grid);
void init_grid(char *grid);
bool is_alive(int row, int col, char *grid);
void update_grid(char *src, char *dst);
void update_grid_multithread(char *src, char *dst, int num_threads);
#define SIZE 30