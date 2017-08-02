#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>
#include <ctype.h>

#include "public.h"


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

    // reset communication ports
    Lib_ComReset(COM1);
    Lib_ComReset(COM2);
    Lib_ComReset(AT_COM);
    Lib_UsbReset();
    Wls_Reset();

    // initialize printer
    if (Lib_PrnInit()) return 1;

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

	while (TRUE) {
		Lib_LcdCls();
		Lib_KbFlush();
		
        DispTimer2();

		Lib_LcdPrintxy(0,0*8,0x80,"    Demo-App[v%s.%s]    ",MAJ_VER, MIN_VER);
		Lib_LcdClrLine(1*8, 8*8);
		Lib_LcdSetFont(16, 16, 0);

        // display page
		if (page == PAGE_WAITING) {
			Lib_LcdGotoxy(x, y);
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




