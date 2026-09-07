#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/wait.h>

char* check_chaining(char *input) {
    // Gibt Startpointer von Substring zurück 
    char* pos = strstr(input, "&&");

    if (pos == NULL) {
        return NULL;
    }
     // Ersetze durch null byte
     // Speicheradresse von input nun vor && begrenzt durch null byte
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

int handle_builtin(char *args[]) {
    if (strcmp(args[0], "exit") == 0) {
            exit(0);
        }

    if (strcmp(args[0], "cd") == 0) {
        if (args[1] == NULL) {
                fprintf(stderr, "cd: fehlender Pfad\n");
        } else {
            chdir(args[1]);
        }
        return 1;
        }
    return 0;
        
}

int main() {
    char input[256];
    char *args[10];

   /*  char test[] = "ls && echo hello";
    char *second = check_chaining(test);

    printf("Erstens: %s\n", test);
    printf("Zweitens: %s\n", second); */

    while (1) {
        print_prompt();
        
        if (read_input(input, sizeof(input)) == 1) {
            break;
        }
        
        parse_input(input, args);
        
        if (args[0] == NULL) {
            continue;
        }

        if (handle_builtin(args) == 1) {
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
