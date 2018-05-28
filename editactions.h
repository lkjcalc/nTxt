#ifndef _EDITACTIONS_H_
#define _EDITACTIONS_H_

//unlogged means not saved in the history. used by the undo/redo functions
int insert_range_action_unlogged(char** textbufferp, int pos, int len, char* ins); //checks if enough memory

void remove_range_action_unlogged(char* textbuffer, int pos, int len);

//the normal insert/remove actions. saved in the history so they can be un/redone
int insert_range_action(char** textbufferp, int pos, int len, char* ins); //checks if enough memory

int remove_range_action(char* textbuffer, int pos, int len);


int delete_action(char* textbuffer, int* pos, int* cursorscreenrow, int* cursorscreencol, int* selectionstart, int* selectionend, int softnewline); //difference to remove_range: manages pos, cursorscreenrow etc

int copy_action(char* textbuffer, int selectionstart, int selectionend);

int cut_action(char* textbuffer, int* pos, int* cursorscreenrow, int* cursorscreencol, int* selectionstart, int* selectionend, int softnewline);

int paste_action(char** textbufferp, int* pos, int* cursorscreenrow, int* cursorscreencol, int* selectionstart, int* selectionend, int softnewline);

#endif
