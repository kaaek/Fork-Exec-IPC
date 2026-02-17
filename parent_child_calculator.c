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

int main (int numberOfArguments, int argumentVector[]){
    displayMenu();
}

void displayMenu() {
    int totalCalculationsPerformed = 0;
    while (true) {
        printf("---- Parent-Child Calculator ----\n\nParent PID: %d\n\n", getpid());
        printf("Operations Menu:\n1. Addition\n2. Subtraction\n3. Multiplication\n4. Division\n5. Modulo\n6. Maximum\n7. Minimum\n8. Exit\n\n");
        
        int operationChoice;
        printf("Choose operation (1-8): ");
        validateInput(operationChoice);

        switch(operationChoice){
            case 1:
                additionRoutine();
                totalCalculationsPerformed++;
            break;
            case 2:
                subtractionRoutine();
                totalCalculationsPerformed++;
            break;
            case 3:
                multiplicationRoutine();
                totalCalculationsPerformed++;
            break;
            case 4:
                divisionRoutine();
                totalCalculationsPerformed++;
            break;
            case 5:
                moduloRoutine();
                totalCalculationsPerformed++;
            break;
            case 6:
                maximumRoutine();
                totalCalculationsPerformed++;
            break;
            case 7:
                minimumRoutine();
                totalCalculationsPerformed++;
            break;
            case 8:
                printf("Total calculations performed: %d", totalCalculationsPerformed);
                exit(EXIT_SUCCESS);
            break;
        }
    }
}

int* getOperands() {
    int number1;
    printf("Enter first number: ");
    validateInput(number1);
    int number2;
    printf("Enter second number: ");
    validateInput(number2);
    int* operandsArray = (int*)malloc(2*sizeof(int));
    operandsArray[0] = number1;
    operandsArray[1] = number2;
    return operandsArray;
}

void additionRoutine() {

    int* operandsArray = getOperands();
    printf("[Parent] Creating child process for Addition");

    int pipes[1][2]; // One pipe (one child) with two ends
    if (pipe(pipes[0]) == -1) {
        fprintf(stderr, "Error: Failed to create pipe\n");
        exit(EXIT_FAILURE);
    }

    int sum;

    pid_t pid = fork();
    if (pid) { // Child process executes this
        printf("[Child PID: %d] calculating %d + %d", getpid(), operandsArray[0], operandsArray[1]);
        close(pipes[0][0]);             // Close read end of the pipe in child
        int writePipe = pipes[0][1];    // Write end of the pipe in child
        sum = operandsArray[0]+operandsArray[1];
        printf("[Child PID: %d] Result: %d, exiting...", getpid(), sum);
        write(writePipe, sum, sizeof(int)); // Write to pipe
    } else { // Parent process executes this
        waitpid(pid, NULL, 0);
        printf("[Parent] Child %d completed.", pid);
        read(pipes[0][0], sum, sizeof(int));    // Parent reads via pipes
        close(pipes[0][0]);                     // Close read end after reading
        printf("Result: %d + %d = %d", operandsArray[0], operandsArray[1], sum);
    }
}

void validateInput (int number) {
    if (scanf("%d", number) != 1) { // Validate input
        fprintf(stderr, "Error: Invalid input. Please enter a number\n");
        exit(EXIT_FAILURE);
    }
}