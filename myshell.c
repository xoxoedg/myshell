#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/wait.h>
// Return 0 isPipe False
// Return 1 isPipe True
int split_pipe_to_buffers(char *input, char *cmd1, char *cmd2) {
    char* pos = strstr(input, "|");

    if (pos == NULL) {
        strcpy(cmd1, input);
        return 0;
    }

    size_t len1 = pos - input;
    //Kopiere von Anfang des Strings bis len1 Symbole also bis | beginnt
    strncpy(cmd1, input, len1);
    
    cmd1[len1] = '\0';
    // +1 Skip Pipe 
    strcpy(cmd2, pos+1);
    return 1;
}

char* find_and_split(char *input, int *isAnd) {
    // Gibt Startpointer von Substring zurück 
    char* pos = strstr(input, "&&");

    if (pos == NULL) {
        // Falls kein &&, schaue ob || gibt
        pos = strstr(input, "||");
        if (pos == NULL) {
            return NULL;
        } else {
            *isAnd = 0;
        }
    } else {
        *isAnd = 1;
    }
     // Ersetze durch null byte
     // Speicheradresse von input nun vor && / || begrenzt durch null byte
    
    *pos = '\0';
    return pos + 3;
}

void print_prompt() {
    char cwd[256];
    getcwd(cwd, sizeof(cwd));
    printf("%s> ", cwd);
}

void parse_input(char *input, char *args[]) {
    int i = 0;
    char *token = strtok(input, " ");
    while(token != NULL) {
        args[i] = token;
        i++;
        token = strtok(NULL, " ");
    }
    args[i] = NULL;
}

int read_input(char *input, size_t size) {
    if (fgets(input, size, stdin) == NULL) {
            return 1;
        }
    else {                
        // Ersetze \n mit \0
        input[strcspn(input, "\n")] = '\0';
        return 0;
    }
}
// 0 Erfolg 1 Misserfolg

int handle_builtin(char *args[], int* was_builtin) {
    if (strcmp(args[0], "exit") == 0) {
            exit(0);
        }

    if (strcmp(args[0], "cd") == 0) {
        *was_builtin = 1; 
        if (args[1] == NULL) {
                fprintf(stderr, "cd: fehlender Pfad\n");
                return 1;
        } 
        // Man chdir success = 0
        if (chdir(args[1]) != 0) {
            perror("could not change directory");
            return 1;
        }
        return 0;        
        }

    *was_builtin = 0;
    return 0;

}
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


int main() {
    char input[256];
    int isAnd;

    while (1) {
        print_prompt();
        
        if (read_input(input, sizeof(input)) == 1) {
            break;
        }
        char* second_command = find_and_split(input, &isAnd);
        

        int exit_code = execute_commands(input);

        if (second_command != NULL) {
            int is_success = (exit_code == 0);
               if ((isAnd && is_success) || (!isAnd && !is_success)) {
                   execute_commands(second_command); 
                }
            }
        }
    return 0;
}

  

