#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <time.h>

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
    double dx = atof(argv[1]);
    int n = atoi(argv[2]);

    int fd[n][2];
    pid_t pid[n];
    double sums[n];
    double sum = 0;

    clock_t start = clock();

    for (int i = 0; i < n; i++)
    {
        pipe(fd[i]);

        pid[i] = fork();

        if (pid[i] == 0) 
        {
            close(fd[i][0]); 
            double tmp = riemann_sum((double)i / n, (double)(i + 1) / n, dx);
            write(fd[i][1], &tmp, sizeof(double)); 
            close(fd[i][1]);                       
            return 0;
        }
        else 
        {
            close(fd[i][1]); 
            waitpid(pid[i], NULL, 0);
            read(fd[i][0], &sums[i], sizeof(double));
            close(fd[i][0]);
        }
    }

    for (int i = 0; i < n; i++)
    {
        sum += sums[i];
    }

    clock_t end = clock();

    double time = (double)(end - start) / CLOCKS_PER_SEC;

    printf("Result: %f\n", sum);
    printf("Time: %f s\n", time);

    printf("dx=%lf n=%d\n", dx, n);

    return 0;
}