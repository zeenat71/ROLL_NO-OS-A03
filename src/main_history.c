#include "shell.h"
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <ctype.h>

#define HISTORY_SIZE 20

// History storage
static char* history[HISTORY_SIZE];
static int history_count = 0;

// Add command to history
void add_to_history(const char* cmd) {
    if(history_count < HISTORY_SIZE) {
        history[history_count++] = strdup(cmd);
    } else {
        free(history[0]);
        for(int i = 1; i < HISTORY_SIZE; i++) {
            history[i-1] = history[i];
        }
        history[HISTORY_SIZE-1] = strdup(cmd);
    }
}

// Print history commands
void print_history() {
    for(int i = 0; i < history_count; i++) {
        printf("%d %s\n", i+1, history[i]);
    }
}

// Get command from history by number
char* get_history_command(int index) {
    if(index < 1 || index > history_count) return NULL;
    return history[index-1];
}

// Built-in commands
int handle_builtin(char** arglist) {
    if(arglist == NULL || arglist[0] == NULL) return 0;

    if(strcmp(arglist[0], "exit") == 0) {
        exit(0);
    } else if(strcmp(arglist[0], "cd") == 0) {
        if(arglist[1] == NULL || chdir(arglist[1]) != 0) {
            perror("cd failed");
        }
        return 1;
    } else if(strcmp(arglist[0], "help") == 0) {
        printf("Built-in commands:\n");
        printf("cd <dir>\nexit\nhelp\njobs\nhistory\n!n (re-execute command)\n");
        return 1;
    } else if(strcmp(arglist[0], "jobs") == 0) {
        printf("Job control not yet implemented\n");
        return 1;
    } else if(strcmp(arglist[0], "history") == 0) {
        print_history();
        return 1;
    }
    return 0;
}

// Main loop
int main() {
    char* cmdline;
    char** arglist;

    while((cmdline = read_cmd(PROMPT, stdin)) != NULL) {

        // Handle !n re-execute
        if(cmdline[0] == '!') {
            int n = atoi(&cmdline[1]);
            char* hist_cmd = get_history_command(n);
            if(hist_cmd) {
                free(cmdline);
                cmdline = strdup(hist_cmd);
                printf("%s\n", cmdline);
            } else {
                printf("No such command in history\n");
                free(cmdline);
                continue;
            }
        }

        // Add to history
        if(cmdline[0] != '\0') add_to_history(cmdline);

        if((arglist = tokenize(cmdline)) != NULL) {

            if(!handle_builtin(arglist)) {
                execute(arglist);
            }

            for(int i = 0; arglist[i] != NULL; i++) free(arglist[i]);
            free(arglist);
        }

        free(cmdline);
    }

    printf("\nShell exited.\n");
    return 0;
}
