#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#define BUFFER_SIZE 4096

int main(int argc, char *argv[])
{
    FILE *fortune_output = popen("fortune", "r");
    char fortune_buffer[BUFFER_SIZE];
    fread(fortune_buffer, sizeof(char),BUFFER_SIZE, fortune_output);
    pclose(fortune_output);

    FILE *cowsay_input = popen("cowsay","w");
    fwrite(fortune_buffer,sizeof(char),strlen(fortune_buffer),cowsay_input);
    pclose(cowsay_input);

    return 0;
}

