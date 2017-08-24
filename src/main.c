#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>
#include <ctype.h>

#include "public.h"
#include "logos.h"
#include "icons.h"
#include "topbar.h"
#include "sms_pdu.h"

#define VIEW_WAITING      0
#define VIEW_MAIN         1
#define VIEW_ORDER        2
#define VIEW_ORDER_LIST   3
#define VIEW_SETTINGS     4

#define VIEW_SETTINGS_DISPLAY  41
#define VIEW_SETTINGS_SOUND    42
#define VIEW_SETTINGS_NETWORK  43
#define VIEW_SETTINGS_PRINTER  44
#define VIEW_SETTINGS_CLOCK    45
#define VIEW_SETTINGS_BATTERY  46

#define STATUS_UNKNOWN    0
#define STATUS_NEW        1
#define STATUS_PENDING    2
#define STATUS_PICKED_UP  3
#define STATUS_DELIVERED  4

char *title = NULL;

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
    // "[5] Clock",
    // "[6] Battery",
    NULL
};

char *Settings_Display_Menu[] = {
    "[1] Contrast",
    "[2] Backlight",
    NULL
};

char *Settings_Sound_Menu[] = {
    "[1] Keypad",
    "[2] Ringtone",
    NULL
};

char *Settings_Network_Menu[] = {
    "[1] Select SIM",
    "[2] Enter PIN",
    "[3] Check status",
    NULL
};

char *Settings_Printer_Menu[] = {
    "[1] Contrast",
    "[2] Speed",
    NULL
};

char *Settings_Display_Contrast_List[] = {
    "Very High",
    "High",
    "Medium",
    "Low",
    "Very Low",
    NULL
};

char *Settings_Display_Backlight_List[] = {
    "Always off",
    "Sometimes on",
    "Always on",
    NULL
};

char *Settings_Sound_Keypad_List[] = {
    "Mute",
    "Unmute",
    NULL
};

char *Settings_Sound_Ringtone_List[] = {
    "Ringtone #1",
    "Ringtone #2",
    "Ringtone #3",
    "Ringtone #4",
    NULL
};

char *Settings_Network_Sim_List[] = {
    "SIM 1",
    "SIM 2",
    NULL
};

char *Settings_Printer_Speed_List[] = {
    "Fast",
    "Slow",
    NULL
};

