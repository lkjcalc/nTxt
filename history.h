#ifndef _HISTORY_H_
#define _HISTORY_H_

typedef struct
{
    int is_insert;  // 1 if insert, 0 if delete
    int pos;        // start of the range to delete or location where to insert, respectively
    int len;        // length of the range to delete/insert
    char* text;     // pointer to a copy of the inserted/deleted text. NOT null terminated. will be freed internally, don't do anything with it after passing it to a push_action anymore
} action_t;

//initialize internal pointers and memory
//stacksize == number of entries in the history
int history_init(int stacksize);

//free internal memory
void history_free();

//put a new action into the history. may be grouped together with other actions. returns 1 on malloc failure, else 0
int history_push_action(action_t action);

//undo an action
//returns the undone action or NULL if no action left
action_t* history_undo();

//redo an action
//returns the redone action or NULL if no action left
action_t* history_redo();

#endif
