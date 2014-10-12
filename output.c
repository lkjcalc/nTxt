#include <os.h>
#include "output.h"
#include "navigation.h"
#include "charmap_8x12.h"

#define STR_SELECTION_COLOR (has_colors?0b011101111111111:0xA)

static inline void setPixelBuf_color(void* scrbuf, unsigned x, unsigned y, uint16_t color)
{
    if (x < SCREEN_WIDTH && y < SCREEN_HEIGHT) {
        uint16_t* p = (uint16_t * )(scrbuf + 2 * x + 2 * SCREEN_WIDTH * y);
        *p = color;
    }
}

static inline void setPixelBuf_grey(void* scrbuf, unsigned x, unsigned y, uint8_t color)
{
    if (x < SCREEN_WIDTH && y < SCREEN_HEIGHT) {
        uint8_t* p = (uint8_t*) scrbuf + 2 * x + SCREEN_WIDTH / 2 * y;
        if (x % 2)
            *p = (*p & 0xF0) | color;
        else
            *p = (*p & 0x0F) | color << 4;
    }
}

void putChar(void* scrbuf, unsigned x, unsigned y, uint8_t chr)
{
    int i, j, pixelOn;
    if (has_colors) {
        for (i = 0; i < CHAR_HEIGHT; i++) {
            for (j = 0; j < CHAR_WIDTH; j++) {
                pixelOn = charMap_ascii[chr][i] << j;
                pixelOn = pixelOn & 0b10000000;
                if (pixelOn) {
                    setPixelBuf_color(scrbuf, x + j, y + i, 0);
                }
                //else{
                //    setPixelBuf(scrbuf, x + j, y + i, has_colors ? 0xFFFF : 0xF);
                //}
            }
        }
    }
    else {
        for (i = 0; i < CHAR_HEIGHT; i++) {
            for (j = 0; j < CHAR_WIDTH; j++) {
                pixelOn = charMap_ascii[chr][i] << j;
                pixelOn = pixelOn & 0b10000000;
                if (pixelOn) {
                    setPixelBuf_grey(scrbuf, x + j, y + i, 0);
                }
                //else{
                //    setPixelBuf(scrbuf, x + j, y + i, has_colors ? 0xFFFF : 0xF);
                //}
            }
        }
    }
}

void putCharColor(void* scrbuf, unsigned x, unsigned y, uint8_t chr, uint16_t chrcolor, uint16_t bgcolor)
{
    int i, j, pixelOn;
    if (has_colors) {
        for (i = 0; i < CHAR_HEIGHT; i++) {
            for (j = 0; j < CHAR_WIDTH; j++) {
                pixelOn = charMap_ascii[chr][i] << j;
                pixelOn = pixelOn & 0x80;
                setPixelBuf_color(scrbuf, x + j, y + i, pixelOn ? chrcolor : bgcolor);
            }
        }
    }
    else {
        for (i = 0; i < CHAR_HEIGHT; i++) {
            for (j = 0; j < CHAR_WIDTH; j++) {
                pixelOn = charMap_ascii[chr][i] << j;
                pixelOn = pixelOn & 0x80;
                setPixelBuf_grey(scrbuf, x + j, y + i, pixelOn ? chrcolor : bgcolor);
            }
        }
    }
}

void dispString(void* scrbuf, unsigned x, unsigned y, const char* message)
{
    int l = strlen(message);
    int i;
    unsigned line = 0, col = 0;
    for (i = 0; i < l; i++) {
        if (message[i] == '\n') {
            col = 0;
            ++line;
        }
        else if (message[i] == '\t')
            col += TAB_WIDTH - (col % TAB_WIDTH);
        else {
            putChar(scrbuf, x + col * CHAR_WIDTH, y + line * CHAR_HEIGHT, message[i]);
            ++col;
        }
        if (x + col * CHAR_WIDTH >= SCREEN_WIDTH) {
            col = 0;
            ++line;
        }
        if (y + line * CHAR_HEIGHT >= SCREEN_HEIGHT)
            break;
    }
}

