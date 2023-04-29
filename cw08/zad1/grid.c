#include "grid.h"
#include <stdlib.h>
#include <time.h>
#include <ncurses.h>

const int grid_width = SIZE;
const int grid_height = SIZE;

char *create_grid()
{
    return malloc(sizeof(char) * grid_width * grid_height);
}

void destroy_grid(char *grid)
{
    free(grid);
}

void draw_grid(char *grid)
{
    for (int i = 0; i < grid_height; ++i)
    {
        // Two characters for more uniform spaces (vertical vs horizontal)
        for (int j = 0; j < grid_width; ++j)
        {
            if (grid[i * grid_width + j])
            {
                mvprintw(i, j * 2, "■");
                mvprintw(i, j * 2 + 1, " ");
            }
            else
            {
                mvprintw(i, j * 2, " ");
                mvprintw(i, j * 2 + 1, " ");
            }
        }
    }

    refresh();
}

void init_grid(char *grid)
{
    for (int i = 0; i < grid_width * grid_height; ++i)
        grid[i] = rand() % 2 == 0;
}

bool is_alive(int row, int col, char *grid)
{

    int count = 0;
    for (int i = -1; i <= 1; i++)
    {
        for (int j = -1; j <= 1; j++)
        {
            if (i == 0 && j == 0)
            {
                continue;
            }
            int r = row + i;
            int c = col + j;
            if (r < 0 || r >= grid_height || c < 0 || c >= grid_width)
            {
                continue;
            }
            if (grid[grid_width * r + c])
            {
                count++;
            }
        }
    }

    if (grid[row * grid_width + col])
    {
        if (count == 2 || count == 3)
            return true;
        else
            return false;
    }
    else
    {
        if (count == 3)
            return true;
        else
            return false;
    }
}

void update_grid(char *src, char *dst)
{
    for (int i = 0; i < grid_height; ++i)
    {
        for (int j = 0; j < grid_width; ++j)
        {
            dst[i * grid_width + j] = is_alive(i, j, src);
        }
    }
}

struct thread_data
{
    char *src;
    char *dst;
    int row;
    int col;
};

void *thread_func(void *arg)
{
    struct thread_data *data = (struct thread_data *)arg;
   

    while (1)
    {
        data->dst[data->row * grid_width + data->col] = is_alive(data->row, data->col, data->src);
        pause();
        char *tmp = data->src;
        data->src = data->dst;
        data->dst = tmp;
    }
    pthread_exit(NULL);
}

void ignore_handler(int signo, siginfo_t *info, void *context) {
   


}

void update_grid_multithread(char *src, char *dst, int num_threads)
{   
    static pthread_t *threads = NULL;
    int thread_count = 0;

    struct sigaction action;
    sigemptyset(&action.sa_mask);
    action.sa_sigaction = ignore_handler;
    sigaction(SIGUSR1, &action, NULL);

    if (!threads)
    {
        threads = malloc(sizeof(pthread_t) * num_threads);

        
        for (int i = 0; i < grid_height; ++i)
        {
            for (int j = 0; j < grid_width; ++j)
            {
                struct thread_data* thread_data_= malloc(sizeof(struct thread_data));
                thread_data_->src = src;
                thread_data_->dst = dst;
                thread_data_->row = i;
                thread_data_->col = j;
                
                pthread_create(threads+thread_count, NULL, thread_func,(void*) thread_data_);
                thread_count++;
            }
        }
    }
    for (int i = 0; i < num_threads; i++)
    {
        pthread_kill(threads[i], SIGUSR1);
    }
}
