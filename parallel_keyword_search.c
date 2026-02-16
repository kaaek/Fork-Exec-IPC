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
void searchFile(char *filename, char *keyword, int occurrences);
int countKeywordOccurrences(char *filename, char *keyword);
int countKeywordOccurrences(char *filename, char *keyword);

int main(int numberOfArguments, char* argumentVector[]) {

    printf("---- Parallel Keyword Search ----\nParent Process PID: %d\n\n", getpid());

    char keyword[50];
    printf("Enter keyword to search: ");
    fgets(keyword, sizeof(keyword), stdin); // TODO validate input
    keyword[strcspn(keyword, "\n")] = '\0';  // Remove trailing newline

    int numberOfFiles;
    printf("Enter number of files to search: "); // TODO validate input (should be at least 3)
    scanf("%d", &numberOfFiles);

    char *filenames[numberOfFiles];

    for(int counter = 0; counter < numberOfFiles; counter++){
        char filename[50];
        printf("%d. ", counter + 1);
        fgets(filename, sizeof(filename), stdin);
        filename[strcspn(filename, "\n")] = '\0';  // Remove trailing newline
    }

    int numberOfChildren = numberOfFiles; // There will be as many children as there are files in the argument.
    pid_t childrenPIDs[numberOfChildren];

    printf("[Parent] Creating %d child processes...\n", numberOfChildren); 

    int occurrences[numberOfFiles];

    for(int fileCounter = 0; fileCounter < numberOfFiles; fileCounter++) { // numberOfArguments is at least 1 always, since argumentvector[0] is the binary's name
        char *filename = filenames[fileCounter];
        pid_t pid = fork();
        if (pid == 0) { // Child executes this block
            searchFile(filename, keyword, occurrences[fileCounter]);
        } else { // Parent executes this block.
            childrenPIDs[fileCounter] = pid;
        }
    }
    printf("---- SEARCH RESULTS ----\nKeyword: %s\n\n", keyword);
    waitForChildren(childrenPIDs, numberOfChildren, filenames, occurrences); // the Children can't execute this because they exit(...) from searchFile(...)

}

int waitForChildren(const pid_t *childrenPIDs, const int numberOfChildren, char **filenames, const int occurences[]){
    int filesWithKeyword = 0;
    int filesWithoutKeyword = 0;
    int totalOccurrences = 0;

    printf("Files with keyword:\n");

    for (int pidCounter = 0; pidCounter < numberOfChildren; pidCounter++) {
        int status;
        waitpid(childrenPIDs[pidCounter], &status, 0);

        if (WIFEXITED(status)) {
            int exitCode = WEXITSTATUS(status);
            if (exitCode == 0) {
                filesWithKeyword++;
                char *filename = filenames[pidCounter];
                int occurencesInFile = occurences[pidCounter];
                totalOccurrences += occurencesInFile;
                printf("%s - %d occurrences\n", filename, occurencesInFile);
            }
        }
    }

    printf("Files without keyword:\n");

    for (int pidCounter = 0; pidCounter < numberOfChildren; pidCounter++) {
        int status;
        waitpid(childrenPIDs[pidCounter], &status, 0);

        if (WIFEXITED(status)) {
            int exitCode = WEXITSTATUS(status);
            if (exitCode) {
                filesWithoutKeyword++;
                char *filename = filenames[pidCounter];
                printf("%s - %d occurrences\n", filename, 0);
            }
        }
    }

    printf("Summary:\n\nTotal files searched: %d\n", numberOfChildren);
    printf("Files containing keyword: %d\n", filesWithKeyword);
    printf("Total occurrences: %d\n", totalOccurrences);

    printf("[Parent] Search Completed Successfully.");
}

void searchFile(char *filename, char *keyword, int occurrences) {
    printf("[Child PID: %d] Searching in %s...\n", getpid(), filename);

    occurrences = countKeywordOccurrences(filename, keyword);

    printf("[Child PID: %d] Found %d occurrences in %s\n", getpid(), occurrences, filename);

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