# Interactive Linux Shell

## Overview
The Interactive Linux Shell is a C program that emulates a basic command-line shell environment. It supports running standard Linux commands, executing multiple commands either sequentially or in parallel, and redirecting output to files. The shell displays the current working directory in its prompt, providing a familiar and intuitive interface.

## Features
- **Single Command Execution:**
  - Supports running basic commands like `ls`, `pwd`, `echo`, etc.
  - Parses commands and arguments using a simple tokenizer.

- **Multiple Commands:**
  - Sequential Execution (##): Runs commands one after another.
  - Parallel Execution (&&): Runs commands simultaneously.

- **Output Redirection (>):**
  - Redirects the output of a command to a specified file.
  - Automatically creates the file if it doesn't exist and overwrites if it does.
  
- **Built-in Exit Command:**
  - Type exit to terminate the shell gracefully.
 
- **Prompt with Current Working Directory:**
  - The shell prompt dynamically displays the current working directory.

- **System Call Integration:**
  - Uses `fork()`, `execvp()`, `wait()`, `dup2()`, and `open()` to execute and manage commands.

## Key Components
- **Command Parsing:**
  - `parseInput()` splits input strings into commands based on specified delimiters.
  - Handles empty tokens and trims unnecessary whitespace.

- **Execution Functions:**
  - `executeCommand()` runs a single command in a child process.
  - `executeSequentialCommands()` runs commands separated by ## in sequence.
  - `executeParallelCommands()` runs commands separated by && in parallel.
  - `executeCommandRedirection()` handles output redirection using >.

- **Process Management:**
  - Commands are executed in forked processes to avoid blocking the shell.
  - Parent process waits for child processes when required.

## Usage
1. **Compilation:**
   - Compile the program using a C compiler (e.g., gcc).
     ```bash
     gcc main.c -o shell
     ```

2. **Execution:**
   - Run the compiled executable.
     ```bash
     ./shell
     ```

3. **Example Commands:**
   - Run the following commands one by one.
     ```bash
     ls
     pwd
     ls ## pwd
     ls && pwd
     ls > files.txt
     exit
     ```

## Notes
- The shell assumes valid Linux commands are entered.
- Output redirection currently supports overwriting files (`O_TRUNC`).
- Parallel and sequential execution delimiters (&&, ##) must be typed exactly with no surrounding quotes.
- This shell is for learning purposes and does not implement advanced features like piping, background jobs, or environment variable expansion.



Contributions to improve the Interactive Linux Shell are welcome! Bug fixes, new features, and optimizations are appreciated.
