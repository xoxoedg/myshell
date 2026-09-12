#include <stddef.h>
#include <string.h>
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