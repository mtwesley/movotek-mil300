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

#define FONT_SMALL   8, 16, 0
#define FONT_MEDIUM  12, 12, 0
#define FONT_LARGE   16, 16, 0

#define VIEW_WAITING     0
#define VIEW_MENU        1
#define VIEW_ORDER       2
#define VIEW_ORDER_LIST  3
#define VIEW_SETTINGS    4

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
