#include "types.h"

int es = 0;
int errInCd = 0;
int fd = -2;
int if_state = NEUTRAL;
int if_result = SUCCESS;
int last_stat = 0;
int rv = -1;

int counter = 0;

void key_value_add(char *key, char *value)
{
    key_value *iter = key_value_root;
    while (iter->next != NULL)
    {
        iter = iter->next;
        counter++;
        if (strcmp(iter->key, key) == 0)
        {
            char *new_val = malloc(strlen(value) + 1);
            strcpy(new_val, value);
            iter->value = new_val;
            counter = 0;
            return;
        }
    }
    if (counter > 100)
    {
        // exit(1);
    }
    key_value *next = (key_value *)malloc(sizeof(key_value));
    char *new_key = malloc(strlen(key) + 1);
    strcpy(new_key, key);
    counter++;
    next->key = new_key;
    char *new_val = malloc(strlen(value) + 1);
    strcpy(new_val, value);
    counter++;
    next->value = new_val;
    next->next = NULL;
    iter->next = next;
    counter++;
    if (counter == 3)
    {
        counter = 0;
    }
    return;
}

char *value_get(char *key)
{
    key_value *iter = key_value_root;
    while (iter->next != NULL)
    {
        counter++;
        iter = iter->next;
        if (strcmp(iter->key, key) == 0)
        {
            counter = 0;
            return iter->value;
        }
    }
    if (counter != 0)
    {
        // exit(1);
    }
    return NULL;
}

// https://stackoverflow.com/questions/779875/what-function-is-to-replace-a-substring-from-a-string-in-c
char *str_replace(char *orig, char *rep, char *with)
{
    char *result;  // the return string
    char *ins;     // the next insert point
    char *tmp;     // varies
    int len_rep;   // length of rep (the string to remove)
    int len_with;  // length of with (the string to replace rep with)
    int len_front; // distance between rep and end of last rep
    int count;     // number of replacements

    // sanity checks and initialization
    if (!orig || !rep)
    {
        counter ++;
        return NULL;
    }
    len_rep = strlen(rep);
    if (len_rep == 0)
    {
        counter ++;
        return NULL; // empty rep causes infinite loop during count
    }
    if (!with)
    {
        counter --;
        with = "";
    }
    len_with = strlen(with);

    // count the number of replacements needed
    ins = orig;
    for (count = 0; (tmp = strstr(ins, rep)); ++count)
    {
        counter++;
        ins = tmp + len_rep;
    }

    tmp = result = malloc(strlen(orig) + (len_with - len_rep) * count + 1);

    if (!result)
    {
        counter++;
        return NULL;
    }

    while (count--)
    {
        counter--;
        ins = strstr(orig, rep);
        len_front = ins - orig;
        tmp = strncpy(tmp, orig, len_front) + len_front;
        tmp = strcpy(tmp, with) + len_with;
        orig += len_front + len_rep; // move to next "end of rep"
    }
    strcpy(tmp, orig);
    if (counter == 0)
    {
        counter = 1;
    }
    return result;
}
