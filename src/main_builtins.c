#include "shell.h"

// Function to handle built-in commands
int handle_builtin(char** arglist) {
    if (arglist == NULL || arglist[0] == NULL) return 0;

    // exit command
    if (strcmp(arglist[0], "exit") == 0) {
        printf("Exiting shell...\n");
        exit(0);
    }

    // cd command
    if (strcmp(arglist[0], "cd") == 0) {
        if (arglist[1] == NULL) {
            fprintf(stderr, "cd: missing argument\n");
        } else if (chdir(arglist[1]) != 0) {
            perror("cd failed");
        }
        return 1;
    }

    // help command
    if (strcmp(arglist[0], "help") == 0) {
        printf("Built-in commands:\n");
        printf("cd <directory> : Change directory\n");
        printf("exit           : Exit the shell\n");
        printf("help           : Show this help message\n");
        printf("jobs           : Show jobs (not yet implemented)\n");
        return 1;
    }

    // jobs command
    if (strcmp(arglist[0], "jobs") == 0) {
        printf("Job control not yet implemented.\n");
        return 1;
    }

    return 0; // Not a built-in command
}

// Main shell loop
int main() {
    char* cmdline;
    char** arglist;

    while ((cmdline = read_cmd(PROMPT, stdin)) != NULL) {
        if ((arglist = tokenize(cmdline)) != NULL) {

            // Handle built-ins before creating child process
            if (!handle_builtin(arglist)) {
                execute(arglist);
            }

            // Free memory allocated by tokenize()
            for (int i = 0; arglist[i] != NULL; i++) {
                free(arglist[i]);
            }
            free(arglist);
        }
        free(cmdline);
    }

    printf("\nShell exited.\n");
    return 0;
}

