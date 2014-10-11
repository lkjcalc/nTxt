#include <os.h>
#include "editactions.h"
#include "history.h"
#include "navigation.h"
//because of CHAR_WIDTH etc
#include "output.h"


static unsigned clipboard_get_addrs[] = {0x104A1B94, 0x104860A8, 0x10483218, 0x104839C8, 0, 0, 0x1058ABCC, 0x1056F3F8, 0x1056C814, 0x1056D2AC};
#define clipboard_get SYSCALL_CUSTOM(clipboard_get_addrs, int, char**, char*)

static unsigned clipboard_set_addrs[] = {0x104A1E38, 0x1048634C, 0x104834BC, 0x10483C6C, 0, 0, 0x1058AE58, 0x1056F684, 0x1056CAA0, 0x1056D538};
#define clipboard_set SYSCALL_CUSTOM(clipboard_set_addrs, int16_t, char*, int, char*)

static unsigned clipboard_open_addrs[] = {0x104A1EE4, 0x104863F8, 0x10483568, 0x10483D18, 0, 0, 0x1058AF34, 0x1056F760, 0x1056CB7C, 0x1056D614};
#define clipboard_open SYSCALL_CUSTOM(clipboard_open_addrs, int, void)

static unsigned clipboard_close_addrs[] = {0x104A1E28, 0x1048633C, 0x104834AC, 0x10483C5C, 0, 0, 0x1058AE48, 0x1056F674, 0x1056CA90, 0x1056D528};
#define clipboard_close SYSCALL_CUSTOM(clipboard_close_addrs, int, void)

#define CLIPBOARD_TYPE_TEXT "t\0e\0x\0t\0/\0p\0l\0a\0i\0n\0\0"



inline int insert_range_action_unlogged(char** textbufferp, int pos, int len, char* ins){
    if(strlen(*textbufferp) % 1024 + len > 1023){//allocate enough memory
        char* tmp = realloc(*textbufferp, strlen(*textbufferp) + len + 1024 - (strlen(*textbufferp) + len) % 1024);
        if(tmp == NULL)
            return 1;
        *textbufferp = tmp;
    }
    memmove((*textbufferp) + pos + len, (*textbufferp) + pos, strlen(*textbufferp) + 1 - pos);//move everything including null char!
    memcpy((*textbufferp) + pos, ins, len);
    return 0;
}

inline void remove_range_action_unlogged(char* textbuffer, int pos, int len){
    if(textbuffer[pos+len] == '\0')
        textbuffer[pos] = '\0';
    else
        memmove(textbuffer + pos, textbuffer + pos + len, strlen(textbuffer) + 1 - (pos + len));//move everything after the range (including null character)
}

inline int insert_range_action(char** textbufferp, int pos, int len, char* ins){
    action_t action;
    action.is_insert = 1;
    action.pos = pos;
    action.len = len;
    action.text = (char*) malloc(len);
    if(action.text == NULL)
        return 1;
    memcpy(action.text, ins, len);
    if(history_push_action(action) == 1)
        return 1;
    return insert_range_action_unlogged(textbufferp, pos, len, ins);
}

inline int remove_range_action(char* textbuffer, int pos, int len){
    action_t action;
    action.is_insert = 0;
    action.pos = pos;
    action.len = len;
    action.text = (char*) malloc(len);
    if(action.text == NULL)
        return 1;
    memcpy(action.text, textbuffer+pos, len);
    if(history_push_action(action) == 1)
        return 1;
    remove_range_action_unlogged(textbuffer, pos, len);
    return 0;
}



inline int delete_action(char* textbuffer, int* pos, int* cursorscreenrow, int* cursorscreencol, int* selectionstart, int* selectionend, int softnewline){
    if(*pos == *selectionend){
        if(softnewline)
            *cursorscreenrow -= countsoftnewl_withinitialw(textbuffer+*selectionstart, getw(textbuffer, *selectionstart), *selectionend - *selectionstart);
        else{
            char restore = textbuffer[*selectionend];
            textbuffer[*selectionend] = '\0';
            *cursorscreenrow -= countnewl(textbuffer+*selectionstart);
            textbuffer[*selectionend] = restore;
            *cursorscreencol -= getw_nosoftbreak(textbuffer, *selectionend) - getw_nosoftbreak(textbuffer, *selectionstart);
        }
    }
    if(remove_range_action(textbuffer, *selectionstart, *selectionend - *selectionstart) == 1)
        return 1;
    *pos = *selectionstart;
    *selectionend = *selectionstart;
    return 0;
}

inline int copy_action(char* textbuffer, int selectionstart, int selectionend){
    char type[] = CLIPBOARD_TYPE_TEXT;
    if(selectionstart != selectionend){
        clipboard_open();
        int16_t ret = clipboard_set(textbuffer + selectionstart, selectionend - selectionstart, type);
        clipboard_close();
        if(ret == -1)
            return 1;
    }
    return 0;
}

inline int cut_action(char* textbuffer, int* pos, int* cursorscreenrow, int* cursorscreencol, int* selectionstart, int* selectionend, int softnewline){
    if(*selectionstart != *selectionend){
        if(copy_action(textbuffer, *selectionstart, *selectionend) == 1)
            return 1;
        if(delete_action(textbuffer, pos, cursorscreenrow, cursorscreencol, selectionstart, selectionend, softnewline) == 1)
            return 1;
    }
    return 0;
}

inline int paste_action(char** textbufferp, int* pos, int* cursorscreenrow, int* cursorscreencol, int* selectionstart, int* selectionend, int softnewline){
    char* clipboardcontent;
    char type[] = CLIPBOARD_TYPE_TEXT;
    int clipboardlength = clipboard_get(&clipboardcontent, type);
    char restore;
    if(clipboardlength != -1){
        if(*selectionstart != *selectionend){
            if(delete_action(*textbufferp, pos, cursorscreenrow, cursorscreencol, selectionstart, selectionend, softnewline) == 1)
                return 1;
        }
        if(insert_range_action(textbufferp, *pos, clipboardlength, clipboardcontent) == 1)
            return 1;
        *pos += clipboardlength;
        *selectionstart = *pos;
        *selectionend = *pos;
        restore = (*textbufferp)[*pos];
        (*textbufferp)[*pos] = '\0';
        if(softnewline == TRUE)
            *cursorscreenrow += countsoftnewl(*textbufferp + *pos - clipboardlength);
        else{
            *cursorscreenrow += countnewl(*textbufferp + *pos - clipboardlength);
            *cursorscreencol += getw_nosoftbreak(*textbufferp, *pos) - *cursorscreencol;
        }
        (*textbufferp)[*pos] = restore;
    }
    return 0;
}
