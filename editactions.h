#ifndef _EDITACTIONS_H_
#define _EDITACTIONS_H_

//unlogged means not saved in the history. used by the undo/redo functions
inline int insert_range_action_unlogged(char** textbufferp, int pos, int len, char* ins); //checks if enough memory

inline void remove_range_action_unlogged(char* textbuffer, int pos, int len);

//the normal insert/remove actions. saved in the history so they can be un/redone
inline int insert_range_action(char** textbufferp, int pos, int len, char* ins); //checks if enough memory

inline int remove_range_action(char* textbuffer, int pos, int len);


inline int delete_action(char* textbuffer, int* pos, int* cursorscreenrow, int* cursorscreencol, int* selectionstart, int* selectionend, int softnewline); //difference to remove_range: manages pos, cursorscreenrow etc

inline int copy_action(char* textbuffer, int selectionstart, int selectionend);

inline int cut_action(char* textbuffer, int* pos, int* cursorscreenrow, int* cursorscreencol, int* selectionstart, int* selectionend, int softnewline);

inline int paste_action(char** textbufferp, int* pos, int* cursorscreenrow, int* cursorscreencol, int* selectionstart, int* selectionend, int softnewline);

#endif
