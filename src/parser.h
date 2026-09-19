#ifndef PARSER_H
#define PARSER_H

char** split_pipes(char *input, int* commands_count);
char* find_and_split(char *input, int *isAnd);
void parse_input(char *input, char *args[]);

#endif