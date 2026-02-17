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

// Forward declarations
void displayMenu();
int* getOperands();
void createPipe(int pipes[2]);
void performOperation(int operation);
void validateInput(int* number);

int main (int numberOfArguments, char* argumentVector[]){
    displayMenu();
}

void displayMenu() {
    int totalCalculationsPerformed = 0;
    while (true) {
        printf("---- Parent-Child Calculator ----\n\nParent PID: %d\n\n", getpid());
        printf("Operations Menu:\n1. Addition\n2. Subtraction\n3. Multiplication\n4. Division\n5. Modulo\n6. Maximum\n7. Minimum\n8. Exit\n\n");
        
        int operationChoice;
        printf("Choose operation (1-8): ");
        validateInput(&operationChoice);

        switch(operationChoice){
            case 1:
                performOperation(1);
                totalCalculationsPerformed++;
            break;
            case 2:
                performOperation(2);
                totalCalculationsPerformed++;
            break;
            case 3:
                performOperation(3);
                totalCalculationsPerformed++;
            break;
            case 4:
                performOperation(4);
                totalCalculationsPerformed++;
            break;
            case 5:
                performOperation(5);
                totalCalculationsPerformed++;
            break;
            case 6:
                performOperation(6);
                totalCalculationsPerformed++;
            break;
            case 7:
                performOperation(7);
                totalCalculationsPerformed++;
            break;
            case 8:
                printf("\nTotal calculations performed: %d\n", totalCalculationsPerformed);
                exit(EXIT_SUCCESS);
            break;
        }
    }
}

int* getOperands() {
    int number1;
    printf("Enter first number: ");
    validateInput(&number1);
    int number2;
    printf("Enter second number: ");
    validateInput(&number2);
    int* operandsArray = (int*)malloc(2*sizeof(int));
    operandsArray[0] = number1;
    operandsArray[1] = number2;
    return operandsArray;
}

void createPipe(int pipes[2]) {
    if(pipe(pipes) == -1) {
        fprintf(stderr, "Error: Failed to create pipe\n");
        exit(EXIT_FAILURE);
    }
}

