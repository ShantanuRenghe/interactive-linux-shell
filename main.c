#include <stdio.h>
#include <string.h>
#include <stdlib.h>         // exit()
#include <unistd.h>         // fork(), getpid(), exec()
#include <sys/wait.h>       // wait()
#include <signal.h>         // signal()
#include <fcntl.h>          // open(), O_WRONLY, O_CREAT, O_TRUNC

#define MAX_CMD_LEN 1024
#define MAX_ARGS 100

// Parse input based on a delimiter using strsep
void parseInput(char *input, char **commands, const char *delimiter) {
    /*
       This function parses the input string into multiple commands based on the specified delimiter 
       and stores them in the `commands` array using `strsep`.
    */
    int i = 0;
    while ((commands[i++] = strsep(&input, delimiter)) != NULL) {
        // Empty token handling (skip over empty tokens)
        if (commands[i-1][0] == '\0') {
            i--;
        }
    }
    commands[i] = NULL;  // Null-terminate the array
}

void executeCommand(char *command) {
    /*
       This function forks a new process to execute a single command.
       It handles simple commands like `ls`, `pwd`, etc.
    */
    char *args[MAX_ARGS];
    int i = 0;

    // Parse the command into arguments using strsep
    while ((args[i++] = strsep(&command, " ")) != NULL) {
        // Skip over empty tokens (caused by consecutive spaces)
        if (args[i-1][0] == '\0') {
            i--;
        }
    }
    args[i] = NULL;  // Null-terminate the arguments array

    if (args[0] == NULL) {
        return;  // Empty command, return without executing
    }

    pid_t pid = fork();
    if (pid < 0) {
        perror("Fork failed");
        exit(1);
    } else if (pid == 0) {
        // In child process, execute the command
        if (execvp(args[0], args) < 0) {
            // If execvp fails, print an error and exit
            perror("Shell: Incorrect command");
            exit(1);
        }
    } else {
        // In parent process, wait for the child to finish
        wait(NULL);
    }
}

void executeParallelCommands(char *input) {
    /*
       This function handles the execution of multiple commands in parallel (using && as the delimiter).
    */
    char *commands[MAX_ARGS];
    parseInput(input, commands, "&&");  // Parse commands separated by &&

    for (int i = 0; commands[i] != NULL; i++) {
        if (strlen(commands[i]) == 0) {
            // Empty command handling, skip
            continue;
        }
        
        pid_t pid = fork();
        if (pid == 0) {
            // In child process, execute the command
            executeCommand(commands[i]);
            exit(0);
        }
    }

    // In the parent process, wait for all parallel commands to finish
    for (int i = 0; commands[i] != NULL; i++) {
        wait(NULL);
    }
}

void executeSequentialCommands(char *input) {
    /*
       This function handles the execution of multiple commands sequentially (using ## as the delimiter).
    */
    char *commands[MAX_ARGS];
    parseInput(input, commands, "##");  // Parse commands separated by ##

    for (int i = 0; commands[i] != NULL; i++) {
        if (strlen(commands[i]) == 0) {
            // Empty command handling, skip
            continue;
        }
        // Execute each command one after the other
        executeCommand(commands[i]);
    }
}

void executeCommandRedirection(char *input) {
    /*
       This function handles the execution of a single command with output redirection.
       It expects the format: command > outputfile
    */
    char *commands[MAX_ARGS];
    parseInput(input, commands, ">");  // Split the input into command and output file

    if (commands[0] == NULL || commands[1] == NULL) {
        printf("Shell: Incorrect command\n");
        return;
    }

    // Trim spaces from command and output file
    char *command = commands[0];
    char *output_file = commands[1];

    // Fork and execute the command
    pid_t pid = fork();
    if (pid == 0) {
        // Open the output file for writing (create if it doesn't exist, truncate if it does)
        int fd = open(output_file, O_WRONLY | O_CREAT | O_TRUNC, 0644);
        if (fd < 0) {
            perror("Failed to open file");
            exit(1);
        }

        // Redirect stdout to the output file
        dup2(fd, STDOUT_FILENO);
        close(fd);

        // Execute the command
        executeCommand(command);
        exit(0);
    } else {
        // Wait for the child to finish
        wait(NULL);
    }
}

int main() {
    char input[MAX_CMD_LEN];

    while (1) {
        // Print the prompt in the format: currentWorkingDirectory$
        char cwd[1024];
        getcwd(cwd, sizeof(cwd));
        printf("%s$ ", cwd);

        // Accept input using 'getline()'
        if (fgets(input, MAX_CMD_LEN, stdin) == NULL) {
            continue;  // If input is invalid, continue the loop
        }

        // Remove newline character from input
        input[strcspn(input, "\n")] = '\0';

        // Check for exit command
        if (strcmp(input, "exit") == 0) {
            printf("Exiting shell...\n");
            break;
        }

        // Handle empty command (just pressing Enter)
        if (strlen(input) == 0) {
            continue;  // Display prompt again without any errors
        }

        // Handle parallel commands (&&)
        if (strstr(input, "&&") != NULL) {
            executeParallelCommands(input);
        }
        // Handle sequential commands (##)
        else if (strstr(input, "##") != NULL) {
            executeSequentialCommands(input);
        }
        // Handle command redirection (>)
        else if (strstr(input, ">") != NULL) {
            executeCommandRedirection(input);
        }
        // Handle single command
        else {
            executeCommand(input);
        }
    }

    return 0;
}
