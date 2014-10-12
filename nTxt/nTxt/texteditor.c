#include <os.h>
#include "input.h"
#include "output.h"
#include "utils.h"
#include "menu.h"
#include "fileio.h"
#include "navigation.h"
#include "actions.h"
#include "editactions.h"
#include "history.h"
#include "undoredo.h"

#define DELAY_FIRST 60 /*~650 to 850ms*/
#define DELAY_AFTER 6

#define HISTORY_SIZE 100

/*  v21:
    - history zu einem nicht-globalen objekt machen, damit sie für tabs geeignet ist
    - löschen der history bei schliessen an anderen ort
    - alles auf tabs vorbereiten (d.h. schauen dass alles weiterhin funktioniert)
        -> structs vorbereiten vl.
    - tabs

    v22:
    - settings fenster (zb mit checkboxen)
    - windows-style newline (\r \n) erkennen und optional benutzen (wahrscheinlich modus eines docs erkennen, alle ersetzen, beim speichern zurückschreiben)
    - code cleanen
    - letzte graphische glitches bei undo und redo wegmachen (zB "Der Baum ist ein Krueppel.", dann "genialer" vor Krueppel, dann alles undo, dann redo, bei genialer glitch)


    wenn syscalls:
    - custom syscalls durch echte ersetzen bei clipboard
    - unicode
    - verschieden grosse fonts
    - sonderzeichenpopup
    - usb

    - disp wenn selections verbessern: nur ein fünftel so schnell wie ohne selections (vor verschiebung von has_colors halb so schnell, aber optim brachte nur *2 statt *5)
    - speicher wird gehortet aber nicht gemerkt dass schon alloziert! bei löschen kein realloc und keine var die allozierten speicher sich merkt!
        -> kein grosses problem weil speicher nur behalten nachdem nicht mehr nötig, kein leak
        -> merken, wie viel speicher alloziert
        -> komplett neues speichermanagement, das für tabs geeignet ist
    - replace
    - goto line
    - cursorscreenfreie lösung? damit nicht probleme wie bei multilines
    - gui improvements
    - warnung falls an anderem ort im filesystem als in /documents/ -> weil absolute paths einfach zu erkennen
    - bei esc nur meldung anzeigen falls änderung seit letztem speichern
    - vlt. echten timer einsetzen für input stat zählervariablen -> kann schlafen bis etwas passiert (keypress)
    - scrollbalken
    - scrolling vl. mit swipen, oder so
    - option zum automatischen einfügen von leerzeichen statt tabs
    - ist editieren von files die '\0' enthalten nötig? würde nötig machen mem... statt str... und v.a. ____andere kontrolle der bufferlänge_____
    - main übersichtlicher machen
    - filebrowser optimieren: fps sinkt rapide mit anzahl einträgen im aktuellen verzeichnis (auf 20 bei 20 einträgen) und vl auch wenn oberste nicht sichtbar
    - scrolling bei ctrl navigs konsistent mit os machen
    - überdenken des auto-.txt anhängen
    - line numbers
    - syntax highlighting
    - slowdown bei grossen dateien untersuchen (2MB ist halb so schnell wie normal gross, 4MB nochmal halb)
    - irgendwie das management von cursorscreencol usw von anderer funktionalität trennen
*/

/*
mem:    0   |   1   |   2   |   ...
pos: 0      1       2       3       ...
*/

