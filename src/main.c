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

int main(void) {
	int iret;
	int page = PAGE_WAITING;
	unsigned char ucKey;
	unsigned char ucWlsResetFlag;

	Lib_AppInit();
 
    // clear screen and keyboard
    Lib_LcdCls();
    Lib_LcdClrDotBuf();
    Lib_KbFlush();

	Lib_LcdSetFont(12, 12, 0);

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

	while (TRUE) {
		Lib_LcdCls();
		Lib_KbFlush();

        // display page
		if (page == PAGE_WAITING) {
			Lib_LcdGotoxy(0, 4);
            Lib_LcdDrawLogo(g_Display_logo_128);
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
        if (page == PAGE_WAITING) {
			switch(ucKey) {
			default:
				break; 
			}
		}
	}
	return 0;			
}




