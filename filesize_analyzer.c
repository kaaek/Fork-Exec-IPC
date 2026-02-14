#define _DEFAULT_SOURCE
#include <dirent.h>
#include <errno.h>
#include <limits.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <unistd.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/wait.h>

typedef struct {       // Dynamically-allocated array to hold the directory contents.
    char **files;      // Array of file path strings
    int size;          // Number of files currently stored
    int capacity;      // Total allocated capacity in the array
} FileArray;

void initializeFileArray(FileArray *array);
void addFile(FileArray *array, const char *filename);
void freeFileArray(FileArray *array);
bool areArgumentsCorrect(int numberOfArguments, char* argumentVector[]);
long long readDirectory(char *directoryPath);
long long processGroups(FileArray *groupOne, FileArray *groupTwo, FileArray *groupThree);
long long getGlobalStatistics(char groupOneFile[], char groupTwoFile[], char groupThreeFile[]);
void getGroupStatistics(FileArray *group, char outputFilename[]);
void createNewEmptyFile(char fileName[]);
void appendToFile(char fileName[], off_t fileSizeToWrite);

int main(int numberOfArguments, char* argumentVector[])
{    
    if(!areArgumentsCorrect(numberOfArguments, argumentVector)) exit(EXIT_FAILURE);
    
    char* directoryPath = argumentVector[1];

    fprintf(stdout, readDirectory(directoryPath));

    return 0;
}

void initializeFileArray(FileArray *array) {
    array->files = NULL;
    array->size = 0;
    array->capacity = 0;
}

