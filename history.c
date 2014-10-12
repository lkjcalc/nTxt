#include <os.h>
#include "history.h"

#define HISTORY_BUFFERSIZE 10

static action_t* undostack_bottom = NULL;
static action_t* undoredostack_top;
static action_t* redostack_bottom;
static int undoredo_stacksize;

action_t bufferaction; //when .text == NULL, all other fields are invalid


static void history_flush();

static void history_delete_redostack();

int history_init(int stacksize)
{ //stacksize is given in entries, not bytes
    bufferaction.text = NULL;

    undoredo_stacksize = stacksize;
    undostack_bottom = (action_t*) malloc(stacksize * sizeof(action_t));
    undoredostack_top = undostack_bottom;
    redostack_bottom = undostack_bottom;
    if (undostack_bottom == NULL) {
        printf("undoredo: malloc failed\n");
        return 1;
    }
    return 0;
}

void history_free()
{
    history_flush();
    while (redostack_bottom > undostack_bottom) {
        redostack_bottom--;
        free(redostack_bottom->text);
    }
    free(undostack_bottom);
    undostack_bottom = NULL;
}

static void history_delete_redostack()
{
    while (redostack_bottom > undoredostack_top) {
        redostack_bottom--;
        free(redostack_bottom->text);
    }
}

static void history_flush()
{
    if (bufferaction.text == NULL)
        return;
    history_delete_redostack();
    if (undoredostack_top - undostack_bottom == undoredo_stacksize) {
        free(undostack_bottom->text);
        memmove(undostack_bottom, undostack_bottom + 1, (undoredo_stacksize - 1) * sizeof(action_t));
        undoredostack_top--;
        redostack_bottom--;
    }
    *undoredostack_top = bufferaction;
    undoredostack_top++;
    redostack_bottom++;
    bufferaction.text = NULL;
}

int history_push_action(action_t action)
{
    if (action.len > 1) {
        history_flush();
        bufferaction.is_insert = action.is_insert;
        bufferaction.len = action.len;
        bufferaction.pos = action.pos;
        bufferaction.text = action.text;
        history_flush();
    }
    else {
        if (bufferaction.text != NULL) {
            if (bufferaction.is_insert != action.is_insert || bufferaction.len >= HISTORY_BUFFERSIZE) {
                history_flush();
            }
            else {
                if (action.is_insert && bufferaction.pos + bufferaction.len == action.pos) {
                    bufferaction.text[bufferaction.len] = action.text[0];
                    bufferaction.len++;
                }
                else if (!action.is_insert && bufferaction.pos - bufferaction.len == action.pos) {
                    memmove(bufferaction.text + 1, bufferaction.text, bufferaction.len);
                    bufferaction.text[0] = action.text[0];
                    bufferaction.len++;
                    bufferaction.pos--;
                }
                else {
                    history_flush();
                }
            }
        }
        if (bufferaction.text == NULL) {
            bufferaction.text = malloc(HISTORY_BUFFERSIZE);
            if (bufferaction.text == NULL) {
                printf("undoredo: malloc failed\n");
                return 1;
            }
            bufferaction.is_insert = action.is_insert;
            bufferaction.len = 1;
            bufferaction.pos = action.pos;
            bufferaction.text[0] = action.text[0];
        }
        free(action.text);
    }
    return 0;
}

action_t* history_undo()
{
    history_flush();
    if (undoredostack_top == undostack_bottom)
        return NULL;
    return --undoredostack_top;
}

action_t* history_redo()
{
    history_flush();
    if (undoredostack_top == redostack_bottom)
        return NULL;
    return undoredostack_top++;
}
