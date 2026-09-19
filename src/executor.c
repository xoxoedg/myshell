#include "builtins.h"
#include "parser.h"
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/wait.h>
#include <unistd.h>

void clean_pipes_in_child(int child_id, int commands_count, int (*pipes)[2]) {
    for (int j = 0; j < commands_count - 1; j++) {
        if (j != child_id - 1) {
            close(pipes[j][0]);
        }
        if (j != child_id) {
            close(pipes[j][1]);
        }
    }
}

// 0 Erfolg -1 Missefolg
void execute_in_child(char *args[]) {
    execvp(args[0], args);
    perror("execvp fehlgeschlagen");
    exit(1);
}

int run_single_command(char *input, int *was_builtin) {
    char *args[10];
    parse_input(input, args);

    if (args[0] == NULL) {
        return 0;
    }
    int exitcode = handle_builtin(args, was_builtin);
    if (*was_builtin == 1) {
        return exitcode;
    }

    pid_t pid = fork();

    if (pid == 0) {
        execute_in_child(args);
    }

    int status;
    wait(&status);

    if (WIFEXITED(status)) {
        return WEXITSTATUS(status);
    }
    return -1;
}

void close_pipes_in_parent(int commands_count, int (*pipes)[2]) {
    for (int i = 0; i < commands_count - 1; i++) {
        close(pipes[i][0]);
        close(pipes[i][1]);
    }
}

void configurer_pipeline(int i, int commands_count, int (*pipes)[2]) {
    if (i == 0) {
        clean_pipes_in_child(i, commands_count, pipes);
        // Vor dup2
        // fd-Tabelle (dieser Prozess):
        // fd 1 (stdout)     → Open File Description A → Terminal
        // fd pipes[i][1]    → Open File Description B → Pipe-Puffer
        dup2(pipes[i][1], STDOUT_FILENO);
        // Nach dup2
        // fd-Tabelle (dieser Prozess):
        // fd 1 (stdout)     → Open File Description B → Pipe-Puffer   (GEÄNDERT)
        // fd pipes[i][1]    → Open File Description B → Pipe-Puffer   (unverändert, ZEIGT
        // AUF DASSELBE B!)
        close(pipes[i][1]);
    } else if (i == commands_count - 1) {
        clean_pipes_in_child(i, commands_count, pipes);
        dup2(pipes[i - 1][0], STDIN_FILENO);
        close(pipes[i - 1][0]);
    } else {
        clean_pipes_in_child(i, commands_count, pipes);
        dup2(pipes[i][1], STDOUT_FILENO);
        dup2(pipes[i - 1][0], STDIN_FILENO);
        close(pipes[i - 1][0]);
        close(pipes[i][1]);
    }
}

int run_pipeline(char **commands, int commands_count, int *was_builtin) {
    int (*pipes)[2] = malloc((commands_count - 1) * sizeof(int[2]));
    pid_t *pids = malloc(commands_count * sizeof(pid_t));

    if (pipes == NULL) {
        perror("pipe malloc fehlgeschlagen");
        exit(1);
    }

    if (pids == NULL) {
        perror("pid malloc fehlgeschlagen");
        exit(1);
    }

    for (int i = 0; i < commands_count - 1; i++) {
        if (pipe(pipes[i]) == -1) {
            perror("creating pipes failed");
            exit(1);
        }
    }

    for (int i = 0; i < commands_count; i++) {
        pids[i] = fork();
        if (pids[i] == 0) {
            configurer_pipeline(i, commands_count, pipes);

            char *args[10];
            parse_input(commands[i], args);

            if (args[0] == NULL) {
                exit(0);
            }

            int exitcode = handle_builtin(args, was_builtin);
            if (*was_builtin == 1) {
                exit(exitcode);
            }

            execute_in_child(args);
        }
    }

    close_pipes_in_parent(commands_count, pipes);

    int status;
    for (int i = 0; i < commands_count; i++) {
        waitpid(pids[i], &status, 0);
    }

    free(pids);
    free(pipes);

    if (WIFEXITED(status)) {
        return (WEXITSTATUS(status));
    }
    return -1;
}

int execute_commands(char *cmd_str) {
    int was_builtin;
    int count;
    int result;

    char **commands = split_pipes(cmd_str, &count);

    if (count == 1) {
        result = run_single_command(commands[0], &was_builtin);
    } else {
        result = run_pipeline(commands, count, &was_builtin);
    }

    free(commands);
    return result;
}
