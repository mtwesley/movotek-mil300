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

extern int Test_WlsAuTo(void);
extern int DispBattery(uchar x, uchar y);
extern int DispWlsSignal(uchar x, uchar y);
extern int AllTest_Battery(void);
extern int All_test_Print(void);


#define WLS_RST_TM    1
#define WLS_TM  3

uchar g_WlsResetStep = 0xff;
uchar g_NeedWlsReset = 0;  //1 need , 0 no need
uchar g_CurSimChannel;  //0- SIM1, 1-SIM2
uchar g_BatChargeStep = 0;





int main(void) {
	int iret;
	int page = 0;
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

    // test printer
    if (!Lib_PrnInit()) return 1;

    // set up environment
    if (Lib_FileExist("EnvFile") < 0) {
		Lib_FilePutEnv("DIALNUM", "CMNET");
		Lib_FilePutEnv("USERID",  "card");
		Lib_FilePutEnv("USERPWD", "card");
		Lib_FilePutEnv("IP",      "");
		Lib_FilePutEnv("PORT",    "9005");
    }	

	Lib_LcdCls();
	Lib_Lcdprintf("\nReset Wireless..");

	g_CurSimChannel = CHANNEL_SIM1;
	
    Wls_SelectSim(g_CurSimChannel);
	Wls_Reset();

	if (Wls_Init() != 0) {
		if (Wls_Init() != 0) {
			Lib_LcdCls();
			Lib_Lcdprintf("\n\n Reset error!!!");
			Lib_KbGetCh();
		}
	}

	Lib_LcdCls();
	Lib_Lcdprintf("\nReset Wireless OK");
	Lib_DelayMs(1000);

	while (1) {
		Lib_LcdCls();
		Lib_KbFlush();
		
        DispTimer2();

		Lib_LcdPrintxy(0,0*8,0x80,"    Demo-App[v%s.%s]    ",MAJ_VER, MIN_VER);
		Lib_LcdClrLine(1*8, 8*8);
		Lib_LcdSetFont(16, 16, 0);

		if (page==0) {
			Lib_LcdGotoxy(0,2*8);
			Lib_Lcdprintf("  1-BEEP 2-KB  ");
			Lib_LcdGotoxy(0,4*8);
			Lib_Lcdprintf("  3-LCD  4-RTC ");
			Lib_LcdGotoxy(0,6*8);
			Lib_Lcdprintf("  5-WLS  6-PRN ");
		} else {
			Lib_LcdGotoxy(0,2*8); 
			Lib_Lcdprintf("  1-BAT 2-PICC");
		}
		
		while (1) {
			DispWlsSignal(0, 2*8);
			DispBattery(128-11, 2*8);

			if (Lib_KbCheck()) continue;
			ucKey = Lib_KbGetCh();  

			if (ucKey>='0' && ucKey<='9') break;

			if (ucKey==KEYUP && page==1) {
				page=0;
				Lib_SetTimer(WLS_TM, 0);   
				break;
			}

			if (ucKey==KEYDOWN && page==0) {
				page=1;
				Lib_SetTimer(WLS_TM, 0);  
				break;
			}
			
            if (ucKey==KEYENTER) {
				page ^= 1;
				Lib_SetTimer(WLS_TM, 0);   
				break;
			}
			
            if (ucKey==KEYCANCEL) {
				Lib_KbFlush();
				return 0;
			}
		}

		if (!(ucKey>='0' && ucKey<='9')) continue;
		
        if (page==0) {
			switch(ucKey) {
			case '1':
				BeepFun_Test();
				break;
			case '2':
				Test_KbFun(1);
				break;
			case '3':
				LCD_Test(); 
				break;
			case '4':
				CLK_Test();
				break;
			case '5': 
				Test_Wls();
				break;
			case '6':
				PRN_Test();	 
				break; 
			}
		} else {
			switch(ucKey){
			case '1':
				Battery_Test();			
				break;
			case '2':
				Picc_Test();
				break; 
			}
		}
	}
	return 0;			
}




