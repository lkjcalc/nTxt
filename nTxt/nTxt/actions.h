#ifndef _ACTIONS_H_
#define _ACTIONS_H_

inline int save_on_close_action(void* scrbuf, char* path, char* savepath, char* textbuffer);

inline int open_action(void* scrbuf, char* path, char* savepath, char** textbufferp, int* selectionstart, int* selectionend, int* pos, int* cursorscreenrow, int* cursorscreencol);

inline int silent_open_action(char* file, char* savepath, char** textbufferp);

inline void save_action(void* scrbuf, char* path, char* savepath, char* textbuffer);

inline void saveAs_action(void* scrbuf, char* path, char* savepath, char* textbuffer);

inline void change_line_wrapping_mode_action(int* softnewline, int* pos, int* cursorscreenrow, int* cursorscreencol, int* selectionstart, int* selectionend);

#endif