void addFile(FileArray *array, const char *filename) {
    if (array->size >= array->capacity) {
        int newCapacity = (array->capacity == 0) ? 10 : array->capacity * 2;
        
        char **newFiles = realloc(array->files, newCapacity * sizeof(char*));
        if (newFiles == NULL) {
            fprintf(stderr, "Failed to reallocate memory for FileArray");
            exit(EXIT_FAILURE);
        }
        
        array->files = newFiles;
        array->capacity = newCapacity;
    }
    
    array->files[array->size] = malloc(strlen(filename) + 1);
    if (array->files[array->size] == NULL) {
        fprintf(stderr, "Failder to reallocate memory for %s", filename);
        exit(EXIT_FAILURE);
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

bool areArgumentsCorrect(int numberOfArguments, char* argumentVector[]){
    if (numberOfArguments < 2) {
        fprintf(stderr, "Usage: %s [directory path]\n", argumentVector[0]); return false;
    }

    char* argument = argumentVector[1];

    struct stat pathStatistics;
    if (stat(argument, &pathStatistics) != 0 || !S_ISDIR(pathStatistics.st_mode)) { // Is the provided argument a directory (using stat())?
        fprintf(stderr, "Provided path is not a directory\n");
        return false;
    }

    return true;
}

long long readDirectory(char *directoryPath){
    fprintf(stdout, "Processing directory: %s\n\n", directoryPath);

    DIR *directoryPointer;
    struct dirent *directoryStream;
    directoryPointer = opendir(directoryPath);

    if (directoryPointer == NULL) {
        fprintf(stderr, "Cannot open the file\n");
        exit(EXIT_FAILURE);
    }

    FileArray groupOne; initializeFileArray(&groupOne);
    FileArray groupTwo; initializeFileArray(&groupTwo);
    FileArray groupThree; initializeFileArray(&groupThree);

    int roundRobinCounter = 1; // Indicates in which group the file/dir will go during each read operation

    while ((directoryStream = readdir(directoryPointer)) != NULL){
        if(directoryStream->d_type == DT_REG){ // Is the entry's data type (d_type) a regular file (DT_REG)?
            
            printf("FILE: %s\n", directoryStream->d_name);

            char fullPath[PATH_MAX]; // Cap the size of the file path to prevent a buffer overflow (limits.h)
            snprintf(fullPath, sizeof(fullPath), "%s/%s", directoryPath, directoryStream->d_name); // Takes directoryPath (input) and the file name (read from directoryStream) to produce: /path/to/file
            
            switch(roundRobinCounter){
                case 1:
                    addFile(&groupOne, fullPath);
                break;
                case 2:
                    addFile(&groupTwo, fullPath);
                break;
                case 3:
                    addFile(&groupThree, fullPath);
                break;
            }
            roundRobinCounter++;
            if (roundRobinCounter > 3) roundRobinCounter = 1; // Reset round robin counter
        }
    }

    if(closedir(directoryPointer) == -1) {
        fprintf(stderr, "Cannot close the directory\n");
        exit(EXIT_FAILURE);
    }

    return processGroups(&groupOne, &groupTwo, &groupThree);

}

long long processGroups(FileArray *groupOne, FileArray *groupTwo, FileArray *groupThree){

    long long cumulativeSize = -1;

    char groupOneFile[] = "group1.txt";
    char groupTwoFile[] = "group2.txt";
    char groupThreeFile[] = "group3.txt";

    pid_t pid1 = fork();
    if(pid1<0){
        fprintf(stderr, "Fork fail");
        exit(EXIT_FAILURE);

    } else if (pid1 == 0){ // Child 1 executes this block

        getGroupStatistics(groupOne, groupOneFile);
        exit(EXIT_SUCCESS);

    } else {

        pid_t pid2 = fork();
        if(pid2<0){
            fprintf(stderr, "Fork fail");
            exit(EXIT_FAILURE);

        } else if (pid2 == 0) { // Child 2 executes this block
            
            getGroupStatistics(groupTwo, groupTwoFile);
            exit(EXIT_SUCCESS);

        } else {
            
            pid_t pid3 = fork();
            if(pid3<0){
                fprintf(stderr, "Fork fail");
                exit(EXIT_FAILURE);

            } else if (pid3 == 0) { // Child 3 executes this block
                
                getGroupStatistics(groupThree, groupThreeFile);
                exit(EXIT_SUCCESS);

            } else { // Parent finally executes this block
                
                waitpid(pid1, NULL, 0);
                waitpid(pid2, NULL, 0);
                waitpid(pid3, NULL, 0);

                cumulativeSize = getGlobalStatistics(groupOneFile, groupTwoFile, groupThreeFile);

                freeFileArray(groupOne);
                freeFileArray(groupTwo);
                freeFileArray(groupThree);
            }
        }
    }

    return cumulativeSize;
}

long long getGlobalStatistics(char groupOneFile[], char groupTwoFile[], char groupThreeFile[]){

    long long cumulativeSize = 0;

    FILE *groupOneFilePointer;
    FILE *groupTwoFilePointer;
    FILE *groupThreeFilePointer;

    groupOneFilePointer = fopen(groupOneFile, "r"); 
    groupTwoFilePointer = fopen(groupTwoFile, "r"); 
    groupThreeFilePointer = fopen(groupThreeFile, "r");

    if (groupOneFilePointer == NULL || groupTwoFilePointer == NULL || groupThreeFilePointer == NULL) {
        fprintf(stderr, "fopen failed");
        exit(EXIT_FAILURE);
    }

    long long value;

    if (fscanf(groupOneFilePointer, "%lld", &value) != 1) {
        fprintf(stderr, "Failed to read number\n");
        fclose(groupOneFilePointer);
        exit(EXIT_FAILURE);
    }

    cumulativeSize += value;

    if (fscanf(groupTwoFilePointer, "%lld", &value) != 1) {
        fprintf(stderr, "Failed to read number\n");
        fclose(groupTwoFilePointer);
        exit(EXIT_FAILURE);
    }

    cumulativeSize += value;

    if (fscanf(groupThreeFilePointer, "%lld", &value) != 1) {
        fprintf(stderr, "Failed to read number\n");
        fclose(groupThreeFilePointer);
        exit(EXIT_FAILURE);
    }

    cumulativeSize += value;
    
    fclose(groupOneFilePointer);
    fclose(groupTwoFilePointer);
    fclose(groupThreeFilePointer);

    return cumulativeSize;
}

void getGroupStatistics(FileArray *group, char outputFilename[]){

    createNewEmptyFile(outputFilename); // To save the cumulative size of the group of files

    off_t totalSize = 0; // Cumulative size
        
    int numberOfFiles = group->size;

    struct stat fileStatistics; // struct that stat() uses to save its output   

    for (int i=0; i<numberOfFiles; i++){
                
        char* file = group->files[i];
        if(stat(file, &fileStatistics) < 0) exit(EXIT_FAILURE);
        
        off_t fileSize = fileStatistics.st_size;
        totalSize += fileSize;
    }
    
    appendToFile(outputFilename, totalSize);
}

void createNewEmptyFile(char fileName[]){
    FILE *filePointer;
    filePointer = fopen(fileName, "w");
    if (filePointer == NULL) {
        fprintf(stderr, "Error creating file. Check file permissions or path.\n");
        exit(EXIT_FAILURE);
    }
    fclose(filePointer);
}

void appendToFile(char fileName[], off_t fileSizeToWrite){
    FILE *filePointer;
    filePointer = fopen(fileName, "a");
    if (filePointer == NULL) {
        fprintf(stderr, "Error creating file. Check file permissions or path.\n");
        exit(EXIT_FAILURE);
    }
    fprintf(filePointer, "%lld\n", (long long)fileSizeToWrite);
    fclose(filePointer);
}