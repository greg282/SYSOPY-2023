#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <unistd.h>
#include <sys/wait.h>
#include <time.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <string.h>

#define BUFF_SIZE 1024
#define PIPE_PATH "userfifo"

int main(int argc, char **argv)
{
    double dx = atof(argv[1]);
    int n = atoi(argv[2]);

    pid_t pid[n];
    double sum = 0;

    clock_t start = clock();

    mkfifo(PIPE_PATH, 0666);

    for (int i = 0; i < n; i++)
    {
        pid[i] = fork();

        if (pid[i] == 0)
        {
            char buff[BUFF_SIZE];
            snprintf(buff, BUFF_SIZE, "%d", i);
            execl("./calculate", "calculate", buff, argv[2], argv[1], NULL);
        }
    }

    int pipe_fd = open(PIPE_PATH, O_RDONLY);
    int results_read = 0;
    double buff;
    while (results_read < n)
    {
        size_t size = read(pipe_fd, &buff, sizeof(double));
        if (size > 0)
        {
            sum += buff;
            results_read++;
        }
    }

    close(pipe_fd);
    remove(PIPE_PATH);

    clock_t end = clock();

    double time = (double)(end - start) / CLOCKS_PER_SEC;

    printf("Result: %f\n", sum);
    printf("Time: %f s\n", time);

    printf("dx=%.10f n=%d\n", dx, n);

    return 0;
}