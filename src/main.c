#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>
#include <ctype.h>

#include "public.h"
#include "logos.h"
#include "icons.h"
#include "topbar.h"

#define VIEW_WAITING      0
#define VIEW_MAIN         1
#define VIEW_ORDER        2
#define VIEW_ORDER_LIST   3
#define VIEW_SETTINGS     4

#define STATUS_UNKNOWN    0
#define STATUS_NEW        1
#define STATUS_PENDING    2
#define STATUS_PICKED_UP  3
#define STATUS_DELIVERED  4

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
    "CS146004",
    "CS146005",
    NULL
};

static int view     = VIEW_WAITING;
static int status   = STATUS_UNKNOWN;

void Clear_Topbar(void) {
	Lib_LcdDrawLogo(g_Display_topbar);
}

void Clear_Content(void) {
    Lib_LcdClrLine(12, 63);
}

void Display_Loading(int level) {
	Lib_LcdSetFont(LCD_FONT_MEDIUM);
	Lib_LcdGotoxy(0, 26);
	
	if (level == 0)      Lib_Lcdprintf("    Loading (0%%)     ");
	else if (level <= 3) Lib_Lcdprintf("    Loading (25%%)    ");
	else if (level <= 6) Lib_Lcdprintf("    Loading (50%%)    ");
	else if (level <= 9) Lib_Lcdprintf("    Loading (75%%)    ");
	else if (level > 9)  Lib_Lcdprintf("    Loading (100%%)   ");
}

void Display_Topbar(char *title) {
    Clear_Topbar();
	Lib_LcdSetFont(LCD_FONT_SMALL);

	// wireless signal
	Lib_LcdGotoxy(1, 0);
	Lib_LcdDrawLogo(g_Display_icon_sig_4);

	// battery logo
	Lib_LcdGotoxy(128 - 17, 0);
	Lib_LcdDrawLogo(g_Display_icon_bat_2);

	if (title != NULL) Lib_LcdPrintxy(24, 2, 0x80, "%s", title);
    else {
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

		if (hour < 1)       Lib_LcdPrintxy(40, 2, 0x80,  "12:%02d AM", minute);
		else if (hour < 10) Lib_LcdPrintxy(40, 2, 0x80, " %d:%02d AM", hour, minute);
		else if (hour < 12) Lib_LcdPrintxy(40, 2, 0x80,  "%d:%02d AM", hour, minute);
		else if (hour < 13) Lib_LcdPrintxy(40, 2, 0x80,  "12:%02d PM", minute);
		else if (hour > 12) Lib_LcdPrintxy(40, 2, 0x80,  "%d:%02d PM", hour - 12, minute);
	}
}

