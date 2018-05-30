#ifndef _CLIPBOARD_H_
#define _CLIPBOARD_H_

// clipboard is shared with the OS if the OS version is supported. otherwise falls back to internal clipboard

// sets the clipboard to the first len characters from s. returns 0 on success, 1 on failure
int clipboard_settext(const char* s, unsigned len);

// stores a pointer to the clipboard content in s (should be copied for later use), stores the length in len (not null terminated). returns 0 on success, 1 on failure
int clipboard_gettext(char** s, int* len);

// clean up internal resources. must be called before exiting the program
void clipboard_free();

#endif
