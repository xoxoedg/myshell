#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <stdio.h>

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