unsigned char Display_Waiting(void) {
    unsigned char ucKey;

	// draw logo
    Lib_LcdCls();
	Lib_LcdGotoxy(0, 2);
	Lib_LcdDrawLogo(g_Display_logo_128);
	Lib_LcdGotoxy(0, 64 - 14);

	// draw text
	Lib_LcdSetFont(LCD_FONT_SMALL);
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
			// case KEYBACKSPACE:
			case KEYMENU:
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

    // menu length
    while (menu[len++] != NULL); len--;	

    // scrolling
    while (len) {
        int count = 0;
        char** temp = (menu + scroll);

        Clear_Content();
        Lib_LcdSetFont(LCD_FONT_MEDIUM);
        Lib_LcdGotoxy(0, 12);
        
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
    unsigned char ucKey;

    // list length
    while (list[len++] != NULL); len--;	

    // scrolling
    while (len) {
        int count = 0;
        char** temp = (list + ((scroll / lines) * lines));

        Clear_Content();
        Lib_LcdSetFont(LCD_FONT_MEDIUM);
        Lib_LcdGotoxy(0, 12);
        
        while (*temp != NULL && count < lines) {
            if (count == (scroll % lines)) Lib_Lcdprintf("[*] %s\n", *temp++);
            else Lib_Lcdprintf("[ ] %s\n", *temp++);
            count++;
        }

		Lib_KbFlush();
        while (TRUE) {
			int breakout = FALSE;

            if (Lib_KbCheck()) continue;
            ucKey = Lib_KbGetCh();

			switch (ucKey) {
				case KEYDOWN:
					if (scroll < (len - 1)) {
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
                
				case KEYCANCEL:
				case KEYMENU:
				// case KEYCLEAR:
				// case KEYBACKSPACE:
				// case KEYFN:
					return ucKey;

				default:
					continue;
			}
			if (breakout) break;
        }
    }
	return 1;
}

int Display_Confirm(char *message, char *yes, char *no) {
    unsigned char ucKey;

    Clear_Content();
    Lib_LcdSetFont(LCD_FONT_MEDIUM);
    
    // print message
    Lib_LcdGotoxy(0, 12);
    Lib_Lcdprintf("%s\n", message);

    // print confirmation
    Lib_LcdGotoxy(0, 12 * 3);
    Lib_Lcdprintf("[YES] %s\n", yes);
    Lib_Lcdprintf(" [NO] %s\n", no);

    Lib_KbFlush();
    while (TRUE) {
        int breakout = FALSE;

        if (Lib_KbCheck()) continue;
        ucKey = Lib_KbGetCh();

        switch (ucKey) {
            case KEYENTER:
                return 1;

            case KEYCANCEL:            
            case KEYBACKSPACE:
            case KEYCLEAR:
                return 0;
            
            default:
                continue;
        }
        if (breakout) break;
    }
    return 0;
}

void Display_Notice(char *message) {
    unsigned char ucKey;

    Clear_Content();
    Lib_LcdSetFont(LCD_FONT_MEDIUM);
    
    // print message
    Lib_LcdGotoxy(0, 12);
    Lib_Lcdprintf("%s\n", message);

    // print confirmation
    Lib_LcdGotoxy(0, 12 * 4);
	Lib_Lcdprintf("[OK] Continue");

    Lib_KbFlush();
    while (TRUE) {
        if (Lib_KbCheck()) continue;
        ucKey = Lib_KbGetCh();

        switch (ucKey) {
            case KEYOK:
            case KEYENTER:
                return;

            
            default:
                continue;
        }
    }
}

void Print_Order(char *order) {
	Lib_PrnInit();

    Lib_PrnStr("\n\n");
    Lib_PrnLogo(g_Display_logo_384);

    Lib_PrnSetFont(PRN_FONT_LARGE);

    Lib_PrnStr("\n\n");
    Lib_PrnStr("ABCDEFGHIJKLMNOPQRSTUVWXYZ\n");
    Lib_PrnStr("abcdefghijklmnopqrstuvwxyz\n");
    Lib_PrnStr("1234567890 1234567890 1234567890 1234567890 1234567890 1234567890 1234567890 1234567890\n");
    Lib_PrnStr("\n\n");

    Lib_PrnStart();
    Lib_PrnStart();

    // Lib_PrnSetFont(PRN_FONT_MEDIUM);

    // Lib_PrnStr("\n\n");
    // Lib_PrnStr("ABCDEFGHIJKLMNOPQRSTUVWXYZ\n");
    // Lib_PrnStr("abcdefghijklmnopqrstuvwxyz\n");
    // Lib_PrnStr("1234567890 1234567890 1234567890 1234567890 1234567890 1234567890 1234567890 1234567890\n");
    // Lib_PrnStr("\n\n");

	// start printing
    // Lib_PrnStart();
}

int main(void) {
	Lib_AppInit();
 
    // clear screen and keyboard
    Lib_LcdCls();
    // Lib_LcdClrDotBuf();
    // Lib_KbFlush();

	// Display_Loading(0);
	// Lib_DelayMs(1000);

    // reset communication ports
 	// Display_Loading(3);
    // Lib_ComReset(COM1);
    // Lib_ComReset(COM2);
    // Lib_ComReset(AT_COM);
    // Lib_UsbReset(); // FIXME: find the port number for USB
    // Wls_Reset();

 	// Display_Loading(6);
	// Lib_DelayMs(1000);

    // initialize wireless
    // if (Wls_Init()) return 1;

    // initialize printer
	// if (Lib_PrnInit()) return 1;

  	// Display_Loading(9);
	// Lib_DelayMs(1500);

   // set up environment
    // if (Lib_FileExist("EnvFile") < 0) {
	// 	Lib_FilePutEnv("DIALNUM", "CMNET");
	// 	Lib_FilePutEnv("USERID",  "card");
	// 	Lib_FilePutEnv("USERPWD", "card");
	// 	Lib_FilePutEnv("IP",      "");
	// 	Lib_FilePutEnv("PORT",    "9005");
    // }

    // intro beeps
	// Display_Loading(10);
    // Lib_Beef(7, 200);
	// Lib_DelayMs(100);

    // Lib_Beef(0, 200);
	// Lib_DelayMs(100);
    // Lib_Beef(1, 200);
	// Lib_DelayMs(100);
    // Lib_Beef(2, 200);
	// Lib_DelayMs(100);
    // Lib_Beef(3, 200);
	// Lib_DelayMs(100);
    // Lib_Beef(4, 200);
	// Lib_DelayMs(100);
    // Lib_Beef(5, 200);
	// Lib_DelayMs(100);
    // Lib_Beef(6, 200);

	while (TRUE) {
		Lib_LcdCls();
		Lib_KbFlush();

        // display content using views
		if (view == VIEW_WAITING) {
			if (Display_Waiting()) view = VIEW_MAIN;
			else return 0;

		} else if (view == VIEW_MAIN) {
			Display_Topbar(NULL);
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
			if (status == STATUS_NEW) 		     Display_Topbar("New orders");
			else if (status == STATUS_PENDING)   Display_Topbar("Pending orders");
			else if (status == STATUS_PICKED_UP) Display_Topbar("Picked-up");
			else if (status == STATUS_DELIVERED) Display_Topbar("Delivered");

			switch (Display_List(Sample_Order_List, 4)) {
				case KEYCANCEL:
                case KEYMENU:
                    view = VIEW_MAIN;
                    break;

				default:
					// FIXME: first check if it is a number and there exists
					// an order with that number

					// order = (int) ucKey;
					view = VIEW_ORDER;
			}

		} else if (view == VIEW_ORDER) {
			Display_Topbar("Order CS146001");			
            if (status == STATUS_NEW) {
                switch (Display_Menu(New_Order_Menu, 4)) {
                    case KEY1:
                        if (Display_Confirm("Confirm acceptance\nof this order?", "Yes", "No")) {
                            Display_Notice("Order accepted.");
                            view = VIEW_MAIN;
                        }
                        break;

                    case KEY2:
                        if (Display_Confirm("Request changes to\nthis order?", "Yes", "No")) {
                            Display_Notice("Order changes \nrequested.");
                            view = VIEW_MAIN;
                        }
                        break;
                    
                    case KEY3:
                        if (Display_Confirm("Confirm rejection of\nthis order?", "Yes", "No")) {
                            Display_Notice("Order rejected.");
                            view = VIEW_MAIN;
                        }
                        break;
                    
                    case KEY4:
                        break;

                    case KEY5:
                        Print_Order("CS146001");
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
                        if (Display_Confirm("Request changes to\nthis order?", "Yes", "No")) {
                            Display_Notice("Order changes \nrequested.");
                            view = VIEW_MAIN;
                        }
                        break;
                    
                    case KEY2:
                        if (Display_Confirm("Confirm cancellation\nof this order?", "Yes", "No")) {
                            Display_Notice("Order cancelled.");
                            view = VIEW_MAIN;
                        }
                        break;
                    
                    case KEY3:
                        break;

                    case KEY4:
                        Print_Order("CS146001");
                        break;
                        
                    case KEYCANCEL:
                    case KEYBACKSPACE:
                        view = VIEW_ORDER_LIST;
                        break;

                    default:
                        view = VIEW_ORDER;
                }
            
            } else if (status == STATUS_PICKED_UP || status == STATUS_DELIVERED) {
                switch (Display_Menu(Non_Pending_Order_Menu, 4)) {
                    case KEY1:
                        break;

                    case KEY2:
                        Print_Order("CS146001");
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
			Display_Topbar("Settings");
            switch (Display_Menu(Settings_Menu, 4)) {
				default:
					view = VIEW_MAIN;
			}
		}
	}
	return 1;			
}




