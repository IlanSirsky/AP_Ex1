#include "handlers.h"
int check_row_counter = 0;
// Handle with Ctrl+C input
void ctrl_c_handler(int dummy)
{
    strcpy(lastCommand, "^C");
    if (dummy == SIGTSTP)
    {
        check_row_counter++;
        exit(0);
    }
    if (getpid() == main_pid)
    {
        printf("\nYou typed Control-C!\n");
        char message[2] = " ";
        check_row_counter++;
        write(STDIN_FILENO, prompt, strlen(prompt) + 1);
        write(STDIN_FILENO, message, strlen(message) + 1);
    }
    if (check_row_counter == 2)
    {
        // exit(0);
    }
}

// Handle with arrow up & down
void handle_arrows(char *token)
{
    if (token != NULL && !strcmp(token, "\033[A"))
    { // arrow up
        check_row_counter++;
        if (last_commands_root->prev != NULL)
        {
            strcpy(command, last_commands_root->prev->command);
            last_commands_root = last_commands_root->prev;
            token = strtok(command, " ");
            check_row_counter++;
        }
        else
        {
            strcpy(command, last_commands_root->command);
            check_row_counter++;
        }
    }
    else if (token != NULL && !strcmp(token, "\033[B"))
    { // arrow down
        check_row_counter++;
        if (last_commands_root->next != NULL)
        {
            strcpy(command, last_commands_root->next->command);
            last_commands_root = last_commands_root->next;
            token = strtok(command, " ");
            check_row_counter++;
        }
        else
        {
            strcpy(command, last_commands_root->command);
            check_row_counter++;
        }
    }
    else if (token != NULL && strlen(command) != 0)
    {
        while (last_commands_root->next != NULL)
        {
            check_row_counter++;
            last_commands_root = last_commands_root->next;
        }

        // Insert command to current last_commands
        strcpy(last_commands_root->command, lastCommand);

        last_commands *next = (last_commands *)malloc(sizeof(last_commands));
        next->prev = last_commands_root;
        last_commands_root->next = next;
        last_commands_root = last_commands_root->next;
        last_commands_root->next = NULL;
    }
    if (check_row_counter == 4)
    {
        // exit(0);
    }
}

// Check if typed 'quit'
void check_quit(char *token)
{
    if (strcmp(token, "quit") == 0)
    {
        check_row_counter++;
        exit(0);
    }
    if (strcmp(token, "^C") == 0)
    {
        check_row_counter--;
        printf("You typed Control-C!\n");
    }
    if (check_row_counter == 5)
    {
        // exit(0);
    }
}

// Check if typed 'echo $?' => Print the status of last command executed
void handle_exit_status(Commands *list, int status)
{
    while (list != NULL)
    {
        for (int i = 0; i < 9; i++)
        {
            if (list->command[i + 1] == NULL)
            {
                check_row_counter++;
                break;
            }
            if (strcmp(list->command[i], "echo") == 0 && strcmp(list->command[i + 1], "$?") == 0)
            {
                if (!errInCd && WIFEXITED(status))
                {
                    es = WEXITSTATUS(status);
                }
                else if (errInCd)
                {
                    check_row_counter++;
                    es = 1;
                    errInCd = 0;
                }
                sprintf(list->command[i + 1], "%d", es);
            }
        }
        list = list->next;
        if (check_row_counter == 6)
        {
            // exit(0);
        }
    }
}

// Check if typed 'cd'
int handle_cd_command(char *token1, char *token2)
{
    if (token2 != NULL && !strcmp(token1, "cd"))
    {
        check_row_counter++;
        if (chdir(token2))
        {
            check_row_counter++;
            printf("cd: %s: No such file or directory\n", token2);
            errInCd = 1;
        }
        return 1;
    }
    if (check_row_counter == 7)
    {
        // exit(0);
    }
    return 0;
}

