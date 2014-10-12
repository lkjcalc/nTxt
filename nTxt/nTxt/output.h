#ifndef _OUTPUT_H_
#define _OUTPUT_H_

#define CHAR_HEIGHT 12
#define CHAR_WIDTH 8

#define TAB_WIDTH 4
#define MAX_COL SCREEN_WIDTH/CHAR_WIDTH
#define MAX_LINE SCREEN_HEIGHT/CHAR_HEIGHT

inline void setPixelBuf(void* scrbuf, unsigned x, unsigned y, uint16_t color);

void putChar(void* scrbuf, unsigned x, unsigned y, uint8_t chr);

void putCharColor(void* scrbuf, unsigned x, unsigned y, uint8_t chr, uint16_t chrcolor, uint16_t bgcolor);

void dispString(void* scrbuf, unsigned x, unsigned y, const char* message);/*40 cols à 20 lines*/

void dispStringColor(void* scrbuf, unsigned x, unsigned y, const char* message, uint16_t chrcolor, uint16_t bgcolor);

void dispStringWithSelection(void* scrbuf, unsigned x, unsigned y, char* message, int selectionstart, int selectionend);

void dispStringWithSelection_nosoftbreak(void* scrbuf, const char* textbuffer, int pos, int cursorscreencol, int selectionstart, int selectionend);

void dispCursor(void* scrbuf, int offset, char* displinep);

void dispCursor_nosoftbreak(void* scrbuf, int offset, char* displinep, int cursorscreencol);

void dispRect(void* scrbuf, int x, int y, int w, int h, int color);

void filledRect(void* scrbuf, int x, int y, int w, int h, int color);

inline void dispHorizLine(void* scrbuf, int x, int y, int l, int color);

inline void dispVertLine(void* scrbuf, int x, int y, int l, int color);

void dispRect(void* scrbuf, int x, int y, int w, int h, int color);

void filledRect(void* scrbuf, int x, int y, int w, int h, int color);

#endif
