#include "josapi.h"
#include "wlsapi.h"

#ifndef _PUB_H
#define _PUB_H

#define MAC_ERR   (-7500)

#define LEFT_DISP       0
#define MIDDLE_DISP     1
#define RIGHT_DISP      2
#define NORMAL_DISP     0
#define REVERSE_DISP    1

#define ALLOW_ZERO_IN   0x80
#define BIG_IN          0x40
#define NUM_IN          0x20
#define ALPHA_IN        0x10
#define PASS_IN         0x08
#define CARRY_IN        0x04
#define AMOUNT_IN       0x02
#define ECHO_IN         0x01

#define TIMER_FOR_APILIB    5 

#define LINE1   0
#define LINE2   1
#define LINE3   2
#define LINE4   3
#define LINE5   4
#define LINE6   5
#define LINE7   6
#define LINE8   7
#define LINE9   9

#define LINE_HEIGHT  8
#define LINE_WIDTH   128

#define NO_NEED_RESET	0
#define CHANNEL_SIM1	0
#define CHANNEL_SIM2	1

#define TRUE  1
#define FALSE 0

#define LCD_FONT_SMALL   8, 16, 0
#define LCD_FONT_MEDIUM  12, 12, 0
#define LCD_FONT_LARGE   16, 16, 0

#define PRN_FONT_MEDIUM  16, 16, 0
#define PRN_FONT_LARGE   24, 24, 0

#define TIMER_1SEC   1 * 10
#define TIMER_2SEC   2 * 10
#define TIMER_5SEC   5 * 10
#define TIMER_10SEC  10 * 10
#define TIMER_30SEC  30 * 10

#define TIMER_TOPBAR   1
#define TIMER_WAITING  2

#define BATTERY_LEVEL5  8200
#define BATTERY_LEVEL4  7500
#define BATTERY_LEVEL3  7000
#define BATTERY_LEVEL2  6500
#define BATTERY_LEVEL1  6000


void DispTimer2(void);
void vDispTitle(char *mesg);
void vDisp2(uchar line,char *mesg);
void vDisp(uchar line,char *mesg);

uchar vMessage(char *mesg);
uint Ramdom(uint MaxNum);
uchar PressKey(void);

void ShowErr(char *msg);
unsigned char WaitEsc(unsigned short seconds);
void ok_beep(void); 
 
void DispTitle(int mode);
 
void s_UartOpen(void);
void s_UartClose(void);
int ss_UartPrint(char *fmt,...);

int Test_KbFun(int mode);
int SYS_Test(void); 
int CLK_Test(void);
int LCD_Test(void);
int MEM_Test(void); 
int COM_Test(void);
int PRN_Test(void);  

#endif
