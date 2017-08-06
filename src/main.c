#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>
#include <ctype.h>

#include "public.h"
#include "logo_128.h"
#include "logo_384.h"

#define VIEW_WAITING     0
#define VIEW_MAIN        1
#define VIEW_ORDER       2
#define VIEW_ORDER_LIST  3
#define VIEW_SETTINGS    4

#define STATUS_UNKNOWN   0
#define STATUS_NEW       1
#define STATUS_PENDING   2
#define STATUS_PICKED_UP 3
#define STATUS_DELIVERED 4

const APP_MSG App_Msg = {
	"COOKSHOP.biz",
	"0.1", 0, 0, 0,
	"Cookshop Food Services, Inc",
    "Vendor point of sale (POS) device software.",
	"", 0, ""
};

// FIXME: Instead of a menu being an array of strings, it should be a struct 
// that contains a constant value for a unique command an a string

char *Main_Menu[] = {
    "[1] New orders",
    "[2] Pending orders",
    "[3] Picked-up orders",
    "[4] Delivered orders",
    "[5] Settings",
    NULL
};

char *New_Order_Menu[] = {
    "[1] Accept order",
    "[2] Change order",
    "[3] Reject order",
    "[4] View order",
    "[5] Print order",
    NULL
};

char *Pending_Order_Menu[] = {
    "[1] Change order",
    "[2] Cancel order",
    "[3] View order",
    "[4] Print order",
    NULL
};

char *Non_Pending_Order_Menu[] = {
    "[1] View order",
    "[2] Print order",
    NULL
};

char *Settings_Menu[] = {
    "[1] Display",
    "[2] Sound",
    "[3] Network",
    "[4] Printer",
    "[5] Clock",
    "[6] Battery",
    NULL
};

char *Sample_Order_List[] = {
    "CS146001",
    "CS146002",
    "CS146003",
    "CS146003",
    "CS146005",
    NULL
};


void Clear_Topbar(void) {
    Lib_LcdClrLine(0, 11);
}

void Clear_Content(void) {
    Lib_LcdClrLine(12, 63);
}

void Display_Loading(int level) {
	Lib_LcdSetFont(FONT_MEDIUM);
	Lib_LcdGotoxy(0, 26);
	
	if (level <= 3)      Lib_Lcdprintf("    Loading (25%%)    ");
	else if (level <= 6) Lib_Lcdprintf("    Loading (50%%)    ");
	else if (level <= 9) Lib_Lcdprintf("    Loading (75%%)    ");
	else if (level > 9)  Lib_Lcdprintf("    Loading (100%%)   ");
}

void Display_Topbar(void) {
	Clear_Topbar();

    int x, y;
    for (x = 0; x < 128; x++) {
		for (y = 0; y < 12; y++) {
			Lib_LcdDrawPlot(x, y, 1);
        }
    }

    // print time YYMMDDhhmmssww
	int year, month, day, hour, minute, second, week;
	unsigned char buff[8];
	
    Lib_GetDateTime(buff);
	
    // year   = (buff[0] >> 4) * 10 + (buff[0] & 0x0f) + 1900;
	// month  = (buff[1] >> 4) * 10 + (buff[1] & 0x0f);
	// day    = (buff[2] >> 4) * 10 + (buff[2] & 0x0f);
	hour   = (buff[3] >> 4) * 10 + (buff[3] & 0x0f);
	minute = (buff[4] >> 4) * 10 + (buff[4] & 0x0f);
	// second = (buff[5] >> 4) * 10 + (buff[5] & 0x0f);
    // week   = (buff[6] >> 4) * 10 + (buff[6] & 0x0f);

    // if (buff[0] <= 0x49) year += 100;

    if (hour == 0)       Lib_LcdPrintxy(40, 2, 0x80, "12:%02d AM", minute);
    if (hour == 12)      Lib_LcdPrintxy(40, 2, 0x80, "12:%02d PM", minute);
    else if (hour < 12)  Lib_LcdPrintxy(40, 2, 0x80, "%d:%02d AM", hour, minute);
    else if (hour > 12)  Lib_LcdPrintxy(40, 2, 0x80, "%d:%02d PM", hour - 12, minute);

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
	Lib_Lcdprintf("  Press OK for menu");

	// wait for OK, ESC, CANCEL, or MENU
	Lib_KbFlush();
	while (TRUE) {
		if (Lib_KbCheck()) continue;

		ucKey = Lib_KbGetCh();
		switch (ucKey) {
			// case KEYCANCEL:
			// case KEYCLEAR:
			// case KEYFN:
			// case KEYMENU:
			// case KEYBACKSPACE:
			case KEYENTER:
            case KEYOK:
				return ucKey;
		}
	}
	return 1;
}

unsigned char Display_Menu(char **menu, int lines) {
    int len = 0;
    int scroll = 0;
    unsigned char ucKey;

	Lib_LcdGotoxy(0, 12);

    // menu length
    while (menu[len++] != NULL); len--;	

    // scrolling
    while (len) {
        int count = 0;
        char** temp = (menu + scroll);

        Clear_Content();
        Lib_LcdSetFont(FONT_MEDIUM);
        while (*temp != NULL && count < lines) {
            Lib_Lcdprintf("%s\n", *temp++);
            count++;
        }

		Lib_KbFlush();
        while (TRUE) {
			int breakout = FALSE;

            if (Lib_KbCheck()) continue;
            ucKey = Lib_KbGetCh();

			switch (ucKey) {
				case KEYDOWN:
					if ((scroll + lines) < len) {
						scroll++;
						breakout = TRUE;
					}
					break;

				case KEYUP:
					if (scroll > 0) {
						scroll--;
						breakout = TRUE;
					}
					break;
				
				default:
					return ucKey;
			}
			if (breakout) break;
        }
    }
	return 1;
}

