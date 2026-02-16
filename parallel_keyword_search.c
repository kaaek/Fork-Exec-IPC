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

#define BUFFER_SIZE 256 // Define a suitable buffer size

int waitForChildren(const pid_t *childrenPIDs, const int numberOfChildren, char **filenames, const int occurences[]);
void searchFile(char *filename, char *keyword, int writePipe);
int countKeywordOccurrences(char *filename, char *keyword);

int main(int numberOfArguments, char* argumentVector[]) {

    printf("---- Parallel Keyword Search ----\nParent Process PID: %d\n\n", getpid());

    char keyword[50];
    printf("Enter keyword to search: ");
    fgets(keyword, sizeof(keyword), stdin);
    keyword[strcspn(keyword, "\n")] = '\0';  // Remove trailing newline
    
    if (strlen(keyword) == 0) { // Validate keyword input
        fprintf(stderr, "Error: Keyword cannot be empty\n");
        exit(EXIT_FAILURE);
    }

    int numberOfFiles;
    printf("Enter number of files to search: ");
    
    if (scanf("%d", &numberOfFiles) != 1) { // Validate number of files input
        fprintf(stderr, "Error: Invalid input. Please enter a number\n");
        exit(EXIT_FAILURE);
    }
    
    if (numberOfFiles < 3) {
        fprintf(stderr, "Error: Number of files must be at least 3\n");
        exit(EXIT_FAILURE);
    }
    
    while (getchar() != '\n'); // Clear the input buffer after scanf

    char *filenames[numberOfFiles];

    for(int counter = 0; counter < numberOfFiles; counter++){
        char filename[50];
        printf("%d. ", counter + 1);
        fgets(filename, sizeof(filename), stdin);
        filename[strcspn(filename, "\n")] = '\0';  // Remove trailing newline
        
        // Allocate memory and store the filename
        filenames[counter] = malloc(strlen(filename) + 1);
        if (filenames[counter] == NULL) {
            fprintf(stderr, "Error: Memory allocation failed\n");
            exit(EXIT_FAILURE);
        }
        strcpy(filenames[counter], filename);
    }

    int numberOfChildren = numberOfFiles;   // There will be as many children as there are files in the argument.
    pid_t childrenPIDs[numberOfChildren];
    int pipes[numberOfFiles][2];            // Array of pipes for child-to-parent communication
    int occurrences[numberOfFiles];

    printf("[Parent] Creating %d child processes...\n", numberOfChildren);

    for(int fileCounter = 0; fileCounter < numberOfFiles; fileCounter++) {
        // Create a pipe for this child to communicate back to parent
        if (pipe(pipes[fileCounter]) == -1) {
            fprintf(stderr, "Error: Failed to create pipe\n");
            exit(EXIT_FAILURE);
        }
        
        char *filename = filenames[fileCounter];
        pid_t pid = fork();
        if (pid == 0) { // Child executes this block
            close(pipes[fileCounter][0]);                           // Close read end in child
            searchFile(filename, keyword, pipes[fileCounter][1]);   // Write result to write end in child
        } else { // Parent executes this block.
            close(pipes[fileCounter][1]);  // Close write end in parent
            childrenPIDs[fileCounter] = pid;
        }
    }
    
    for(int fileCounter = 0; fileCounter < numberOfFiles; fileCounter++) {
        read(pipes[fileCounter][0], &occurrences[fileCounter], sizeof(int)); // Parent reads occurrence counts from all children via pipes
        close(pipes[fileCounter][0]);                                        // Close read end after reading
    }
    printf("---- SEARCH RESULTS ----\nKeyword: %s\n\n", keyword);
    waitForChildren(childrenPIDs, numberOfChildren, filenames, occurrences); // the Children can't execute this because they exit(...) from searchFile(...)

    // Free allocated memory
    for (int counter = 0; counter < numberOfFiles; counter++) {
        free(filenames[counter]);
    }

    return 0;
}

int waitForChildren(const pid_t *childrenPIDs, const int numberOfChildren, char **filenames, const int occurences[]){
    int filesWithKeyword = 0;
    int filesWithoutKeyword = 0;
    int totalOccurrences = 0;
    int exitStatuses[numberOfChildren]; // Store exit statuses for all children

    // Wait for all children and store their exit statuses
    for (int pidCounter = 0; pidCounter < numberOfChildren; pidCounter++) {
        int status;
        waitpid(childrenPIDs[pidCounter], &status, 0);
        
        if (WIFEXITED(status)) {
            exitStatuses[pidCounter] = WEXITSTATUS(status);
        } else {
            exitStatuses[pidCounter] = -1; // Abnormal exit
        }
    }

    printf("Files with keyword:\n");

    for (int pidCounter = 0; pidCounter < numberOfChildren; pidCounter++) {
        int exitCode = exitStatuses[pidCounter];
        if (exitCode == 0) {
            filesWithKeyword++;
            char *filename = filenames[pidCounter];
            int occurencesInFile = occurences[pidCounter];
            totalOccurrences += occurencesInFile;
            printf("%s - %d occurrences\n", filename, occurencesInFile);
        }
    }

    printf("Files without keyword:\n");

    for (int pidCounter = 0; pidCounter < numberOfChildren; pidCounter++) {
        int exitCode = exitStatuses[pidCounter];
        if (exitCode == 1) {
            filesWithoutKeyword++;
            char *filename = filenames[pidCounter];
            printf("%s - %d occurrences\n", filename, 0);
        }
    }

    printf("Summary:\n\nTotal files searched: %d\n", numberOfChildren);
    printf("Files containing keyword: %d\n", filesWithKeyword);
    printf("Total occurrences: %d\n", totalOccurrences);

    printf("[Parent] Search Completed Successfully.\n");
}

void searchFile(char *filename, char *keyword, int writePipe) {
    printf("[Child PID: %d] Searching in %s...\n", getpid(), filename);

    int occurrences = countKeywordOccurrences(filename, keyword);

    printf("[Child PID: %d] Found %d occurrences in %s\n", getpid(), occurrences, filename);

    // Write the occurrence count back to parent via pipe
    write(writePipe, &occurrences, sizeof(int));
    close(writePipe);

    if (occurrences > 0) {
        exit(0);
    } else {
        exit(1);
    }
}

int countKeywordOccurrences(char *filename, char *keyword) {
    FILE *filePointer;
    char buffer[BUFFER_SIZE];
    filePointer = fopen(filename, "r");

    if (filePointer == NULL) {
        fprintf(stderr, "[Child PID: %d] Error opening file %s\n", getpid(), filename);
        exit(EXIT_FAILURE);
    }
    
    int occurences = 0;
    int keywordLength = strlen(keyword);

    while (fgets(buffer, BUFFER_SIZE, filePointer) != NULL) {
        char *bufferPointer = buffer;
        while ((bufferPointer = strstr(bufferPointer, keyword)) != NULL) {
            occurences++;
            bufferPointer += keywordLength;
        }
    }

    fclose(filePointer);

    return occurences;
}