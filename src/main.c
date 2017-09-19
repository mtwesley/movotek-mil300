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

        Lib_LcdSetFont(LCD_FONT_SMALL);
        if (title != NULL) Display_Title(title);
        else Display_Time();

        Display_Battery();
        Display_Signal();

        Lib_SetTimer(TIMER_TOPBAR, TIMER_5SEC);
    }
}

unsigned char Display_Waiting(int force) {
    unsigned char ucKey;

	if (!Lib_CheckTimer(TIMER_WAITING) || force) {
	// if (force) {
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
            unsigned char msg[SMS_MESSAGE_LENGTH];
            int msg_len;

            memset(msg, 0, sizeof(msg));
            sms_get_msg(&msg, &msg_len, SMS_MESSAGE_LENGTH);
            
            // char *test_bencode = "d6:ordersld8:cash_due5:37.008:currency3:USD10:directions66:Sinkor 10th st.-sea side-Payne avenue-Fayad building-Appartment #68:discount1:03:due5:37.005:extra1:03:fee1:02:idi15385e12:instructions0:5:itemsld4:code0:4:cost4:4.008:currency3:USD2:idi861e6:margin4:0.004:name10:Coke (Can)5:price4:2.008:quantityi2e10:restaurantd4:code3:DRK2:idi16e4:name6:Drinkse13:restaurant_idi16e3:tax1:05:total4:4.00ed4:code0:4:cost4:7.008:currency3:USD2:idi3903e6:margin4:0.004:name23:Ura Maki Atlantic Green5:price4:7.008:quantityi1e10:restaurantd4:code3:BSB2:idi203e4:name29:Barracuda Seafood & Sushi Bare13:restaurant_idi203e3:tax1:05:total4:7.00ed4:code0:4:cost4:6.008:currency3:USD2:idi3949e6:margin4:0.004:name24:Temaki Crispy Spicy Tuna5:price4:6.008:quantityi1e10:restaurantd4:code3:BSB2:idi203e4:name29:Barracuda Seafood & Sushi Bare13:restaurant_idi203e3:tax1:05:total4:6.00ed4:code0:4:cost5:20.008:currency3:USD2:idi3988e6:margin4:0.004:name12:Crab Platter5:price5:20.008:quantityi1e10:restaurantd4:code3:BSB2:idi203e4:name29:Barracuda Seafood & Sushi Bare13:restaurant_idi203e3:tax1:05:total5:20.00ee8:locationd4:code3:SKR2:idi4e4:name6:Sinkore11:location_idi4e6:numberi115385e4:owed1:04:paid1:06:status1:C9:timestampi1468003305e5:total5:37.004:type1:D4:userd5:email16:i.15@hotmail.com2:idi1205e4:name9:Ali Fayad5:phone10:0776773333e7:user_idi1205eeee";
            // strcpy(msg, test_bencode);

            if (strlen(msg)) {
                order_t order;

                memset(order.bencode, 0, sizeof(order.bencode));
                strcpy(order.bencode, msg);
                
                if (order_parse(&order)) {
                    Print_Order(&order);
                    
                    int fid;
                    char fname[16];

                    // open file for status
                    memset(fname, 0, sizeof(fname));
                    sprintf(fname, "Order_%s", order.status);
                    if (Lib_FileExist(fname) != FILE_NOTEXIST) Lib_FileRemove(fname);                    
                    fid = Lib_FileOpen(fname, O_CREATE);
                    Lib_FileSeek(fid, 0, FILE_SEEK_SET);
                    Lib_FileWrite(fid, order.bencode, strlen(order.bencode));

                    // scroll through status files to check for order
                    // memset(fname, 0, sizeof(fname));
                    // sprintf(fname, "Order_%s", order.status);
                    
                }
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
        // if (Display_Waiting(FALSE)) break;

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

    Lib_PrnStr("\n\n\n"); // give some space
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
        sprintf(large_line, "%*s\n", center_padding(32, strlen(text_medium)), text_medium);
        Lib_PrnStr(large_line);        
        
        if (strlen(text_long)) {
            memset(large_line, 0, sizeof(large_line));
            sprintf(large_line, "%*s\n", center_padding(32, strlen(text_long)), text_long);
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
                sprintf(large_line, "%s\n", text_medium);
                Lib_PrnStr(large_line);  

                // restaurant name
                memset(text_short, 0, sizeof(text_short));
                memset(text_medium, 0, sizeof(text_medium));
                memset(text_long, 0, sizeof(text_long));
                if (order_get_item_restaurant(order, i, &integer, text_medium, text_short)) {
                    Lib_PrnSetFont(PRN_FONT_MEDIUM);
                    memset(medium_line, 0, sizeof(medium_line));
                    sprintf(medium_line, "%s\n", text_medium);
                    Lib_PrnStr(medium_line);
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
    if (order_get_fee(order, text_short) && strlen(text_short) && (strcmp(text_short, "0"))) {
        sprintf(large_line, "Fees %27.27s\n", text_short);
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

    // // cash due
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
        sprintf(large_line, "Delivery Instructions: %s\n", text_long);
        Lib_PrnStr(large_line);        
    }

    // special instructions
    memset(text_long, 0, sizeof(text_long));
    if (order_get_instructions(order, text_long) && strlen(text_long)) {
        Lib_PrnStr("\n\n");
        sprintf(large_line, "Special Instructions: %s\n", text_long);
        Lib_PrnStr(large_line);        
    }

    Lib_PrnStr("\n\n\n");
    Lib_PrnStr("  * * * www.cookshop.biz * * *  \n");
    Lib_PrnStr("\n\n\n\n\n\n\n\n\n\n");

	// start printing
    Lib_PrnStart();

    return 0;
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
    Wls_Init();

    // PDU mode
    // Wls_SendCmdRequest("AT+CMGF=0\r", 10);

    // initialize printer
	// if (Lib_PrnInit()) return 1;

  	// Display_Loading(9);
	// Lib_DelayMs(1500);

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
                        // Print_Order("CS146001"); 
                        break;

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
                        // Print_Order("CS146001");
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
                        // Print_Order("CS146001"); 
                        break;
                        
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
                            Display_Notice("Resetting network\nsettings.");
                            Wls_Reset();
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




