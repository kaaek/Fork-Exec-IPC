#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <ctype.h>

#define MAX_STRING_LENGTH 100
#define INPUT_FILE "input.txt"
#define OUTPUT_FILE_1 "child1_output.txt"
#define OUTPUT_FILE_2 "child2_output.txt"
#define OUTPUT_FILE_3 "child3_output.txt"
#define OUTPUT_FILE_4 "child4_output.txt"
#define OUTPUT_FILE_5 "child5_output.txt"

// Function prototypes
void countVowelsConsonants(char *str, char *outputFile);
void convertToUppercase(char *str, char *outputFile);
void reverseString(char *str, char *outputFile);
void countWords(char *str, char *outputFile);
void removeSpaces(char *str, char *outputFile);
void displayResults();

/**
 * Counts vowels and consonants in the input string and writes results to output file.
 */
void countVowelsConsonants(char *string, char *outputFile) {
    int vowels = 0, consonants = 0;
    
    for (int i = 0; string[i] != '\0'; i++) { // Looping over non-end-of-string characters
        char character = tolower(string[i]);
        if (isalpha(character)) {
            if (character == 'a' || character == 'e' || character == 'i' || character == 'o' || character == 'u') {
                vowels++;
            } else {
                consonants++;
            }
        }
    }
    
    FILE *filePointer = fopen(outputFile, "w");
    if (filePointer == NULL) {
        fprintf(stderr, "Error opening output file");
        exit(EXIT_FAILURE);
    }
    
    fprintf(filePointer, "Vowels: %d\n", vowels);
    fprintf(filePointer, "Consonants: %d\n", consonants);
    
    fclose(filePointer);
}

/**
 * Converts the input string to uppercase and writes result to output file.
 */
void convertToUppercase(char *string, char *outputFile) {
    char result[MAX_STRING_LENGTH + 1];
    
    for (int i = 0; string[i] != '\0'; i++) {
        result[i] = toupper(string[i]);
    }
    result[strlen(string)] = '\0';
    
    FILE *filePointer = fopen(outputFile, "w");
    if (filePointer == NULL) {
        fprintf(stderr, "Error opening output file");
        exit(EXIT_FAILURE);
    }
    
    fprintf(filePointer, "%s\n", result);
    
    fclose(filePointer);
}

/**
 * Reverses the input string and writes result to output file.
 */
void reverseString(char *string, char *outputFile) {
    int stringLength = strlen(string);
    char result[MAX_STRING_LENGTH + 1];
    
    for (int i = 0; i < stringLength; i++) {
        result[i] = string[stringLength - 1 - i];
    }
    result[stringLength] = '\0'; // end-of-string character
    
    FILE *filePointer = fopen(outputFile, "w");
    if (filePointer == NULL) {
        perror("Error opening output file");
        exit(EXIT_FAILURE);
    }
    
    fprintf(filePointer, "%s\n", result);
    
    fclose(filePointer);
}

/**
 * Counts the number of words in the input string and writes result to output file.
 */
void countWords(char *string, char *outputFile) {
    int wordCount = 0;
    int inWord = 0; // A toggle for reading words/not reading words
    
    for (int i = 0; string[i] != '\0'; i++) {
        if (string[i] != ' ') {
            if (!inWord) {
                wordCount++;
                inWord = 1;
            }
        } else {
            inWord = 0;
        }
    }
    
    FILE *filePointer = fopen(outputFile, "w");
    if (filePointer == NULL) {
        fprintf(stderr, "Error opening output file");
        exit(EXIT_FAILURE);
    }
    
    fprintf(filePointer, "Number of words: %d\n", wordCount);
    
    fclose(filePointer);
}

/**
 * Removes all spaces from the input string and writes result to output file.
 */
void removeSpaces(char *string, char *outputFile) {
    char result[MAX_STRING_LENGTH + 1];
    int j = 0;
    
    for (int i = 0; string[i] != '\0'; i++) {
        if (string[i] != ' ') { // If the current character is not a space, add to the result string.
            result[j++] = string[i];
        }
    }
    result[j] = '\0'; // end-of-string character.
    
    FILE *filePointer = fopen(outputFile, "w");
    if (filePointer == NULL) {
        fprintf(stderr, "Error opening output file");
        exit(EXIT_FAILURE);
    }
    
    fprintf(filePointer, "%s\n", result);
    
    fclose(filePointer);
}

/**
 * Reads and displays all results from output files.
 */
