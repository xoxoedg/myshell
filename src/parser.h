#ifndef PARSER_H
#define PARSER_H

int split_pipe_to_buffers(char *input, char *cmd1, char *cmd2);
char* find_and_split(char *input, int *isAnd);
void parse_input(char *input, char *args[]);

#endif