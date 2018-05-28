#ifndef _NAVIGATION_H_
#define _NAVIGATION_H_

int linenum(char* textbuffer, int pos);

int softlinenum(char* textbuffer, int pos);


int countsoftnewl(char* textbuffer);

int countsoftnewl_withinitialw(char* textbuffer, int initialw, int countlen);

int countnewl(char* textbuffer);


int prevline(char* textbuffer, int pos);

int prevline_nosoftbreak(char* textbuffer, int pos);


int currentline(char* textbuffer, int pos);

int currentline_nosoftbreak(const char* textbuffer, int pos);


int checkednextline(char* textbuffer, int pos); //if in last line, returns 0

int checkednextline_nosoftbreak(const char* textbuffer, int pos);


int nextline(char* textbuffer, int pos); //if in last line, returns start of last line

int nextline_nosoftbreak(char* textbuffer, int pos);


int _getw(char* textbuffer, int pos);

int getw_nosoftbreak(const char* textbuffer, int pos);


int gotow(char* textbuffer, int pos, int w);

int gotow_nosoftbreak(char* textbuffer, int pos, int w);


int go_up(char* textbuffer, int pos);

int go_up_nosoftbreak(char* textbuffer, int pos);


int go_down(char* textbuffer, int pos);

int go_down_nosoftbreak(char* textbuffer, int pos);


int go_left(char* textbuffer, int pos);

int go_left_nosoftbreak(char* textbuffer, int pos);


int go_right(char* textbuffer, int pos);

int go_right_nosoftbreak(char* textbuffer, int pos);

#endif
