#include <unistd.h>
#include <sys/wait.h>
#include <stdio.h>
#include "parser.h"
#include "builtins.h"
#include <stdlib.h>

// 0 Erfolg -1 Missefolg
int run_single_command(char* input, int* was_builtin) {
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
        execvp(args[0], args);
        perror("execvp fehlgeschlagen");
        exit(1);
    }

    int status;
    wait(&status);

    if (WIFEXITED(status) ) {
        return WEXITSTATUS(status);
    }
    return -1;
} 

int run_pipeline(char* cmd1_buf, char* cmd2_buf, int* was_builtin) {
    int pipefd[2];
    pid_t pid1, pid2;
    char* args[10];
    char* args2[10];


    if (pipe(pipefd) == -1) {
        perror("pipe fehlgeschlagen");
        return -1;
    }

    // --------------------------------------------------
    // KIND 1: Führt den ersten Befehl aus (schreibt in Pipe)
    // --------------------------------------------------
    pid1 = fork();

    if (pid1==0) {
        // Lese Ende schließen
        close(pipefd[0]);
        // STDOUT FD bleibt auf 1. Nur wohin dieser pointed (file) wird verändert
        dup2(pipefd[1], STDOUT_FILENO);
        close(pipefd[1]);
        parse_input(cmd1_buf, args);

        if (args[0] == 0) {
            exit(0);
        }

        int exit_code = handle_builtin(args, was_builtin);
        if (*was_builtin) {
            exit(exit_code);
        }

        execvp(args[0], args);
        perror("execvp fehlgeschlagen");
        exit(1);
    }
    // Wieder Elternprozess
    pid2 = fork();

    if (pid2 == 0) {
        close(pipefd[1]);
        dup2(pipefd[0], STDIN_FILENO);
        close(pipefd[0]);
        parse_input(cmd2_buf, args2);

        if (args2[0] == 0) {
            exit(0);
        }

        int exit_code = handle_builtin(args2, was_builtin);
        if (*was_builtin) {
            exit(exit_code);
        }

        execvp(args2[0], args2);
        perror("execvp fehlgeschlagen");
        exit(1);

    }

    close(pipefd[0]);
    close(pipefd[1]);

    int status1, status2;
    waitpid(pid1, &status1, 0);
    waitpid(pid2, &status2, 0);

    return WIFEXITED(status2) ? WEXITSTATUS(status2) : -1;
 
}

int execute_commands(char *cmd_str) {
    char cmd1_buf[256];
    char cmd2_buf[256];
    int was_builtin;

    int is_pipe = split_pipe_to_buffers(cmd_str, cmd1_buf, cmd2_buf);

    if(is_pipe == 1) {
        return run_pipeline(cmd1_buf, cmd2_buf, &was_builtin);
    } else {
        return run_single_command(cmd1_buf, &was_builtin);
    }
}