char *Settings_Printer_Contrast_List[] = {
    "Very High",
    "High",
    "Medium",
    "Low",
    "Very Low",
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

void Refresh_Settings(void) {
    unsigned char value[2] = "\0";

    // Display
    if (!Lib_FileGetEnv("LCDGRAY", value)) Lib_LcdSetGray(((value[0] - 0x30) + 4) * 11);
    if (!Lib_FileGetEnv("BCKLIGHT", value)) Lib_LcdSetBackLight(value[0]);

    // Sound
    if (!Lib_FileGetEnv("KBMUTE", value)) Lib_KbMute(value[0]);

    // Printer
    if (!Lib_FileGetEnv("PRNGRAY", value)) Lib_PrnSetGray((value[0] - 0x30) + 3);
    if (!Lib_FileGetEnv("PRNSPEED", value)) Lib_PrnSetSpeed(((value[0] - 0x30) * 10) + 13);
}

void Clear_Topbar(void) {
    Lib_LcdGotoxy(0, 0);
    Lib_LcdDrawLogo(g_Display_topbar);
}

void Clear_Content(void) {
    Lib_LcdClrLine(14, 63);
}

void Display_Loading(int level) {
	// draw logo
    Lib_LcdCls();
	Lib_LcdGotoxy(0, 2);
	Lib_LcdDrawLogo(g_Display_logo_128);
	Lib_LcdGotoxy(0, 64 - 14);

	// draw text
	Lib_LcdSetFont(LCD_FONT_SMALL);
	
    if (level < 0);
	else if (level == 0) Lib_Lcdprintf("    Loading (0%%)     ");
	else if (level <= 3) Lib_Lcdprintf("    Loading (25%%)    ");
	else if (level <= 6) Lib_Lcdprintf("    Loading (50%%)    ");
	else if (level <= 9) Lib_Lcdprintf("    Loading (75%%)    ");
	else if (level > 9)  Lib_Lcdprintf("    Loading (100%%)   ");
}

void Display_Signal() {
	int signal = -1;

    Lib_LcdGotoxy(1, 0);

    // if sim or no signal
    if (Wls_CheckSim() || Wls_CheckSignal(&signal)) signal = -1;
    switch (signal) {
        case SIGNAL_VERY_STRONG:
            Lib_LcdDrawLogo(g_Display_icon_sig_4); break;
        
        case SIGNAL_STRONG:
            Lib_LcdDrawLogo(g_Display_icon_sig_4); break;
        
        case SIGNAL_NORMAL:
            Lib_LcdDrawLogo(g_Display_icon_sig_3); break;
        
        case SIGNAL_WEAK:
            Lib_LcdDrawLogo(g_Display_icon_sig_2); break;
        
        case SIGNAL_VERY_WEAK:
            Lib_LcdDrawLogo(g_Display_icon_sig_1); break;

        default:
            Lib_LcdDrawLogo(g_Display_icon_sig_0);
    }
}

void Display_Battery() {
    int voltage = 0;
    Lib_LcdGotoxy(128 - 17, 0);

	if (Lib_GetBatChargeStatus() == BAT_CHARGE_ING) Lib_LcdDrawLogo(g_Display_icon_chg);
    else {
        voltage = Lib_GetBatteryVolt();
        if (voltage >= BATTERY_LEVEL5) Lib_LcdDrawLogo(g_Display_icon_bat_5);
        else if (voltage >= BATTERY_LEVEL4) Lib_LcdDrawLogo(g_Display_icon_bat_5);
        else if (voltage >= BATTERY_LEVEL3) Lib_LcdDrawLogo(g_Display_icon_bat_3);
        else if (voltage >= BATTERY_LEVEL2) Lib_LcdDrawLogo(g_Display_icon_bat_2);
        else if (voltage >= BATTERY_LEVEL1) Lib_LcdDrawLogo(g_Display_icon_bat_1);
        else Lib_LcdDrawLogo(g_Display_icon_bat_0);
	}
}

void Display_Title(char *title) {
    Lib_LcdPrintxy(24, 2, 0x80, "%-14s", title);    
}

void Display_Time() {
    int year, month, day, hour, minute, second, week;
    unsigned char datetime[8];
    
    Lib_GetDateTime(datetime);
    
    // year   = (datetime[0] >> 4) * 10 + (datetime[0] & 0x0f) + 1900;
    // month  = (datetime[1] >> 4) * 10 + (datetime[1] & 0x0f);
    // day    = (datetime[2] >> 4) * 10 + (datetime[2] & 0x0f);
    hour   = (datetime[3] >> 4) * 10 + (datetime[3] & 0x0f);
    minute = (datetime[4] >> 4) * 10 + (datetime[4] & 0x0f);
    // second = (datetime[5] >> 4) * 10 + (datetime[5] & 0x0f);
    // week   = (datetime[6] >> 4) * 10 + (datetime[6] & 0x0f);
    // if (datetime[0] <= 0x49) year += 100;

    if (hour < 1)       Lib_LcdPrintxy(24, 2, 0x80,  "  12:%02d AM  ", minute);
    else if (hour < 10) Lib_LcdPrintxy(24, 2, 0x80,  "   %d:%02d AM   ", hour, minute);
    else if (hour < 12) Lib_LcdPrintxy(24, 2, 0x80,  "  %d:%02d AM  ", hour, minute);
    else if (hour < 13) Lib_LcdPrintxy(24, 2, 0x80,  "  12:%02d PM  ", minute);
    else if (hour > 12) Lib_LcdPrintxy(24, 2, 0x80,  "   %d:%02d PM   ", hour - 12, minute);
}

void Display_Topbar(int force) {
	if (!Lib_CheckTimer(TIMER_TOPBAR) || force) {
        Clear_Topbar();

        Display_Signal();
        Display_Battery();

        Lib_LcdSetFont(LCD_FONT_SMALL);

        if (title != NULL) Display_Title(title);
        else Display_Time();

        Lib_SetTimer(TIMER_TOPBAR, TIMER_5SEC);
    }
}

unsigned char Display_Waiting(int force) {
    uchar buf[120];
    uchar *p;
    unsigned char ucKey;

	if (!Lib_CheckTimer(TIMER_WAITING) || force) {
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
            ushort len;
            char *text;
            uchar *rsp;
            
            Lib_Beef(6,300);
            Lib_Beef(3,300);
            Lib_DelayMs(1000);
            
            Wls_Init();
            memset(buf, 0, sizeof(buf));
            switch (Wls_SendCmd("AT+CMGR=1\r", rsp, len, 1000)) {
                case WLS_PARAMERR: text = "Parameter error"; break;
                case WLS_RSPERR: text = "Module error"; break;
                case WLS_NORSP: text = "No response"; break;
                case WLS_PORTERR: text = "Comm error"; break;
                case WLS_OK: 
                    sprintf(text, "Length: %i\nMessage: %s", len, rsp);
                    break;
            }

            Lib_LcdCls();
            Lib_LcdGotoxy(0, 2);
            Lib_Lcdprintf(text);
            Lib_DelayMs(5000);

            // int *iRes;
            // unsigned int len;
            // unsigned char *phone, *timestamp, *msg;
            
            // Lib_Beef(6,300);
            // Lib_Beef(3,300);
            // Lib_DelayMs(1000);

            // iRes = Wls_Readmessage(phone, timestamp, msg, len);
            
            // if (iRes) {
            //     Lib_Beef(6, 300);
            //     Lib_Beef(3, 300);
            //     char *text;
            //     sprintf(text, "Return: %i\nMessage: %s", iRes, *msg);

            //     Lib_LcdCls();
            //     Lib_LcdGotoxy(0, 2);
            //     Lib_Lcdprintf(text);
            //     Lib_DelayMs(5000);
            // } else {
            //     Lib_LcdGotoxy(0, 2);
            //     // Lib_Lcdprintf("Message found");
            //     Lib_DelayMs(5000);
            // }

            // if (Wls_GetModuleType(moduletype)) text = "Error";
            // else switch (*moduletype) {
            //     case WLS_UNKNOWN: text = "Unknown"; break;
            //     case WLS_GPRS_SIM340W: text = "SIM 340W"; break;
            //     case WLS_GPRS_G610: text = "G610"; break;
            //     default: text = "Default";
            // }
    
            
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
                    Lib_SetTimer(TIMER_WAITING, TIMER_30SEC);
                    return ucKey;
            }
        }
    }
    return 0;
}

