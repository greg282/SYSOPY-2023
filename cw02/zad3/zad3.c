#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <dirent.h>
#include <sys/stat.h>

void search_curr_dir(){
    DIR *dir;
    struct dirent *dp;
    struct stat file_stat;
    long long total_size = 0;
    char *dir_name = ".";

    dir = opendir(dir_name);
    if (dir == NULL) {
        printf("Error during loading directory");    
    }
    
    while ((dp = readdir(dir)) != NULL) {
        if (strcmp(dp->d_name, ".") == 0 || strcmp(dp->d_name, "..") == 0)
        {
            continue;
        }
        
        char *file_name = dp->d_name;
        char file_path[1024];
        sprintf(file_path, "%s/%s", dir_name, file_name);

        if (stat(file_path, &file_stat) < 0) {
            printf("ERROR occured during reading file stat");
            continue;
        }

        if (!S_ISDIR(file_stat.st_mode)) {
            printf("File: %s Size: %lld bytes\n", file_name,(long long)file_stat.st_size);
            total_size += file_stat.st_size;
        }
    }

    printf("Total size: %lld bytes\n", total_size);

    closedir(dir);
}

int main() {
    search_curr_dir();

}