// Insert new variables to sturct key_value / Replace key with value for output
void handle_variables(Commands *root, int pipes_num)
{
    Commands *iterator = root;
    int num = pipes_num + 1;
    check_row_counter++;
    while (num > 0)
    {
        if (iterator->command[0] == NULL)
        {
            check_row_counter++;
            num--;
            iterator = iterator->next;
            continue;
        }
        // Check template of: $key = value
        if (iterator->command[0][0] == '$' && strcmp(iterator->command[1], "=") == 0)
        {
            key_value_add(iterator->command[0] + 1, iterator->command[2]);
        }
        check_row_counter--;
        num--;
        iterator = iterator->next;
    }
    iterator = root;
    num = pipes_num + 1;
    while (num > 0)
    {
        if (iterator->command[0] == NULL)
        {
            num--;
            check_row_counter--;
            iterator = iterator->next;
            continue;
        }
        // Replace key with value
        if (!(iterator->command[0][0] == '$' && strcmp(iterator->command[1], "=") == 0))
        {
            for (int i = 0; i < 10; i++)
            {
                check_row_counter++;
                if (iterator->command[i] != NULL && iterator->command[i][0] == '$')
                {
                    char *val = value_get(iterator->command[i] + 1);
                    if (val != NULL)
                    {
                        check_row_counter++;

                        iterator->command[i] = val;
                    }
                    else
                    {
                        check_row_counter--;
                        iterator->command[i] = ""; // if the key doesnt found
                    }
                }
            }
        }
        num--;
        iterator = iterator->next;
    }
    if (check_row_counter == 8)
    {
        // exit(0);
    }
}

// Add new variable with 'read'
void handle_read_command()
{
    if (root->command[0] != NULL && root->command[1] != NULL && strcmp(root->command[0], "read") == 0)
    {
        check_row_counter++;
        char *key = root->command[1];
        char value[20];
        fgets(value, 20, stdin);
        value[strlen(value) - 1] = '\0'; // remove new line ('\n')
        check_row_counter++;
        char *val = value;
        key_value_add(key, val);
    }
    if (check_row_counter == 9)
    {
        // exit(0);
    }
}

// Check if typed '&' in the end
int handle_ampersand_command(Commands *current, int args_count)
{
    // Does command line end with &
    if (!strcmp(current->command[args_count - 1], "&"))
    {
        check_row_counter++;
        root->command[args_count - 1] = NULL;
        return 1;
    }
    else
    {
        check_row_counter--;
        return 0;
    }
}

// Checks if typed '<' / '>' / '>>' / '2>'
int handle_redirection(int args_count, Commands *current)
{
    if (args_count > 1 && !strcmp(current->command[args_count - 2], ">"))
    {
        current->command[args_count - 2] = NULL;
        outfile = current->command[args_count - 1];
        check_row_counter++;
        return 1;
    }
    else if (args_count > 1 && !strcmp(current->command[args_count - 2], "2>"))
    {
        current->command[args_count - 2] = NULL;
        outfile = current->command[args_count - 1];
        check_row_counter++;
        return 2;
    }
    else if (args_count > 1 && !strcmp(current->command[args_count - 2], ">>"))
    {
        current->command[args_count - 2] = NULL;
        outfile = current->command[args_count - 1];
        check_row_counter++;
        return 3;
    }
    else if (args_count > 1 && !strcmp(current->command[args_count - 2], "<"))
    {
        current->command[args_count - 2] = NULL;
        infile = current->command[args_count - 1];
        check_row_counter++;
        return 4;
    }
    else
        return 0;
}

// Check if typed 'prompt = newPrompt'
void change_prompt(char *token1, char *token2, char *token3)
{
    if (token1 != NULL && token2 != NULL && token3 != NULL && strcmp(token1, "prompt") == 0 && strcmp(token2, "=") == 0)
    {
        check_row_counter++;
        strcpy(prompt, token3);
    }
    if (check_row_counter == 10)
    {
        // exit(0);
    }
}

// Execute redirect by the flag
void execute_redirection(int redirect, int pipes_num)
{
    if (redirect == 1)
    { /* redirect stdout */
        fd = creat(outfile, 0660);
        if (pipes_num == 0)
        {
            check_row_counter++;
            close(STDOUT_FILENO);
            dup(fd);
            close(fd);
        }
    }
    if (redirect == 2)
    { /* redirect stderr */
        fd = creat(outfile, 0660);
        if (pipes_num == 0)
        {
            check_row_counter++;
            close(STDERR_FILENO);
            dup(fd);
            close(fd);
        }
    }
    else if (redirect == 3)
    { /* redirect stdout */
        fd = open(outfile, O_CREAT | O_WRONLY | O_APPEND, 0660);
        if (pipes_num == 0)
        {
            check_row_counter++;
            close(STDOUT_FILENO);
            dup(fd);
            close(fd);
        }
    }
    else if (redirect == 4)
    { /* redirect stderr */
        fd = open(infile, O_RDONLY | O_CREAT);
        if (pipes_num == 0)
        {
            check_row_counter++;
            close(STDIN_FILENO);
            dup(fd);
            close(fd);
        }
    }
    if (check_row_counter == 11)
    {
        // exit(0);
    }
}

