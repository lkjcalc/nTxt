#include <os.h>
#include "menu.h"
#include "output.h"

#define MENU_BACKGROUND_COLOR ( has_colors?0b1111111111111111:0xC )
#define MENU_SHADOW_COLOR (has_colors?0b0111101111101111:0x5 )
#define MENU_SELECTION_COLOR (has_colors?0b011101111111111:0x0)

#define INPUTPAUSE 30

static int dispMenu(void* scrbuf, int x, int y, const char* entrylist[], int selected){//returns number of items in menu
    int h = 0;
    int w = 0;
    int i;

    for(i=0; entrylist[i][0] != '\0'; i++)
        w = max(w, strlen(entrylist[i]));
    h = i*(CHAR_HEIGHT + 4);
    w = 2 + (w + 1)*CHAR_WIDTH;

    dispRect(scrbuf, x, y, w+2, h+2, 0);
    filledRect(scrbuf, x+1, y+1, w, h, MENU_BACKGROUND_COLOR);
    dispHorizLine(scrbuf, x+1, y+h+2, w+2, MENU_SHADOW_COLOR);
    dispHorizLine(scrbuf, x+2, y+h+3, w+2, MENU_SHADOW_COLOR);
    dispVertLine(scrbuf, x+w+2, y+1, h+2, MENU_SHADOW_COLOR);
    dispVertLine(scrbuf, x+w+3, y+2, h+2, MENU_SHADOW_COLOR);

    y += 3;
    int j;
    for(j=0; j<i; j++, y += CHAR_HEIGHT + 4){
        if(j == selected - 1){
            filledRect(scrbuf, x+1, y-2, w, CHAR_HEIGHT+4, MENU_SELECTION_COLOR);
            dispStringColor(scrbuf, x+3, y, entrylist[j], has_colors?0xFFFF:0xF, MENU_SELECTION_COLOR);
        }
        else
            dispString(scrbuf, x+3, y, entrylist[j]);
    }

    return i;
}

int menu(void* scrbuf, const char* menuentries[]){
    int selecteditem = 1;
    int lastitem;

    int blockarrow = 0;
    int inputpause = INPUTPAUSE;

    while(1){
        sleep(10);//results in around 63 fps

        lastitem = dispMenu(scrbuf, 0, 0, menuentries, selecteditem);
        memcpy(SCREEN_BASE_ADDRESS, scrbuf, SCREEN_BYTES_SIZE);

        //closing menu:
        if(isKeyPressed(KEY_NSPIRE_ESC) || on_key_pressed())
            return 0;

        //choosing an option:
        if(isKeyPressed(KEY_NSPIRE_ENTER))
            return selecteditem;

        //navigating:
        if(isKeyPressed(KEY_NSPIRE_UP) && (blockarrow != 0x1 || inputpause <= 0)){
            inputpause = INPUTPAUSE;
            blockarrow = 0x1;
            selecteditem--;
            if(selecteditem == 0)
                selecteditem = lastitem;
        }
        else if(isKeyPressed(KEY_NSPIRE_DOWN) && (blockarrow != 0x2 || inputpause <= 0)){
            inputpause = INPUTPAUSE;
            blockarrow = 0x2;
            selecteditem++;
            if(selecteditem > lastitem)
                selecteditem = 1;
        }

        if(!isKeyPressed(KEY_NSPIRE_UP) && !isKeyPressed(KEY_NSPIRE_DOWN))
            blockarrow = 0;
        else
            inputpause--;
    }
}
