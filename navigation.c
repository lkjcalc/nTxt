#include <os.h>
#include "navigation.h"
#include "output.h"
#include "utils.h"

#define HORIZONTAL_SCROLL_WIDTH (SCREEN_WIDTH/CHAR_WIDTH)

int linenum(char* textbuffer, int pos)
{
    int p = 0, line = 0;
    for (; p <= pos; line++) {
        p = checkednextline_nosoftbreak(textbuffer, p);
        if (p == 0) { //last line
            line++;
            break;
        }
    }
    return line;
}

int softlinenum(char* textbuffer, int pos)
{
    int p = 0, line = 0;
    for (; p <= pos; line++) {
        p = checkednextline(textbuffer, p);
        if (p == 0) {
            line++;
            break;
        }
    }
    return line;
}


int countsoftnewl(char* textbuffer)
{ //returns number of lines textbuffer uses or SCREEN_HEIGHT/CHAR_HEIGHT - 1 if the whole screen is used
    int pos = 0, line = 0;
    for (; (pos = checkednextline(textbuffer, pos)) != 0 && line < SCREEN_HEIGHT / CHAR_HEIGHT - 1; line++);
    return line;
}

int countsoftnewl_withinitialw(char* textbuffer, int initialw, int countlen)
{ //returns number of lines textbuffer uses or SCREEN_HEIGHT/CHAR_HEIGHT - 1 if the whole screen is used. first char is at w = initialw
    int pos = 0;
    int line;
    int w = initialw;
    for (line = 0; pos < countlen && line < SCREEN_HEIGHT / CHAR_HEIGHT - 1; line++) {
        for (; pos < countlen && textbuffer[pos] != '\n' && w < SCREEN_WIDTH / CHAR_WIDTH && textbuffer[pos] != '\0'; pos++) {
            if (textbuffer[pos] == '\t')
                w += TAB_WIDTH - (w % TAB_WIDTH);
            else
                w++;
        }
        w = 0;
        if (textbuffer[pos] == '\0')
            break;
        if (textbuffer[pos] == '\n' && w < SCREEN_WIDTH / CHAR_WIDTH)//a newline char needs place -> there can be a soft- and a hardnewline directly after each other
            pos++;
    }
    return line;
}

int countnewl(char* textbuffer)
{ //returns number of lines textbuffer uses in nosoftbreak mode or SCREEN_HEIGHT/CHAR_HEIGHT - 1 if the whole screen is used
    int pos = 0, line = 0;
    for (; (pos = checkednextline_nosoftbreak(textbuffer, pos)) != 0 && line < SCREEN_HEIGHT / CHAR_HEIGHT - 1; line++);
    return line;
}

int getnum(char* textbuffer, int pos)
{
    int tmppos = currentline_nosoftbreak(textbuffer, pos);
    int w = 0;
    int num = 0;
    while (tmppos <= pos) {
        num = 0;
        for (w = 0; textbuffer[tmppos] != '\n' && w < SCREEN_WIDTH / CHAR_WIDTH; tmppos++) {
            if (tmppos >= pos)//how many characters into the line is pos?
                break;
            if (textbuffer[tmppos] == '\t')
                w += TAB_WIDTH - (w % TAB_WIDTH);
            else
                w++;
            num++;
        }
        if (textbuffer[tmppos] == '\n') {
            if (tmppos >= pos)
                break;
            tmppos++;
        }
        if (tmppos >= pos)
            break;
    }
    if (w >= SCREEN_WIDTH / CHAR_WIDTH)
        return 0;
    return num;
}

int currentline(char* textbuffer, int pos)
{
    return pos - getnum(textbuffer, pos);
}

int currentline_nosoftbreak(const char* textbuffer, int pos)
{
    pos--;
    while (pos >= 0 && textbuffer[pos] != '\n')
        pos--;
    return ++pos;
}

int prevline(char* textbuffer, int pos)
{ //if in first line, returns 0
    int tmppos;
    int w;
    int linestart = 0;
    pos = currentline(textbuffer, pos);
    if (pos == 0)
        return 0;
    pos--;
    tmppos = currentline_nosoftbreak(textbuffer, pos);
    while (tmppos <= pos) {
        linestart = tmppos; //save start of line
        for (w = 0; textbuffer[tmppos] != '\n' && w < SCREEN_WIDTH / CHAR_WIDTH && textbuffer[tmppos] != '\0'; tmppos++) {
            if (textbuffer[tmppos] == '\t')
                w += TAB_WIDTH - (w % TAB_WIDTH);
            else
                w++;
        }
        if ((textbuffer[tmppos] == '\n' || textbuffer[tmppos] == '\0') && w < SCREEN_WIDTH / CHAR_WIDTH)
            tmppos++;
    }
    return linestart;
}

int prevline_nosoftbreak(char* textbuffer, int pos)
{ //if in first line, returns 0
    pos = currentline_nosoftbreak(textbuffer, pos);
    if (pos != 0)
        pos = currentline_nosoftbreak(textbuffer, pos - 1);
    return pos;
}

