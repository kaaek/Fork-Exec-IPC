#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>
#include <sys/types.h>

#define N 8

typedef struct {
    int x;
    int y;
} Coordinate;

char maze[N][N] = {
    {'#','#','#','#','#','#','#','#'},
    {'#','S','.','.','#','.','.','#'},
    {'#','#','#','.','#','.','#','#'},
    {'#','.','.','.','.','.','#','#'},
    {'#','.','#','#','#','.','#','#'},
    {'#','.','.','.','#','.','.','#'},
    {'#','#','#','.','.','.','.','#'},
    {'#','#','#','#','E','#','#','#'}
};

int maze_debug = 1;
int visited[N][N] = {0};

int search(Coordinate c) {
    // Mark current position as visited
    visited[c.y][c.x] = 1;
    
    // Print current location and PID
    pid_t pid = getpid();
    printf("PID %d visiting (%d,%d)\n", pid, c.x, c.y);
    
    // Check if we found the exit
    if (maze[c.y][c.x] == 'E') {
        printf("PID %d FOUND EXIT at (%d,%d)\n", pid, c.x, c.y);
        exit(0);
    }
    
    // Explore all four directions: North, South, East, West
    // North: (x, y-1), South: (x, y+1), East: (x+1, y), West: (x-1, y)
    int dx[] = {0, 0, 1, -1};  // x offsets for E, W, S, N
    int dy[] = {-1, 1, 0, 0};  // y offsets for N, S, E, W
    
    for (int i = 0; i < 4; i++) {
        int newX = c.x + dx[i];
        int newY = c.y + dy[i];
        
        // Check if new position is valid
        if (newX >= 0 && newX < N && newY >= 0 && newY < N) {
            // Check if it's not a wall and not visited
            if (maze[newY][newX] != '#' && !visited[newY][newX]) {
                // Fork a child process to explore this direction
                pid_t childPID = fork();
                
                if (childPID < 0) {
                    fprintf(stderr, "Fork failed");
                    exit(EXIT_FAILURE);
                } else if (childPID == 0) { // Child process (explore this direction)
                    Coordinate next = {newX, newY};
                    search(next);
                    exit(EXIT_SUCCESS);
                }
                // Parent continues to check other directions
            }
        }
    }
    
    // Wait for all children to complete
    while (wait(NULL) > 0);
    
    return 0;
}

int main(void) {
    Coordinate start = {1, 1};
    search(start);
    return 0;
}