void displayResults() {
    char line[MAX_STRING_LENGTH + 50];
    char originalString[MAX_STRING_LENGTH + 1];
    
    // Read original string from input file
    FILE *inputFilePointer = fopen(INPUT_FILE, "r");
    if (inputFilePointer == NULL) {
        perror("Error opening input file");
        return;
    }
    fgets(originalString, sizeof(originalString), inputFilePointer);
    // Remove trailing newline if present
    originalString[strcspn(originalString, "\n")] = '\0';
    fclose(inputFilePointer);
    
    printf("\n---- RESULTS ----\n");
    printf("Original String:\n");
    printf("\"%s\"\n\n", originalString);
    
    printf("Child 1 - Vowels & Consonants:\n");
    FILE *filePointer1 = fopen(OUTPUT_FILE_1, "r");
    if (filePointer1) {
        while (fgets(line, sizeof(line), filePointer1)) {
            printf("%s", line);
        }
        fclose(filePointer1);
    }
    printf("\n");
    
    printf("Child 2 - Uppercase:\n");
    FILE *filePointer2 = fopen(OUTPUT_FILE_2, "r");
    if (filePointer2) {
        fgets(line, sizeof(line), filePointer2);
        line[strcspn(line, "\n")] = '\0';
        printf("\"%s\"\n", line);
        fclose(filePointer2);
    }
    printf("\n");
    
    printf("Child 3 - Reversed:\n");
    FILE *filePointer3 = fopen(OUTPUT_FILE_3, "r");
    if (filePointer3) {
        fgets(line, sizeof(line), filePointer3);
        line[strcspn(line, "\n")] = '\0';
        printf("\"%s\"\n", line);
        fclose(filePointer3);
    }
    printf("\n");
    
    printf("Child 4 - Word Count:\n");
    FILE *filePointer4 = fopen(OUTPUT_FILE_4, "r");
    if (filePointer4) {
        while (fgets(line, sizeof(line), filePointer4)) {
            printf("%s", line);
        }
        fclose(filePointer4);
    }
    printf("\n");
    
    // Child 5 - No Spaces
    printf("Child 5 - No Spaces:\n");
    FILE *filePointer5 = fopen(OUTPUT_FILE_5, "r");
    if (filePointer5) {
        fgets(line, sizeof(line), filePointer5);
        line[strcspn(line, "\n")] = '\0';
        printf("\"%s\"\n", line);
        fclose(filePointer5);
    }
    printf("\n");
}

int main() {
    char inputString[MAX_STRING_LENGTH + 1];
    pid_t childPIDs[5];
    int childrenSpawned = 0;
    
    printf("---- Process-Based String Processor ----\n");
    printf("Parent Process PID: %d\n", getpid());
    
    // Read input string from user
    printf("Enter a string: ");
    fgets(inputString, sizeof(inputString), stdin);
    inputString[strcspn(inputString, "\n")] = '\0'; // Remove trailing newline
    
    // Write input to file
    FILE *inputFilePointer = fopen(INPUT_FILE, "w");
    if (inputFilePointer == NULL) {
        fprintf(stderr, "Error creating input file");
        return 1;
    }
    fprintf(inputFilePointer, "%s", inputString);
    fclose(inputFilePointer);
    
    printf("[Parent] Input saved to %s\n", INPUT_FILE);
    printf("[Parent] Creating 5 child processes...\n");
    
    for (int i = 0; i < 5; i++) {
        pid_t pid = fork();
        
        if (pid < 0) {
            perror("Fork failed");
            exit(EXIT_FAILURE);
        } else if (pid == 0) { // Child process
            char string[MAX_STRING_LENGTH + 1];
            
            // Read input from file
            FILE *filePointer = fopen(INPUT_FILE, "r");
            if (filePointer == NULL) {
                perror("Child: Error opening input file");
                exit(1);
            }
            fgets(string, sizeof(string), filePointer);
            string[strcspn(string, "\n")] = '\0';
            fclose(filePointer);
            
            // Perform specific task based on child number
            switch (i) {
                case 0:
                    printf("[Child 1, PID: %d] Counting vowels and consonants...\n", getpid());
                    countVowelsConsonants(string, OUTPUT_FILE_1);
                    printf("[Child 1, PID: %d] Completed! Result written to child1_output.txt\n", getpid());
                    break;
                case 1:
                    printf("[Child 2, PID: %d] Converting to uppercase...\n", getpid());
                    convertToUppercase(string, OUTPUT_FILE_2);
                    printf("[Child 2, PID: %d] Completed! Result written to child2_output.txt\n", getpid());
                    break;
                case 2:
                    printf("[Child 3, PID: %d] Reversing string...\n", getpid());
                    reverseString(string, OUTPUT_FILE_3);
                    printf("[Child 3, PID: %d] Completed! Result written to child3_output.txt\n", getpid());
                    break;
                case 3:
                    printf("[Child 4, PID: %d] Counting words...\n", getpid());
                    countWords(string, OUTPUT_FILE_4);
                    printf("[Child 4, PID: %d] Completed! Result written to child4_output.txt\n", getpid());
                    break;
                case 4:
                    printf("[Child 5, PID: %d] Removing spaces...\n", getpid());
                    removeSpaces(string, OUTPUT_FILE_5);
                    printf("[Child 5, PID: %d] Completed! Result written to child5_output.txt\n", getpid());
                    break;
            }
            
            exit(EXIT_SUCCESS);
        } else {
            // Parent process
            childPIDs[i] = pid;
            childrenSpawned++;
        }
    }
    
    // Wait for all child processes to complete
    for (int i = 0; i < 5; i++) {
        waitpid(childPIDs[i], NULL, 0);
    }
    
    printf("[Parent] All children completed. Reading results...\n");
    
    displayResults();
    
    printf("[Parent] Process completed successfully.\n");
    printf("Total children spawned: %d\n", childrenSpawned);
    
    return 0;
}
