#ifndef _OUTPUT_H_
#define _OUTPUT_H_

#define CHAR_HEIGHT 12
#define CHAR_WIDTH 8

#define TAB_WIDTH 4

#define MENU_BACKGROUND_COLOR (has_colors ? 0xFFFF : 0xC)
#define MENU_SHADOW_COLOR (has_colors ? 0b0111101111101111 : 0x5)
#define MENU_SELECTION_COLOR (has_colors ? 0b0011101111111111 : 0x0)
#define WHITE_COLOR (has_colors ? 0xFFFF : 0xF)
#define STR_SELECTION_COLOR (has_colors?0b0011101111111111:0xA)


void putChar(void* scrbuf, unsigned x, unsigned y, uint8_t chr);

void putCharColor(void* scrbuf, unsigned x, unsigned y, uint8_t chr, uint16_t chrcolor, uint16_t bgcolor);

void dispString(void* scrbuf, unsigned x, unsigned y, const char* message);

/*40 cols à 20 lines*/

void dispStringColor(void* scrbuf, unsigned x, unsigned y, const char* message, uint16_t chrcolor, uint16_t bgcolor);

void dispStringWithSelection(void* scrbuf, unsigned x, unsigned y, char* message, int selectionstart, int selectionend);

void dispStringWithSelection_nosoftbreak(void* scrbuf, const char* textbuffer, int pos, int cursorscreencol, int selectionstart, int selectionend);

void dispCursor(void* scrbuf, int offset, char* displinep);

void dispCursor_nosoftbreak(void* scrbuf, int offset, char* displinep, int cursorscreencol);


void dispHorizLine(void* scrbuf, int x, int y, int l, int color);

void dispVertLine(void* scrbuf, int x, int y, int l, int color);

void dispRect(void* scrbuf, int x, int y, int w, int h, int color);

void filledRect(void* scrbuf, int x, int y, int w, int h, int color);

void clearScreen(void* scrbuf);


void ntxt_show_msgbox(const char* title, const char* msg);

unsigned ntxt_show_msgbox_2b(const char* title, const char* msg, const char* button1, const char* button2);

unsigned ntxt_show_msgbox_3b(const char* title, const char* msg, const char* button1, const char* button2, const char* button3);

int ntxt_show_msg_user_input(const char* title, const char* msg, char* defaultvalue, char** value_ref);


void showBuffer(void* scrbuf);

void* initScrbuf();

void freeScrbuf(void* scrbuf);

#endif
