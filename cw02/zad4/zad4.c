#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <ftw.h>

long long int total_size = 0; 

int count_file_size(const char *fpath, const struct stat *s, int typeflag) {
    if (typeflag == FTW_F) {
        printf("Size: %lld bytes File: %s\n", (long long)s->st_size, fpath); 
        total_size += s->st_size;
    }
    return 0; 
}

int main(int argc, char *argv[]) {
    if (argc<2)
    {
        printf("NO ARGS PASSED\n");
        return 0;
    }
    
    if (ftw(argv[1], count_file_size, 50) == -1) { 
        printf("ERROR DURING FTW\n");
    }
    
    printf("Total size of files: %lld bytes\n", total_size);
}