void performOperation(int operation) {
    
    char operationName[50];

    switch(operation){
        case 1:
            strcpy(operationName, "Addition"); 
        break;
        case 2:
            strcpy(operationName, "Subtraction"); 
        break;
        case 3:
            strcpy(operationName, "Multiplication");
        break;
        case 4:
            strcpy(operationName, "Division");
        break;
        case 5:
            strcpy(operationName, "Modulo");
        break;
        case 6:
            strcpy(operationName, "Maximum");
        break;
        case 7:
            strcpy(operationName, "Minimum");
        break;
    }

    int* operandsArray = getOperands();
    printf("\n[Parent] Creating child process for %s\n", operationName);

    int pipes[2];
    createPipe(pipes);

    int result;
    int status;

    pid_t pid = fork();
    
    switch(operation) {
        case 1: // Addition
            int sum;
            if (pid == 0) { // Child process executes this
                printf("[Child PID: %d] calculating %d + %d\n", getpid(), operandsArray[0], operandsArray[1]);
                sum = operandsArray[0]+operandsArray[1];
                printf("[Child PID: %d] Result: %d, exiting...\n", getpid(), sum);
                close(pipes[0]);                    // Close read end of the pipe in child
                write(pipes[1], &sum, sizeof(int));  // Write to pipe
                close(pipes[1]);
                exit(0);
            } else { // Parent process executes this
                close(pipes[1]);                    // Close write end of the pipe in parent
                waitpid(pid, &status, 0);
                printf("[Parent] Child %d completed.\n", pid);
                if (WIFEXITED(status) && WEXITSTATUS(status) == 0) {
                    read(pipes[0], &sum, sizeof(int));   // Parent reads via pipes
                    close(pipes[0]);
                    printf("Result: %d + %d = %d\n\n", operandsArray[0], operandsArray[1], sum);
                    result = sum;
                } else {
                    close(pipes[0]);
                    printf("[Parent] Child process failed. No result to display.\n\n");
                }
            }
        break;
        case 2: // Subtraction
            int difference;
            if (pid == 0) { // Child process executes this
                printf("[Child PID: %d] calculating %d - %d\n", getpid(), operandsArray[0], operandsArray[1]);
                difference = operandsArray[0]-operandsArray[1];
                printf("[Child PID: %d] Result: %d, exiting...\n", getpid(), difference);
                close(pipes[0]);                    // Close read end of the pipe in child
                write(pipes[1], &difference, sizeof(int));  // Write to pipe
                close(pipes[1]);
                exit(0);
            } else { // Parent process executes this
                close(pipes[1]);                    // Close write end of the pipe in parent
                waitpid(pid, &status, 0);
                printf("[Parent] Child %d completed.\n", pid);
                if (WIFEXITED(status) && WEXITSTATUS(status) == 0) {
                    read(pipes[0], &difference, sizeof(int));   // Parent reads via pipes
                    close(pipes[0]);
                    printf("Result: %d - %d = %d\n\n", operandsArray[0], operandsArray[1], difference);
                    result = difference;
                } else {
                    close(pipes[0]);
                    printf("[Parent] Child process failed. No result to display.\n\n");
                }
            }
        break;
        case 3: // Multiplication
            int product;
            if (pid == 0) { // Child process executes this
                printf("[Child PID: %d] calculating %d * %d\n", getpid(), operandsArray[0], operandsArray[1]);
                product = operandsArray[0]*operandsArray[1];
                printf("[Child PID: %d] Result: %d, exiting...\n", getpid(), product);
                close(pipes[0]);                    // Close read end of the pipe in child
                write(pipes[1], &product, sizeof(int));  // Write to pipe
                close(pipes[1]);
                exit(0);
            } else { // Parent process executes this
                close(pipes[1]);                    // Close write end of the pipe in parent
                waitpid(pid, &status, 0);
                printf("[Parent] Child %d completed.\n", pid);
                if (WIFEXITED(status) && WEXITSTATUS(status) == 0) {
                    read(pipes[0], &product, sizeof(int));   // Parent reads via pipes
                    close(pipes[0]);
                    printf("Result: %d * %d = %d\n\n", operandsArray[0], operandsArray[1], product);
                    result = product;
                } else {
                    close(pipes[0]);
                    printf("[Parent] Child process failed. No result to display.\n\n");
                }
            }
        break;
        case 4: // Division
            int quotient;
            if (pid == 0) { // Child process executes this
                if (operandsArray[1] == 0) {
                    fprintf(stderr, "[Child PID: %d] Error: Division by zero\n", getpid());
                    exit(EXIT_FAILURE);
                }
                printf("[Child PID: %d] calculating %d / %d\n", getpid(), operandsArray[0], operandsArray[1]);
                quotient = operandsArray[0]/operandsArray[1];
                printf("[Child PID: %d] Result: %d, exiting...\n", getpid(), quotient);
                close(pipes[0]);                    // Close read end of the pipe in child
                write(pipes[1], &quotient, sizeof(int));  // Write to pipe
                close(pipes[1]);
                exit(0);
            } else { // Parent process executes this
                close(pipes[1]);                    // Close write end of the pipe in parent
                waitpid(pid, &status, 0);
                printf("[Parent] Child %d completed.\n", pid);
                if (WIFEXITED(status) && WEXITSTATUS(status) == 0) {
                    read(pipes[0], &quotient, sizeof(int));   // Parent reads via pipes
                    close(pipes[0]);
                    printf("Result: %d / %d = %d\n\n", operandsArray[0], operandsArray[1], quotient);
                    result = quotient;
                } else {
                    close(pipes[0]);
                    printf("[Parent] Child process failed. No result to display.\n\n");
                }
            }
        break;
        case 5: // Modulo
            int remainder;
            if (pid == 0) { // Child process executes this
                if (operandsArray[1] == 0) {
                    fprintf(stderr, "[Child PID: %d] Error: Modulo by zero\n", getpid());
                    exit(EXIT_FAILURE);
                }
                printf("[Child PID: %d] calculating %d %% %d\n", getpid(), operandsArray[0], operandsArray[1]);
                remainder = operandsArray[0]%operandsArray[1];
                printf("[Child PID: %d] Result: %d, exiting...\n", getpid(), remainder);
                close(pipes[0]);                    // Close read end of the pipe in child
                write(pipes[1], &remainder, sizeof(int));  // Write to pipe
                close(pipes[1]);
                exit(0);
            } else { // Parent process executes this
                close(pipes[1]);                    // Close write end of the pipe in parent
                waitpid(pid, &status, 0);
                printf("[Parent] Child %d completed.\n", pid);
                if (WIFEXITED(status) && WEXITSTATUS(status) == 0) {
                    read(pipes[0], &remainder, sizeof(int));   // Parent reads via pipes
                    close(pipes[0]);
                    printf("Result: %d %% %d = %d\n\n", operandsArray[0], operandsArray[1], remainder);
                    result = remainder;
                } else {
                    close(pipes[0]);
                    printf("[Parent] Child process failed. No result to display.\n\n");
                }
            }
        break;
        case 6: // Maximum
            int max;
            if (pid == 0) { // Child process executes this
                printf("[Child PID: %d] calculating max{%d, %d}\n", getpid(), operandsArray[0], operandsArray[1]);
                max = (operandsArray[0] > operandsArray[1]) ? operandsArray[0] : operandsArray[1];
                printf("[Child PID: %d] Result: %d, exiting...\n", getpid(), max);
                close(pipes[0]);                    // Close read end of the pipe in child
                write(pipes[1], &max, sizeof(int));  // Write to pipe
                close(pipes[1]);
                exit(0);
            } else { // Parent process executes this
                close(pipes[1]);                    // Close write end of the pipe in parent
                waitpid(pid, &status, 0);
                printf("[Parent] Child %d completed.\n", pid);
                if (WIFEXITED(status) && WEXITSTATUS(status) == 0) {
                    read(pipes[0], &max, sizeof(int));   // Parent reads via pipes
                    close(pipes[0]);
                    printf("Result: max{%d, %d} = %d\n\n", operandsArray[0], operandsArray[1], max);
                    result = max;
                } else {
                    close(pipes[0]);
                    printf("[Parent] Child process failed. No result to display.\n\n");
                }
            }
        break;
        case 7: // Minimum
            int min;
            if (pid == 0) { // Child process executes this
                printf("[Child PID: %d] calculating min{%d, %d}\n", getpid(), operandsArray[0], operandsArray[1]);
                min = (operandsArray[0] < operandsArray[1]) ? operandsArray[0] : operandsArray[1];
                printf("[Child PID: %d] Result: %d, exiting...\n", getpid(), min);
                close(pipes[0]);                    // Close read end of the pipe in child
                write(pipes[1], &min, sizeof(int));  // Write to pipe
                close(pipes[1]);
                exit(0);
            } else { // Parent process executes this
                close(pipes[1]);                    // Close write end of the pipe in parent
                waitpid(pid, &status, 0);
                printf("[Parent] Child %d completed.\n", pid);
                if (WIFEXITED(status) && WEXITSTATUS(status) == 0) {
                    read(pipes[0], &min, sizeof(int));   // Parent reads via pipes
                    close(pipes[0]);
                    printf("Result: min{%d, %d} = %d\n\n", operandsArray[0], operandsArray[1], min);
                    result = min;
                } else {
                    close(pipes[0]);
                    printf("[Parent] Child process failed. No result to display.\n\n");
                }
            }
        break;
    }

    free(operandsArray);
}

void validateInput (int* number) {
    if (scanf("%d", number) != 1) { // Validate input
        while (getchar() != '\n'); // Clear input buffer
        fprintf(stderr, "Error: Invalid input. Please enter a number\n");
        exit(EXIT_FAILURE);
    }
}