int Display_List(char **list, int lines) {
    int len = 0;
    int scroll = 0;

	Lib_LcdGotoxy(0, 12);

    // list length
    while (list[len++] != NULL); len--;	

    // scrolling
    while (len) {
        int count = 0;
        char** temp = (list + ((scroll / lines) * lines));

        Lib_LcdCls();
        Lib_LcdSetFont(FONT_MEDIUM);
        while (*temp != NULL && count < lines) {
            if (count == (scroll % lines)) Lib_Lcdprintf("[*] %s\n", *temp++);
            else (count == (scroll % lines)) Lib_Lcdprintf("[ ] %s\n", *temp++);
            count++;
        }

		Lib_KbFlush();
        while (TRUE) {
			int breakout = FALSE;

            if (Lib_KbCheck()) continue;
            ucKey = Lib_KbGetCh();

			switch (ucKey) {
				case KEYDOWN:
					if ((scroll + lines) < len) {
						scroll++;
						breakout = TRUE;
					}
					break;

				case KEYUP:
					if (scroll > 0) {
						scroll--;
						breakout = TRUE;
					}
					break;
				
                case KEYENTER:
                case KEYOK:
                    return scroll;
                    
				default:
					continue;
			}
			if (breakout) break;
        }
    }
	return 1;
}

int main(void) {
    int view = VIEW_WAITING;
    int status = STATUS_UNKNOWN;

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
	Display_Loading(6);

    // initialize printer
	if (Lib_PrnInit()) return 1;
	Display_Loading(9);

    // initialize wireless
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
	Lib_DelayMs(1000);
	Display_Loading(10);

    // intro beeps
	Lib_DelayMs(100);
    Lib_Beef(7, 200);
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
	Lib_DelayMs(100);
    Lib_Beef(6, 200);

	while (TRUE) {
		Lib_LcdCls();
		Lib_KbFlush();

        // display content using views
		if (view == VIEW_WAITING) {
			if (Display_Waiting()) view = VIEW_MAIN;
			else return 0;

		} else if (view == VIEW_MAIN) {
			Display_Topbar();
			switch (Display_Menu(Main_Menu, 4)) {
				case KEY1:
                    view = VIEW_ORDER_LIST; 
                    status = STATUS_NEW;
                    break;

				case KEY2:
                    view = VIEW_ORDER_LIST;
                    status = STATUS_PENDING;
                    break;
				
                case KEY3:
                    view = VIEW_ORDER_LIST;
                    status = STATUS_PICKED_UP;
                    break;
				
				case KEY4:
                    view = VIEW_ORDER_LIST;
                    status = STATUS_DELIVERED;
                    break;
				
				case KEY5:
                    view = VIEW_SETTINGS;
                    break;

				default:
					view = VIEW_WAITING;
			}

		} else if (view == VIEW_ORDER_LIST) {
			Display_Topbar();
            switch (Display_Menu(Sample_Order_List, 4)) {
                case KEYENTER:
                    view = VIEW_ORDER;
                    break;

				case KEYCANCEL:
                case KEYBACKSPACE:
                case KEYMENU:
                    view = VIEW_MAIN;
                    break;

				default:
					view = VIEW_ORDER_LIST;
			}

		} else if (view == VIEW_ORDER) {
			Display_Topbar();
            if (status == STATUS_NEW) {
                switch (Display_Menu(New_Order_Menu, 4)) {
                    case KEY1:
                    case KEY2:
                    case KEY3:
                    case KEY4:
                        break;

                    case KEYCANCEL:
                    case KEYBACKSPACE:
                        view = VIEW_ORDER_LIST;
                        break;

                    default:
                        view = VIEW_ORDER;
                }
            
            } else if (status == STATUS_PENDING) {
                switch (Display_Menu(Pending_Order_Menu, 4)) {
                    case KEY1:
                    case KEY2:
                    case KEY3:
                    case KEY4:
                        break;

                    case KEYCANCEL:
                    case KEYBACKSPACE:
                        view = VIEW_ORDER_LIST;
                        break;

                    default:
                        view = VIEW_ORDER;
                }
            
            } else if (status == STATUS_PICKED_UP) {
                switch (Display_Menu(Non_Pending_Order_Menu, 4)) {
                    case KEY1:
                    case KEY2:
                    case KEY3:
                    case KEY4:
                        break;

                    case KEYCANCEL:
                    case KEYBACKSPACE:
                        view = VIEW_ORDER_LIST;
                        break;

                    default:
                        view = VIEW_ORDER;
                }
            
            } else if (status == STATUS_DELIVERED) {
                switch (Display_Menu(Non_Pending_Order_Menu, 4)) {
                    case KEY1:
                    case KEY2:
                    case KEY3:
                    case KEY4:
                        break;

                    case KEYCANCEL:
                    case KEYBACKSPACE:
                        view = VIEW_ORDER_LIST;
                        break;

                    default:
                        view = VIEW_ORDER;
                }
            
            }

		} else if (view == VIEW_SETTINGS) {
			Display_Topbar();
            switch (Display_Menu(Settings_Menu, 4)) {
				default:
					view = VIEW_ORDER_LIST;
			}
		}
	}
	return 1;			
}




