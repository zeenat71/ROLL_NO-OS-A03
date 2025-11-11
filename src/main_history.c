#include "shell.h"
#include <ctype.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <unistd.h>

// Built-in commands handler
int handle_builtin(char** arglist) {
    if (arglist[0] == NULL) return 0;

    // exit command
    if (strcmp(arglist[0], "exit") == 0) {
        exit(0);
    } 
    // cd command
    else if (strcmp(arglist[0], "cd") == 0) {
        if (arglist[1] == NULL) {
            fprintf(stderr, "cd: missing argument\n");
        } else if (chdir(arglist[1]) != 0) {
            perror("cd");
        }
        return 1;
    } 
    // help command
    else if (strcmp(arglist[0], "help") == 0) {
        printf("Built-in commands:\n");
        printf("exit    - exit the shell\n");
        printf("cd DIR  - change directory to DIR\n");
        printf("help    - show this help message\n");
        printf("history - show last %d commands\n", HISTORY_SIZE);
        printf("!n      - execute the nth command from history\n");
        return 1;
    } 
    // history command
    else if (strcmp(arglist[0], "history") == 0) {
        print_history();
        return 1;
    }

    return 0; // not a builtin
}

// Main shell loop
int main() {
    char* cmdline;
    char** arglist;

    while ((cmdline = read_cmd(PROMPT, stdin)) != NULL) {

        // Handle !n command
        if (cmdline[0] == '!') {
            int n = atoi(cmdline + 1);
            char* hist_cmd = get_history_command(n);
            if (hist_cmd == NULL) {
                printf("No such command in history.\n");
                free(cmdline);
                continue;
            }
            free(cmdline);
            cmdline = strdup(hist_cmd);
            printf("%s\n", cmdline); // show the command being executed
        }

        // Add to history
        add_to_history(cmdline);

        // Tokenize command
        if ((arglist = tokenize(cmdline)) != NULL) {

            // Check built-ins
            if (!handle_builtin(arglist)) {
                // Execute external command
                execute(arglist);
            }

            // Free tokenized args
            for (int i = 0; arglist[i] != NULL; i++)
                free(arglist[i]);
            free(arglist);
        }

        free(cmdline);
    }

    printf("\nShell exited.\n");
    return 0;
}
