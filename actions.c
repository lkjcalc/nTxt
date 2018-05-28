#include <os.h>
#include "actions.h"
#include "fileio.h"

int save_on_close_action(void* scrbuf, char* path, char* savepath, char* textbuffer)
{ //returns 0 if closing is cancelled, 1 otherwise
    FILE* file;
    wait_no_key_pressed();
    unsigned inp = show_msgbox_3b("", "Save Document?", "Yes", "No", "Cancel");
    if (inp == 1) {
        if (save(scrbuf, path, savepath, textbuffer) == 1) {
            file = fopen(strcat(path, "noname.txt.tns"), "ab");
            *(strrchr(path, '/') + 1) = '\0'; //revert the strcat
            fwrite(textbuffer, 1, strlen(textbuffer), file);
            fclose(file);
        }
        wait_no_key_pressed();
        return 1;
    }
    wait_no_key_pressed();
    if (inp == 2)
        return 1;
    return 0;
}

int open_action(void* scrbuf, char* path, char* savepath, char** textbufferp, int* selectionstart, int* selectionend, int* pos, int* cursorscreenrow, int* cursorscreencol)
{
    int ret;
    ret = open(scrbuf, path, savepath, textbufferp);
    if (ret == 0) { //only if really opened a new file
        *selectionstart = 0;
        *selectionend = 0;
        *pos = 0;
        *cursorscreenrow = 0;
        *cursorscreencol = 0;
    }
    else if (ret == -1) {
        wait_no_key_pressed();
        show_msgbox("", "Couldn't open file!");
        wait_no_key_pressed();
        return 1;
    }
    wait_no_key_pressed();
    return 0;
}

int silent_open_action(char* file, char* savepath, char** textbufferp)
{
    strcpy(savepath, file);
    FILE* f = fopen(savepath, "rb");
    if (f == NULL)
        return 1;
    fseek(f, 0, SEEK_END);
    size_t filelen = (size_t) ftell(f);
    rewind(f);
    free(*textbufferp);
    *textbufferp = malloc(filelen + 1024 - (filelen % 1024));
    if (*textbufferp == NULL)
        return 1;
    fread(*textbufferp, 1, filelen, f);
    fclose(f);
    printf("%i\n", (int) filelen);
    (*textbufferp)[filelen] = '\0';
    return 0;
}

void save_action(void* scrbuf, char* path, char* savepath, char* textbuffer)
{
    if (save(scrbuf, path, savepath, textbuffer) == 1) {
        wait_no_key_pressed();
        show_msgbox("", "File wasn't saved!");
    }
    wait_no_key_pressed();
}

void saveAs_action(void* scrbuf, char* path, char* savepath, char* textbuffer)
{
    if (saveAs(scrbuf, path, savepath, textbuffer) == 1) {
        wait_no_key_pressed();
        show_msgbox("", "File wasn't saved!");
    }
    wait_no_key_pressed();
}

void change_line_wrapping_mode_action(int* softnewline, int* pos, int* cursorscreenrow, int* cursorscreencol, int* selectionstart, int* selectionend)
{
    *softnewline = !(*softnewline);
    *pos = 0;
    *cursorscreenrow = 0;
    *cursorscreencol = 0;
    *selectionstart = 0;
    *selectionend = 0;
    wait_no_key_pressed();
}
