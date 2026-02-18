<img width="998" height="904" alt="Pasted image 20260217204029" src="https://github.com/user-attachments/assets/b825c5a1-ea5f-40d2-a57f-bafc5eeb7c80" />This directory contains five C programs demonstrating process management and inter-process communication (IPC) concepts.

---

## Exercise 1: `filesize_analyzer.c`

Analyzes total file sizes in a directory using parallel processing.

**It works by**:

- Scanning the specified directory and collects all regular files (not directories!)
- Distributes files into three groups using round-robin allocation (better than counting the total number of files, dividing by three, etc. It achieves the same result.)
- Spawns three child processes, each responsible for calculating the total size of one group.
- Each child uses the `stat()` system call to get individual file sizes and writes the group total to a dedicated output file. `opendir(), readdir(), closedir()` are used to access directory functions like opening and reading through it.
- The parent process waits for all children to complete, then reads the results from the output files.
- Aggregates the group totals to produce final statistics including total size and identification of the largest group

<img width="1147" height="531" alt="Pasted image 20260217202410" src="https://github.com/user-attachments/assets/aa201c10-ef92-46b2-91f0-0551721af348" />

---

## Exercise 2: `parallel_keyword_search.c`

Searches for a keyword across multiple text files in parallel.

**It works by**:

- Takes user input for a keyword and list of filenames to search
- Creates one pipe using `pipe()` for each file to enable communication between parent and child
- Spawns one child process per file for parallel searching
- Each child opens its assigned file and counts keyword occurrences using string matching (`strstr()`)
- Child writes the count back to parent through its pipe and exits with status 0 (found) or 1 (not found)
- Parent reads occurrence counts from all pipes, then waits for all children to collect exit statuses
- Displays results showing which files contain the keyword and total occurrences

<img width="765" height="664" alt="Pasted image 20260217203007" src="https://github.com/user-attachments/assets/c86ca50a-8a04-454c-a782-8ec0451421df" />
<img width="764" height="153" alt="Pasted image 20260217203026" src="https://github.com/user-attachments/assets/21847e2d-9c65-47ac-a6af-6bd73c5d4992" />

---

## Exercise 3: `parent_child_calculator.c`

Performs arithmetic operations using parent-child process delegation.

**How It Works**:

- Parent  displays a menu and collects user input for operation type and operands
- For each calculation request, parent forks a child process dedicated to performing the selected operation and creates a pipe using `pipe()` for the child to write output.
- Child process calculates the result (addition, subtraction, multiplication, division, modulo, max, or min)
- Child writes the result to the pipe and exits with status 0 on success
- Parent waits for child completion, and reads the result
- Parent validates the child's exit status before displaying the result to the user
- Process repeats in a loop until user selects exit option

<img width="1117" height="912" alt="Pasted image 20260217203238" src="https://github.com/user-attachments/assets/35a6a739-95fa-4458-af16-e80b08b9706e" />
<img width="961" height="973" alt="Pasted image 20260217203304" src="https://github.com/user-attachments/assets/34d94e6b-b314-4951-a503-763d2987e8bc" />

---

## Exercise 4: `string_processor.c`

Applies multiple string transformations concurrently using separate processes.

**How It Works**:

- Parent process reads input string and writes it to a shared file (`input.txt`)
- Creates five child processes, each assigned a different string processing task:
  1. Count vowels and consonants
  2. Convert to uppercase
  3. Reverse the string
  4. Count words
  5. Remove all spaces
- Each child reads from the common input file, performs its specific transformation, and writes results to its own output file for example `output1.txt` this is an alternate to pipe-based IPC.
- Parent waits for all five children to complete execution
- After all children finish, parent reads all output files and displays a formatted results summary

<img width="998" height="904" alt="Pasted image 20260217204029" src="https://github.com/user-attachments/assets/21275d9b-29b2-4ce7-b497-b505072b7d44" />

---

## Exercise 5: `maze_solver.c`

Solves a maze using parallel depth-first search exploration.

**It works by**:

- Starts from the 'S' (start) position in an 8x8 character maze
- At each position, marks the current cell as visited to prevent cycles
- Checks all four adjacent directions (North, South, East, West) for valid moves
- For each valid, unvisited non-wall cell, forks a new child process to explore that direction
- Each child process recursively applies the same exploration strategy
- The first process to reach 'E' (exit) reports success and terminates
- Parent processes wait for their children to complete before returning
- This creates a tree of processes exploring all possible paths simultaneously

<img width="1077" height="999" alt="Pasted image 20260217204116" src="https://github.com/user-attachments/assets/da7731bd-1322-4114-8997-5b014f40c036" />
