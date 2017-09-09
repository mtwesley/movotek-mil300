#include "public.h"

#define NULL 0

unsigned int bin_ts(int x) {
	return (x >> 4) * 10 + (x & 0x0f);
}

static unsigned short days[4][12] = {
    {   0,  31,  60,  91, 121, 152, 182, 213, 244, 274, 305, 335},
    { 366, 397, 425, 456, 486, 517, 547, 578, 609, 639, 670, 700},
    { 731, 762, 790, 821, 851, 882, 912, 943, 974,1004,1035,1065},
    {1096,1127,1155,1186,1216,1247,1277,1308,1339,1369,1400,1430},
};


unsigned int datetime_to_epoch(int *datetime) {
    unsigned int second = datetime[5];  // 0-59
    unsigned int minute = datetime[4];  // 0-59
    unsigned int hour   = datetime[3];  // 0-23
    unsigned int day    = datetime[2];  // 0-30
    unsigned int month  = datetime[1];  // 0-11
    unsigned int year   = datetime[0];  // 0-99
    return (((year/4*(365*4+1)+days[year%4][month]+day)*24+hour)*60+minute)*60+second;
}


void epoch_to_date_time(int *date_time, unsigned int epoch)
{
    date_time[5] = epoch%60; epoch /= 60;
    date_time[4] = epoch%60; epoch /= 60;
    date_time[3]   = epoch%24; epoch /= 24;

    unsigned int years = epoch/(365*4+1)*4; epoch %= 365*4+1;

    unsigned int year;
    for (year=3; year>0; year--) {
        if (epoch >= days[year][0])
            break;
    }

    unsigned int month;
    for (month=11; month>0; month--) {
        if (epoch >= days[year][month])
            break;
    }

    date_time[0] = years+year;
    date_time[1] = month+1;
    date_time[2] = epoch-days[year][month]+1;
}

void DispTimer2(void) {
	int year;
	unsigned char font,buff[8];
	Lib_GetDateTime(buff);
	year=1900+(buff[0] >>4)*10+(buff[0] & 0x0f);
	if(buff[0]<=0x49) year+=100;
	buff[1]=(buff[1] >>4)*10+(buff[1] & 0x0f);
	buff[2]=(buff[2] >>4)*10+(buff[2] & 0x0f);
	buff[3]=(buff[3] >>4)*10+(buff[3] & 0x0f);
	buff[4]=(buff[4] >>4)*10+(buff[4] & 0x0f);
	buff[5]=(buff[5] >>4)*10+(buff[5] & 0x0f);
	Lib_LcdPrintxy(0,1*8,0," %04d/%02d/%02d %02d:%02d:%02d ",year,buff[1],buff[2],buff[3],buff[4],buff[5]);
}

void vDispTitle(char *mesg) {
	uint  uiLen;
	uchar ucBuf[17];
	
	memset(ucBuf,' ',16);
	uiLen=strlen(mesg);
	if(uiLen>16) memcpy(ucBuf,mesg,16);
	else memcpy(ucBuf+(16-uiLen)/2,mesg,uiLen);
	ucBuf[16]=0;
	Lib_LcdGotoxy(0,0);	
	Lib_LcdSetFont(16, 16, 0);
	Lib_LcdSetAttr(1);
	Lib_Lcdprintf((char *)ucBuf);
	Lib_LcdSetAttr(0);
}	

void vDisp2(uchar line,char *mesg) {
	uint uiLen;
	uchar ucBuf[17];
	
	uiLen=strlen(mesg);
	if(uiLen>16) memcpy(ucBuf,mesg,16);
	else strcpy((char *)ucBuf,mesg);
	ucBuf[16]=0;
	Lib_LcdClrLine((uchar)(line*2)*8,(uchar)(line*2+1)*8);
	Lib_LcdGotoxy(0,(uchar)(line*2)*8);	
	Lib_LcdSetFont(16, 16, 0);
	Lib_Lcdprintf((char *)ucBuf);
}	

unsigned char bcd_to_byte(unsigned char ucBcd) {
	return (((ucBcd >> 4) & 0x0f) * 10 + (ucBcd & 0x0f));
}

void vDisp(uchar line,char *mesg) {
	uint uiLen;
	uchar ucBuf[30];
	
	uiLen=strlen(mesg);
	if(uiLen>21) memcpy(ucBuf,mesg,21);
	else strcpy((char *)ucBuf,mesg);
	ucBuf[21]=0;
	Lib_LcdClrLine(line,line);
	Lib_LcdGotoxy(0,line*8);
	Lib_LcdSetFont(8, 16, 0);
	Lib_Lcdprintf((char *)ucBuf);
}	
	
