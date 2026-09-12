#include <stdio.h>
#include <unistd.h>  
#include "parser.h"
#include "executor.h"
#include <string.h>


void print_prompt() {
    char cwd[256];
    getcwd(cwd, sizeof(cwd));
    printf("%s> ", cwd);
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

  

