#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>
#include <ctype.h>

#include "public.h"
#include "logo_128.h"
#include "logo_384.h"


const APP_MSG App_Msg = {
	"COOKSHOP.biz",
	"0.1",
	0,
	0,
	0,
	"Cookshop Food Services, Inc",
    "Vendor point of sale (POS) device software.",
	"",
	0,
	""
};

// FIXME: Instead of a menu being an array of strings, it should be a struct that contains a constant value for a unique command an a string

char *Menu[] = {
    "1: New orders",
    "2: Pending orders",
    "3: Picked-up orders",
    "4: Delivered orders",
    "5: Settings",
    NULL
};

void Display_Loading(int level) {
	Lib_LcdSetFont(FONT_MEDIUM);
	Lib_LcdCls();
	Lib_LcdGotoxy(0, 26);
	
	if (level <= 3) Lib_Lcdprintf("       Loading      ");
	else if (level <= 6) Lib_Lcdprintf("      Loading...    ");
	else if (level <= 9) Lib_Lcdprintf("    Loading......   ");
	else if (level > 9) Lib_Lcdprintf("         OK         ");
}

unsigned char Display_Waiting(void) {
    unsigned char ucKey;

	Lib_LcdCls();

	// draw logo
	Lib_LcdGotoxy(0, 0);
	Lib_LcdDrawLogo(g_Display_logo_128);
	Lib_LcdGotoxy(0, 64 - 14);

	// draw text
	Lib_LcdSetFont(FONT_SMALL);
	Lib_Lcdprintf("  Press ESC for menu");

	// wait for OK, ESC, CANCEL, or MENU
	Lib_KbFlush();
	while (TRUE) {
		if (Lib_KbCheck()) continue;

		ucKey = Lib_KbGetCh();
		switch (ucKey) {
			case KEYCANCEL:
			case KEYCLEAR:
			case KEYFN:
			case KEYMENU:
			case KEYENTER:
			case KEYBACKSPACE:
				return ucKey;
		}
	}
	return 1;
}

unsigned char Display_Menu(char **menu, int lines) {
    int len = 0;
    int scroll = 0;
    unsigned char ucKey;

    // menu length
    while (menu[len++] != NULL); len--;	

    // scrolling
    while (len) {
        int count = 0;
        char** temp = (menu + scroll);

        Lib_LcdCls();
        Lib_LcdSetFont(FONT_MEDIUM);
        while (*temp != NULL && count < lines) {
            Lib_Lcdprintf("%s\n", *temp++);
            count++;
        }

		Lib_KbFlush();
        while (TRUE) {
            if (Lib_KbCheck()) continue;

            ucKey = Lib_KbGetCh();
			switch (ucKey) {
				case KEYUP:
					if ((scroll + lines) < len) scroll++;
					break;

				case KEYDOWN:
					if (scroll > 0) scroll--;
					break;
				
				default:
					return ucKey;
			}
        }
    }
	return 1;
}

int main(void) {
	int iret;
	int view = VIEW_WAITING;
    int scroll = 0;
    char **curr_menu;
	unsigned char ucKey;
	unsigned char ucWlsResetFlag;

	Lib_AppInit();
 
    // clear screen and keyboard
    Lib_LcdCls();
    Lib_LcdClrDotBuf();
    Lib_KbFlush();
	Display_Loading(0);

    // reset communication ports
    Lib_ComReset(COM1);
    Lib_ComReset(COM2);
    Lib_ComReset(AT_COM);
	Display_Loading(3);

    // Lib_UsbReset(); // FIXME: find the port number for USB
    Wls_Reset();

    // initialize printer
	Display_Loading(6);
	if (Lib_PrnInit()) return 1;

    // initialize wireless
	Display_Loading(9);
    if (Wls_Init()) return 1;

    // set up environment
    if (Lib_FileExist("EnvFile") < 0) {
		Lib_FilePutEnv("DIALNUM", "CMNET");
		Lib_FilePutEnv("USERID",  "card");
		Lib_FilePutEnv("USERPWD", "card");
		Lib_FilePutEnv("IP",      "");
		Lib_FilePutEnv("PORT",    "9005");
    }

	// moment of silence for our lost hommies
	Display_Loading(10);
	Lib_DelayMs(1000);

    // intro beeps
    Lib_Beef(0, 200);
	Lib_DelayMs(100);
    Lib_Beef(1, 200);
	Lib_DelayMs(100);
    Lib_Beef(2, 200);
	Lib_DelayMs(100);
    Lib_Beef(3, 200);
	Lib_DelayMs(100);
    Lib_Beef(4, 200);
	Lib_DelayMs(100);
    Lib_Beef(5, 200);

	while (TRUE) {
		Lib_LcdCls();
		Lib_KbFlush();

        // display page
		if (view == VIEW_WAITING) {
			if (Display_Waiting()) view = VIEW_MENU;
			else return 0;

		} else if (view = VIEW_MENU) {
			switch (Display_Menu(Menu, 4)) {
				case KEY1:
				case KEY2:
				case KEY3:
				case KEY4:
				case KEY5:
				case KEY6:
				case KEY7:
				case KEY8:
				case KEY9:
				case KEY0:

				case KEYCANCEL:
				default:
					view = VIEW_WAITING;
					continue;
			}

		} else if (view = VIEW_ORDER) {

		} else if (view = VIEW_ORDER_LIST) {

		} else if (view = VIEW_SETTINGS) {

		}
	}
	return 1;			
}




