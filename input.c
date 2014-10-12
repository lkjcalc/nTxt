#include <os.h>
#include "input.h"

uint8_t readarrow()
{
    if (isKeyPressed(KEY_NSPIRE_UP))
        return ARROW_UP_NUM;
    if (isKeyPressed(KEY_NSPIRE_RIGHT))
        return ARROW_RIGHT_NUM;
    if (isKeyPressed(KEY_NSPIRE_DOWN))
        return ARROW_DOWN_NUM;
    if (isKeyPressed(KEY_NSPIRE_LEFT))
        return ARROW_LEFT_NUM;
    return 0;
}

char readc()
{
    if (isKeyPressed(KEY_NSPIRE_TAB))
        return '\t';
    if (isKeyPressed(KEY_NSPIRE_ENTER))
        return '\n';
    if (isKeyPressed(KEY_NSPIRE_DEL))
        return '\b';
    if (isKeyPressed(KEY_NSPIRE_A))
        return isKeyPressed(KEY_NSPIRE_SHIFT) ? 'A' : 'a';
    if (isKeyPressed(KEY_NSPIRE_B))
        return isKeyPressed(KEY_NSPIRE_SHIFT) ? 'B' : 'b';
    if (isKeyPressed(KEY_NSPIRE_C))
        return isKeyPressed(KEY_NSPIRE_SHIFT) ? 'C' : 'c';
    if (isKeyPressed(KEY_NSPIRE_D))
        return isKeyPressed(KEY_NSPIRE_SHIFT) ? 'D' : 'd';
    if (isKeyPressed(KEY_NSPIRE_E))
        return isKeyPressed(KEY_NSPIRE_SHIFT) ? 'E' : 'e';
    if (isKeyPressed(KEY_NSPIRE_F))
        return isKeyPressed(KEY_NSPIRE_SHIFT) ? 'F' : 'f';
    if (isKeyPressed(KEY_NSPIRE_G))
        return isKeyPressed(KEY_NSPIRE_SHIFT) ? 'G' : 'g';
    if (isKeyPressed(KEY_NSPIRE_H))
        return isKeyPressed(KEY_NSPIRE_SHIFT) ? 'H' : 'h';
    if (isKeyPressed(KEY_NSPIRE_I))
        return isKeyPressed(KEY_NSPIRE_SHIFT) ? 'I' : 'i';
    if (isKeyPressed(KEY_NSPIRE_J))
        return isKeyPressed(KEY_NSPIRE_SHIFT) ? 'J' : 'j';
    if (isKeyPressed(KEY_NSPIRE_K))
        return isKeyPressed(KEY_NSPIRE_SHIFT) ? 'K' : 'k';
    if (isKeyPressed(KEY_NSPIRE_L))
        return isKeyPressed(KEY_NSPIRE_SHIFT) ? 'L' : 'l';
    if (isKeyPressed(KEY_NSPIRE_M))
        return isKeyPressed(KEY_NSPIRE_SHIFT) ? 'M' : 'm';
    if (isKeyPressed(KEY_NSPIRE_N))
        return isKeyPressed(KEY_NSPIRE_SHIFT) ? 'N' : 'n';
    if (isKeyPressed(KEY_NSPIRE_O))
        return isKeyPressed(KEY_NSPIRE_SHIFT) ? 'O' : 'o';
    if (isKeyPressed(KEY_NSPIRE_P))
        return isKeyPressed(KEY_NSPIRE_SHIFT) ? 'P' : 'p';
    if (isKeyPressed(KEY_NSPIRE_Q))
        return isKeyPressed(KEY_NSPIRE_SHIFT) ? 'Q' : 'q';
    if (isKeyPressed(KEY_NSPIRE_R))
        return isKeyPressed(KEY_NSPIRE_SHIFT) ? 'R' : 'r';
    if (isKeyPressed(KEY_NSPIRE_S))
        return isKeyPressed(KEY_NSPIRE_SHIFT) ? 'S' : 's';
    if (isKeyPressed(KEY_NSPIRE_T))
        return isKeyPressed(KEY_NSPIRE_SHIFT) ? 'T' : 't';
    if (isKeyPressed(KEY_NSPIRE_U))
        return isKeyPressed(KEY_NSPIRE_SHIFT) ? 'U' : 'u';
    if (isKeyPressed(KEY_NSPIRE_V))
        return isKeyPressed(KEY_NSPIRE_SHIFT) ? 'V' : 'v';
    if (isKeyPressed(KEY_NSPIRE_W))
        return isKeyPressed(KEY_NSPIRE_SHIFT) ? 'W' : 'w';
    if (isKeyPressed(KEY_NSPIRE_X))
        return isKeyPressed(KEY_NSPIRE_SHIFT) ? 'X' : 'x';
    if (isKeyPressed(KEY_NSPIRE_Y))
        return isKeyPressed(KEY_NSPIRE_SHIFT) ? 'Y' : 'y';
    if (isKeyPressed(KEY_NSPIRE_Z))
        return isKeyPressed(KEY_NSPIRE_SHIFT) ? 'Z' : 'z';
    if (isKeyPressed(KEY_NSPIRE_1))
        return '1';
    if (isKeyPressed(KEY_NSPIRE_2))
        return '2';
    if (isKeyPressed(KEY_NSPIRE_3))
        return '3';
    if (isKeyPressed(KEY_NSPIRE_4))
        return '4';
    if (isKeyPressed(KEY_NSPIRE_5))
        return '5';
    if (isKeyPressed(KEY_NSPIRE_6))
        return '6';
    if (isKeyPressed(KEY_NSPIRE_7))
        return '7';
    if (isKeyPressed(KEY_NSPIRE_8))
        return '8';
    if (isKeyPressed(KEY_NSPIRE_9))
        return '9';
    if (isKeyPressed(KEY_NSPIRE_0))
        return '0';
    if (isKeyPressed(KEY_NSPIRE_SPACE))
        return ' ';
    if (isKeyPressed(KEY_NSPIRE_PERIOD))
        return isKeyPressed(KEY_NSPIRE_SHIFT) ? ':' : '.';
    if (isKeyPressed(KEY_NSPIRE_COLON))
        return ':';
    if (isKeyPressed(KEY_NSPIRE_COMMA))
        return isKeyPressed(KEY_NSPIRE_SHIFT) ? ';' : ',';
    if (isKeyPressed(KEY_NSPIRE_QUESEXCL) || isKeyPressed(KEY_NSPIRE_QUES))
        return isKeyPressed(KEY_NSPIRE_SHIFT) ? '!' : '?';
    if (isKeyPressed(KEY_NSPIRE_PLUS))
        return isKeyPressed(KEY_NSPIRE_SHIFT) ? '&' : '+';
    if (isKeyPressed(KEY_NSPIRE_NEGATIVE) || isKeyPressed(KEY_NSPIRE_MINUS)) {
        if (isKeyPressed(KEY_NSPIRE_SHIFT))
            return '_';
        if (isKeyPressed(KEY_NSPIRE_CTRL))
            return '~';
        else
            return '-';
    }
    if (isKeyPressed(KEY_NSPIRE_MULTIPLY))
        return '*';
    if (isKeyPressed(KEY_NSPIRE_DIVIDE)) {
        if (isKeyPressed(KEY_NSPIRE_SHIFT))
            return '\\';
        if (isKeyPressed(KEY_NSPIRE_CTRL))
            return '|';
        else
            return '/';
    }
    if (isKeyPressed(KEY_NSPIRE_BAR))
        return '|';
    if (isKeyPressed(KEY_NSPIRE_QUOTE))
        return '"';
    if (isKeyPressed(KEY_NSPIRE_APOSTROPHE))
        return '\'';
    if (isKeyPressed(KEY_NSPIRE_EXP)) {
        if (isKeyPressed(KEY_NSPIRE_SHIFT))
            return '"';
        if (isKeyPressed(KEY_NSPIRE_CTRL))
            return '\'';
        else
            return '^';
    }
    if (isKeyPressed(KEY_NSPIRE_EQU)) {
        if (isKeyPressed(KEY_NSPIRE_SHIFT))
            return '>';
        if (isKeyPressed(KEY_NSPIRE_CTRL))
            return '<';
        else
            return '=';
    }
    if (isKeyPressed(KEY_NSPIRE_LTHAN))
        return '<';
    if (isKeyPressed(KEY_NSPIRE_GTHAN))
        return '>';
    if (isKeyPressed(KEY_NSPIRE_LP)) {
        if (isKeyPressed(KEY_NSPIRE_SHIFT))
            return '[';
        if (isKeyPressed(KEY_NSPIRE_CTRL))
            return '{';
        else
            return '(';
    }
    if (isKeyPressed(KEY_NSPIRE_RP)) {
        if (isKeyPressed(KEY_NSPIRE_SHIFT))
            return ']';
        if (isKeyPressed(KEY_NSPIRE_CTRL))
            return '}';
        else
            return ')';
    }
    if (isKeyPressed(KEY_NSPIRE_FLAG)) {
        return '%';
    }
    return '\0';
}

