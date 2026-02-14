#define _DEFAULT_SOURCE
#include <stdio.h>
#include <stdbool.h>
#include <errno.h>
#include <string.h>
#include <stdlib.h>
#include <dirent.h>
#include <sys/types.h>

typedef struct {       // Dynamically-allocated array to hold the directory contents.
    char **files;      // Array of file path strings
    int size;          // Number of files currently stored
    int capacity;      // Total allocated capacity in the array
} FileArray;

void initializeFileArray(FileArray *array) {
    array->files = NULL;
    array->size = 0;
    array->capacity = 0;
}

int addFile(FileArray *array, const char *filename) {
    if (array->size >= array->capacity) {
        int newCapacity = (array->capacity == 0) ? 10 : array->capacity * 2;
        
        char **newFiles = realloc(array->files, newCapacity * sizeof(char*));
        if (newFiles == NULL) {
            fprintf(stderr, "Failed to reallocate memory for FileArray");
            exit(1);
        }
        
        array->files = newFiles;
        array->capacity = newCapacity;
    }
    
    array->files[array->size] = malloc(strlen(filename) + 1);
    if (array->files[array->size] == NULL) {
        fprintf(stderr, "Failder to reallocate memory for %s", filename);
        exit(1);
    }
    strcpy(array->files[array->size], filename);
    
    array->size++;
}

void freeFileArray(FileArray *array) {
    for (int i = 0; i < array->size; i++) {
        free(array->files[i]);
    }
    free(array->files);
    array->files = NULL;
    array->size = 0;
    array->capacity = 0;
}

int main(int numberOfArguments, char* argumentVector[])
{    
    if(!areArgumentsCorrect) return 1;
    
    char* directoryPath = argumentVector[1];

    readDirectory(directoryPath);

    return 0;
}

bool areArgumentsCorrect(int numberOfArguments, char* argumentVector[]){
    if (numberOfArguments < 2) {
        fprintf(stderr, "Usage: %s [directory path]\n", argumentVector[0]); return false;
    }

    // TODO Careful with directories that have spaces in their name. Add later.

    return true;
}

void readDirectory(directoryPath){
    fprintf(stdout, "Processing directory: %s\n\n", directoryPath);

    DIR *directoryPointer;
    struct dirent *readDirectory;
    directoryPointer = opendir(directoryPath);

    if (directoryPointer == NULL) {
        perror("Cannot open the file\n"); return 1;
    }

    FileArray groupOne; initializeFileArray(&groupOne);
    FileArray groupTwo; initializeFileArray(&groupTwo);
    FileArray groupThree; initializeFileArray(&groupThree);

    int roundRobinCounter = 1; // Indicates in which group the file/dir will go during each read operation.

    while ((readDirectory = readdir(directoryPointer)) != NULL){
        if(readDirectory->d_type == DT_REG){ // Is the entry's data type (d_type) a regular file (DT_REG)?
            
            char filename[] = readDirectory->d_name;
            
            printf("FILE: %s\n", filename);
            
            switch(roundRobinCounter){
                case 1:
                    addFile(&groupOne, &filename);
                break;
                case 2:
                    addFile(&groupTwo, &filename);
                break;
                case 3:
                    addFile(&groupThree, &filename);
                break;
            }
            roundRobinCounter++;
        }
    }

    if(closedir(directoryPointer) == -1) {
        perror("Cannot close the dir\n"); return 1;
    }
}