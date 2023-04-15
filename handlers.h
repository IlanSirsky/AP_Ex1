#include "types.h"

void ctrl_c_handler(int dummy);
void handle_arrows(char *token);
void check_quit(char *token);
void handle_exit_status(Commands *list, int status);
int handle_cd_command(char *token1, char *token2);
void handle_variables(Commands *root, int pipes_num);
void handle_read_command();
int handle_ampersand_command(Commands *current, int args_count);
int handle_redirection(int args_count, Commands *current);
void change_prompt(char *token1, char *token2, char *token3);
void execute_redirection(int redirect, int pipes_num);

int handle_if_else_statment(Commands *current, int pipes_num);