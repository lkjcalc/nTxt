#include <os.h>
#include "editactions.h"
#include "history.h"
#include "navigation.h"
#include "clipboard.h"

int insert_range_action_unlogged(char** textbufferp, int pos, int len, char* ins)
{
    if (strlen(*textbufferp) % 1024 + len > 1023) { //allocate enough memory
        char* tmp = realloc(*textbufferp, strlen(*textbufferp) + len + 1024 - (strlen(*textbufferp) + len) % 1024);
        if (tmp == NULL)
            return 1;
        *textbufferp = tmp;
    }
    memmove((*textbufferp) + pos + len, (*textbufferp) + pos, strlen(*textbufferp) + 1 - pos); //move everything including null char!
    memcpy((*textbufferp) + pos, ins, len);
    return 0;
}

void remove_range_action_unlogged(char* textbuffer, int pos, int len)
{
    if (textbuffer[pos + len] == '\0')
        textbuffer[pos] = '\0';
    else
        memmove(textbuffer + pos, textbuffer + pos + len, strlen(textbuffer) + 1 - (pos + len)); //move everything after the range (including null character)
}

int insert_range_action(char** textbufferp, int pos, int len, char* ins)
{
    action_t action;
    action.is_insert = 1;
    action.pos = pos;
    action.len = len;
    action.text = (char*) malloc(len);
    if (action.text == NULL)
        return 1;
    memcpy(action.text, ins, len);
    if (history_push_action(action) == 1)
        return 1;
    return insert_range_action_unlogged(textbufferp, pos, len, ins);
}

int remove_range_action(char* textbuffer, int pos, int len)
{
    action_t action;
    action.is_insert = 0;
    action.pos = pos;
    action.len = len;
    action.text = (char*) malloc(len);
    if (action.text == NULL)
        return 1;
    memcpy(action.text, textbuffer + pos, len);
    if (history_push_action(action) == 1)
        return 1;
    remove_range_action_unlogged(textbuffer, pos, len);
    return 0;
}


int delete_action(char* textbuffer, int* pos, int* cursorscreenrow, int* cursorscreencol, int* selectionstart, int* selectionend, int softnewline)
{
    if (*pos == *selectionend) {
        if (softnewline)
            *cursorscreenrow -= countsoftnewl_withinitialw(textbuffer + *selectionstart, _getw(textbuffer, *selectionstart), *selectionend - *selectionstart);
        else {
            char restore = textbuffer[*selectionend];
            textbuffer[*selectionend] = '\0';
            *cursorscreenrow -= countnewl(textbuffer + *selectionstart);
            textbuffer[*selectionend] = restore;
            *cursorscreencol -= getw_nosoftbreak(textbuffer, *selectionend) - getw_nosoftbreak(textbuffer, *selectionstart);
        }
    }
    if (remove_range_action(textbuffer, *selectionstart, *selectionend - *selectionstart) == 1)
        return 1;
    *pos = *selectionstart;
    *selectionend = *selectionstart;
    return 0;
}

int copy_action(char* textbuffer, int selectionstart, int selectionend)
{
    if (selectionstart == selectionend) 
        return 0;
    if (clipboard_settext(textbuffer + selectionstart, selectionend - selectionstart) != 0)
        return 1;
    return 0;
}

int cut_action(char* textbuffer, int* pos, int* cursorscreenrow, int* cursorscreencol, int* selectionstart, int* selectionend, int softnewline)
{
    if (*selectionstart != *selectionend) {
        if (copy_action(textbuffer, *selectionstart, *selectionend) == 1)
            return 1;
        if (delete_action(textbuffer, pos, cursorscreenrow, cursorscreencol, selectionstart, selectionend, softnewline) == 1)
            return 1;
    }
    return 0;
}

int paste_action(char** textbufferp, int* pos, int* cursorscreenrow, int* cursorscreencol, int* selectionstart, int* selectionend, int softnewline)
{
    char* clipboardcontent;
    int clipboardlength;
    if (clipboard_gettext(&clipboardcontent, &clipboardlength) != 0)
        return 1;
    char restore;
    if (clipboardlength > 0) {
        if (*selectionstart != *selectionend) {
            if (delete_action(*textbufferp, pos, cursorscreenrow, cursorscreencol, selectionstart, selectionend, softnewline) == 1)
                return 1;
        }
        if (insert_range_action(textbufferp, *pos, clipboardlength, clipboardcontent) == 1)
            return 1;
        *pos += clipboardlength;
        *selectionstart = *pos;
        *selectionend = *pos;
        restore = (*textbufferp)[*pos];
        (*textbufferp)[*pos] = '\0';
        if (softnewline == TRUE)
            *cursorscreenrow += countsoftnewl(*textbufferp + *pos - clipboardlength);
        else {
            *cursorscreenrow += countnewl(*textbufferp + *pos - clipboardlength);
            *cursorscreencol += getw_nosoftbreak(*textbufferp, *pos) - *cursorscreencol;
        }
        (*textbufferp)[*pos] = restore;
    }
    return 0;
}
