#include <os.h>
#include <dirent.h>
#include "filebrowser.h"
#include "output.h"
#include "input.h"
#include "fileio.h"

#define TITLE_Y 0
#define TITLE_HEIGHT (CHAR_HEIGHT + 4)
#define DIR_Y (TITLE_Y + TITLE_HEIGHT)
#define DIR_HEIGHT (CHAR_HEIGHT + 10)
#define DIR_DIRLIST_DIST CHAR_HEIGHT
#define DIRLIST_Y (DIR_Y + DIR_HEIGHT + DIR_DIRLIST_DIST)
#define FILENAMEINPUT_Y (SCREEN_HEIGHT - FILENAMEINPUT_HEIGHT)
#define FILENAMEINPUT_HEIGHT (CHAR_HEIGHT + 10)
#define FILENAMEINPUT_DIRLIST_DIST CHAR_HEIGHT
#define FILENAME_WIDTH 240

#define FILES_SHOWN ((FILENAMEINPUT_Y - DIRLIST_Y - FILENAMEINPUT_DIRLIST_DIST) / CHAR_HEIGHT)

#define MENU_SELECTION_COLOR (has_colors?0b011101111111111:0x0)

#define INPUTPAUSE 50

//TODO: display long directory names correctly

//some parts are adapted from hoffa's code

static inline void free_filenames(char** filenames)
{
    int i;
    for (i = 0; filenames[i] != NULL; i++)
        free(filenames[i]);
}

int is_dir(const char* path)
{
    if (!strcmp(path, ".."))//stat would fail on ".." in top-level directories
        return 1;
    struct stat filestat;
    stat(path, &filestat);
    return S_ISDIR(filestat.st_mode);
    /*//if stat returns an error, use this fallback:
    int isdir = 0;
    FILE* f = fopen(path, "r");
    if(f == NULL)
        isdir = 1;
    fclose(f);
    return isdir;*/
}

void get_filesize(const char* path, char* buffer)
{
    struct stat filestat;
    stat(path, &filestat);
    int size = filestat.st_size;
    const char units[4][3] = {"B\0", "kB\0", "MB\0", "GB\0"};
    int i = 0;
    while (size >= 1024) {
        size = size >> 10;
        ++i;
    }
    sprintf(buffer, "%d %s", size + 1, units[i]);
}

int get_filenames(char* folder, char** filenames)
{
    DIR* dp;
    struct dirent* ep;
    int i;
    chdir(folder);
    dp = opendir(folder);
    if (dp == NULL) {
        printf("opendir error\n");
        return -1;
    }
    for (i = 0; (ep = readdir(dp)) != NULL; i++) {
        char* entryname = (char*) malloc(strlen(ep->d_name) + 1);
        if (entryname == NULL) {
            printf("malloc error\n");
            filenames[i] = NULL;
            free_filenames(filenames);
            return -1;
        }
        strcpy(entryname, ep->d_name);
        //printf("%s\n", ep->d_name);
        filenames[i] = entryname;
    }
    closedir(dp);
    filenames[i] = NULL;
    return i;
}

/*int get_filenames_NU(char* folder, char** result){
    chdir(folder);
    struct dstat dir;
    int i = 0;
    if((NU_Get_First(&dir, "*.*"))){
        printf("NU_Get_First error\n");
        NU_Done(&dir);
        abort();
    }
    do {
        char* dirname = (char*) malloc(strlen(dir.filepath) + 1);
        strcpy(dirname, dir.filepath);
        result[i] = dirname;
        i++;
    } while (NU_Get_Next(&dir) == 0);
    NU_Done(&dir);
    return i;
}*/

