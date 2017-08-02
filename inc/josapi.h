#ifndef  _JOS_API_H
#define  _JOS_API_H 

//Standard header files

#ifdef __cplusplus
extern "C" {
#endif

#ifndef BYTE
#define BYTE   unsigned char
#endif
#ifndef WORD
#define WORD   unsigned short
#endif
#ifndef DWORD
#define DWORD  unsigned long
#endif
#ifndef uchar
#define uchar  unsigned char
#endif
#ifndef uint
#define uint   unsigned int
#endif
#ifndef ulong
#define ulong  unsigned long
#endif
#ifndef ushort
#define ushort unsigned short
#endif 

#define  POSTYPE_PC1000 				"PC-1000"
#define  POSTYPE_PC1000_ID 			"PC-1000-ID" 

#define  APP_RUN_SPACE          0x20500000
#define  APPMEM_SHARE_SPACE     0x20400000

int    Lib_ReadLibVer(void);
//=================================================
//               System functions
//==================================================
int    Lib_AppInit(void);
void   Lib_DelayMs(int ms);  
void   Lib_Beep(void);
void   Lib_Beef(BYTE mode,int DlyTimeMs);

//for real time
#define TIME_FORMAT_ERR         (-900)
#define TIME_YEAR_ERR           (-901)
#define TIME_MONTH_ERR          (-902)
#define TIME_DAY_ERR            (-903)
#define TIME_HOUR_ERR           (-904)
#define TIME_MINUTE_ERR         (-905)
#define TIME_SECOND_ERR         (-906)
#define TIME_WEEK_ERR           (-907)
#define TIME_SET_ERR            (-908)
#define TIME_GET_ERR            (-909)
#define TIME_RAMADDR_OVER       (-910)
#define TIME_RAMLEN_OVER        (-911)


int   Lib_SetDateTime(BYTE *datetime);
int   Lib_GetDateTime(BYTE *datetime);

//for timer event
void  Lib_SetTimer(BYTE TimerNo, WORD Cnt100ms);
WORD  Lib_CheckTimer(BYTE TimerNo);
void  Lib_StopTimer(BYTE TimerNo);

//for all hardware and software version
void  Lib_ReadSN(BYTE *SerialNo);
int   Lib_EraseSN(void);
int   Lib_ReadVerInfo(BYTE *VerInfo); 
int		Lib_ReadTerminalType(uchar *posType);

void  Lib_Reboot(void);
void  Lib_GetRand(uchar *out_rand ,uchar  out_len);

#define BAT_CHARGE_DONE		0	//charge complete
#define BAT_CHARGE_ING		1	//charging
#define BAT_CHARGE_SLEEP	2	//is not charging, sleep status
#define BAT_CHARGE_FAULT	3	//fault occurs,including charge suspend,input over-voltage,timer fault and battery absent
int		Lib_GetBatChargeStatus(void);
int   Lib_GetBatteryVolt(void); 
 
int   Lib_ShareMemWrite(DWORD offset, BYTE *buff, int wlen);
int   Lib_ShareMemRead(DWORD offset,BYTE *buff,int rlen); 

int   Lib_ReadTermID(BYTE  *termID);
int   Lib_WriteTermID(BYTE  *termID);
//=================================================
//               KeyBoard functions
//==================================================
#define KEY1             0x31
#define KEY2             0x32
#define KEY3             0x33
#define KEY4             0x34
#define KEY5             0x35
#define KEY6             0x36
#define KEY7             0x37
#define KEY8             0x38
#define KEY9             0x39
#define KEY0             0x30

#define KEYCLEAR         0x08
#define KEYALPHA         0x07
#define KEYUP            0x05
#define KEYDOWN          0x06
#define KEYFN            0x15
#define KEYMENU          0x14
#define KEYENTER         0x0d
#define KEYCANCEL        0x1b
#define KEYBACKSPACE     0x1c
#define KEYPRINTERUP     0x1d
#define NOKEY            0xff
#define KEYDOT           0x2e
#define KEY00            0x3a
#define KEYOK            0x3b

#define KB_NoKey_Err        (-1000)
#define KB_InputPara_Err    (-1010)
#define KB_InputTimeOut_Err (-1011)
#define KB_UserCancel_Err   (-1012)
#define KB_UserEnter_Exit   (-1013)

BYTE  Lib_KbGetCh(void);
BYTE  Lib_KbUnGetCh(void);
int   Lib_KbCheck(void);
void  Lib_KbFlush(void);
int   Lib_KbSound(BYTE mode,WORD DlyTimeMs);
int   Lib_KbMute(BYTE mode);
int   Lib_KbGetStr(BYTE *str,BYTE minlen, BYTE maxlen,BYTE mode,WORD timeoutsec); 
int   Lib_KbGetHzStr(uchar *outstr, uchar max, ushort TimeOut);

//=================================================
//               LCD Display functions
//==================================================
#define  ASCII      0x00
#define  CFONT      0x01
#define  REVER      0x80 

#define  LCD_SetFont_Err   (-1500)
#define  LCD_GetFont_Err   (-1501)

void Lib_LcdCls(void);
void Lib_LcdClrLine(BYTE startline, BYTE endline);
void Lib_LcdSetBackLight(BYTE mode);
void Lib_LcdSetGray(BYTE level);
void Lib_LcdGotoxy(BYTE x, BYTE y);
int  Lib_LcdSetFont(BYTE AsciiFontHeight, BYTE ExtendFontHeight, BYTE Zoom);
int  Lib_LcdGetFont(BYTE *AsciiFontHeight, BYTE *ExtendFontHeight, BYTE *Zoom);
BYTE Lib_LcdSetAttr(BYTE attr);
int  Lib_Lcdprintf(char *fmt,...);
void Lib_SprintfFloat(char *strOut, float fNumber, int iDecFractionLen);
void Lib_LcdPrintFloat(float fNumber, int iDecFractionLen);
void Lib_LcdPrintxy(BYTE col,BYTE row,BYTE mode,char *str,...);
void Lib_LcdDrawPlot (BYTE XO,BYTE YO,BYTE Color);
void Lib_LcdDrawLogo(BYTE *pDataBuffer);
void Lib_LcdDrawBox(BYTE x1,BYTE y1,BYTE x2,BYTE y2);
int  Lib_LcdRestore(BYTE mode);

void Lib_LcdGetSize(BYTE * x, BYTE *y);
void Lib_LcdDrawLine (BYTE x1,BYTE y1,BYTE x2,BYTE y2, BYTE byColor);

BYTE Lib_LcdGetSelectItem(BYTE *pbyItemString, BYTE byItemNumber, BYTE byExitMode);
BYTE Lib_LcdGetSelectItemCE(BYTE *pbyChnItemString, BYTE *pbyEngItemString,
                     BYTE byItemNumber, BYTE byExitMode);
                     
void Lib_LcdPrintfCE(char *pCHN , char *pEN);
void Lib_LcdPrintxyCE(BYTE col, BYTE row, BYTE mode, char *pCHN , char *pEN);
void Lib_LcdPrintxyExtCE(BYTE col, BYTE row, BYTE mode, char * pCHN, char * pEN, int iPara);

void Lib_LcdSetDotBuf(BYTE *dotbuf);
void Lib_LcdGetDotBuf(BYTE *dotbuf);
void Lib_LcdClrDotBuf(void);
void Lib_LcdDispDotBuf(void);
//========================================
//     Encrypt and decrypt functions
//=========================================
#define ENCRYPT 1
#define DECRYPT 0
 

void  Lib_Des(uchar *input, uchar *output,uchar *deskey, int mode);
void  Lib_Rsa(uchar* Modul,uint ModulLen,uchar* Exp,uint ExpLen,uchar* DataIn,uchar* DataOut);
void  Lib_Hash(uchar* DataIn, uint DataInLen, uchar* DataOut);

//=============================================
//     Asynchronism communication functions
//
//=============================================  
#define VCOM1       1
#define VCOM2       2
#define VCOM3       3
#define VCOM4       4
#define VCOM5       5
#define VCOM_DEBUG  0


#define RS232A     VCOM1
#define RS232B     VCOM2
#define COM1       VCOM1
#define COM2       VCOM2

#define COM_DEBUG  VCOM1 

#define DEBUG_COM	1  //RS232a
#define AT_COM		5 



#define  COM_INVALID_PARA     (-6501)
#define  COM_NOT_OPEN         (-6502)
#define  COM_INVALID_PORTNO   (-6503)
#define  COM_RX_TIMEOUT       (-6504)
#define  COM_TX_TIMEOUT       (-6505)
#define  COM_TX_MEMOVER       (-6506)
#define  COM_NO_EMPTYPORT     (-6507)

int  Lib_ComReset(BYTE port);
int  Lib_ComWrite(BYTE port,BYTE *writebyte,int write_len);
int  Lib_ComRecvByte(BYTE port,BYTE *recv_byte,int waitms);
int  Lib_ComRecv(BYTE port,BYTE *recv_data,int max_len,int *recv_len,int waitms);
int  Lib_ComSendByte(BYTE port,BYTE send_byte);
int  Lib_ComSend(BYTE port,BYTE *send_data,int send_len);
int  Lib_ComClose(BYTE port);
int  Lib_ComOpen(BYTE port, char *ComPara);


//============================================================
//       USB Host and Device Operation Functions
//============================================================
#define  USBD     0
#define  USBH     1 

#define USB_SUCCESS          	(0)
#define USB_INVALID_PORT      (-6000)
#define USB_NOT_CONNECT       (-6001)
#define USB_RECV_TIMEOUT      (-6002)
#define USB_SEND_OVERFLOW     (-6003)
#define USB_RECV_OVERFLOW     (-6004)
#define USB_SEND_TIMEOUT      (-6005)
#define USBD_NOT_CONFIG       (-6100) 
 
int  Lib_UsbOpen(uchar port);
int  Lib_UsbClose(uchar port);
int  Lib_UsbReset(uchar port);
int  Lib_UsbSend(uchar port,uchar *send_data, int send_len);
int  Lib_UsbSendByte(uchar port,uchar send_byte);
int  Lib_UsbRecv(uchar port,uchar *recv_data,int max_len, int *recv_len,int  waitms);
int  Lib_UsbRecvByte(uchar port,uchar *recv_byte,int waitms);


//===========================================
//        Defined for file system
//
//============================================
#define   FILE_EXIST               (-8001)
#define   FILE_NOTEXIST            (-8002)
#define   FILE_MEM_OVERFLOW        (-8003)
#define   FILE_TOO_MANY_FILES      (-8004)
#define   FILE_INVALID_HANDLE      (-8005)
#define   FILE_INVALID_MODE        (-8006)
#define   FILE_NO_FILESYS          (-8007)
#define   FILE_NOT_OPENED          (-8008)
#define   FILE_OPENED              (-8009)
#define   FILE_END_OVERFLOW        (-8010)
#define   FILE_TOP_OVERFLOW        (-8011)
#define   FILE_NO_PERMISSION       (-8012)
#define   FILE_FS_CORRUPT          (-8013)
#define   FILE_INVALID_PARA        (-8014)
#define   FILE_WRITE_ERR           (-8015)
#define   FILE_READ_ERR            (-8016)
#define   FILE_NO_MATCH_RECORD     (-8017)

#define   O_RDWR                   0x01
#define   O_CREATE                 0x02

#define   FILE_SEEK_CUR            0
#define   FILE_SEEK_SET            1
#define   FILE_SEEK_END            2



typedef struct
{
     BYTE  fid;
     BYTE  attr;
     BYTE  type;
     char   name[17];
     DWORD  length;
} FILE_INFO;

//File operation functions
int  Lib_FileOpen(char *filename, BYTE mode);
int  Lib_FileRead(int fid, BYTE *dat, int len);
int  Lib_FileWrite(int fid, BYTE *dat, int len);
int  Lib_FileClose(int fid);
int  Lib_FileSeek(int fid, long offset, BYTE fromwhere);
long Lib_FileSize(char *filename);
long Lib_FileFreeSize(void);
int  Lib_FileTruncate(int fid,long len);
int  Lib_FileExist(char *filename);
int  Lib_FileInfo(FILE_INFO* finfo);
int  Lib_FileExOpen(char *filename, BYTE mode,BYTE* attr);
int  Lib_FileRemove(const char *filename);
int  Lib_FileGetEnv(char *name, BYTE *value);
int  Lib_FilePutEnv(char *name, BYTE *value);


//===================================================
//   Multiple Application Manage Functions
//===================================================

typedef struct {
     uchar Name[32]; 
     uchar Version[16]; 
     uchar Num;
     uchar Priority;
     uchar NeedConfirm; 
     uchar Provider[32];
     uchar Descript[64];
     uchar LoadTime[14];
     ulong MainEntry;
     uchar Reserve[91];
}APP_MSG; 

int  Lib_AppReadInfo(BYTE AppNo, APP_MSG* ai);
int  Lib_AppReadState(BYTE AppNo);
int  Lib_AppSetActive(BYTE AppNo, BYTE flag);
int  Lib_AppRun(BYTE AppNo);
 
//========================================================
//                   PED AND PCI API
//==========================================================
int  Lib_PciWriteMKey(BYTE key_no,BYTE key_len,BYTE *key_data,BYTE mode);
int  Lib_PciWritePinKey(BYTE key_no,BYTE key_len,BYTE *key_data, BYTE mode, BYTE mkey_no);
int  Lib_PciWriteMacKey(BYTE key_no,BYTE key_len,BYTE *key_data, BYTE mode, BYTE mkey_no);
int  Lib_PciWriteDesKey(BYTE key_no,BYTE key_len,BYTE *key_data, BYTE mode, BYTE mkey_no);
int  Lib_PciDerivePinKey(BYTE mkey_n,BYTE pinkey_n1,BYTE pinkey_n2,BYTE mode);
int  Lib_PciDeriveMacKey(BYTE mkey_n,BYTE mackey_n1,BYTE mackey_n2,BYTE mode);
int  Lib_PciDeriveDesKey(BYTE mkey_n,BYTE deskey_n1,BYTE deskey_n2,BYTE mode);

int  Lib_PciGetPin(BYTE pinkey_n,BYTE min_len,BYTE max_len,BYTE *card_no,BYTE mode,BYTE *pin_block,WORD waittime_sec);
int  Lib_PciGetMac(BYTE mackey_n,WORD inlen,BYTE *indata,BYTE *macout,BYTE mode);
int  Lib_PciDes(BYTE deskey_n,BYTE *indata,BYTE *outdata,BYTE mode);

int  Lib_PciGetRnd (BYTE *rnd8);
int  Lib_PciAccessAuth(BYTE *auth_data,BYTE mode);


#define PCI_Locked_Err        (-7000)
#define PCI_KeyType_Err       (-7001)
#define PCI_KeyLrc_Err        (-7002)
#define PCI_KeyNo_Err         (-7003)
#define PCI_KeyLen_Err        (-7004)
#define PCI_KeyMode_Err       (-7005)

#define PCI_InputLen_Err      (-7006)
#define PCI_InputCancel_Err   (-7007)
#define PCI_InputNotMatch_Err (-7008)
#define PCI_InputTimeOut_Err  (-7009)
#define PCI_CallTimeInte_Err  (-7010)

#define PCI_NoKey_Err         (-7011)
#define PCI_WriteKey_Err      (-7012)
#define PCI_ReadKey_Err       (-7013)
#define PCI_RsaKey_HashErr    (-7014)
#define PCI_DataLen_Err       (-7015)

#define PCI_NoInput_Err       (-7016)
#define PCI_AppNumOver_Err    (-7017)

#define PCI_ReadMMK_Err       (-7020)
#define PCI_WriteMMK_Err      (-7021)
  
#define DUKPT_OK                  0
#define DUKPT_NoKey               (-7050)
#define DUKPT_CounterOverFlow     (-7051)
#define DUKPT_NoEmptyList         (-7052)
#define DUKPT_InvalidAppNo        (-7053)
#define DUKPT_InvalidKeyID        (-7054)
#define DUKPT_InvalidFutureKeyID  (-7055)
#define DUKPT_InvalidCrc          (-7056)
#define DUKPT_InvalidBDK          (-7057)
#define DUKPT_InvalidKSN          (-7058)
#define DUKPT_InvalidMode         (-7059)
#define DUKPT_NotFound            (-7060)


int Lib_PciLoadKeyDukpt(uchar KeyId,uchar BdkLen,uchar KsnLen,uchar *BdkKsn);
int Lib_PciGetPinDukpt(uchar key_n,uchar min_len,uchar max_len,uchar *card_no,uchar mode,uchar *pin_block,ushort waittime_sec,uchar *out_ksn);
int Lib_PciGetMacDukpt(uchar key_n,ushort inlen,uchar *indata,uchar *macout,uchar mode,uchar *out_ksn);

//==================================================================
//    Printer API
//===================================================================
#define PRN_BUSY              (-4001)
#define PRN_NOPAPER           (-4002)
#define PRN_DATAERR           (-4003)
#define PRN_FAULT             (-4004)
#define PRN_TOOHEAT           (-4005)
#define PRN_UNFINISHED        (-4006)
#define PRN_NOFONTLIB         (-4007)
#define PRN_BUFFOVERFLOW      (-4008)
#define PRN_SETFONTERR        (-4009)
#define PRN_GETFONTERR        (-4010)
#define PRN_BATFORBID         (-4011)
#define PRN_OK                (0)

int  Lib_PrnInit(void);
int  Lib_PrnStr(char *str,...); 
int  Lib_PrnLogo(BYTE *logo);
int  Lib_PrnStart(void);
int  Lib_PrnCheck(void);
int  Lib_PrnStep(int pixel);
int  Lib_PrnSetLeftIndent(int x);
int  Lib_PrnSetSpace(BYTE x, BYTE y);
int  Lib_PrnGetFont(BYTE *AsciiFontHeight, BYTE *ExtendFontHeight, BYTE *Zoom);
int  Lib_PrnSetFont(BYTE AsciiFontHeight, BYTE ExtendFontHeight, BYTE Zoom); 
int  Lib_PrnSetGray(BYTE nLevel);
int  Lib_PrnSetSpeed(int iSpeed);
int  Lib_PrnPaper(uchar mode, WORD pixel);
int  Lib_PrnReadVersion(uchar * VersionBuf);

//////////////////////////////////////////////////////////////////////////
//add 2011/5/12
int Lib_PrnBarcode128(uchar *str,uchar mode,ushort height,uchar width);
///////////////////////////////////////////////////////////////////////////

//==================================================================
//    FONT API
//===================================================================
#define FONT_NOEXISTERR        (-1600)
#define FONT_NOSUPPORTERR      (-1601)

int  Lib_FontFileCheck(void);
int  Lib_FontGetCharSet(BYTE *bCharSet, BYTE *bNum);
int  Lib_FontGetHeight(BYTE bCharSet, BYTE *bHeight, BYTE * bHeightNum);
//int  Lib_FontGetStyle(VFONT_LIB_STRUCT *style_info);
int Lib_GetFontDotMatrix(BYTE *strIn, BYTE byFontHeight,
                     BYTE *pbyCharDotMatrix, BYTE *pbyOutCharLen);

//========================================================
//                   Wireless comm API
//==========================================================

#define WLS_PWREN_IO        1	
#define WLS_RST_IO          2	
#define WLS_SIM1EN_IO       3
#define WLS_SIM2EN_IO       4
#define WLS_RESERVE1_IO     5
#define WLS_RESERVE2_IO     6

#define WLS_IO_LOW      	0
#define WLS_IO_HIGH     	1


void  Lib_WlsIoCtl(unsigned char ioname,unsigned char iomode);


//=============================================
//     PICC  Reader Functions
//=============================================



typedef struct{
     uchar  Command[4];
     ushort Lc;
     uchar  DataIn[512];
     ushort Le;
}APDU_SEND;

typedef struct{
     ushort LenOut;
     uchar  DataOut[512];
     uchar  SWA;
     uchar  SWB;
}APDU_RESP;



#define PICC_OK                (0)
#define PICC_ChipIDErr         (-3500)
#define PICC_OpenErr           (-3501)
#define PICC_NotOpen           (-3502)
#define PICC_ParameterErr      (-3503)
#define PICC_TxTimerOut        (-3504)
#define PICC_RxTimerOut        (-3505)
#define PICC_RxDataOver        (-3506)
#define PICC_TypeAColl         (-3507)
#define PICC_FifoOver          (-3508)
#define PICC_CRCErr            (-3509)
#define PICC_SOFErr            (-3510)
#define PICC_ParityErr         (-3511)
#define PICC_KeyFormatErr      (-3512) 
#define PICC_RequestErr        (-3513)
#define PICC_AntiCollErr       (-3514)
#define PICC_UidCRCErr         (-3515)
#define PICC_SelectErr         (-3516)
#define PICC_RatsErr           (-3517)
#define PICC_AttribErr         (-3518)
#define PICC_HaltErr           (-3519)
#define PICC_OperateErr        (-3520)
#define PICC_WriteBlockErr     (-3521)
#define PICC_ReadBlockErr      (-3522)
#define PICC_AuthErr           (-3523)
#define PICC_ApduErr           (-3524)
#define PICC_HaveCard          (-3525) //§á
#define PICC_Collision         (-3526) //?
#define PICC_CardTyepErr       (-3527) //A RATS 
#define PICC_CardStateErr      (-3528) 
#define PICC_PCBErr            (-3529)  
#define PICC_UidLenErr         (-3530)
#define PICC_ATAErr            (-3531) 
#define PICC_ATBErr            (-3532)   
#define PICC_PROTOCOLErr       (-4000) 

int  Lib_PiccOpen(void);
int  Lib_PiccCheck(uchar mode,uchar *CardType,uchar *SerialNo);
int  Lib_PiccCommand(APDU_SEND *ApduSend, APDU_RESP *ApduResp); 
void Lib_PiccHalt(void);
int  Lib_PiccRemove(void);
int  Lib_PiccReset(void);
int  Lib_PiccClose(void); 
int  Lib_PiccM1Authority(uchar Type,uchar BlkNo,uchar *Pwd,uchar *SerialNo);
int  Lib_PiccM1ReadBlock(uchar BlkNo,uchar *BlkValue);
int  Lib_PiccM1WriteBlock(uchar BlkNo,uchar *BlkValue);
int  Lib_PiccM1Operate(uchar Type,uchar BlkNo,uchar *Value,uchar UpdateBlkNo);
int  Lib_PiccCommandSRT512(uchar needreceive,uchar *incmd,uchar cmdlen,uchar *rsp,int *rsp_len); 


#ifdef __cplusplus
}
#endif

#endif