unsigned char View_Menu(char **menu) {
    int len = 0;
    int scroll = 0;
    int lines = 4;
    unsigned char ucKey;

    // menu length
    while (menu[len++] != NULL); len--;

    Clear_Content();
    Display_Topbar(TRUE);

    // scrolling
    while (len) {
        int count = 0;
        char** temp = (menu + scroll);

        Lib_LcdSetFont(LCD_FONT_MEDIUM);
        Lib_LcdGotoxy(0, 14);
        
        while (*temp != NULL && count < lines) {
            Lib_Lcdprintf("%s\n", *temp++);
            count++;
        }

		Lib_KbFlush();
        while (TRUE) {
			int breakout = FALSE;

            Display_Topbar(FALSE);
            if (Display_Waiting(FALSE)) break;

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

int View_List(char **list, int scroll) {
    int len = 0;
    // int scroll = 0;
    int lines = 4;
    unsigned char ucKey;

    // list length
    while (list[len++] != NULL); len--;	

    if (scroll < 0 || len < scroll) scroll = 0;

    Display_Topbar(TRUE);
    Lib_LcdSetFont(LCD_FONT_MEDIUM);

    // scrolling
    while (len) {
        int count = 0;
        char** temp = (list + ((scroll / lines) * lines));

        Clear_Content();
        Lib_LcdGotoxy(0, 14);
        
        while (*temp != NULL && count < lines) {
            if (count == (scroll % lines)) Lib_Lcdprintf("[*] %s\n", *temp++);
            else Lib_Lcdprintf("[ ] %s\n", *temp++);
            count++;
        }

		Lib_KbFlush();
        while (TRUE) {
			int breakout = FALSE;

            Display_Topbar(FALSE);
            if (Display_Waiting(FALSE)) break;

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
    Display_Topbar(TRUE);
    Lib_LcdSetFont(LCD_FONT_MEDIUM);
    
    // print message
    Lib_LcdGotoxy(0, 14);
    Lib_Lcdprintf("%s\n", message);

    // print confirmation
    Lib_LcdGotoxy(0, 4 + (12 * 3));
    Lib_Lcdprintf("[YES] %s\n", yes);
    Lib_Lcdprintf(" [NO] %s\n", no);

    Lib_KbFlush();
    while (TRUE) {
        int breakout = FALSE;

        Display_Topbar(FALSE);
        if (Display_Waiting(FALSE)) break;

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

int Display_Notice(char *message) {
    unsigned char ucKey;

    Clear_Content();
    Display_Topbar(TRUE);
    Lib_LcdSetFont(LCD_FONT_MEDIUM);
    
    // print message
    Lib_LcdGotoxy(0, 14);
    Lib_Lcdprintf("%s\n", message);

    // print confirmation
    Lib_LcdGotoxy(0, 4 + (12 * 4));
	Lib_Lcdprintf("[OK] Continue");

    Lib_KbFlush();
    while (TRUE) {
        Display_Topbar(FALSE);
        if (Display_Waiting(FALSE)) break;

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

    Lib_PrnSetFont(PRN_FONT_MEDIUM);

    Lib_PrnStr("\n\n");
    Lib_PrnStr("ABCDEFGHIJKLMNOPQRSTUVWXYZ\n");
    Lib_PrnStr("abcdefghijklmnopqrstuvwxyz\n");
    Lib_PrnStr("1234567890 1234567890 1234567890 1234567890 1234567890 1234567890 1234567890 1234567890\n");

    Lib_PrnSetFont(PRN_FONT_LARGE);

    Lib_PrnStr("\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n");

	// start printing
    Lib_PrnStart();
}

int main(void) {
    unsigned char *env_value;
    unsigned char list_value;
    unsigned char list_env_value[2] = "\0";

	Lib_AppInit();
 
    // clear screen and keyboard
    Lib_LcdCls();
    Lib_LcdClrDotBuf();
    Lib_KbFlush();

    // Loading
 	Display_Loading(-1);
 	Lib_DelayMs(500);
    Lib_Beef(6, 300);
    Lib_Beef(3, 300);

 	Display_Loading(0);
	Lib_DelayMs(1500);

	Display_Loading(3);
	Lib_DelayMs(1500);

 	Display_Loading(6);
	Lib_DelayMs(1500);

    // reset communication ports
    Lib_ComReset(COM1);
    Lib_ComReset(COM2);
    Lib_ComReset(AT_COM);
    // Lib_UsbReset(); // FIXME: find the port number for USB

 	Display_Loading(9);
	Lib_DelayMs(0);

    // initialize wireless
    Wls_SelectSim((int)list_env_value[0]);
    Wls_Reset();
    if (Wls_Init()) return 1;

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

    // Lib_Beep();
	// Lib_DelayMs(1000);

    // Lib_Beef(0, 2000);
	// Lib_DelayMs(100);
    // Lib_Beef(1, 2000);
	// Lib_DelayMs(100);
    // Lib_Beef(2, 2000);
	// Lib_DelayMs(100);
    // Lib_Beef(3, 2000);
	// Lib_DelayMs(100);
    // Lib_Beef(4, 2000);
	// Lib_DelayMs(100);
    // Lib_Beef(5, 2000);
	// Lib_DelayMs(100);
    // Lib_Beef(6, 2000);

    // Lib_Beef(6, 150);
    // Lib_Beef(3, 250);
    
	while (TRUE) {
        Lib_LcdCls();
        Lib_LcdClrDotBuf();
        Lib_KbFlush();
        Refresh_Settings();

        // display content using views
		if (view == VIEW_WAITING) {
            title = NULL;
			if (Display_Waiting(TRUE)) view = VIEW_MAIN;
			else return 0;

		} else if (view == VIEW_MAIN) {
			title = NULL;
			switch (View_Menu(Main_Menu)) {
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
			if (status == STATUS_NEW) 		     title = "New orders";
			else if (status == STATUS_PENDING)   title = "Pending orders";
			else if (status == STATUS_PICKED_UP) title = "Picked-up";
			else if (status == STATUS_DELIVERED) title = "Delivered";

			switch (View_List(Sample_Order_List, 0)) {
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
			title = "Order CS146001";			
            if (status == STATUS_NEW) {
                switch (View_Menu(New_Order_Menu)) {
                    case KEY1:
                        if (Display_Confirm("Confirm acceptance\nof this order?", "Yes", "No")) {
                            Display_Notice("Order accepted.");
                            view = VIEW_MAIN;
                        } break;

                    case KEY2:
                        if (Display_Confirm("Request changes to\nthis order?", "Yes", "No")) {
                            Display_Notice("Order changes \nrequested.");
                            view = VIEW_MAIN;
                        } break;
                    
                    case KEY3:
                        if (Display_Confirm("Confirm rejection of\nthis order?", "Yes", "No")) {
                            Display_Notice("Order rejected.");
                            view = VIEW_MAIN;
                        } break;
                    
                    case KEY4:
                        break;

                    case KEY5:
                        Print_Order("CS146001"); break;

                    case KEYCANCEL:
                    case KEYBACKSPACE:
                        view = VIEW_ORDER_LIST;
                        break;

                    default:
                        view = VIEW_ORDER;
                }
            
            } else if (status == STATUS_PENDING) {
                switch (View_Menu(Pending_Order_Menu)) {
                    case KEY1:
                        if (Display_Confirm("Request changes to\nthis order?", "Yes", "No")) {
                            Display_Notice("Order changes \nrequested.");
                            view = VIEW_MAIN;
                        } break;
                    
                    case KEY2:
                        if (Display_Confirm("Confirm cancellation\nof this order?", "Yes", "No")) {
                            Display_Notice("Order cancelled.");
                            view = VIEW_MAIN;
                        } break;
                    
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
                switch (View_Menu(Non_Pending_Order_Menu)) {
                    case KEY1:
                        break;

                    case KEY2:
                        Print_Order("CS146001"); break;
                        
                    case KEYCANCEL:
                    case KEYBACKSPACE:
                        view = VIEW_ORDER_LIST; break;

                    default:
                        view = VIEW_ORDER;
                }
            
            }

		} else if (view == VIEW_SETTINGS) {
			title = "Settings";
            switch (View_Menu(Settings_Menu)) {
                case KEY1:
                    view = VIEW_SETTINGS_DISPLAY; break;

                case KEY2:
                    view = VIEW_SETTINGS_SOUND; break;

                case KEY3:
                    view = VIEW_SETTINGS_NETWORK;
                    title = "Network";
                    break;

                case KEY4:
                    view = VIEW_SETTINGS_PRINTER;
                    title = "Printer";
                    break;

                case KEY5:
                    view = VIEW_SETTINGS_CLOCK;
                    title = "Clock";
                    break;

                case KEY6:
                    view = VIEW_SETTINGS_BATTERY;
                    title = "Battery";
                    break;

				case KEYCANCEL:
                case KEYMENU:
                    view = VIEW_MAIN;
                    break;

				default:
					view = VIEW_SETTINGS;
			}
		} else if (view == VIEW_SETTINGS_DISPLAY) {
			title = "Display";
            switch (View_Menu(Settings_Display_Menu)) {
                case KEY1:
                    title = "Contrast";
                    if (Lib_FileGetEnv("LCDGRAY", list_env_value)) list_env_value[0] = 0x30;
                    list_value = View_List(Settings_Display_Contrast_List, list_env_value[0]);
                    switch (list_value) {
                        case KEYCANCEL:
                        case KEYMENU:
                            view = VIEW_SETTINGS_DISPLAY;
                            break;

                        default:
                            list_env_value[0] = list_value;
                            Lib_FilePutEnv("LCDGRAY", list_env_value);
                            view = VIEW_SETTINGS_DISPLAY;
                    }
                    break;

                case KEY2:
                    title = "Backlight";
                    if (Lib_FileGetEnv("BCKLIGHT", list_env_value)) list_env_value[0] = 0x30;
                    list_value = View_List(Settings_Display_Backlight_List, list_env_value[0]);
                    switch (list_value) {
                        case KEYCANCEL:
                        case KEYMENU:
                            view = VIEW_SETTINGS_DISPLAY;
                            break;

                        default:
                            list_env_value[0] = list_value;
                            Lib_FilePutEnv("BCKLIGHT", list_env_value);
                            view = VIEW_SETTINGS_DISPLAY;
                    }
                    break;

                case KEYCANCEL:
                case KEYMENU:
                    view = VIEW_SETTINGS;
                    break;

                default:
                    view = VIEW_SETTINGS_DISPLAY;
            }
		} else if (view == VIEW_SETTINGS_SOUND) {
            title = "Sound";
            switch (View_Menu(Settings_Sound_Menu)) {
                case KEY1:
                    title = "Keypad";
                    if (Lib_FileGetEnv("KBMUTE", list_env_value)) list_env_value[0] = 0x30;
                    list_value = View_List(Settings_Sound_Keypad_List, list_env_value[0]);
                    switch (list_value) {
                        case KEYCANCEL:
                        case KEYMENU:
                            view = VIEW_SETTINGS_SOUND;
                            break;

                        default:
                            list_env_value[0] = list_value;
                            Lib_FilePutEnv("KBMUTE", list_env_value);
                            view = VIEW_SETTINGS_SOUND;
                    }
                    break;

                case KEY2:
                    title = "Ringtone";
                    if (Lib_FileGetEnv("RINGTONE", list_env_value)) list_env_value[0] = 0x30;
                    list_value = View_List(Settings_Sound_Ringtone_List, list_env_value[0]);
                    switch (list_value) {
                        case KEYCANCEL:
                        case KEYMENU:
                            view = VIEW_SETTINGS_SOUND;
                            break;

                        default:
                            list_env_value[0] = list_value;
                            Lib_FilePutEnv("RINGTONE", list_env_value);
                            view = VIEW_SETTINGS_SOUND;
                    }
                    break;

                case KEYCANCEL:
                case KEYMENU:
                    view = VIEW_SETTINGS;
                    break;

                default:
                    view = VIEW_SETTINGS_SOUND;
            }
		} else if (view == VIEW_SETTINGS_NETWORK) {
			title = "Network";
            switch (View_Menu(Settings_Network_Menu)) {
                case KEY1:
                    title = "Select SIM";
                    if (Lib_FileGetEnv("SIMNO", list_env_value)) list_env_value[0] = 0x30;
                    list_value = View_List(Settings_Network_Sim_List, list_env_value[0]);
                    switch (list_value) {
                        case KEYCANCEL:
                        case KEYMENU:
                            view = VIEW_SETTINGS_NETWORK;
                            break;

                        default:
                            list_env_value[0] = list_value;
                            Lib_FilePutEnv("SIMNO", list_env_value);
                            Wls_SelectSim((int)list_env_value[0]);
                            view = VIEW_SETTINGS_NETWORK;
                    }
                    break;

                case KEY2:
                    title = "Enter PIN";
                    Display_Notice("Enter PIN");
                    break;

                case KEY3:
                    title = "Check Status";
                    switch (Wls_CheckSim()) {
                        case WLS_OK: Display_Notice("SIM status OK."); break;
                        case WLS_NOSIM: Display_Notice("No SIM found."); break;
                        case WLS_NEEDPIN: Display_Notice("PIN needed for SIM."); break;
                        case WLS_NEEDPUK: Display_Notice("PUK needed for SIM."); break;
                        case WLS_RSPERR: Display_Notice("SIM module error."); break;
                        case WLS_NORSP: Display_Notice("SIM no response."); break;
                        case WLS_UNKNOWNTYPE: Display_Notice("SIM status OK."); break;
                        case WLS_PORTERR: Display_Notice("Wireless port error."); break;
                        case WLS_OTHERR:
                        default: Display_Notice("Unknown error\noccurred."); break;
                    }
                    break;

                case KEYCANCEL:
                case KEYMENU:
                    view = VIEW_SETTINGS;
                    break;

                default:
                    view = VIEW_SETTINGS_NETWORK;
            }
		} else if (view == VIEW_SETTINGS_PRINTER) {
			title = "Printer";
            switch (View_Menu(Settings_Printer_Menu)) {
                case KEY1:
                    title = "Contrast";
                    if (Lib_FileGetEnv("PRNGRAY", list_env_value)) list_env_value[0] = 0x30;
                    list_value = View_List(Settings_Printer_Contrast_List, list_env_value[0]);
                    switch (list_value) {
                        case KEYCANCEL:
                        case KEYMENU:
                            view = VIEW_SETTINGS_PRINTER;
                            break;

                        default:
                            list_env_value[0] = list_value;
                            Lib_FilePutEnv("PRNGRAY", list_env_value);
                            view = VIEW_SETTINGS_PRINTER;
                    }
                    break;

                case KEY2:
                    title = "Speed";
                    if (Lib_FileGetEnv("PRNSPEED", list_env_value)) list_env_value[0] = 0x30;
                    list_value = View_List(Settings_Printer_Speed_List, list_env_value[0]);
                    switch (list_value) {
                        case KEYCANCEL:
                        case KEYMENU:
                            view = VIEW_SETTINGS_PRINTER;
                            break;

                        default:
                            list_env_value[0] = list_value;
                            Lib_FilePutEnv("PRNSPEED", list_env_value);
                            view = VIEW_SETTINGS_PRINTER;
                    }
                    break;

                case KEYCANCEL:
                case KEYMENU:
                    view = VIEW_SETTINGS;
                    break;

                default:
                    view = VIEW_SETTINGS_PRINTER;
            }
		} else {
            view = VIEW_WAITING;
            title = NULL;
        }
    }
	return 1;			
}




