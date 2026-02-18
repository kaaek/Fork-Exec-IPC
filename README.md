This directory contains five C programs demonstrating process management and inter-process communication (IPC) concepts.

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

![[Pasted image 20260217202410.png]]

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

![[Pasted image 20260217203007.png]]
![[Pasted image 20260217203026.png]]

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

![[Pasted image 20260217203238.png]]
![[Pasted image 20260217203304.png]]

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

![[Pasted image 20260217204029.png]]

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

![[Pasted image 20260217204116.png]]