//Ersatz fuer reads: show_msg_user_input(...) aus libndls
/*char* reads(uint8_t* scrbuf, int x, int y, char* str, int num){
    int i = 0;
    char tmp;
    if(num < 0)
        return str;
    while((i<num - 1) && !isKeyPressed(KEY_NSPIRE_ESC)){
        tmp = readc();
        if(!isKeyPressed(KEY_NSPIRE_ENTER) && !isKeyPressed(KEY_NSPIRE_ESC) && !isKeyPressed(KEY_NSPIRE_DEL))//so these keys work but others are only read once
            wait_no_key_pressed();
        if(isKeyPressed(KEY_NSPIRE_DEL) && (i > 0)){
            str[--i] = '\0';
            tmp = 0;
            wait_no_key_pressed();
        }
        if(isKeyPressed(KEY_NSPIRE_ENTER)){
            str[i] = '\0';
            return str;
        }
        if(tmp != 0){
            str[i] = tmp;
            i++;
        }
        putChar(scrbuf, x + i * CHAR_WIDTH, y, ' ');
        dispString(scrbuf, x, y, str);
        memcpy(SCREEN_BASE_ADDRESS, scrbuf, SCREEN_BYTES_SIZE);
    }
    str[i] = '\0';
    return str;
}*/
/*struct event {
    unsigned int timestamp;
    unsigned short type;
    unsigned short ascii;
    unsigned int key;
    unsigned int unknown[3];
    unsigned int control;
};

static const unsigned get_event_addrs[] = {0x1016C354, 0x1016e36c, 0x101b2724, 0x101b2fec};  //OS 1.7 ,OS2.0.1.60
#define get_event SYSCALL_CUSTOM(get_event_addrs, int,  struct event *)

int getch() {
    struct event event;
    do {
        get_event(&event);
    } while (event.type != 8 || event.ascii == 0);
    return event.ascii;
}
*/