uchar vMessage(char *mesg) {
	vDisp2(3,mesg);
	Lib_KbFlush();
	if(Lib_KbGetCh()==KEYCANCEL) return 0xff;
	return 0;
}
 
void DispTitle(int mode) {
	Lib_LcdCls();
	Lib_LcdGotoxy(0,0*8);
	Lib_LcdSetAttr(1);
	Lib_LcdSetFont(16, 16,CFONT);
	if(mode)  Lib_Lcdprintf("    One Test    ");
	else      Lib_Lcdprintf("    All Test    ");
	Lib_LcdSetAttr(0);
}
 
void ShowErr(char *msg) {
	int i;

	Lib_LcdClrLine(6*8,8*8);
	Lib_LcdGotoxy(0,6*8);
	Lib_Lcdprintf(msg);
	for(i=0;i<10;i++) Lib_Beef(6,100);
	Lib_KbGetCh();
}

unsigned char WaitEsc(unsigned short seconds) {
	Lib_SetTimer(0, seconds*10);
	while(1){
		if(!Lib_CheckTimer(0)) break;
		if (!Lib_KbCheck()){
			if(Lib_KbGetCh()== KEYCANCEL) 
			{
				Lib_LcdSetBackLight(1);
				return 1;
			}
			return 0;
		}
	}
	return 0;
}

void ok_beep(void) {
	Lib_Beef(2, 100);
} 

void Api_LcdClrLine(uchar startline, uchar endline) {
	Lib_LcdClrLine(startline*8, (endline+1)*8-1);
}

/****************************************************************************
 ��������:      ��ָ��������ʾһ�к��ֻ��ߴ��ַ�Ӣ��
 �������:
			psMessage ��Ҫ��ʵ���ַ������С�Ӣ�ģ�
			ucLine:	��ʾ����
			ucLoc:	λ�� LEFT_DISP, MIDDLE_DISP, RIGHT_DISP
			ucBack:	0:������ʾ NORMAL_DISP��1��������ʾ REVERSE_DISP
 �������:
 �� �� ֵ:   none
****************************************************************************/
void Api_DispString(uchar *psMessage,uchar ucLine,uchar ucLoc,uchar ucBack) {
	int  l;
	uchar  buf[17];
	int i=0;

	Lib_KbFlush();
	i = strlen((char *)psMessage);
    
	Lib_LcdGotoxy(0,ucLine*8);
	Api_LcdClrLine(ucLine,(uchar)(ucLine+1));
	Lib_LcdSetAttr(ucBack);
	Lib_LcdGotoxy (0,ucLine*8);
	
	Lib_LcdSetFont(16, 16, 0);
	
	switch(ucLoc){

	case MIDDLE_DISP:
		memset(buf,' ',16);
		buf[16]=0;
		if(i > 16) i = 16;
		l = (16 - i)/2;
		memcpy(buf+l, psMessage, i);
		Lib_Lcdprintf((char *)buf);
		break;
	case RIGHT_DISP:
		memset(buf,' ',16);
		buf[16]=0;
		l = 16 - i;
		strcpy((char *)(buf+l),(char *)psMessage);
		Lib_Lcdprintf((char *)buf);
		break;
	case LEFT_DISP:
	default:
		Lib_Lcdprintf((char *)psMessage);
		break;
	}

	Lib_LcdSetFont(16, 16, 0);
	Lib_LcdSetAttr(0);
}

void ScrGotoXY(uchar a, uchar b) {
	Lib_LcdGotoxy((uchar) (a * 8), b*8);
}

void ScrPrintC(void *s, uchar len) {
	/*~~~~~~~~~~~~~~~~~~~~~~~~*/
	uchar	buffer[129];
	/*~~~~~~~~~~~~~~~~~~~~~~~~*/

	if(len > 128) len = 128;  
	if(len & 0x80)
	{
		len -= 0x80;
		Lib_LcdSetAttr(1);
	}
	else
	{
		Lib_LcdSetAttr(0);
	}

	Lib_LcdSetFont(16, 16, 0);
	memset(buffer, 0, sizeof(buffer));
	memcpy(buffer, s, len);
	Lib_Lcdprintf((char *) buffer);
}

void apiLcdGotoRel16(uchar px, uchar py, int offset) {
	/*~~~~~~~~~~~~~~~~*/
	int		absoffset;
	uchar	ch = py % 2;
	/*~~~~~~~~~~~~~~~~*/

	absoffset = px + (py / 2) * 16 + offset;
	while(absoffset < 0) absoffset += 64;
	py = (absoffset / 16) * 2;
	px = absoffset % 16;
	ScrGotoXY(px, (uchar) (py + ch));
}