int filebrowser(void* scrbuf, char* file, char* title)
{
    char currentdir[FILENAME_MAX + 1];
    char* filenames[1024];
    char filenameinput[FILENAME_MAX + 1] = "";
    char blockc = '\0';
    int focus_filenameinput = 1;
    int num_files;
    int filescroll = 0;
    int fileselected = 0;
    int i;
    int inputpause = 0;
    if (get_last_doc(currentdir) != 0) {
        getcwd(currentdir, FILENAME_MAX + 1);
    }
    else {
        *(strrchr(currentdir, '/') + 1) = '\0';
    }

    num_files = get_filenames(currentdir, filenames);
    if (num_files == -1)
        return 1;

    wait_no_key_pressed();
    while (1) {
        //disp:
        memset(scrbuf, 0xFF, SCREEN_BYTES_SIZE);
        filledRect(scrbuf, 0, TITLE_Y, SCREEN_WIDTH, TITLE_HEIGHT, 0);
        dispStringColor(scrbuf, 4, (TITLE_HEIGHT - CHAR_HEIGHT) / 2, title, has_colors ? 0xFFFF : 0xF, 0);
        dispHorizLine(scrbuf, 0, DIR_Y + DIR_HEIGHT - 1, 320, 0);
        dispString(scrbuf, 4, DIR_Y + (DIR_HEIGHT - CHAR_HEIGHT) / 2, currentdir);
        dispHorizLine(scrbuf, 0, FILENAMEINPUT_Y, 320, 0);
        dispString(scrbuf, 4, FILENAMEINPUT_Y + (FILENAMEINPUT_HEIGHT - CHAR_HEIGHT) / 2, "Filename:");
        dispString(scrbuf, 4 + 10 * CHAR_WIDTH, FILENAMEINPUT_Y + (FILENAMEINPUT_HEIGHT - CHAR_HEIGHT) / 2, filenameinput);
        if (focus_filenameinput)//if cursor is in the file name input field, disp an underline to show it
            putChar(scrbuf, 4 + (10 + strlen(filenameinput)) * CHAR_WIDTH, FILENAMEINPUT_Y + (FILENAMEINPUT_HEIGHT - CHAR_HEIGHT) / 2, '_');
        for (i = filescroll; i < filescroll + FILES_SHOWN && i < num_files; i++) {
            //disp file symbol
            if (is_dir(filenames[i]) && strcmp(filenames[i], ".") && strcmp(filenames[i], ".."))
                putChar(scrbuf, CHAR_WIDTH * 2, DIRLIST_Y + (i - filescroll) * CHAR_HEIGHT, '\\');
            //disp filename, filesize and selection
            if (i != fileselected || focus_filenameinput) {
                dispString(scrbuf, CHAR_WIDTH * 3, DIRLIST_Y + (i - filescroll) * CHAR_HEIGHT, filenames[i]);
                if (!is_dir(filenames[i])) {
                    char size[16];
                    get_filesize(filenames[i], size);
                    dispString(scrbuf, FILENAME_WIDTH, DIRLIST_Y + (i - filescroll) * CHAR_HEIGHT, size);
                }
            }
            else {
                filledRect(scrbuf, CHAR_WIDTH * 3, DIRLIST_Y + (i - filescroll) * CHAR_HEIGHT - 1, SCREEN_WIDTH - CHAR_WIDTH * 4, CHAR_HEIGHT + 2, MENU_SELECTION_COLOR);
                dispStringColor(scrbuf, CHAR_WIDTH * 3, DIRLIST_Y + (i - filescroll) * CHAR_HEIGHT, filenames[i], has_colors ? 0xFFFF : 0xF, MENU_SELECTION_COLOR);
                if (!is_dir(filenames[i])) {
                    char size[16];
                    get_filesize(filenames[i], size);
                    dispStringColor(scrbuf, FILENAME_WIDTH, DIRLIST_Y + (i - filescroll) * CHAR_HEIGHT, size, has_colors ? 0xFFFF : 0xF, MENU_SELECTION_COLOR);
                }
            }
        }
        memcpy(SCREEN_BASE_ADDRESS, scrbuf, SCREEN_BYTES_SIZE);


        //input
        if (!any_key_pressed())
            inputpause = 0;
        else
            inputpause--;

        if (isKeyPressed(KEY_NSPIRE_ESC))
            break;
        if (isKeyPressed(KEY_NSPIRE_TAB)) {
            focus_filenameinput = !focus_filenameinput;
            wait_no_key_pressed();
        }

        if (!focus_filenameinput) {
            if (isKeyPressed(KEY_NSPIRE_ENTER) && (blockc != '\n' || inputpause <= 0)) {
                inputpause = INPUTPAUSE;
                blockc = '\n';
                if (is_dir(filenames[fileselected])) {
                    chdir(filenames[fileselected]);
                    filescroll = 0;
                    fileselected = 0;
                    free_filenames(filenames);
                    getcwd(currentdir, FILENAME_MAX + 1);
                    num_files = get_filenames(currentdir, filenames);
                }
                else {
                    strcpy(file, currentdir);
                    strcat(file, filenames[fileselected]);
                    free_filenames(filenames);
                    printf("%s\n", file);
                    return 0;
                }
            }
            else if (isKeyPressed(KEY_NSPIRE_UP) && (blockc != 0x1 || inputpause <= 0)) {
                inputpause = INPUTPAUSE;
                blockc = 0x1;
                fileselected--;
                if (fileselected < 0) {
                    fileselected = num_files - 1;
                    filescroll = num_files - FILES_SHOWN;
                    if (filescroll < 0)
                        filescroll = 0;
                }
                else if (fileselected - filescroll < 0)
                    filescroll--;
                strcpy(filenameinput, filenames[fileselected]);
            }
            else if (isKeyPressed(KEY_NSPIRE_DOWN) && (blockc != 0x2 || inputpause <= 0)) {
                inputpause = INPUTPAUSE;
                blockc = 0x2;
                fileselected++;
                if (fileselected >= num_files) {
                    fileselected = 0;
                    filescroll = 0;
                }
                else if (fileselected - filescroll >= FILES_SHOWN)
                    ++filescroll;
                strcpy(filenameinput, filenames[fileselected]);
            }
        }
        else { //focus_filenameinput
            char c = readc();
            if (c == blockc && inputpause > 0)
                continue;
            if (c != '\0' && c != '\t' && c != '\n') {
                inputpause = INPUTPAUSE;
                blockc = c;
                if (c == '\b') {
                    if (filenameinput[0] != '\0')
                        filenameinput[strlen(filenameinput) - 1] = '\0';
                }
                else {
                    filenameinput[strlen(filenameinput)] = c;
                    filenameinput[strlen(filenameinput) + 1] = '\0';
                }
            }
            if (isKeyPressed(KEY_NSPIRE_UP) || isKeyPressed(KEY_NSPIRE_TAB)) {
                inputpause = INPUTPAUSE;
                focus_filenameinput = 0;
            }
            if (isKeyPressed(KEY_NSPIRE_ENTER)) {
                strcpy(file, currentdir);
                strcat(file, filenameinput);
                free_filenames(filenames);
                printf("%s\n", file);
                return 0;
            }
        }
        sleep(10);
    }

    free_filenames(filenames);
    return 1;
}