void dispStringColor(void* scrbuf, unsigned x, unsigned y, const char* message, uint16_t chrcolor, uint16_t bgcolor)
{
    const char* p;
    unsigned line = 0, col = 0;
    for (p = message; *p; ++p) {
        if (*p == '\n') {
            putCharColor(scrbuf, x + col * CHAR_WIDTH, y + line * CHAR_HEIGHT, ' ', chrcolor, bgcolor); //disp colored blank
            col = 0;
            ++line;
        }
        else if (*p == '\t') {
            while (TAB_WIDTH - col % TAB_WIDTH) {
                putCharColor(scrbuf, x + col * CHAR_WIDTH, y + line * CHAR_HEIGHT, ' ', chrcolor, bgcolor);
                col++;
            }
        }
        else {
            putCharColor(scrbuf, x + col * CHAR_WIDTH, y + line * CHAR_HEIGHT, *p, chrcolor, bgcolor);
            ++col;
        }
        if (x + col * CHAR_WIDTH >= SCREEN_WIDTH) {
            col = 0;
            ++line;
        }
        if (y + line * CHAR_HEIGHT >= SCREEN_HEIGHT)
            break;
    }
}

void dispStringWithSelection(void* scrbuf, unsigned x, unsigned y, char* message, int selectionstart, int selectionend)
{
    int l = strlen(message);
    int i;
    unsigned line = 0, col = 0;
    for (i = 0; i < l; i++) {
        if (message[i] == '\n') {
            if (i >= selectionstart && i < selectionend)
                putCharColor(scrbuf, x + col * CHAR_WIDTH, y + line * CHAR_HEIGHT, ' ', 0, STR_SELECTION_COLOR); //disp colored blank
            col = 0;
            ++line;
        }
        else if (message[i] == '\t') {
            if (i >= selectionstart && i < selectionend) {
                do {
                    putCharColor(scrbuf, x + col * CHAR_WIDTH, y + line * CHAR_HEIGHT, ' ', 0, STR_SELECTION_COLOR);
                    col++;
                } while (col % TAB_WIDTH);
            }
            else
                col += TAB_WIDTH - (col % TAB_WIDTH);
        }
        else {
            if (i >= selectionstart && i < selectionend)
                putCharColor(scrbuf, x + col * CHAR_WIDTH, y + line * CHAR_HEIGHT, message[i], has_colors ? 0xFFFF : 0xF, STR_SELECTION_COLOR);
            else
                putChar(scrbuf, x + col * CHAR_WIDTH, y + line * CHAR_HEIGHT, message[i]);
            ++col;
        }

        if (x + col * CHAR_WIDTH >= SCREEN_WIDTH) {
            col = 0;
            ++line;
        }
        if (y + line * CHAR_HEIGHT >= SCREEN_HEIGHT)
            break;
    }
}

