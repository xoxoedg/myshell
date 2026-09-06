#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/wait.h>

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


int main() {
    char input[256];
    char *args[10];

    while (1) {
        char cwd[256];
        getcwd(cwd, sizeof(cwd));
        printf("%s> ", cwd);
        if (fgets(input, sizeof(input), stdin) == NULL) {
            break;
        }
        // Ersetze \n mit \0
        input[strcspn(input, "\n")] = '\0';
        parse_input(input, args);
        
        if (args[0] == NULL) {
            continue;
        }

        if (strcmp(args[0], "exit") == 0) {
            break;
        }

        if (strcmp(args[0], "cd") == 0) {
            if (args[1] == NULL) {
                fprintf(stderr, "cd: fehlender Pfad\n");
            } else {
                chdir(args[1]);
            }
            continue; 
        }
        
        pid_t pid = fork();

        if (pid == 0) {
            execvp(args[0], args);
            perror("execvp fehlgeschlagen");   
            exit(1);                        
        } else {
            wait(NULL);
        }
    }

    return 0;
}