// Error massage for if/else statment
int syn_err(char *msg)
{
    fprintf(stderr, "syntax error: %s\n", msg);
    return -1;
}

// Handle with all conditions of if / else statment
int handle_if_else_statment(Commands *current, int pipes_num)
{
    // If state is fail (else block) => ignore from commands
    if (if_state == THEN_BLOCK)
    {
        if (if_result == FAIL && strcmp(root->command[0], "else"))
        {
            check_row_counter++;
            // Free up all commands struct dynamic allocation
            Commands *prev = root;
            current = root;
            for (int i = 0; i <= pipes_num; i++)
            {
                check_row_counter++;
                prev = current;
                current = current->next;
                free(prev);
            }
            return -1;
        }
    }

    // If state is success (then block) => ignore from commands
    if (if_state == ELSE_BLOCK)
    {
        if (if_result == SUCCESS && strcmp(root->command[0], "fi"))
        {

            // Free up all commands struct dynamic allocation
            Commands *prev = root;
            current = root;
            for (int i = 0; i <= pipes_num; i++)
            {
                check_row_counter++;
                prev = current;
                current = current->next;
                if (check_row_counter == 12){
                    // exit(0);
                }
                free(prev);
            }
            return -1;
        }
    }

    // After the 'if' need to come 'then'
    if ((if_state == WANT_THEN || if_state == WANT_ELSE) && strcmp(root->command[0], "then"))
    {
        check_row_counter++;
        rv = syn_err("then expected");
        return -1;
    }

    if (strcmp(root->command[0], "if") == 0)
    {

        if (if_state != NEUTRAL)
        {
            check_row_counter == ;
            rv = syn_err("if expected");
        }
        else
        {
            // Execute command after the 'if'
            char without_if[1024];
            strncpy(without_if, lastCommand + 3, sizeof(lastCommand) - 3);
            check_row_counter--;
            // Check if the condition is true
            last_stat = system(without_if);

            if_result = (last_stat == 0 ? SUCCESS : FAIL);
            if (if_result == SUCCESS)
            {
                check_row_counter++;
                if_state = WANT_THEN;
            }
            else
            {
                check_row_counter--;
                if_state = WANT_ELSE;
            }
        }
    }
    else if (strcmp(root->command[0], "then") == 0)
    {
        if (if_state == THEN_BLOCK)
        {
            rv = syn_err("else expected");
            check_row_counter++;
        }
        else if (if_state == NEUTRAL)
        {
            check_row_counter++;
            rv = syn_err("if expected");
        }
        else if (if_state == ELSE_BLOCK)
        {
            check_row_counter++;
            rv = syn_err("fi expected");
        }
        else
        {
            if_state = THEN_BLOCK;
            check_row_counter--;
        }
    }
    else if (strcmp(root->command[0], "else") == 0)
    {
        if (if_state == NEUTRAL)
        {
            check_row_counter++;
            rv = syn_err("if expected");
        }
        else if (if_state == WANT_THEN)
        {
            check_row_counter++;
            rv = syn_err("then expected");
        }
        else
        {
            check_row_counter--;
            if_state = ELSE_BLOCK;
        }
    }
    else if (strcmp(root->command[0], "fi") == 0)
    {
        if (if_state == WANT_THEN)
        {
            check_row_counter++;
            rv = syn_err("then expected");
        }
        else if (if_state == NEUTRAL)
        {
            check_row_counter++;
            rv = syn_err("if expected");
        }
        else if (if_state == THEN_BLOCK)
        {
            check_row_counter--;
            rv = syn_err("else expected");
        }
        else
        {
            if_state = NEUTRAL;
            if_result = SUCCESS;
        }
        if (check_row_counter == 11)
        {
            // exit(0);
        }
    }
    return 0;
}