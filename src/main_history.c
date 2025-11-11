#include "shell.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#define HISTORY_SIZE 20

char* history[HISTORY_SIZE];
int history_count = 0;

void add_to_history(const char* cmd) {
    if (history_count < HISTORY_SIZE) {
        history[history_count++] = strdup(cmd);
    } else {
        free(history[0]);
        for (int i = 1; i < HISTORY_SIZE; i++)
            history[i-1] = history[i];
        history[HISTORY_SIZE-1] = strdup(cmd);
    }
}

void print_history() {
    for (int i = 0; i < history_count; i++)
        printf("%d %s\n", i+1, history[i]);
}

char* get_history_command(int index) {
    if (index < 1 || index > history_count) return NULL;
    return history[index-1];
}

int handle_builtin(char** arglist) {
    if (arglist[0] == NULL) return 0;

    if (strcmp(arglist[0], "exit") == 0) {
        exit(0);
    } else if (strcmp(arglist[0], "cd") == 0) {
        if (arglist[1] == NULL || chdir(arglist[1]) != 0)
            perror("cd failed");
        return 1;
    } else if (strcmp(arglist[0], "help") == 0) {
        printf("Built-in commands: exit, cd, help, jobs, history, !n\n");
        return 1;
    } else if (strcmp(arglist[0], "jobs") == 0) {
        printf("Job control not yet implemented.\n");
        return 1;
    } else if (strcmp(arglist[0], "history") == 0) {
        print_history();
        return 1;
    }
    return 0;
}

int execute(char* arglist[]) {
    int status;
    int cpid = fork();
    switch (cpid) {
        case -1:
            perror("fork failed");
            exit(1);
        case 0:
            execvp(arglist[0], arglist);
            perror("Command not found");
            exit(1);
        default:
            waitpid(cpid, &status, 0);
            return 0;
    }
}

char* read_cmd(char* prompt, FILE* fp) {
    printf("%s", prompt);
    char* cmdline = malloc(MAX_LEN);
    int c, pos = 0;

    while ((c = getc(fp)) != EOF) {
        if (c == '\n') break;
        cmdline[pos++] = c;
    }
    if (c == EOF && pos == 0) {
        free(cmdline);
        return NULL;
    }
    cmdline[pos] = '\0';
    return cmdline;
}

char** tokenize(char* cmdline) {
    if (cmdline == NULL || cmdline[0] == '\0' || cmdline[0] == '\n') return NULL;

    char** arglist = malloc(sizeof(char*) * (MAXARGS + 1));
    for (int i = 0; i < MAXARGS+1; i++) {
        arglist[i] = malloc(ARGLEN);
        bzero(arglist[i], ARGLEN);
    }

    char* cp = cmdline;
    char* start;
    int len;
    int argnum = 0;

    while (*cp != '\0' && argnum < MAXARGS) {
        while (*cp == ' ' || *cp == '\t') cp++;
        if (*cp == '\0') break;

        start = cp;
        len = 1;
        while (*++cp != '\0' && !(*cp==' '||*cp=='\t')) len++;
        strncpy(arglist[argnum], start, len);
        arglist[argnum][len] = '\0';
        argnum++;
    }
    if (argnum == 0) {
        for (int i = 0; i < MAXARGS+1; i++) free(arglist[i]);
        free(arglist);
        return NULL;
    }
    arglist[argnum] = NULL;
    return arglist;
}

int main() {
    char* cmdline;
    char** arglist;

    while ((cmdline = read_cmd(PROMPT, stdin)) != NULL) {

        // handle !n commands
        if (cmdline[0] == '!') {
            int n = atoi(&cmdline[1]);
            char* history_cmd = get_history_command(n);
            if (history_cmd == NULL) {
                printf("No such command in history\n");
                free(cmdline);
                continue;
            } else {
                free(cmdline);
                cmdline = strdup(history_cmd);
            }
        }

        add_to_history(cmdline);

        if ((arglist = tokenize(cmdline)) != NULL) {
            if (!handle_builtin(arglist)) {
                execute(arglist);
            }

            for (int i=0; arglist[i]!=NULL; i++) free(arglist[i]);
            free(arglist);
        }
        free(cmdline);
    }
    printf("\nShell exited.\n");
    return 0;
}
