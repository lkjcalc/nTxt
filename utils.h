#ifndef _UTILS_H_
#define _UTILS_H_

void crashsave(char* textbuffer, char* path);//saves the textbuffer to path/crashsave.txt.tns and prints a malloc error message. modifies path


char* getline(char* buffer, int position, int cursorscreenrow);//returns pointer to first character of screen containing position in row=cursorscreenrow

char* getline_nosoftbreak(char* textbuffer, int pos, int cursorscreenrow);

void update_selection(int oldpos, int newpos, int* selectionstart, int* selectionend);//updates selectionstart and selectionend


char* strrstr(char* s1, char* s2);

char* case_insensitive_strstr(char* s1, char* s2);

char* case_insensitive_strrstr(char* s1, char* s2);

#endif
