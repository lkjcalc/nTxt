#include <os.h>
#include "fileio.h"
#include "filebrowser.h"
#include "actions.h"

int get_last_doc(char* path)
{
    FILE* file;
    file = fopen("/documents/ntxt.last.tns", "r");
    if (file == NULL)
        return 1;
    fread(path, 1, FILENAME_MAX, file);
    fclose(file);

    printf("Last path: %s\n", path);

    return 0;
}

int set_last_doc(char* path)
{
    FILE* file;
    file = fopen("/documents/ntxt.last.tns", "w");
    if (file == NULL)
        return 1;
    fwrite(path, 1, strlen(path) + 1, file);
    fclose(file);

    printf("Set last path: %s\n", path);

    return 0;
}

int open(uint8_t* scrbuf, char* path, char* savepath, char** textbufferp)
{
    FILE* file;
    char tmp[FILENAME_MAX];
    int filelen;
    if (save_on_close_action(scrbuf, savepath, path, *textbufferp) == 0)//if cancelled
        return 1;
    chdir(path);
    if (filebrowser(scrbuf, tmp, "Open File") == 1)
        return 1;
    file = fopen(tmp, "rb");
    if (file == NULL) {
        wait_no_key_pressed();
        ntxt_show_msgbox("", "This file doesn't exist!");
        return 1;
    }
    strcpy(savepath, tmp);
    fseek(file, 0, SEEK_END);
    filelen = ftell(file);
    printf("filelen %i\n", filelen);
    rewind(file);
    free(*textbufferp);
    *textbufferp = malloc(filelen + 1024 - (filelen % 1024));
    if (*textbufferp == NULL) {
        printf("open: malloc failed\n");
        fclose(file);
        return -1;
    }
    fread(*textbufferp, 1, filelen, file);
    fclose(file);
    (*textbufferp)[filelen] = '\0';

    set_last_doc(tmp);
    return 0;
}

int save(uint8_t* scrbuf, char* path, char* savepath, char* textbuffer)
{
    FILE* file;
    if (strlen(savepath) == 0)
        return saveAs(scrbuf, path, savepath, textbuffer);
    else {
        file = fopen(savepath, "wb");
        fwrite(textbuffer, 1, strlen(textbuffer), file);
        fclose(file);
        set_last_doc(savepath);
        return 0;
    }
}

int saveAs(uint8_t* scrbuf, char* path, char* savepath, char* textbuffer)
{
    FILE* file;
    char tmp[FILENAME_MAX];
    chdir(path);
    wait_no_key_pressed();
    if (filebrowser(scrbuf, tmp, "Save As...") == 1)
        return 1;
    strcpy(savepath, tmp);
    file = fopen(savepath, "r");
    if (file != NULL) {
        fclose(file);
        if (ntxt_show_msgbox_2b("Save As", "This file already exists! Do you really want to overwrite it?", "Yes", "No") == 2)
            return 1;
    }
    if (strstr(savepath, ".tns") != strrchr(savepath, '\0') - 4) {
        if (ntxt_show_msgbox_2b("No filename extensinon .tns", "Append .tns to the path? Else the file won't be visible in the My Documents screen.", "Yes", "No") == 1)
            strcat(savepath, ".tns");
    }
    file = fopen(savepath, "wb");
    fwrite(textbuffer, 1, strlen(textbuffer), file);
    fclose(file);
    set_last_doc(savepath);
    return 0;
}
