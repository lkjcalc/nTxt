#include <os.h>
#include "clipboard.h"

//returns 0 on success
static unsigned clipboard_get_addrs[] = {0x104A1B94, 0x104860A8, 0x10483218, 0x104839C8, 0, 0, 0x1058ABCC, 0x1056F3F8, 0x1056C814, 0x1056D2AC};
#define os_clipboard_get SYSCALL_CUSTOM(clipboard_get_addrs, int, char**, const char*)

//returns 0 on success
static unsigned clipboard_add_addrs[] = {0x104A1E38, 0x1048634C, 0x104834BC, 0x10483C6C, 0, 0, 0x1058AE58, 0x1056F684, 0x1056CAA0, 0x1056D538};
#define os_clipboard_add SYSCALL_CUSTOM(clipboard_add_addrs, int, const char*, int, const char*)

//returns 0
static unsigned clipboard_reset_addrs[] = {0x104A1EE4, 0x104863F8, 0x10483568, 0x10483D18, 0, 0, 0x1058AF34, 0x1056F760, 0x1056CB7C, 0x1056D614};
#define os_clipboard_reset SYSCALL_CUSTOM(clipboard_reset_addrs, int, void)

//probably unnecessary
//static unsigned clipboard_close_addrs[] = {0x104A1E28, 0x1048633C, 0x104834AC, 0x10483C5C, 0, 0, 0x1058AE48, 0x1056F674, 0x1056CA90, 0x1056D528};
//#define clipboard_close SYSCALL_CUSTOM(clipboard_close_addrs, int, void)

#define CLIPBOARD_TYPE_TEXT "t\0e\0x\0t\0/\0p\0l\0a\0i\0n\0\0"


int clipboard_settext(const char* s, unsigned len){
    const char textplain[] = CLIPBOARD_TYPE_TEXT;
    if(os_clipboard_reset() != 0){
        printf("os_clipboard_reset() failed\n");
        return 1;
    }
    if(os_clipboard_add(s, len, textplain) != 0){
        printf("os_clipboard_add(...) failed\n");
        return 1;
    }
    return 0;
}

int clipboard_gettext(char** s, int* len){
    const char textplain[] = CLIPBOARD_TYPE_TEXT;
    *s = NULL;
    *len = 0;
    int length = os_clipboard_get(s, textplain);
    if(length < 0){
        printf("os_clipboard_get(...) failed\n");
        return 1;
    }
    *len = length;
    return 0;
}