void dispStringWithSelection_nosoftbreak(void* scrbuf, const char* textbuffer, int pos, int cursorscreencol, int selectionstart, int selectionend)
{
    char* linelist[SCREEN_HEIGHT / CHAR_HEIGHT];
    int poslist[SCREEN_HEIGHT / CHAR_HEIGHT]; //contains the pos of the first char of the respective line
    memset(&linelist, 0, sizeof(char*) * SCREEN_HEIGHT / CHAR_HEIGHT);
    int i;
    const char* p = textbuffer;
    int len = 0;
    for (i = 0; i < SCREEN_HEIGHT / CHAR_HEIGHT; i++) {
        if (strchr(p, '\n') != NULL)
            len = strchr(p, '\n') - p;
        else
            len = strrchr(p, '\0') - p;
        poslist[i] = p - textbuffer;
        linelist[i] = malloc(len + 1);
        if (linelist[i] == NULL) {
            printf("malloc failed\n");
            while (i--)
                free(linelist[i]);
            return;
        }
        strncpy(linelist[i], p, len);
        linelist[i][len] = '\0';
        p += len;
        if (*p == '\0')
            break;
        p++;
    }
    int coloffset = getw_nosoftbreak(textbuffer, pos) - cursorscreencol;
    for (i = 0; linelist[i] != 0; i++) {
        int linepos = gotow_nosoftbreak(linelist[i], 0, coloffset);
        int xoff = getw_nosoftbreak(linelist[i], linepos) - coloffset;
        int w = 0;
        for (; linelist[i][linepos] != '\0' && w < SCREEN_WIDTH / CHAR_WIDTH; linepos++) {
            if (linelist[i][linepos] == '\t') {
                if (poslist[i] + linepos >= selectionstart && poslist[i] + linepos < selectionend) {
                    do {
                        putCharColor(scrbuf, (xoff + w) * CHAR_WIDTH, i * CHAR_HEIGHT, ' ', has_colors ? 0xFFFF : 0xF, STR_SELECTION_COLOR);
                        w++;
                    } while ((w + coloffset + xoff) % TAB_WIDTH);
                }
                else
                    w += TAB_WIDTH - (w + coloffset + xoff) % TAB_WIDTH;
            }
            else {
                if (poslist[i] + linepos >= selectionstart && poslist[i] + linepos < selectionend)
                    putCharColor(scrbuf, (xoff + w) * CHAR_WIDTH, i * CHAR_HEIGHT, linelist[i][linepos], has_colors ? 0xFFFF : 0xF, STR_SELECTION_COLOR);
                else
                    putChar(scrbuf, (xoff + w) * CHAR_WIDTH, i * CHAR_HEIGHT, linelist[i][linepos]);
                w++;
            }
        }
        if (linelist[i + 1] != 0 && poslist[i + 1] - 1 >= selectionstart && poslist[i + 1] - 1 < selectionend)//if newline char is selected, print a selected space
            putCharColor(scrbuf, (xoff + w) * CHAR_WIDTH, i * CHAR_HEIGHT, ' ', has_colors ? 0xFFFF : 0xF, STR_SELECTION_COLOR);
        free(linelist[i]);
    }
}

void dispCursor(void* scrbuf, int offset, char* displinep)
{ //offset=where is cursor relative to displinep
    int row = 0, col = 0;
    int i;
    for (i = 0; i < offset; i++) {
        switch (displinep[i]) {
            case '\n':
                row++;
                col = 0;
                break;
            case '\t':
                col += TAB_WIDTH - (col % TAB_WIDTH);
                break;
            default:
                col++;
        }
        if (col >= SCREEN_WIDTH / CHAR_WIDTH) {
            row++;
            col = 0;
        }
    }
    dispVertLine(scrbuf, col * CHAR_WIDTH, row * CHAR_HEIGHT, CHAR_HEIGHT, 0x0);
}

void dispCursor_nosoftbreak(void* scrbuf, int offset, char* displinep, int cursorscreencol)
{
    char restore = displinep[offset];
    displinep[offset] = '\0';
    int row = countnewl(displinep);
    displinep[offset] = restore;
    dispVertLine(scrbuf, cursorscreencol * CHAR_WIDTH, row * CHAR_HEIGHT, CHAR_HEIGHT, 0x0);
}

inline void dispHorizLine(void* scrbuf, int x, int y, int l, int color)
{
    if (has_colors) {
        while (l--)
            setPixelBuf_color(scrbuf, x++, y, color);
    }
    else{
        while (l--)
            setPixelBuf_grey(scrbuf, x++, y, color);
    }
}

inline void dispVertLine(void* scrbuf, int x, int y, int l, int color)
{
    if (has_colors) {
        while (l--)
            setPixelBuf_color(scrbuf, x, y++, color);
    }
    else{
        while (l--)
            setPixelBuf_grey(scrbuf, x, y++, color);
    }
}

void dispRect(void* scrbuf, int x, int y, int w, int h, int color)
{
    dispHorizLine(scrbuf, x, y, w, color);
    dispHorizLine(scrbuf, x, y + h - 1, w, color);
    dispVertLine(scrbuf, x, y + 1, h - 2, color);
    dispVertLine(scrbuf, x + w - 1, y + 1, h - 2, color);
}

void filledRect(void* scrbuf, int x, int y, int w, int h, int color)
{
    while (h--)
        dispHorizLine(scrbuf, x, y++, w, color);
}
