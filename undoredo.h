#ifndef _UNDOREDO_H_
#define _UNDOREDO_H_

//undo and redo functions doing everything on their own

int undoredo_undo(char** textbufferp, int* pos, int* cursorscreenrow, int* cursorscreencol, int softnewline);

int undoredo_redo(char** textbufferp, int* pos, int* cursorscreenrow, int* cursorscreencol, int softnewline);

#endif
