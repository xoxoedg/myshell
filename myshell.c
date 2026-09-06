#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/wait.h>

int main() {
    char input[256];
    while (1) {
        printf("myshell> ");
        if (fgets(input, sizeof(input), stdin) == NULL) {
            break;
        }
        // Ersetze \n mit \0
        
        input[strcspn(input, "\n")] = '\0';


        if (strcmp(input, "exit") == 0) {
            break;
        }
        
        pid_t pid = fork();

        if (pid == 0) {
            char *args[] = {"ls", NULL};
            execvp("ls", args);
            perror("execvp fehlgeschlagen");   
            exit(1);                        
        } else {
            wait(NULL);
        }
    }

    return 0;
}
