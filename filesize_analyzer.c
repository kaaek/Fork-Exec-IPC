#define _DEFAULT_SOURCE
#include <stdio.h>
#include <errno.h>
#include <string.h>
#include <stdlib.h>
#include <dirent.h>
#include <sys/types.h>

int main(int numberOfArguments, char* argumentVector[])
{    
    if (numberOfArguments < 2) {
        fprintf(stderr, "Usage: %s [directory path]\n", argumentVector[0]); return 1;
    }

    // Careful with directories that have spaces in their name.
    
    char* directoryPath = argumentVector[1];
    fprintf(stdout, "Processing directory: %s\n", directoryPath);

    DIR *directoryPointer;
    struct dirent *readDirectory;
    directoryPointer = opendir(directoryPath); // I am not sure the argument type is correct.

    if (directoryPointer == NULL) {
        perror("Cannot open the file\n"); return 1;
    }

    while ((readDirectory = readdir(directoryPointer)) != NULL){
        if(readDirectory->d_type == DT_REG){
            printf("FILE: %s\n", readDirectory->d_name);
        } else if (readDirectory->d_type == DT_DIR) {
            printf("DIR: %s\n", readDirectory->d_name);
        }
    }
    if(closedir(directoryPath) == -1) {
        perror("Cannot close the dir\n"); return 1;
    }

    return 0;
}