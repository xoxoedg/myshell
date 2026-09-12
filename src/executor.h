#ifndef EXECUTOR_h
#define EXECUTOR_h

int run_single_command(char* input, int* was_builtin);
int run_pipeline(char* cmd1_buf, char* cmd2_buf, int* was_builtin);
int execute_commands(char *cmd_str);

#endif