int main(int argc, char* argv[]){
    assert_ndless_rev(872);
    cfg_register_fileext("txt", "nTxt");

    void* scrbuf = malloc(SCREEN_BYTES_SIZE);
    if(scrbuf == NULL){
        printf("malloc failed\n");
        return 1;
    }
    memset(scrbuf, 0xFF, SCREEN_BYTES_SIZE);

    char path[FILENAME_MAX+1];
    strcpy(path, argv[0]);
    *(strrchr(path,'/') + 1) = '\0';
    char savepath[256];
    savepath[0] = '\0';

    char* textbuffer = malloc(1024);
    if(textbuffer == NULL){
        printf("malloc failed\n");
        free(scrbuf);
        return 1;
    }
    textbuffer[0] = '\0';

    char inpbuf = '\0';
    uint8_t arrowbuf = 0;
    char blockkey = '\0';
    int delaycounter = DELAY_FIRST;

    int pos = 0;
    int selectionstart = 0;
    int selectionend = 0;

    int cursorscreenrow = 0;
    int cursorscreencol = 0;
    char* displinep;
    int softnewline = 0;

    char* searchstr = NULL;

    //if opened through file association
    if(argc >= 2){
        if(silent_open_action(argv[1], savepath, &textbuffer) == 1){
            free(textbuffer);
            free(scrbuf);
            return 1;
        }
    }

    if(history_init(HISTORY_SIZE) == 1){
        free(textbuffer);
        free(scrbuf);
        return 1;
    }

    int fps = 0;
    uint32_t oldtime = 0;

    while(1){
        if(oldtime != *(uint32_t*)0x90090000){
            oldtime = *(uint32_t*)0x90090000;
            printf("fps:%i\n",fps);
            fps = 0;
        }
        ++fps;

        sleep(10);//i.e. <= 100fps

        //disp
        memcpy(SCREEN_BASE_ADDRESS, scrbuf, SCREEN_BYTES_SIZE);
        memset(scrbuf, 0xFF, SCREEN_BYTES_SIZE);

        if(softnewline){
            displinep = getline(textbuffer, pos, cursorscreenrow);
            dispStringWithSelection(scrbuf, 0, 0, displinep, selectionstart - (displinep - textbuffer), selectionend - (displinep - textbuffer));
            dispCursor(scrbuf, pos-(displinep-textbuffer), displinep);
        }
        else{
            displinep = getline_nosoftbreak(textbuffer, pos, cursorscreenrow);
            dispStringWithSelection_nosoftbreak(scrbuf, displinep, pos - (displinep - textbuffer), cursorscreencol, selectionstart - (displinep - textbuffer), selectionend - (displinep - textbuffer));
            dispCursor_nosoftbreak(scrbuf, pos-(displinep-textbuffer), displinep, cursorscreencol);
        }

        //quit nTxt
        if(on_key_pressed() || isKeyPressed(KEY_NSPIRE_ESC)){
            if(save_on_close_action(scrbuf, path, savepath, textbuffer) == 1)//close only if closing isn't cancelled
                break;
        }

        //open menu:
        if(isKeyPressed(KEY_NSPIRE_MENU) || isKeyPressed(KEY_NSPIRE_DOC)){
            const char* menuentries[] = {
                "Open    Ctrl+O",
                "Save    Ctrl+S",
                "Save as",
                "Undo    Ctrl+Z",
                "Redo    Ctrl+Y",
                "Change Text Wrapping Mode",
                "Quit    Esc",
                ""
            };
            int menuretval;
            menuretval = menu(scrbuf, menuentries);
            if(menuretval == 0)
                wait_no_key_pressed();
            else if(menuretval == 1){
                history_free();
                if(history_init(HISTORY_SIZE) == 1){
                    crashsave(textbuffer, path);
                    break;
                }
                if(open_action(scrbuf, path, savepath, &textbuffer, &selectionstart, &selectionend, &pos, &cursorscreenrow, &cursorscreencol) == 1)
                    break;
            }
            else if(menuretval == 2)
                save_action(scrbuf, path, savepath, textbuffer);
            else if(menuretval == 3)
                saveAs_action(scrbuf, path, savepath, textbuffer);
            else if(menuretval == 4){
                if(undoredo_undo(&textbuffer, &pos, &cursorscreenrow, &cursorscreencol, softnewline)){
                    crashsave(textbuffer, path);
                    break;
                }
            }
            else if(menuretval == 5){
                if(undoredo_redo(&textbuffer, &pos, &cursorscreenrow, &cursorscreencol, softnewline) == 1){
                    crashsave(textbuffer, path);
                    break;
                }
            }
            else if(menuretval == 6)
                change_line_wrapping_mode_action(&softnewline, &pos, &cursorscreenrow, &cursorscreencol, &selectionstart, &selectionend);
            else if(menuretval == 7){
                if(save_on_close_action(scrbuf, path, savepath, textbuffer) == 1)
                    break;
            }
            while(isKeyPressed(KEY_NSPIRE_ENTER))
                idle();
        }

        //open document:
        else if(isKeyPressed(KEY_NSPIRE_CTRL) && isKeyPressed(KEY_NSPIRE_O)){
            history_free();
            if(history_init(HISTORY_SIZE) == 1){
                crashsave(textbuffer, path);
                break;
            }
            if(open_action(scrbuf, path, savepath, &textbuffer, &selectionstart, &selectionend, &pos, &cursorscreenrow, &cursorscreencol) == 1)
                break;
        }

        //save document:
        else if(isKeyPressed(KEY_NSPIRE_CTRL) && isKeyPressed(KEY_NSPIRE_S))
            save_action(scrbuf, path, savepath, textbuffer);

        //copy:
        if(isKeyPressed(KEY_NSPIRE_CTRL) && isKeyPressed(KEY_NSPIRE_C)){
            if(copy_action(textbuffer, selectionstart, selectionend) == 1){
                crashsave(textbuffer, path);
                break;
            }
            while(isKeyPressed(KEY_NSPIRE_C))
                idle();
        }

        //cut:
        if(isKeyPressed(KEY_NSPIRE_CTRL) && isKeyPressed(KEY_NSPIRE_X)){
            if(cut_action(textbuffer, &pos, &cursorscreenrow, &cursorscreencol, &selectionstart, &selectionend, softnewline) == 1){
                crashsave(textbuffer, path);
                break;
            }
            while(isKeyPressed(KEY_NSPIRE_X))
                idle();
        }

        //paste:
        if(isKeyPressed(KEY_NSPIRE_CTRL) && isKeyPressed(KEY_NSPIRE_V)){
            if(paste_action(&textbuffer, &pos, &cursorscreenrow, &cursorscreencol, &selectionstart, &selectionend, softnewline) == 1){
                crashsave(textbuffer, path);
                break;
            }
            while(isKeyPressed(KEY_NSPIRE_V))
                idle();
        }

        //find
        if(isKeyPressed(KEY_NSPIRE_CTRL) && isKeyPressed(KEY_NSPIRE_F)){
            int inputlen;
            char* tmp;
            char* foundat;
            wait_no_key_pressed();
            inputlen = show_msg_user_input("Find", "Text to search for", "", &tmp);
            if(inputlen != -1){
                free(searchstr);
                searchstr = (char*) malloc(strlen(tmp) + 1);
                if(searchstr == NULL){
                    crashsave(textbuffer, path);
                    break;
                }
                strcpy(searchstr, tmp);
                free(tmp);
                foundat = case_insensitive_strstr(textbuffer, searchstr);
                if(foundat != NULL){
                    selectionstart = foundat - textbuffer;
                    selectionend = selectionstart + strlen(searchstr);
                    pos = selectionend;
                    cursorscreenrow = SCREEN_HEIGHT/CHAR_HEIGHT/2;
                    if(softnewline == FALSE)
                        cursorscreencol = getw_nosoftbreak(textbuffer, pos);
                    if(cursorscreencol > SCREEN_WIDTH/CHAR_WIDTH)
                        cursorscreencol = SCREEN_WIDTH/CHAR_WIDTH/2;
                }
                else
                    show_msgbox("", "Not found");
            }
            wait_no_key_pressed();
        }

        //find next
        if(isKeyPressed(KEY_NSPIRE_CTRL) && !isKeyPressed(KEY_NSPIRE_SHIFT) && isKeyPressed(KEY_NSPIRE_G)){
            if(searchstr != NULL){
                char* tmp = case_insensitive_strstr(textbuffer + pos, searchstr);
                if(tmp == NULL){
                    show_msgbox("", "Reached the end of the document");
                    tmp = case_insensitive_strstr(textbuffer, searchstr);
                }
                if(tmp == NULL)
                    show_msgbox("", "Not found in document");
                else{
                    selectionstart = tmp - textbuffer;
                    selectionend = selectionstart + strlen(searchstr);
                    pos = selectionend;
                    cursorscreenrow = SCREEN_HEIGHT/CHAR_HEIGHT/2;
                    if(softnewline == FALSE)
                        cursorscreencol = getw_nosoftbreak(textbuffer, pos);
                    if(cursorscreencol > SCREEN_WIDTH/CHAR_WIDTH)
                        cursorscreencol = SCREEN_WIDTH/CHAR_WIDTH/2;
                }
            }
            while(readc())
                idle();
        }

        //find previous
        if(isKeyPressed(KEY_NSPIRE_CTRL) && isKeyPressed(KEY_NSPIRE_SHIFT) && isKeyPressed(KEY_NSPIRE_G)){
            if(searchstr != NULL){
                char restore = textbuffer[selectionstart];
                textbuffer[selectionstart] = '\0';
                char* tmp = case_insensitive_strrstr(textbuffer, searchstr);
                textbuffer[selectionstart] = restore;
                if(tmp == NULL){
                    wait_no_key_pressed();
                    show_msgbox("", "Reached the start of the document");
                    tmp = case_insensitive_strrstr(textbuffer, searchstr);//find last occurence in whole text
                }
                if(tmp == NULL)
                    show_msgbox("", "Not found in document");
                else{
                    selectionstart = tmp - textbuffer;
                    selectionend = selectionstart + strlen(searchstr);
                    pos = selectionend;
                    cursorscreenrow = SCREEN_HEIGHT/CHAR_HEIGHT/2;
                    if(softnewline == FALSE)
                        cursorscreencol = getw_nosoftbreak(textbuffer, pos);
                    if(cursorscreencol > SCREEN_WIDTH/CHAR_WIDTH)
                        cursorscreencol = SCREEN_WIDTH/CHAR_WIDTH/2;
                }
            }
            while(readc())
                idle();
    }

    //undo
    if(isKeyPressed(KEY_NSPIRE_CTRL) && isKeyPressed(KEY_NSPIRE_Z)){
        if(undoredo_undo(&textbuffer, &pos, &cursorscreenrow, &cursorscreencol, softnewline)){
            crashsave(textbuffer, path);
            break;
        }
        while(isKeyPressed(KEY_NSPIRE_Z))
            idle();
    }

    //redo
    if(isKeyPressed(KEY_NSPIRE_CTRL) && isKeyPressed(KEY_NSPIRE_Y)){
        if(undoredo_redo(&textbuffer, &pos, &cursorscreenrow, &cursorscreencol, softnewline) == 1){
            crashsave(textbuffer, path);
            break;
        }
        while(isKeyPressed(KEY_NSPIRE_Y))
            idle();
    }

        //select all:
        if(isKeyPressed(KEY_NSPIRE_CTRL) && isKeyPressed(KEY_NSPIRE_A)){
            selectionstart = 0;
            selectionend = strlen(textbuffer);
            pos = 0;
            cursorscreenrow = 0;
            cursorscreencol = 0;
            while(isKeyPressed(KEY_NSPIRE_A))
                idle();
        }

        //go to start of file
        if(isKeyPressed(KEY_NSPIRE_CTRL) && isKeyPressed(KEY_NSPIRE_7)){
            selectionstart = 0;
            pos = 0;
            cursorscreenrow = 0;
            cursorscreencol = 0;
            if(!isKeyPressed(KEY_NSPIRE_SHIFT))
                selectionend = 0;
            while(isKeyPressed(KEY_NSPIRE_7))
                idle();
        }

        //go to end of file
        if(isKeyPressed(KEY_NSPIRE_CTRL) && isKeyPressed(KEY_NSPIRE_1)){
            if(softnewline == TRUE)
                cursorscreenrow = countsoftnewl(textbuffer);
            else
                cursorscreenrow = countnewl(textbuffer);
            pos = strchr(textbuffer, '\0') - textbuffer;
            if(softnewline == FALSE)
                cursorscreencol = min(getw_nosoftbreak(textbuffer, pos), SCREEN_WIDTH/CHAR_WIDTH - 1);
            selectionend = pos;
            if(!isKeyPressed(KEY_NSPIRE_SHIFT))
                selectionstart = pos;
            while(isKeyPressed(KEY_NSPIRE_1))
                idle();
        }

        //pg up
        if(isKeyPressed(KEY_NSPIRE_CTRL) && isKeyPressed(KEY_NSPIRE_9)){
            int i;
            int oldpos = pos;
            if(softnewline == TRUE){
                int w = getw(textbuffer, pos);
                for(i = 0; i < SCREEN_HEIGHT / CHAR_HEIGHT; i++)
                    pos = prevline(textbuffer, pos);
                pos = gotow(textbuffer, pos, w);
            }
            else{
                int w = getw_nosoftbreak(textbuffer, pos);
                for(i = 0; i < SCREEN_HEIGHT/CHAR_HEIGHT; i++)
                    pos = prevline_nosoftbreak(textbuffer, pos);
                pos = gotow_nosoftbreak(textbuffer, pos, w);
                if(w != getw_nosoftbreak(textbuffer, pos)){
                    cursorscreencol -= w - getw_nosoftbreak(textbuffer, pos);
                    if(cursorscreencol < 0)
                        cursorscreencol = min(getw_nosoftbreak(textbuffer, pos), SCREEN_WIDTH/CHAR_WIDTH-1);
                }
            }
            update_selection(oldpos, pos, &selectionstart, &selectionend);
            while(isKeyPressed(KEY_NSPIRE_9))
                idle();
        }

        //pg down
        if(isKeyPressed(KEY_NSPIRE_CTRL) && isKeyPressed(KEY_NSPIRE_3)){
            int i;
            int oldpos = pos;
            if(softnewline == TRUE){
                int w = getw(textbuffer, pos);
                for(i = 0; i < SCREEN_HEIGHT / CHAR_HEIGHT; i++)
                    pos = nextline(textbuffer, pos);
                pos = gotow(textbuffer, pos, w);
            }
            else{
                int w = getw_nosoftbreak(textbuffer, pos);
                for(i = 0; i < SCREEN_HEIGHT / CHAR_HEIGHT; i++)
                    pos = nextline_nosoftbreak(textbuffer, pos);
                pos = gotow_nosoftbreak(textbuffer, pos, w);
                if(w != getw_nosoftbreak(textbuffer, pos)){
                    cursorscreencol -= w - getw_nosoftbreak(textbuffer, pos);
                    if(cursorscreencol < 0)
                        cursorscreencol = min(getw_nosoftbreak(textbuffer, pos), SCREEN_WIDTH/CHAR_WIDTH-1);
                }
            }
            update_selection(oldpos, pos, &selectionstart, &selectionend);
            while(isKeyPressed(KEY_NSPIRE_3))
                idle();
        }

        //home
        if(isKeyPressed(KEY_NSPIRE_CTRL) && isKeyPressed(KEY_NSPIRE_LEFT)){
            cursorscreencol = 0;
            if(softnewline == TRUE)
                pos = currentline(textbuffer, pos);
            else
                pos = currentline_nosoftbreak(textbuffer, pos);
            selectionstart = pos;
            if(!isKeyPressed(KEY_NSPIRE_SHIFT))
                selectionend = pos;
            while(isKeyPressed(KEY_NSPIRE_LEFT))
                idle();
        }

        //end
        if(isKeyPressed(KEY_NSPIRE_CTRL) && isKeyPressed(KEY_NSPIRE_RIGHT)){
            if(softnewline){
                if((pos = checkednextline(textbuffer, pos) - 1) == -1)//if in last line
                    pos = strlen(textbuffer);
                //uncomment if you want cursor before first char of next line instead of before last char of current line:
                //else if(textbuffer[pos] != '\n')
                //    pos++;
            }
            else{
                if((pos = checkednextline_nosoftbreak(textbuffer, pos) - 1) == -1)
                    pos = strlen(textbuffer);
                cursorscreencol = min(getw_nosoftbreak(textbuffer, pos), SCREEN_WIDTH/CHAR_WIDTH - 1);
            }
            selectionend = pos;
            if(!isKeyPressed(KEY_NSPIRE_SHIFT))
                selectionstart = pos;
            while(isKeyPressed(KEY_NSPIRE_RIGHT))
                idle();
        }

        //input(first:alphanum/newline/tab/del, second:arrow keys):
        inpbuf = readc();
        if(inpbuf != '\0' && (inpbuf != blockkey || !delaycounter)){//if key hasn't been released and repeated keypress delay isn't over, ignore keypress
            if(inpbuf != blockkey)//if they're the same, this means the key has been hold down and is now read for the 2nd time, so use a shorter delay
                delaycounter = DELAY_FIRST;
            else
                delaycounter = DELAY_AFTER;
            blockkey = inpbuf;
            if(selectionstart != selectionend){
                if(delete_action(textbuffer, &pos, &cursorscreenrow, &cursorscreencol, &selectionstart, &selectionend, softnewline) == 1){
                    crashsave(textbuffer, path);
                    break;
                }
            }
            else if(inpbuf == '\b'){//else if because if there was a selection, it's already deleted -> do nothing more
                if(strlen(textbuffer) != 0 && pos != 0){
                    selectionstart = pos - 1;//delete_action deletes the selection, so set it accordingly
                    selectionend = pos;
                    if(delete_action(textbuffer, &pos, &cursorscreenrow, &cursorscreencol, &selectionstart, &selectionend, softnewline) == 1){
                        crashsave(textbuffer, path);
                        break;
                    }
                }
            }
            if(inpbuf != '\b'){
                if(inpbuf == '\n'){
                    cursorscreenrow++;
                    cursorscreencol = 0;
                }
                else if(inpbuf == '\t'){
                    if(softnewline == FALSE)
                        cursorscreencol += TAB_WIDTH - getw_nosoftbreak(textbuffer, pos) % TAB_WIDTH;
                }
                else
                    cursorscreencol++;
                if(insert_range_action(&textbuffer, pos, 1, &inpbuf) == 1){
                    crashsave(textbuffer, path);
                    break;
                }
                pos++;
                selectionstart = pos;
                selectionend = pos;
            }
        }
        else if((arrowbuf = readarrow()) != 0){
            if(arrowbuf != blockkey || !delaycounter){
                if(arrowbuf != blockkey)
                    delaycounter = DELAY_FIRST;
                else
                    delaycounter = DELAY_AFTER;
                blockkey = arrowbuf;
                if(arrowbuf == ARROW_RIGHT_NUM){
                    if(pos < strlen(textbuffer)){
                        if(textbuffer[pos] == '\n'){
                            cursorscreenrow++;
                            cursorscreencol = 0;
                        }
                        else if(textbuffer[pos] == '\t'){
                            if(softnewline == TRUE)
                                cursorscreencol += TAB_WIDTH - getw(textbuffer, pos) % TAB_WIDTH;
                            else
                                cursorscreencol += TAB_WIDTH - getw_nosoftbreak(textbuffer, pos) % TAB_WIDTH;
                        }
                        else
                            cursorscreencol++;
                        update_selection(pos, pos+1, &selectionstart, &selectionend);
                        pos++;
                    }
                }
                else if(arrowbuf == ARROW_LEFT_NUM){
                    if(pos > 0){
                        pos--;
                        if(textbuffer[pos] == '\n'){
                            cursorscreenrow--;
                            cursorscreencol = getw_nosoftbreak(textbuffer, pos);
                        }
                        else if(textbuffer[pos] == '\t'){
                            if(softnewline == TRUE)
                                cursorscreencol -= TAB_WIDTH - getw(textbuffer, pos) % TAB_WIDTH;
                            else
                                cursorscreencol -= TAB_WIDTH - getw_nosoftbreak(textbuffer, pos) % TAB_WIDTH;
                        }
                        else
                            cursorscreencol--;
                        update_selection(pos+1, pos, &selectionstart, &selectionend);
                    }
                }
                else if(arrowbuf == ARROW_UP_NUM){
                    if(softnewline){
                        if(currentline(textbuffer, pos) != 0){//if cursor not in first line of file
                            cursorscreenrow--;
                            update_selection(pos, go_up(textbuffer, pos), &selectionstart, &selectionend);
                            pos = go_up(textbuffer, pos);
                        }
                    }
                    else{
                        if(currentline_nosoftbreak(textbuffer, pos) != 0){
                            cursorscreenrow--;
                            update_selection(pos, go_up_nosoftbreak(textbuffer, pos), &selectionstart, &selectionend);
                            cursorscreencol -= getw_nosoftbreak(textbuffer, pos) - getw_nosoftbreak(textbuffer, go_up_nosoftbreak(textbuffer, pos));
                            pos = go_up_nosoftbreak(textbuffer, pos);
                        }
                    }
                }
                else if(arrowbuf == ARROW_DOWN_NUM){
                    if(softnewline){
                        if(checkednextline(textbuffer, pos) != 0){//if cursor not in last line of file
                            cursorscreenrow++;
                            update_selection(pos, go_down(textbuffer, pos), &selectionstart, &selectionend);
                            pos = go_down(textbuffer, pos);
                        }
                    }
                    else{
                        if(checkednextline_nosoftbreak(textbuffer, pos) != 0){//if cursor not in last line of file
                            cursorscreenrow++;
                            update_selection(pos, go_down_nosoftbreak(textbuffer, pos), &selectionstart, &selectionend);
                            cursorscreencol -= getw_nosoftbreak(textbuffer, pos) - getw_nosoftbreak(textbuffer, go_down_nosoftbreak(textbuffer, pos));
                            pos = go_down_nosoftbreak(textbuffer, pos);
                        }

                    }
                }
            }
        }

        if(readc() == 0 && readarrow() == 0)//if no key pressed <==> if key released
            blockkey = 0;
        if(delaycounter)
            delaycounter--;


        if(cursorscreenrow < 0)
            cursorscreenrow = 0;
        if(cursorscreenrow >= SCREEN_HEIGHT/CHAR_HEIGHT)
            cursorscreenrow = SCREEN_HEIGHT/CHAR_HEIGHT - 1;

        if(cursorscreencol < 0)
            cursorscreencol = 0;
        if(cursorscreencol >= SCREEN_WIDTH/CHAR_WIDTH)
            cursorscreencol = SCREEN_WIDTH/CHAR_WIDTH - 1;
    }
    free(searchstr);//note: free does nothing if nullpointer passed -> no check needed
    free(textbuffer);
    free(scrbuf);
    history_free();
    refresh_osscr();
    return 0;
}
