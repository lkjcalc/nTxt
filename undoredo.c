#include <os.h>
#include "undoredo.h"
#include "history.h"
#include "editactions.h"
#include "navigation.h"
#include "output.h"

//internal functions because undoing an insert and redoing a delete do the same thing

static void undoredo_delete(action_t* action, char* textbuffer, int* pos, int* cursorscreenrow, int* cursorscreencol, int softnewline)
{
    if (softnewline) {
        *cursorscreenrow += softlinenum(textbuffer, action->pos) - softlinenum(textbuffer, *pos);
    }
    else {
        *cursorscreenrow += linenum(textbuffer, action->pos) - linenum(textbuffer, *pos);
        *cursorscreencol += getw_nosoftbreak(textbuffer, action->pos) - getw_nosoftbreak(textbuffer, *pos);
    }
    if (*cursorscreencol < 0)
        *cursorscreencol = 0;
    else if (*cursorscreencol >= SCREEN_WIDTH / CHAR_WIDTH)
        *cursorscreencol = SCREEN_WIDTH / CHAR_WIDTH - 1;
    if (*cursorscreenrow < 0)
        *cursorscreenrow = 0;
    else if (*cursorscreenrow >= SCREEN_HEIGHT / CHAR_HEIGHT)
        *cursorscreenrow = SCREEN_HEIGHT / CHAR_HEIGHT - 1;
    *pos = action->pos;
    remove_range_action_unlogged(textbuffer, action->pos, action->len);
}

static int undoredo_insert(action_t* action, char** textbufferp, int* pos, int* cursorscreenrow, int* cursorscreencol, int softnewline)
{
    int retval = insert_range_action_unlogged(textbufferp, action->pos, action->len, action->text);
    *pos = action->pos + action->len;

    if (softnewline) {
        *cursorscreenrow += softlinenum(*textbufferp, *pos) - softlinenum(*textbufferp, action->pos);
    }
    else {
        *cursorscreenrow += linenum(*textbufferp, *pos) - linenum(*textbufferp, action->pos);
        *cursorscreencol += getw_nosoftbreak(*textbufferp, *pos) - getw_nosoftbreak(*textbufferp, action->pos);
    }
    if (*cursorscreencol < 0)
        *cursorscreencol = 0;
    else if (*cursorscreencol >= SCREEN_WIDTH / CHAR_WIDTH)
        *cursorscreencol = SCREEN_WIDTH / CHAR_WIDTH - 1;
    if (*cursorscreenrow < 0)
        *cursorscreenrow = 0;
    else if (*cursorscreenrow >= SCREEN_HEIGHT / CHAR_HEIGHT)
        *cursorscreenrow = SCREEN_HEIGHT / CHAR_HEIGHT - 1;

    return retval;
}

//externally accessible functions
int undoredo_undo(char** textbufferp, int* pos, int* cursorscreenrow, int* cursorscreencol, int softnewline)
{
    action_t* undoaction = history_undo();
    if (undoaction == NULL)//no action to be undone left => do nothing
        return 0;
    if (undoaction->is_insert) {
        undoredo_delete(undoaction, *textbufferp, pos, cursorscreenrow, cursorscreencol, softnewline);
    }
    else
        return undoredo_insert(undoaction, textbufferp, pos, cursorscreenrow, cursorscreencol, softnewline);
    return 0;
}

int undoredo_redo(char** textbufferp, int* pos, int* cursorscreenrow, int* cursorscreencol, int softnewline)
{
    action_t* redoaction = history_redo();
    if (redoaction == NULL)//no action to be redone left => do nothing
        return 0;
    if (redoaction->is_insert)
        return undoredo_insert(redoaction, textbufferp, pos, cursorscreenrow, cursorscreencol, softnewline);
    else {
        undoredo_delete(redoaction, *textbufferp, pos, cursorscreenrow, cursorscreencol, softnewline);
    }
    return 0;
}