unsigned short apiStrLen(uchar *in, unsigned short len) {
	/*~~~~~~~~~~~~~~~*/
	unsigned short	ii;
	/*~~~~~~~~~~~~~~~*/
	
	if (in==NULL)
	{
	    return 0;
	}

	for(ii = 0; ii < len; ii++)
	{
		if(in[ii] == '\0') break;
	}

	return ii;
}

void apiConvAmount(uchar *ou, uchar *ascamt, uchar len) {
	/*~~~~~~~~~~~~~~~~~~~~~~~*/
	unsigned int	ii, jj, tt;
	uchar	ch;
	uchar	aa;
	uchar	buf[3];
	/*~~~~~~~~~~~~~~~~~~~~~~~*/

	jj = 0;
	tt = 0;

	ou[jj++] = ' ';
	for(ii = 0; ii < len; ii++)
	{
		ch = ascamt[ii];
		if((tt == 0) && (ch == 'C'))
		{
			tt = 1;
		}
		else if((tt == 0) && (ch == 'D'))
		{
			tt = 1;
			ou[jj++] = '-';
		}
		else if(!isdigit(ch))
		{
			break;
		}
	}

	len = ii;
	aa = 0;
	for(ii = tt; (ii + 3) < len; ii++)
	{
		ch = ascamt[ii];
		if((ch == '0') && (aa == 0))
		{
			continue;
		}

		if((isdigit(ch)) /* && (ch !='0') */ )
		{
			aa = 1;
			ou[jj++] = ch;
		}
	}

	tt = ii;
	len = len - ii;
	buf[0] = '0', buf[1] = '0', buf[2] = '0';
	for(ii = 0; ii < len; ii++)
	{
		buf[3 - len + ii] = ascamt[tt++];
	}

	ou[jj++] = buf[0];
	ou[jj++] = '.';
	ou[jj++] = buf[1];
	ou[jj++] = buf[2];
	ou[jj++] = '\0';
}

void apiConvAmountForKey(uchar *ou, uchar *ascamt, uchar len) {
	uchar len1,len2;
	
	len2= 3;

	apiConvAmount(ou,ascamt,len);
	len1 = strlen((char*)ou);
	if (len2>0)
	{
		memmove(ou+len2,ou+1,len1);
		memcpy(ou,"   ", 3);		
	}
}

uchar apiFindNexKey(uchar keyvalue, uchar upal) {
	/*~~~~~~~~~~~~~~~~~~~~*/
	const struct
	{
		uchar	*keystr;
	}

	keymap[] =
	{
		(uchar *) "1QZ qz",
		(uchar *) "2ABCabc",
		(uchar *) "3DEFdef",
		(uchar *) "4GHIghi",
		(uchar *) "5JKLjkl",
		(uchar *) "6MNOmno",
		(uchar *) "7PRSprs",
		(uchar *) "8TUVtuv",
		(uchar *) "9WXYwxy",
		(uchar *) "0,*#!\"$&\'()+-./:;<=>?@[\\]^_`{|}~"
	};
	uchar	ii;
	uchar	*keyptr;
	/*~~~~~~~~~~~~~~~~~~~~*/

	for(ii = 0; ii < 10; ii++)
	{
		keyptr = keymap[ii].keystr;
		while(*keyptr != 0)
		{
			if(keyvalue == *keyptr++)
			{
				keyvalue = *keyptr;
				if(keyvalue == 0)
				{
					keyvalue = *(keymap[ii].keystr);
				}

				ii = 20;
				break;
			}
		}
	}

	return keyvalue;
}

uchar Api_WaitKey(uchar ucTimeOut) {
	Lib_KbFlush();
	if(ucTimeOut == 0) return Lib_KbGetCh();
	Lib_SetTimer(TIMER_FOR_APILIB, (unsigned short)(ucTimeOut*10));
	while(1)
	{
		if(!Lib_KbCheck()) 
	    {
		    return(Lib_KbGetCh());
		}
		if(!Lib_CheckTimer(TIMER_FOR_APILIB)) break;
	}
	return KEYCANCEL;
}

