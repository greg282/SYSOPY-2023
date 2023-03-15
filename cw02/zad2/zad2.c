#include <stdio.h>
#include <time.h>
#define BUFFER_SIZE 1024

void printTimer(clock_t clock_start,clock_t clock_end){
    double realtime=(double) (clock_end-clock_start) / CLOCKS_PER_SEC;;
    
    printf("Time in seconds: Real: %.20f ",realtime);
}

void one_byte_reverse(int argc, char *argv[]){
    char *infilename=argv[1];
    char *outfilename=argv[2];

    FILE *fIn, *fOut;
    char ch;
    long fileSize;

    fIn = fopen(infilename, "rb");
    fOut = fopen(outfilename, "wb");

    fseek(fIn, 0, SEEK_END);
    fileSize = ftell(fIn);
    fseek(fIn, 0, SEEK_SET);

    for (long i = fileSize - 1; i >= 0; i--)
    {
        fseek(fIn, i, SEEK_SET);
        ch = fgetc(fIn);
        fputc(ch, fOut);
    }

    fclose(fIn);
    fclose(fOut);

}

void reverse_1024_byte(int argc, char *argv[]){
    char *infilename=argv[1];
    char *outfilename=argv[2];

    FILE *fIn, *fOut;
    char buffer[BUFFER_SIZE];
    long fileSize, bytesRead;

    fIn = fopen(infilename, "rb");
    fOut = fopen(outfilename, "wb");

    fseek(fIn, 0, SEEK_END);
    fileSize = ftell(fIn);
    fseek(fIn, 0, SEEK_SET);

    for (long i = fileSize - BUFFER_SIZE; i >= 0; i -= BUFFER_SIZE)
    {
        fseek(fIn, i, SEEK_SET);
        bytesRead = fread(buffer, 1, BUFFER_SIZE, fIn);
        for (int j = bytesRead - 1; j >= 0; j--)
        {
            fputc(buffer[j], fOut);
        }
    }

    if (fileSize % BUFFER_SIZE != 0)
    {
        fseek(fIn, 0, SEEK_SET);
        bytesRead = fread(buffer, 1, fileSize % BUFFER_SIZE, fIn);
        for (int j = bytesRead - 1; j >= 0; j--)
        {
            fputc(buffer[j], fOut);
        }
    }

    fclose(fIn);
    fclose(fOut);
}

int main(int argc, char *argv[])
{   
    clock_t clockStart, clockEnd;
    double time_elapsed;

    clockStart=clock();
    one_byte_reverse(argc,argv);
    clockEnd=clock();
    time_elapsed=((double) (clockEnd - clockStart)) / CLOCKS_PER_SEC;
    printf("Odwracanie po jednym znaku czas: %f s\n", time_elapsed);

    printf("----------------------------------\n");

    clockStart=clock();
    reverse_1024_byte(argc,argv);
    clockEnd=clock();

    time_elapsed=((double) (clockEnd - clockStart)) / CLOCKS_PER_SEC;
    printf("Odwracanie po 1024 znakow czas: %f s\n", time_elapsed);

}
