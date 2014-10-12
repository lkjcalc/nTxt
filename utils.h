#ifndef _UTILS_H_
#define _UTILS_H_

#define min(a, b) (((a)<(b))?(a):(b))
#define max(a, b) (((a)>(b))?(a):(b))

void crashsave(char* textbuffer, char* path); //saves the textbuffer to path/crashsave.txt.tns and prints a malloc error message. modifies path

// unused ?
char* _getline(char* buffer, int position, int cursorscreenrow); //returns pointer to first character of screen containing position in row=cursorscreenrow

char* getline_nosoftbreak(char* textbuffer, int pos, int cursorscreenrow);

void update_selection(int oldpos, int newpos, int* selectionstart, int* selectionend); //updates selectionstart and selectionend

// unused ?
char* strrstr(char* s1, char* s2);

char* case_insensitive_strstr(char* s1, char* s2);

char* case_insensitive_strrstr(char* s1, char* s2);

#endif
