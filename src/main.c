#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>
#include <ctype.h>
#include <time.h>

#include "public.h"
#include "logos.h"
#include "icons.h"
#include "topbar.h"
#include "sms_pdu.h"
#include "cookshop.h"
#include "bencode.h"

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
#define STATUS_COMPLETED  3

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
    "[3] Completed orders",
    "[4] Settings",
    NULL
};

char *New_Order_Menu[] = {
    "[1] Print order",
    "[2] Accept order",
    "[3] Change order",
    "[4] Reject order",
    NULL
};

char *Pending_Order_Menu[] = {
    "[1] Print order",
    "[2] Change order",
    "[3] Cancel order",
    NULL
};

char *Non_Pending_Order_Menu[] = {
    "[1] Print order",
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

char title[16];
char order_number[8];

static int view     = VIEW_WAITING;
static int status   = STATUS_UNKNOWN;

void Beep_Cookshop() {
    Lib_Beef(6, 300);
    Lib_Beef(3, 300);    
}

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

        Lib_LcdSetFont(LCD_FONT_SMALL);
        if (strlen(title)) Display_Title(title);
        else Display_Time();

        Display_Battery();
        Display_Signal();

        Lib_SetTimer(TIMER_TOPBAR, TIMER_5SEC);
    }
}

unsigned char Display_Waiting(int force, int reset) {
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

        int beep_now = TRUE;
        while (TRUE) {
            unsigned char msg[SMS_MESSAGE_LENGTH];
            char envvar[120];
            int msg_len;

            // annoying beep if new orders
            Lib_FileGetEnv(ORDERS_NEW, envvar);
            if (beep_now && count_order_numbers(envvar)) {
                Beep_Cookshop();
                view = VIEW_ORDER_LIST;
                status = STATUS_NEW;
                beep_now = FALSE;
            } else beep_now = TRUE;

            memset(msg, 0, sizeof(msg));
            sms_get_msg(&msg, &msg_len, SMS_MESSAGE_LENGTH);
            
            if (strlen(msg)) {
                order_t order;

                memset(order.bencode, 0, sizeof(order.bencode));
                strcpy(order.bencode, msg);
                
                if (order_parse(&order)) order_save(&order);
            }
            
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
                    Clear_Content();
                    Display_Topbar(TRUE);
                    return ucKey;
            }
        }
    }
    if (reset) {
        Lib_StopTimer(TIMER_WAITING);
        Lib_SetTimer(TIMER_WAITING, TIMER_30SEC);
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

    Display_Topbar(TRUE);

    // scrolling
    while (len) {
        int count = 0;
        char** temp = (menu + scroll);

        Clear_Content();
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

            if (Lib_KbCheck()) {
                if (Display_Waiting(FALSE, FALSE)) break;
                else continue;
            } else Display_Waiting(FALSE, TRUE);

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

    // scrolling
    while (len) {
        int count = 0;
        char** temp = (list + ((scroll / lines) * lines));

        Clear_Content();
        Lib_LcdSetFont(LCD_FONT_MEDIUM);
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

            if (Lib_KbCheck()) {
                if (Display_Waiting(FALSE, FALSE)) break;
                else continue;
            } else Display_Waiting(FALSE, TRUE);

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

        if (Lib_KbCheck()) {
            if (Display_Waiting(FALSE, FALSE)) break;
            else continue;
        } else Display_Waiting(FALSE, TRUE);

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

        if (Lib_KbCheck()) continue;
        ucKey = Lib_KbGetCh();

        switch (ucKey) {
            case KEYOK:
            case KEYENTER:
            case KEYCANCEL:
            case KEYCLEAR:
                return;
            
            default:
                continue;
        }
    }
}

int Print_Wrapped_Line(char *text, int len) {
    char buf[50];
    char *start, *end;

    start = end = text;
    do {	
        memset(buf, 0, sizeof(buf));
        while (start[0] == ' ') { start++; end++; continue; }
        if (strlen(start) < len) end = start + strlen(start);
        if (end == start) end = (char *)strrstr(start, "\n", len);
        if (end == start) end = (char *)strrstr(start, " ", len);
        if (end == start) end = start + len;
        
        strncpy(buf, start, (end - start));
        Lib_PrnStr(buf);
        Lib_PrnStr("\n");
        start = ++end;
    } while (start < text + strlen(text));
}

int Print_Order(order_t *order) {
    int i;
    unsigned long int integer;
    char text_short[51];
    char text_medium[101];
    char text_long[501];
    char large_line[33]; // large line holds 32 characters
    char medium_line[49]; // small line holds 48 characters
    struct tm ts;

	Lib_PrnInit();

    Lib_PrnStr("\n"); // give some space
    Lib_PrnLogo(g_Display_logo_384);

    Lib_PrnSetFont(PRN_FONT_LARGE);

    // number
    memset(large_line, 0, sizeof(large_line));
    sprintf(large_line, "        Order: CS%i\n", order->number);
    Lib_PrnStr(large_line);

    // type
    memset(large_line, 0, sizeof(large_line));
    if (order->type == 'P') Lib_PrnStr("          Type: Pickup\n");
    else if (order->type == 'D') Lib_PrnStr("         Type: Delivery\n");

    // location
    memset(text_short, 0, sizeof(text_short));
    if (order_get_location(order, text_short)) {
        memset(text_medium, 0, sizeof(text_medium));
        memset(large_line, 0, sizeof(large_line));
        sprintf(text_medium, "Location: %s", text_short);
        sprintf(large_line, "%*s\n", center_padding(32, strlen(text_medium)), text_medium);
        Lib_PrnStr(large_line);
    }

    // date and time
    memset(text_short, 0, sizeof(text_short));
    memset(text_medium, 0, sizeof(text_medium));
    strftime(text_medium, sizeof(text_medium), "%B %d, %Y at %I:%M %p", localtime(&order->timestamp));
    sprintf(large_line, "\n%*s\n", center_padding(32, strlen(text_medium)), text_medium);
    Lib_PrnStr(large_line);

    // user information
    memset(text_short, 0, sizeof(text_short));
    memset(text_medium, 0, sizeof(text_medium));
    memset(text_long, 0, sizeof(text_long));
    if (order_get_user(order, &integer, text_medium, text_short, text_long)) {
        Lib_PrnStr("\n\n");

        memset(large_line, 0, sizeof(large_line));
        sprintf(large_line, "%*.32s\n", center_padding(32, strlen(text_medium)), text_medium);
        Lib_PrnStr(large_line);        
        
        if (strlen(text_long)) {
            memset(large_line, 0, sizeof(large_line));
            sprintf(large_line, "%*.32s\n", center_padding(32, strlen(text_long)), text_long);
            Lib_PrnStr(large_line); 
        }       
        
        if (strlen(text_short)) {
            memset(large_line, 0, sizeof(large_line));
            sprintf(large_line, "%*s\n", center_padding(32, strlen(text_short)), text_short);
            Lib_PrnStr(large_line);  
        }      
    }

    // items
    if (order->items_length) {
        Lib_PrnStr("\n\n");
        for (i = 0; i < order->items_length; i++) {
            memset(text_short, 0, sizeof(text_short));
            memset(text_medium, 0, sizeof(text_medium));
            memset(text_long, 0, sizeof(text_long));
            if (order_get_item(order, i, &integer, text_medium, text_short)) {
                Lib_PrnStr("--------------------------------\n");

                // item name
                memset(large_line, 0, sizeof(large_line));
                // sprintf(large_line, "%s\n", text_medium);
                Print_Wrapped_Line(text_medium, 32);  

                // restaurant name
                memset(text_short, 0, sizeof(text_short));
                memset(text_medium, 0, sizeof(text_medium));
                memset(text_long, 0, sizeof(text_long));
                if (order_get_item_restaurant(order, i, &integer, text_medium, text_short)) {
                    Lib_PrnSetFont(PRN_FONT_MEDIUM);
                    // memset(medium_line, 0, sizeof(medium_line));
                    // sprintf(medium_line, "%s\n", text_medium);
                    Print_Wrapped_Line(text_medium, 48);
                    Lib_PrnSetFont(PRN_FONT_LARGE);
                }

                // quantity and sub-total
                memset(text_short, 0, sizeof(text_short));
                memset(text_medium, 0, sizeof(text_medium));
                if ((order_get_item_quantity(order, i, &integer)) && (order_get_item_total(order, i, text_medium))) {
                    memset(large_line, 0, sizeof(large_line));
                    sprintf(large_line, "Qty: %-10.iSub-Total: %6.6s\n", integer, text_medium);
                    Lib_PrnStr(large_line);
                }
            }
        }
    }

    // fees
    memset(text_short, 0, sizeof(text_short));
    memset(large_line, 0, sizeof(large_line));
    if (order_get_fee(order, text_short) && strlen(text_short) && (strcmp(text_short, "0"))) {
        sprintf(large_line, "Fees %27.27s\n", text_short);
        Lib_PrnStr("--------------------------------\n");
        Lib_PrnStr(large_line);
    }

    // discount
    memset(text_short, 0, sizeof(text_short));
    memset(large_line, 0, sizeof(large_line));
    if (order_get_discount(order, text_short) && strlen(text_short) && (strcmp(text_short, "0"))) {
        sprintf(large_line, "Discount %22.22s\n", text_short);
        Lib_PrnStr("--------------------------------\n");
        Lib_PrnStr(large_line);
    }

    // extra
    memset(text_short, 0, sizeof(text_short));
    memset(large_line, 0, sizeof(large_line));
    if (order_get_extra(order, text_short) && strlen(text_short) && (strcmp(text_short, "0"))) {
        sprintf(large_line, "Tips %27.27s\n", text_short);
        Lib_PrnStr("--------------------------------\n");
        Lib_PrnStr(large_line);
    }

    // // total
    memset(text_short, 0, sizeof(text_short));
    memset(large_line, 0, sizeof(large_line));
    if (order_get_total(order, text_short) && strlen(text_short)) {
        if (!strcmp(text_short, "0")) strcpy(text_short, "0.00");
        sprintf(large_line, "Total %26.26s\n", text_short);
        Lib_PrnStr("================================\n");
        Lib_PrnStr(large_line);
    }

    // paid
    memset(text_short, 0, sizeof(text_short));
    memset(large_line, 0, sizeof(large_line));
    if (order_get_paid(order, text_short) && strlen(text_short) && strcmp(text_short, "0")) {
        sprintf(large_line, "Total Paid %21.21s\n", text_short);
        Lib_PrnStr("--------------------------------\n");
        Lib_PrnStr(large_line);
    }

    // cash due
    memset(text_short, 0, sizeof(text_short));
    memset(large_line, 0, sizeof(large_line));
    if (order_get_cash_due(order, text_short) && strlen(text_short)) {
        if (!strcmp(text_short, "0")) strcpy(text_short, "0.00");
        sprintf(large_line, "Total Due %22.22s\n", text_short);
        Lib_PrnStr("================================\n");
        Lib_PrnStr(large_line);
    }
    
    Lib_PrnStr("================================\n");

    // directions
    memset(text_long, 0, sizeof(text_long));
    if (order_get_directions(order, text_long) && strlen(text_long)) {
        Lib_PrnStr("\n\n");
        sprintf(large_line, "Delivery Instructions: %s", text_long);
        Print_Wrapped_Line(large_line, 32);        
    }

    // special instructions
    memset(text_long, 0, sizeof(text_long));
    if (order_get_instructions(order, text_long) && strlen(text_long)) {
        Lib_PrnStr("\n\n");
        sprintf(large_line, "Special Instructions: %s", text_long);
        Print_Wrapped_Line(large_line, 32);        
    }

    Lib_PrnStr("\n\n\n");
    Lib_PrnStr("  * * * www.cookshop.biz * * *  \n");
    Lib_PrnStr("\n\n\n\n\n\n\n\n\n\n");

	// start printing
    Lib_PrnStart();

    return 0;
}

int main(void) {
    unsigned char list_value;
    unsigned char list_env_value[2];

	Lib_AppInit();
 
    // clear screen and keyboard
    Lib_LcdCls();
    Lib_LcdClrDotBuf();
    Lib_KbFlush();

    // Loading
 	Display_Loading(-1);
    Lib_DelayMs(500);
    Beep_Cookshop(); 

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
    Wls_Reset();
    memset(list_env_value, 0, sizeof(list_env_value));
    Lib_FileGetEnv("SIMNO", list_env_value);
    Wls_SelectSim((int)list_env_value[0]);
    Wls_Init();

    // setup order files
    unsigned char envvar[120];
    memset(envvar, 0, sizeof(envvar));
    if (Lib_FileGetEnv(ORDERS_NEW, envvar)) Lib_FilePutEnv(ORDERS_NEW, "");
    if (Lib_FileGetEnv(ORDERS_PENDING, envvar)) Lib_FilePutEnv(ORDERS_PENDING, "");
    if (Lib_FileGetEnv(ORDERS_COMPLETED, envvar)) Lib_FilePutEnv(ORDERS_COMPLETED, "");
    
	while (TRUE) {
        Lib_LcdCls();
        Lib_LcdClrDotBuf();
        Lib_KbFlush();
        Refresh_Settings();

        memset(list_env_value, 0, sizeof(list_env_value));

        // display content using views
		if (view == VIEW_WAITING) {
            memset(title, 0, sizeof(title));
			if (Display_Waiting(TRUE, TRUE)) view = VIEW_MAIN;
			else return 0;

		} else if (view == VIEW_MAIN) {
			memset(title, 0, sizeof(title));
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
                    status = STATUS_COMPLETED;
                    break;
				
				case KEY4:
                    view = VIEW_SETTINGS;
                    break;

				default:
					view = VIEW_WAITING;
			}

		} else if (view == VIEW_ORDER_LIST) {
            int i;
            char *p;
            char envval[120];
            char order_list[20][24];
            char *order_list_pp[20];

            memset(title, 0, sizeof(title));
            memset(envval, 0, sizeof(envval));
            if (status == STATUS_NEW) {
                strcpy(title, "New orders");
                Lib_FileGetEnv(ORDERS_NEW, envval);
            }
			else if (status == STATUS_PENDING) {
                strcpy(title, "Pending orders");
                Lib_FileGetEnv(ORDERS_PENDING, envval);
            }
			else if (status == STATUS_COMPLETED) {
                strcpy(title, "Completed orders");
                Lib_FileGetEnv(ORDERS_COMPLETED, envval);
            }

            memset(order_list, 0, sizeof(order_list));
            memset(order_list_pp, 0, sizeof(order_list_pp));
            if (strlen(envval)) {
                // place orders in list
                i = 0; 
                p = strtok(envval, ",");
                while (p != NULL) {
                    strcat(order_list[i], "CS");
                    strcat(order_list[i], p);
                    order_list_pp[i] = *(order_list + i);
                    p = strtok(NULL, ",");
                    i++;
                }
                order_list_pp[i] = NULL;

                int index = View_List(order_list_pp, 0);
                switch (index) {
                    case KEYCANCEL:
                    case KEYMENU:
                        view = VIEW_MAIN;
                        break;

                    default:
                        memset(order_number, 0, sizeof(order_number));
                        strcpy(order_number, order_list[index]);
                        view = VIEW_ORDER;
                }
            } else {
                Display_Notice("No orders found.");
                view = VIEW_MAIN;
            }
		} else if (view == VIEW_ORDER) {
            order_t order;
            int order_found = FALSE;

            if (strlen(order_number)) {
                memset(title, 0, sizeof(title));
                sprintf(title, "Order %s", order_number);

                if (order_find(&order, order_number) && order_parse(&order)) order_found = TRUE;
            }

            if (order_found) {
                if (status == STATUS_NEW) {
                    switch (View_Menu(New_Order_Menu)) {
                        case KEY1:
                            Print_Order(&order);
                            break;

                        case KEY2:
                            if (Display_Confirm("Confirm acceptance\nof this order?", "Yes", "No")) {
                                remove_order_number(order.number, ORDERS_NEW);
                                add_order_number(order.number, ORDERS_PENDING);
                                // Display_Notice("Order accepted.");
                                view = VIEW_ORDER_LIST;
                            } break;

                        case KEY3:
                            if (Display_Confirm("Request changes to\nthis order?", "Yes", "No")) {
                                // FIXME: Send SMS to request changes
                                remove_order_number(order.number, ORDERS_NEW);
                                order_delete(&order);
                                // Display_Notice("Order changes \nrequested.");
                                view = VIEW_ORDER_LIST;
                            } break;

                        case KEY4:
                            if (Display_Confirm("Confirm rejection of\nthis order?", "Yes", "No")) {
                                // FIXME: Send SMS to reject order
                                remove_order_number(order.number, ORDERS_NEW);
                                order_delete(&order);
                                // Display_Notice("Order rejected.");
                                view = VIEW_ORDER_LIST;
                            } break;

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
                            Print_Order(&order);
                            break;

                        case KEY2:
                            if (Display_Confirm("Request changes to\nthis order?", "Yes", "No")) {
                                // FIXME: Send SMS to request changes
                                remove_order_number(order.number, ORDERS_PENDING);
                                order_delete(&order);
                                // Display_Notice("Order changes \nrequested.");
                                view = VIEW_ORDER_LIST;
                            } break;
                        
                        case KEY3:
                            if (Display_Confirm("Confirm cancellation\nof this order?", "Yes", "No")) {
                                // FIXME: Send SMS to cancel order
                                remove_order_number(order.number, ORDERS_NEW);
                                order_delete(&order);
                                // Display_Notice("Order cancelled.");
                                view = VIEW_ORDER_LIST;
                            } break;
                                                    
                        case KEYCANCEL:
                        case KEYBACKSPACE:
                            view = VIEW_ORDER_LIST;
                            break;

                        default:
                            view = VIEW_ORDER;
                    }
                
                } else if (status == STATUS_COMPLETED) {
                    switch (View_Menu(Non_Pending_Order_Menu)) {
                        case KEY1:
                            Print_Order(&order); 
                            break;
                            
                        case KEYCANCEL:
                        case KEYBACKSPACE:
                            view = VIEW_ORDER_LIST; break;

                        default:
                            view = VIEW_ORDER;
                    }
                
                }
            } else {
                Display_Notice("No order found.");
                view = VIEW_ORDER_LIST;
            }

		} else if (view == VIEW_SETTINGS) {
            memset(title, 0, sizeof(title));
			strcpy(title, "Settings");
            switch (View_Menu(Settings_Menu)) {
                case KEY1:
                    view = VIEW_SETTINGS_DISPLAY; break;

                case KEY2:
                    view = VIEW_SETTINGS_SOUND; break;

                case KEY3:
                    view = VIEW_SETTINGS_NETWORK;
                    strcpy(title, "Network");
                    break;

                case KEY4:
                    view = VIEW_SETTINGS_PRINTER;
                    strcpy(title, "Printer");
                    break;

                case KEY5:
                    view = VIEW_SETTINGS_CLOCK;
                    strcpy(title, "Clock");
                    break;

                case KEY6:
                    view = VIEW_SETTINGS_BATTERY;
                    strcpy(title, "Battery");
                    break;

				case KEYCANCEL:
                case KEYMENU:
                    view = VIEW_MAIN;
                    break;

				default:
					view = VIEW_SETTINGS;
			}
		} else if (view == VIEW_SETTINGS_DISPLAY) {
            memset(title, 0, sizeof(title));
			strcpy(title, "Display");
            switch (View_Menu(Settings_Display_Menu)) {
                case KEY1:
                    memset(title, 0, sizeof(title));
                    strcpy(title, "Contrast");
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
                    memset(title, 0, sizeof(title));
                    strcpy(title, "Backlight");
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
            memset(title, 0, sizeof(title));
            strcpy(title, "Sound");
            switch (View_Menu(Settings_Sound_Menu)) {
                case KEY1:
                    memset(title, 0, sizeof(title));
                    strcpy(title, "Keypad");
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
                    memset(title, 0, sizeof(title));
                    strcpy(title, "Ringtone");
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
            memset(title, 0, sizeof(title));
			strcpy(title, "Network");
            switch (View_Menu(Settings_Network_Menu)) {
                case KEY1:
                    memset(title, 0, sizeof(title));
                    strcpy(title, "Select SIM");
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
                            Display_Notice("Restart device to\nreset settings.");
                    }
                    break;

                case KEY2:
                    strcpy(title, "Enter PIN");
                    Display_Notice("Enter PIN");
                    break;

                case KEY3:
                    memset(title, 0, sizeof(title));
                    strcpy(title, "Check Status");
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
            memset(title, 0, sizeof(title));
			strcpy(title, "Printer");
            switch (View_Menu(Settings_Printer_Menu)) {
                case KEY1:
                    memset(title, 0, sizeof(title));
                    strcpy(title, "Contrast");
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
                    memset(title, 0, sizeof(title));
                    strcpy(title, "Speed");
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
            memset(title, 0, sizeof(title));
        }
    }
	return 1;			
}




