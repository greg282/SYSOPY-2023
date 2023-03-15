#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <linux/limits.h>
#include <dirent.h>
#include <sys/stat.h>
#include <sys/wait.h>
int search_curr_dir(char *str,char*dire ){
    //printf("%s\n",dire);
    char *dir_name = dire;
    char *string = str;

    DIR *dir;
    struct dirent *dp;
    struct stat file_stat;

    dir = opendir(dir_name);
    if (dir == NULL) {
        perror("Error during loading directory"); 
        return 1;   
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
            perror("ERROR occured during reading file stat");
            return 1;
        }

        if (S_ISDIR(file_stat.st_mode)) {
            if(strlen(file_path)<=PATH_MAX){
                pid_t pid = fork();
                if (pid == -1) {
                    perror("fork error\n");
                }
                else if(pid==0){
                    return search_curr_dir(string,file_path);
                }
                else{
                    waitpid(pid,NULL,0);
                }
            }

        }else{
            FILE *file = fopen(file_path, "r");
            if (file == NULL) {
                perror("Failed to open file\n");
                return 1;
            }
            char buffer[256];
            if (fgets(buffer, sizeof(buffer), file) != NULL) {
                if (strncmp(buffer, string, strlen(string)) == 0) {
                    printf("PID %d : FILE %s\n", getpid(), file_path);
                }
            }
            fclose(file);
        }
    }


    closedir(dir);
    return 0;
}


int main(int argc, char *argv[]) {
    setbuf(stdout, NULL); 
    if (argc != 3) {
       printf("Niepoprawna liczba argumentow!");
       return 0;
    }

    char *path = argv[1];
    char *string = argv[2];

    if (strlen(string) > 255) {
        printf("Łańcuch nie może mieć więcej niż 255 bajtów.\n");
        return 0;
    }
    return search_curr_dir(string,path);


}
