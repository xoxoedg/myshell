#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
// Return 0 isPipe False
// Return 1 isPipe True

char *l_strip(char *token) {
  int count = 0;
  while (token[count] == ' ') {
    count++;
  }
  return token + count;
}

char *r_strip(char *token) {
  int length = strlen(token);
  int last_char_index = length - 1;
  while (last_char_index >= 0 && token[last_char_index] == ' ') {
    last_char_index--;
  }
  token[last_char_index + 1] = '\0';
  return token;
}

char *strip(char *token) { return r_strip(l_strip(token)); }

char **split_pipes(char *input, int *commands_count) {
  int capacity = 4;
  int count = 0;
  char **commands = malloc(capacity * sizeof(char *));
  if (commands == NULL) {
    perror("malloc fehlgeschlagen");
    exit(1);
  }

  char *token = strtok(input, "|");
  while (token != NULL) {
    if (count >= capacity) {
      capacity *= 2;
      commands = realloc(commands, capacity * sizeof(char *));
      if (commands == NULL) {
        perror("realloc fehlgeschlagen");
        exit(1);
      }
    }
            commands[count] = strip(token);
    count++;
    token = strtok(NULL, "|");
  }
  *commands_count = count;
  return commands;
}

char *find_and_split(char *input, int *isAnd) {
  // Gibt Startpointer von Substring zurück
  char *pos = strstr(input, "&&");

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
  while (token != NULL) {
    args[i] = token;
    i++;
    token = strtok(NULL, " ");
  }
  args[i] = NULL;
}
