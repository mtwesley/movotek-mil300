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

const char *Menu[] = {
    "1: New orders",
    "2: Pending orders",
    "3: Picked-up orders",
    "4: Delivered orders",
    "5: Settings",
    NULL
};

unsigned char Display_Menu(char **menu, int lines) {
    int len = 0;
    int scroll = 0;
    unsigned char ucKey;

    // menu length
    while (menu[len++] != NULL);

    // scrolling
    if (len) {
        int count = 0;
        char** temp = menu + scroll;

        Lib_LcdCls();
        Lib_LcdSetFont(FONT_MEDIUM);
        while (*temp != NULL && count < lines) {
            Lib_Lcdprintf("%s\n", *temp++);
            count++;
        }

        while (TRUE) {
            if (Lib_KbCheck()) continue;			
            ucKey = Lib_KbGetCh();

			if (ucKey == KEYUP && scroll < len) {
				scroll++;
				break;
			} else if (ucKey == KEYDOWN && scroll >= 0) {
				scroll--;
				break;
			}

            return ucKey;
        }
    }
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

	Lib_LcdSetFont(FONT_MEDIUM);

	Lib_LcdCls();
	Lib_LcdGotoxy(0, 26);
	Lib_Lcdprintf("  Checking ports... ");

    // reset communication ports
    Lib_ComReset(COM1);
    Lib_ComReset(COM2);
    Lib_ComReset(AT_COM);
    // Lib_UsbReset(); // FIXME: find the port number for USB
    Wls_Reset();

	Lib_LcdCls();
	Lib_LcdGotoxy(0, 26);
	Lib_Lcdprintf(" Checking devices...");

    // initialize printer
    if (Lib_PrnInit()) return 1;

    // initialize wireless
    if (Wls_Init()) return 1;

	Lib_LcdCls();
	Lib_LcdGotoxy(0, 26);
	Lib_Lcdprintf("   Initializing...  ");

    // set up environment
    if (Lib_FileExist("EnvFile") < 0) {
		Lib_FilePutEnv("DIALNUM", "CMNET");
		Lib_FilePutEnv("USERID",  "card");
		Lib_FilePutEnv("USERPWD", "card");
		Lib_FilePutEnv("IP",      "");
		Lib_FilePutEnv("PORT",    "9005");
    }

	// moment of silence for our lost hommies
	Lib_DelayMs(2000);

    // intro beeps
    Lib_Beef(0, 200);
	Lib_DelayMs(300);
    Lib_Beef(1, 200);
	Lib_DelayMs(300);
    Lib_Beef(2, 200);
	Lib_DelayMs(300);
    Lib_Beef(3, 200);
	Lib_DelayMs(300);
    Lib_Beef(4, 200);
	Lib_DelayMs(300);
    Lib_Beef(5, 200);
	Lib_DelayMs(300);
    Lib_Beef(6, 200);
	Lib_DelayMs(300);
    Lib_Beef(7, 200);
	Lib_DelayMs(300);

	while (TRUE) {
		Lib_LcdCls();
		Lib_KbFlush();

        // display page
		if (page == VIEW_WAITING) {
			Lib_LcdGotoxy(0, 4);
            Lib_LcdDrawLogo(g_Display_logo_128);
            Lib_LcdGotoxy(0, 64 - 12);
            Lib_LcdSetFont(FONT_SMALL);
            Lib_Lcdprintf("Press ESC for menu");
            switch (Display_Menu(Menu, 4)) {
                case KEYCANCEL:
                    return 0;
                default:
                    break;
            }

		} else if (page == VIEW_MENU) {
            Lib_LcdSetFont(FONT_MEDIUM);

		} else if (page == VIEW_ORDER_LIST) {
			return 0;

		} else if (page == VIEW_ORDER_MENU) {
			return 0;

		} else if (page == VIEW_MENU) {
			return 0;

		} else {
			return 0;
		}
		
        // wait for key input
		while (1) {

			if (Lib_KbCheck()) continue;
			ucKey = Lib_KbGetCh();

			if (!ucKey) continue;

			break;
		}

        // page actions
        if (page == VIEW_WAITING) {
			switch(ucKey) {
			default:
				break; 
			}
		}
	}
	return 0;			
}