int checkednextline(char* textbuffer, int pos)
{ //if in last line, returns 0
    int tmppos = currentline_nosoftbreak(textbuffer, pos); //start at first character after newline
    int w;
    while (tmppos <= pos) {
        for (w = 0; textbuffer[tmppos] != '\n' && w < SCREEN_WIDTH / CHAR_WIDTH && textbuffer[tmppos] != '\0'; tmppos++) {
            if (textbuffer[tmppos] == '\t')
                w += TAB_WIDTH - (w % TAB_WIDTH);
            else
                w++;
        }
        if (textbuffer[tmppos] == '\0')//return 0 if in last line
            return 0;
        if (textbuffer[tmppos] == '\n' && w < SCREEN_WIDTH / CHAR_WIDTH)
            tmppos++;
    }
    return tmppos;
}

int checkednextline_nosoftbreak(const char* textbuffer, int pos)
{ //if in last line, returns 0
    if (strchr(textbuffer + pos, '\n') != NULL)
        return strchr(textbuffer + pos, '\n') - textbuffer + 1;
    else
        return 0;
}

int nextline(char* textbuffer, int pos)
{ //if in last line, returns last line again
    if (checkednextline(textbuffer, pos) == 0)
        return currentline(textbuffer, pos);
    return checkednextline(textbuffer, pos);
}

int nextline_nosoftbreak(char* textbuffer, int pos)
{ //if in last line, returns last line again
    if (checkednextline_nosoftbreak(textbuffer, pos) == 0)
        return currentline_nosoftbreak(textbuffer, pos);
    return checkednextline_nosoftbreak(textbuffer, pos);
}

int _getw(char* textbuffer, int pos)
{ //how many characters into the line is pos
    int tmppos = currentline_nosoftbreak(textbuffer, pos);
    int w = 0;
    while (tmppos <= pos) {
        for (w = 0; textbuffer[tmppos] != '\n' && w < SCREEN_WIDTH / CHAR_WIDTH; tmppos++) {
            if (tmppos >= pos)
                break;
            if (textbuffer[tmppos] == '\t')
                w += TAB_WIDTH - (w % TAB_WIDTH);
            else
                w++;
        }
        if (textbuffer[tmppos] == '\n') {
            if (tmppos >= pos)
                break;
            tmppos++;
        }
        if (tmppos >= pos)
            break;
    }
    if (w >= SCREEN_WIDTH / CHAR_WIDTH)
        return 0;
    return w;
}

int getw_nosoftbreak(const char* textbuffer, int pos)
{
    int tmppos = currentline_nosoftbreak(textbuffer, pos);
    int w;
    for (w = 0; tmppos < pos; tmppos++) {
        if (textbuffer[tmppos] == '\t')
            w += TAB_WIDTH - (w % TAB_WIDTH);
        else
            w++;
    }
    return w;
}

int gotow(char* textbuffer, int pos, int w)
{ //returns position of char at width w of the current line
    pos = currentline(textbuffer, pos);
    int tmpw;
    w = min(w, SCREEN_WIDTH/CHAR_WIDTH-1);
    for (tmpw = 0; tmpw < w; pos++) {
        if (textbuffer[pos] == '\n' || textbuffer[pos] == '\0')
            break;
        else if (textbuffer[pos] == '\t')
            tmpw += TAB_WIDTH - (tmpw % TAB_WIDTH);
        else
            tmpw++;
    }
    return pos;
}

int gotow_nosoftbreak(char* textbuffer, int pos, int w)
{ //returns position of char at width w of the current line
    pos = currentline_nosoftbreak(textbuffer, pos);
    int tmpw;
    for (tmpw = 0; tmpw < w; pos++) {
        if (textbuffer[pos] == '\n' || textbuffer[pos] == '\0')
            break;
        else if (textbuffer[pos] == '\t')
            tmpw += TAB_WIDTH - (tmpw % TAB_WIDTH);
        else
            tmpw++;
    }
    return pos;
}

int go_down(char* textbuffer, int pos)
{
    int w = _getw(textbuffer, pos);
    pos = nextline(textbuffer, pos);
    return gotow(textbuffer, pos, w);
}

int go_down_nosoftbreak(char* textbuffer, int pos)
{
    int w = getw_nosoftbreak(textbuffer, pos);
    pos = nextline_nosoftbreak(textbuffer, pos);
    return gotow_nosoftbreak(textbuffer, pos, w);
}

int go_up(char* textbuffer, int pos)
{
    int w = _getw(textbuffer, pos);
    pos = prevline(textbuffer, pos);
    return gotow(textbuffer, pos, w);
}

int go_up_nosoftbreak(char* textbuffer, int pos)
{
    int w = getw_nosoftbreak(textbuffer, pos);
    pos = prevline_nosoftbreak(textbuffer, pos);
    return gotow_nosoftbreak(textbuffer, pos, w);
}

int go_left(char* textbuffer, int pos)
{
    int w = _getw(textbuffer, pos);
    w = max(0, w-HORIZONTAL_SCROLL_WIDTH);
    return gotow(textbuffer, pos, w);
}

int go_left_nosoftbreak(char* textbuffer, int pos)
{
    int w = getw_nosoftbreak(textbuffer, pos);
    w = max(0, w-HORIZONTAL_SCROLL_WIDTH);
    return gotow_nosoftbreak(textbuffer, pos, w);
}

int go_right(char* textbuffer, int pos)
{
    int w = _getw(textbuffer, pos);
    w += HORIZONTAL_SCROLL_WIDTH;
    return gotow(textbuffer, pos, w);
}

int go_right_nosoftbreak(char* textbuffer, int pos)
{
    int w = getw_nosoftbreak(textbuffer, pos);
    w += HORIZONTAL_SCROLL_WIDTH;
    return gotow_nosoftbreak(textbuffer, pos, w);
}
