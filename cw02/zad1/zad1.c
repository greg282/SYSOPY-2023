#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include <time.h>
#define MAX_COMMAND_LEN 1024

void printTimer(clock_t clock_start,clock_t clock_end){
    double realtime=(double) (clock_end-clock_start) / CLOCKS_PER_SEC;;
    
    printf("Time in seconds: Real: %.20f ",realtime);
}


void change_ascii_lib(int argc, char *argv[]){
    char asci_find=argv[1][0];
    char asci_change=argv[2][0];
    char *infilename=argv[3];
    char *outfilename=argv[4];


    FILE *fp1, *fp2;
    char  ch;
    
    fp1 = fopen(infilename, "rb");
    fp2 = fopen(outfilename, "wb");
    
    if (fp1 == NULL) {
        printf("Error: Could not open file.");
    }
    
    while (fread(&ch, sizeof(char), 1, fp1)) {
        if (ch == asci_find) {
            ch = asci_change;
        }
        fwrite(&ch, sizeof(char), 1, fp2);
    }
    
    fclose(fp1);
    fclose(fp2);
    
    
}


void change_ascii_sys(int argc, char *argv[]){
    char asci_find=argv[1][0];
    char asci_change=argv[2][0];
    char *infilename=argv[3];
    char *outfilename=argv[4];
    char ch;
    int fd1, fd2;
    fd1 = open(infilename, O_RDONLY);
    fd2 = open(outfilename, O_WRONLY | O_CREAT | O_TRUNC);
    
    if (fd1 == -1) {
        printf("Error: Could not open file.");
    }
    
    while (read(fd1, &ch, sizeof(char)) == sizeof(char)) {
        if (ch == asci_find) {
            ch = asci_change;
        }
        write(fd2, &ch, sizeof(char));
    }
    
    close(fd1);
    close(fd2);
    
}

int main(int argc, char *argv[]){
    clock_t clockStart, clockEnd;
    double time_elapsed;
    clockStart=clock();
    change_ascii_lib(argc,argv);
    clockEnd=clock();
    time_elapsed=((double) (clockEnd - clockStart)) / CLOCKS_PER_SEC;
    printf("Time for lib functions: %f seconds\n", time_elapsed);
    printf("----------------------------------\n");
    clockStart=clock();
    change_ascii_sys(argc,argv);
    clockEnd=clock();
    time_elapsed=((double) (clockEnd - clockStart)) / CLOCKS_PER_SEC;
    printf("Time for sys functions: %f seconds\n", time_elapsed);
}