uchar Api_GetString_m1000
(
	uchar	*psOut,
	uchar	mode,
	uchar	ucMinlen,
	uchar	ucMaxlen,
	uchar	ucTimeOut,
	uchar   ucBeginLine   //7,6,5,4,3,2,1
)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	uchar	mode1, ch, sp, dstar;
	uchar	displen, enlen;
	uchar	ii, delay;
	uchar	firy, firx, line_max;
	uchar	dp[32];
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	sp = ' ';
	dstar = '*';

	delay = ucTimeOut;
	if(ucMinlen > ucMaxlen)
		ucMinlen = ucMaxlen;

	//firy = 6;
	if(ucBeginLine >7 || ucBeginLine < 1) firy = 6;
	else firy = ucBeginLine-1;
	firx = 0;
	line_max = 16;

	mode1 = mode & (0xff - ECHO_IN - CARRY_IN);
	if((ucMinlen == ucMaxlen) && (mode1 != AMOUNT_IN) && (mode1 != PASS_IN))
	{
		if(ucMinlen <= line_max - 1)
		{
			ScrGotoXY((uchar) (line_max - ucMinlen - 1), firy);
			ScrPrintC("\x10", 1);
		}
	}

	if(mode1 == AMOUNT_IN)
	{
		if(ucMaxlen + 4 >= sizeof(dp)) ucMaxlen = sizeof(dp) - 4;
	}

	if(((mode & ECHO_IN) || (mode & CARRY_IN)) && (mode1 != PASS_IN))
	{
		displen = (uchar) apiStrLen(psOut, ucMaxlen);
		if(mode1 != AMOUNT_IN)
		{
			apiLcdGotoRel16(line_max, firy, 0 - displen);
			ScrPrintC(psOut, ucMaxlen);
		}
		else
		{
			apiConvAmountForKey(dp, psOut, ucMaxlen);
			Api_LcdClrLine(firy, (uchar) (firy + 1));
			apiLcdGotoRel16(line_max, firy, 0 - strlen((char *) dp));
			ScrPrintC(dp, (uchar) strlen((char *) dp));
		}
	}
	else
	{
		displen = 0;
	}


	if(mode1 == AMOUNT_IN)
	{
		displen = 0;
	}
	enlen = 0;
	
	if(mode & CARRY_IN)
	{
		enlen = displen;
	}

	while(1)
	{
		if(delay == 0)
		{
			ch = Lib_KbGetCh();
		}
		else
		{
			ch = Api_WaitKey(delay);
		}

		switch(ch)
		{
		case KEYENTER:
			if((enlen == 0) && (displen != 0))
			{
				return 0;
			}
			else if(enlen >= ucMinlen)
			{
				psOut[enlen] = '\0';
				return 0;
			}
			else
			{
				Lib_Beef(1, 100);
				continue;
			}
			break;

		case KEYCANCEL:
			enlen = 0;
			psOut[enlen] = '\0';
			return 2;
			break;

		case KEYBACKSPACE:  //�˸��ֵ���岻ͬ
		case KEYCLEAR:
			if(enlen > 0)
			{
				enlen--;
			}
			break;

		case KEYALPHA:
		case KEYDOWN:
		case KEYUP:
			if(mode1 == ALPHA_IN)
			{
				psOut[enlen - 1] = apiFindNexKey(psOut[enlen - 1], 1);
			}
			else
			{
				Lib_Beef(1, 100);
				continue;
			}
			break;

		default:
			if((ch == '0') && (enlen == 0) && (mode1 == AMOUNT_IN))
			{
				Lib_Beef(1, 100);
				continue;
			}

			if((isdigit(ch)) && (enlen < ucMaxlen))
			{
				psOut[enlen] = ch;
				enlen++;
			}
			else
			{
				Lib_Beef(6, 200);
				Lib_DelayMs(200);
				continue;
			}
		}

		if(enlen < ucMaxlen)
		{
			psOut[enlen] = '\0';
		}

		switch(mode1)
		{
		case NUM_IN:
		case ALPHA_IN:
			apiLcdGotoRel16(line_max, firy, 0 - displen);
			for(ii = 0; ii < displen; ii++)
			{
				ScrPrintC(&sp, 1);
			}
			
			apiLcdGotoRel16(line_max, firy, 0 - enlen);
			ScrPrintC(psOut, enlen);
			break;

		case PASS_IN:
			if(displen > enlen)
			{
				apiLcdGotoRel16(line_max, firy, 0 - displen);
				for(ii = enlen; ii < displen; ii++)
				{
					ScrPrintC(&sp, 1);
				}
			}
			else
			{
				apiLcdGotoRel16(line_max, firy, 0 - enlen);
				for(ii = displen; ii < enlen; ii++)
				{
					ScrPrintC(&dstar, 1);
				}
			}
			break;

		case AMOUNT_IN:
			apiConvAmountForKey(dp, psOut, enlen);
			Api_LcdClrLine(firy, (uchar) (firy + 1));
			displen = strlen((char *) dp);
			apiLcdGotoRel16(line_max, firy, 0 - displen);
			ScrPrintC(dp, displen);
			break;

		default:
			return 0xff;
		}

		displen = enlen;
	}
} 

uchar GetStr_m1000(uchar *psOut,uchar mode,uchar ucMinlen,uchar ucMaxlen) {
	return Api_GetString_m1000(psOut,mode,ucMinlen,ucMaxlen, 60, 7);
} 
