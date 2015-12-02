#ifndef _FILEIO_H_
#define _FILEIO_H_

int get_last_doc(char* path);

int set_last_doc(char* path);

int open(uint8_t* scrbuf, char* path, char* savepath, char** textbufferp);

int save(uint8_t* scrbuf, char* path, char* savepath, char* textbuffer);

int saveAs(uint8_t* scrbuf, char* path, char* savepath, char* textbuffer);

#endif
