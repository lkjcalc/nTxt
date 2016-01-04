#ifndef _CLIPBOARD_H_
#define _CLIPBOARD_H_

//sets the clipboard to the first len characters from s. returns 0 on success, 1 on failure
int clipboard_settext(const char* s, unsigned len);

//stores a pointer to the clipboard content in s (should be copied for later use), stores the length in len (not null terminated). returns 0 on success, 1 on failure
int clipboard_gettext(char** s, int* len);

#endif
