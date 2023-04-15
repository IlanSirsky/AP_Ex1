#pragma once

#include <sys/stat.h>
#include <sys/wait.h>
#include <fcntl.h>
#include "stdio.h"
#include "errno.h"
#include "stdlib.h"
#include "unistd.h"
#include <string.h>
#include <signal.h>
#include <sys/mman.h>

int main_pid;
char *prompt;
char command[1024];
char lastCommand[1024];
extern int es;
extern int errInCd;
char *infile;
char *outfile;
extern int fd;

// For if else statement
enum states
{
    NEUTRAL,
    WANT_THEN,
    WANT_ELSE,
    ELSE_BLOCK,
    THEN_BLOCK
};

enum results
{
    SUCCESS,
    FAIL
};

extern int if_state;
extern int if_result;
extern int last_stat;
extern int rv;

// Key_value linked list = store named variables
typedef struct key_value
{
    char *key;
    char *value;
    struct key_value *next;
} key_value;

key_value *key_value_root;

// Commands linked list (generated after seperating the whole command with pipe "|")
typedef struct Commands
{
    char *command[10];
    struct Commands *next;
} Commands;

Commands *root;

// last_commands linked list for store all of recent commands
typedef struct last_commands
{
    char command[1024];
    struct last_commands *next;
    struct last_commands *prev;
} last_commands;

last_commands *last_commands_root;

void key_value_add(char *key, char *value);
char *value_get(char *key);
char *str_replace(char *orig, char *rep, char *with);
