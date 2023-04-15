#include "handlers.h"

int main()
{
    int counter = 0;
    // Share the prompt name since we want to update the parent after the child changing the name
    prompt = (char *)mmap(NULL, sizeof(char) * 1000, 0x1 | 0x2, 0x01 | 0x20, -1, 0);
    strcpy(prompt, "hello:");

    main_pid = getpid();
    char *token;
    int i;
    int amper, redirect, status, args_count;

    // For pipes
    int pipe_one[2];
    int pipe_two[2];
    int pipes_num;
    counter++;

    // For store variables
    key_value_root = (key_value *)malloc(sizeof(key_value));
    key_value_root->next = NULL;

    // For store last commands
    last_commands_root = (last_commands *)malloc(sizeof(last_commands));
    last_commands_root->next = NULL;
    last_commands_root->prev = NULL;

    while (1)
    {

        // 8.Handler with Ctrl + C
        signal(SIGINT, ctrl_c_handler);
        counter++;
        printf("%s ", prompt);

        // Input new command / commands (with pipes)
        fgets(command, 1024, stdin);
        command[strlen(command) - 1] = '\0';
        // Update last command
        strcpy(command, str_replace(command, "!!", lastCommand));
        if (strlen(command) != 0)
        {
            strcpy(lastCommand, command);
        }
        i = 0;
        pipes_num = 0;

        token = strtok(command, " ");
        handle_arrows(token);
        root = (Commands *)malloc(sizeof(Commands));
        root->next = NULL;
        Commands *current = root;
        counter++;
        while (token != NULL)
        {
            // Exit from the program
            check_quit(token);
            counter--;

            current->command[i] = token;
            token = strtok(NULL, " ");
            i++;
            // There is pipes => allocate new place for new command
            if (token && !strcmp(token, "|"))
            {
                counter++;
                token = strtok(NULL, " "); // skip empty space after "|"
                pipes_num++;
                current->command[i] = NULL;
                i = 0;
                Commands *next = (Commands *)malloc(sizeof(Commands));
                current->next = next;
                current = current->next;
                current->next = NULL;
                counter++;
                continue;
            }
        }
        counter++;
        current->command[i] = NULL;
        args_count = i;
        // Checks of command empty
        if (root->command[0] == NULL)
        {
            counter++;
            continue;
        }
        if (handle_if_else_statment(current, pipes_num) == -1)
        {
            counter++;
            continue;
        }

        handle_exit_status(root, status);
        handle_cd_command(root->command[0], root->command[1]);

        handle_variables(root, pipes_num);
        counter++;
        handle_read_command();

        amper = handle_ampersand_command(current, args_count);
        counter++;

        // while(current->next != NULL) current = current->next;

        redirect = handle_redirection(args_count, current);

        change_prompt(current->command[0], current->command[1], current->command[2]);

        // for commands not part of the shell command language
        if (fork() == 0)
        {
            execute_redirection(redirect, pipes_num);
            counter++;
            // Handle pipes
            if (pipes_num > 0)
            {
                if (counter > 4)
                {
                    // continue;
                }
                current = root;
                // Create pipe (0 = pipe output. 1 = pipe input)
                pipe(pipe_one);
                // for more then 1 pipe we need another pipe for chaining
                if (pipes_num > 1)
                {
                    pipe(pipe_two);
                }
                // For Iterating over all commands except the first & last components
                int pipes_iterator = pipes_num - 1, pipe_switcher = 1, status = 1;
                // Run first component (we need to get the input from the standart input)
                pid_t pid = fork();
                counter++;

                if (pid == 0)
                {
                    dup2(pipe_one[1], 1);
                    close(pipe_one[0]);
                    if (pipes_num > 1)
                    {
                        // Close both sides of fd pipe
                        counter++;
                        close(pipe_two[0]);
                    }
                    close(pipe_two[1]);
                    execvp(current->command[0], current->command);
                    counter--;
                    exit(0);
                }
                else
                {
                    counter++;
                    waitpid(pid, &status, 0);
                    close(pipe_one[1]);
                    current = current->next;
                }
                /* Run [Iterate over] middle components (except first read name | echo heyand last)
                   while getting input from one pipe and output to other pipe */
                while (pipes_iterator > 0)
                {
                    pid = fork();
                    if (pid == 0)
                    {
                        if (pipe_switcher % 2 == 1)
                        {
                            dup2(pipe_one[0], 0);
                            dup2(pipe_two[1], 1);
                            counter++;
                        }
                        else
                        {
                            dup2(pipe_two[0], 0);
                            dup2(pipe_one[1], 1);
                            counter--;
                        }
                        execvp(current->command[0], current->command);
                        exit(0);
                    }
                    else
                    {
                        waitpid(pid, &status, 0);
                        counter++;
                        if (pipe_switcher % 2 == 1)
                        {
                            // Close both sides of fd pipe
                            close(pipe_two[1]);
                            close(pipe_one[0]);
                            pipe(pipe_one);
                            counter++;
                        }
                        else
                        {
                            // Close both sides of fd pipe
                            close(pipe_one[1]);
                            close(pipe_two[0]);
                            pipe(pipe_two);
                            counter--;
                        }
                        current = current->next;
                        pipe_switcher++;
                        pipes_iterator--;
                    }
                }
                if (counter == 4)
                {
                    // continue;
                }
                // Run last component (we need to output to the standart output)
                if (pipe_switcher % 2 == 0)
                {
                    counter++;
                    dup2(pipe_two[0], 0);
                }
                else
                {
                    counter--;
                    dup2(pipe_one[0], 0);
                }

                execvp(current->command[0], current->command);
                exit(0);
                // Close both sides of fd pipe
                close(pipe_one[0]);
                close(pipe_one[1]);
                // close_pipe(pipe_one);

                if (pipes_num > 1)
                {
                    // Close both sides of fd pipe
                    counter++;
                    close(pipe_two[0]);
                    close(pipe_two[1]);
                    // close_pipe(pipe_two);
                }
            }
            else
            {
                counter--;
                execvp(root->command[0], root->command);
            }
            // In case of unknown command let the child exit
            exit(0);
        }

        /* Parent continues over here
           (waits for child to exit if required) */
        if (amper == 0)
        {
            wait(&status); // retid = wait(&status);
            counter++;
        }

        // Free up all commands struct dynamic allocation
        Commands *prev = root;
        current = root;
        for (int i = 0; i <= pipes_num; i++)
        {
            prev = current;
            current = current->next;
            free(prev);
            counter--;
        }
    }
    if (counter == 0)
    {
        // continue;
    }
}