#ifndef _OUTPUT_H_
#define _OUTPUT_H_

#define CHAR_HEIGHT 12
#define CHAR_WIDTH 8

#define TAB_WIDTH 4

void putChar(void* scrbuf, unsigned x, unsigned y, uint8_t chr);

void putCharColor(void* scrbuf, unsigned x, unsigned y, uint8_t chr, uint16_t chrcolor, uint16_t bgcolor);

void dispString(void* scrbuf, unsigned x, unsigned y, const char* message);

/*40 cols à 20 lines*/

void dispStringColor(void* scrbuf, unsigned x, unsigned y, const char* message, uint16_t chrcolor, uint16_t bgcolor);

void dispStringWithSelection(void* scrbuf, unsigned x, unsigned y, char* message, int selectionstart, int selectionend);

void dispStringWithSelection_nosoftbreak(void* scrbuf, const char* textbuffer, int pos, int cursorscreencol, int selectionstart, int selectionend);

void dispCursor(void* scrbuf, int offset, char* displinep);

void dispCursor_nosoftbreak(void* scrbuf, int offset, char* displinep, int cursorscreencol);

void dispRect(void* scrbuf, int x, int y, int w, int h, int color);

void filledRect(void* scrbuf, int x, int y, int w, int h, int color);

void dispHorizLine(void* scrbuf, int x, int y, int l, int color);

void dispVertLine(void* scrbuf, int x, int y, int l, int color);

void dispRect(void* scrbuf, int x, int y, int w, int h, int color);

void filledRect(void* scrbuf, int x, int y, int w, int h, int color);

#endif
