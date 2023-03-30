#include <unistd.h>
#include <stdio.h>
#include <fcntl.h>
#include <stdlib.h>

#define PIPE_PATH "userfifo"

double f(double x)
{
    return 4.0 / (x * x + 1);
}

double riemann_sum(double a, double b, double dx)
{
    double sum = 0.0;
    double x = a;
    while (x < b)
    {
        sum += f(x) * dx;
        x += dx;
    }
    return sum;
}

int main(int argc, char **argv)
{
    int i = atoi(argv[1]);
    int n = atoi(argv[2]);
    double dx = atof(argv[3]);

    
    double tmp = riemann_sum((double)i / n, (double)(i + 1) / n, dx);

    int pipe_fd = open(PIPE_PATH, O_WRONLY);
    write(pipe_fd,&tmp,sizeof(double));
    close(pipe_fd);


    return 0;
}