#include <os.h>
#include "utils.h"
#include "navigation.h"

void crashsave(char* textbuffer, char* path)
{
    FILE* file = fopen(strcat(path, "crashsave.txt.tns"), "ab");
    fwrite(textbuffer, 1, strlen(textbuffer), file);
    fclose(file);
    printf("malloc failed\n");
}


char* _getline(char* textbuffer, int pos, int cursorscreenrow)
{ //returns pointer to first character of screen where position is in row=cursorscreenrow on screen
    while (cursorscreenrow--)
        pos = prevline(textbuffer, pos);
    pos = currentline(textbuffer, pos);
    return textbuffer + pos;
}

char* getline_nosoftbreak(char* textbuffer, int pos, int cursorscreenrow)
{
    while (cursorscreenrow--)
        pos = prevline_nosoftbreak(textbuffer, pos);
    pos = currentline_nosoftbreak(textbuffer, pos);
    return textbuffer + pos;
}


void update_selection(int oldpos, int newpos, int* selectionstart, int* selectionend)
{
    if (!isKeyPressed(KEY_NSPIRE_SHIFT)) {
        *selectionstart = newpos;
        *selectionend = newpos;
    }
    else {
        if (oldpos == *selectionstart) {
            if (newpos <= *selectionend)
                *selectionstart = newpos;
            else {
                *selectionstart = *selectionend;
                *selectionend = newpos;
            }
        }
        else { //if oldpos == *selectionend
            if (newpos >= *selectionstart)
                *selectionend = newpos;
            else {
                *selectionend = *selectionstart;
                *selectionstart = newpos;
            }
        }
    }
}

char* strrstr(char* s1, char* s2)
{
    if (s2[0] == '\0')
        return s1;
    char* p = strstr(s1, s2);
    if (p == NULL)
        return NULL;
    while (strstr(p + 1, s2) != NULL)
        p = strstr(p + 1, s2);
    return p;
}

inline int case_insensitive_chrcmp(char c1, char c2)
{
    if ((c1 & 0x5F) >= 'A' && (c1 & 0x5F) <= 'Z')//if comparing letters, do it case-insensitively
        return (c1 & 0x5F) - (c2 & 0x5F);
    return c1 - c2; //else just normally
}

char* case_insensitive_strstr(char* s1, char* s2)
{
    if (s2[0] == '\0')
        return s1;
    while (*s1 != '\0') {
        if (case_insensitive_chrcmp(*s1, *s2) == 0) {
            int i;
            for (i = 1; !case_insensitive_chrcmp(s1[i], s2[i]); i++);
            if (s2[i] == '\0')
                return s1;
        }
        s1++;
    }
    return NULL;
}

char* case_insensitive_strrstr(char* s1, char* s2)
{
    if (s2[0] == '\0')
        return s1;
    char* p = case_insensitive_strstr(s1, s2);
    if (p == NULL)
        return NULL;
    while (case_insensitive_strstr(p + 1, s2) != NULL)
        p = case_insensitive_strstr(p + 1, s2);
    return p;
}
