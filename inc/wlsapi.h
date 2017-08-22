#ifndef _WLSAPI_H
#define _WLSAPI_H


#ifdef __cplusplus
extern "C" {
#endif

#ifndef uchar
#define uchar unsigned char
#endif
#ifndef uint
#define uint unsigned int
#endif
#ifndef ulong
#define ulong unsigned long
#endif
#ifndef ushort
#define ushort unsigned short
#endif


#define TM_WLS                  3               //���߶�ʱ��������COM ���ճ�ʱ

#define SIM_NO_MAX              2
#define SIM_NO_1                0
#define SIM_NO_2                1

//define recv data type
#define NORMAL_DATA             0x00 
#define AT_CMD                  0x01 

//Module status related macros
#define WLS_UNKNOWN             0x00 
#define WLS_GPRS_SIM340W        0x01  
#define WLS_GPRS_G610        	0x02

#define TCP_PROTOCOL            0
#define UDP_PROTOCOL            1

#define MAXIDLEN                64
#define MAXPWDLEN               16

#define PORT_CLOSED             0x00
#define PORT_OPENED             0x01 

#define PSEND_OK                0x00    //serial port send successfully
#define PSEND_ERR               0x01    //serial port send error
#define PRECV_UNOPEN            (-2)
#define PRECV_ERR               (-1)    //serial port receive error
#define PRECV_TIMEOUT           (-6504) //serial port receive timeout

//Signal macros
#define SIGNAL_VERY_STRONG      0x00
#define SIGNAL_STRONG           0x01
#define SIGNAL_NORMAL           0x02
#define SIGNAL_WEAK             0x03
#define SIGNAL_VERY_WEAK        0x04
#define NO_SIGNAL               0x05


//**********************************************************************
//General macros
#define WLS_OK                  (0)         //return successfully
#define WLS_ERR                 (-5501)     //error occurred
#define WLS_NORSP               (-5502)     //no response from the module
#define WLS_RSPERR              (-5503)     //"ERROR" is return from the module
#define WLS_NOSIM               (-5504)     //SIM/UIM card is not inserted or not detected
#define WLS_NEEDPIN             (-5505)     //SIM PIN is required
#define WLS_NEEDPUK             (-5506)     //SIM PUK is required
#define WLS_SIMBLOCKED          (-5507)     //SIM Card is permanently blocked
#define WLS_SIMERR              (-5508)     //SIM card does not exist or needs SIM PIN
#define WLS_PINERR              (-5509)     //SIM PIN is incorrect
#define WLS_NOTALLOWED          (-5510)     //operation not allowed
#define WLS_PARAMERR            (-5511)     //parameter error
#define WLS_USERORPWDERR        (-5512)     //user name or password error
#define WLS_SNLTOOWEAK          (-5513)     //the signal is too weak, please check the antenna
#define WLS_LINKCLOSED          (-5514)     //the module is offline
#define WLS_LINKOPENED          (-5515)     //the module is online
#define WLS_LINKOPENING         (-5516)     //the module is connecting to the network
#define WLS_TCPCLOSED           (-5517)     //tcp socket is closed
#define WLS_TCPOPENED           (-5518)     //tcp socket is opened
#define WLS_TCPOPENING          (-5519)     //the module is trying to open a TCP socket
#define WLS_ATTACHED            (-5520)     //Attached
#define WLS_DETTACHED           (-5521)     //Dettached
#define WLS_ATTACHING           (-5522)     //the module is looking for the base station.
#define WLS_NOBOARD             (-5523)     //no GPRS or CDMA board exist
#define WLS_UNKNOWNTYPE         (-5524)     //unknown type
#define WLS_DIALERR             (-5525)     //�û���������ע�ᱻ�����ܾ�������APN���󣬿�Ƿ�ѵ�
#define WLS_TCPSENDERR          (-5526)     //TCP socket send  err
#define WLS_TCPCONNECTOVERTIME  (-5527)     //TCP connect overtime 
#define WLS_DIALING             (-5528)     //dialing 
#define WLS_PORTERR             (-5529)     //serial port error
#define WLS_PORTINUSE           (-5530)     //serial port is in use by another program 
#define WLS_DETECTERR           (-5531)     //ʶ������ģ���������VPOS������ģ��ͨ�Ŵ���
#define WLS_SOCKETOVERRUN       (-5532)     //��TCP socket���ӹ���,�ѳ���3��,��� 
#define WLS_OTHERR              (-5533)     //��������
#define WLS_TCPCONNECTREFUSE    (-5534)   
#define WLS_NOTSUPPORT			(-5535)
#define WLS_TIMEOUT    			(-5536)
#define WLS_NoUNReadMessage		(-5537)
#define WLS_TCPSENDTIMEOUT      (-5538)     //send timeout
#define WLS_SIMLOCK				(-5539)     //SIM card locked
/***********************************************************************/

//------API-----------------------------------------------------------------
int Wls_Init(void);
int Wls_SelectSim(int simno);
int Wls_CheckSignal(int * pSignalLevel);
int Wls_CheckSim(void);
int Wls_InputSimPin(uchar * pin);
int Wls_InputUidPwd(uchar * uid, uchar *pwd);
int Wls_Dial(uchar *dialNum);
int Wls_CheckDial(void);
int Wls_TcpConnect(uchar *destIP, uchar *destPort);
int Wls_TcpPreConnect(uchar *destIP, uchar *destPort);
int Wls_TcpSend(uchar *sendData, ushort sendLen);
int Wls_TcpRecv(uchar *recvData, ushort *pRecvLen, ushort timeOutMs);
int Wls_TcpClose(void);
int Wls_NetClose(void);
int Wls_CheckNetLink (void);
int Wls_CheckTcpLink(void);
int Wls_Reset(void);
int Wls_SendCmd(uchar *cmd, uchar *rsp, ushort rspLen, ushort timeOutMs);  
int Wls_MTcpConnect(int socketNo, uchar *destIP, uchar *destPort);
int Wls_MTcpPreConnect(int socketNo, uchar *destIP, uchar *destPort);
int Wls_CheckMTcpLink(int socketNo);
int Wls_MTcpSend(int socketNo, uchar *sendData, ushort sendLen);
int Wls_MTcpRecv(int socketNo, uchar *recvData, ushort *pRecvLen, ushort timeOutMs);
int Wls_MTcpClose(int socketNo); 
int Wls_TcpConnectExt(uchar *destIP, uchar *destPort, uchar typeProtocol);
int Wls_TcpPreConnectExt(uchar *destIP, uchar *destPort, uchar typeProtocol);
int Wls_MTcpConnectExt(int socketNo, uchar *destIP, uchar *destPort, uchar typeProtocol);
int Wls_MTcpPreConnectExt(int socketNo, uchar *destIP, uchar *destPort, uchar typeProtocol); 
int Wls_GetVerInfo(uchar *wlsType, uchar *moduleHwVer, uchar *moduleSwVer, uchar *boardVer, uchar *driverVer); 
int Wls_SwitchMode(int mode);

int Wls_ExecuteCmd(uchar *cmd, ushort cmdLen, uchar *rspBuf, ushort maxRspLen, ushort* outRsplen, ushort ms);
int Wls_SendCmdRequest(uchar *cmd, ushort cmdLen);
int Wls_RecvCmdResponse(uchar *rspBuf, ushort maxRspLen, ushort* outRsplen, ushort ms);

int Wls_Deletemessage(unsigned char Type);
int Wls_Sendmessage(unsigned char Mode,unsigned char *PhoneNum,unsigned char *String,unsigned int Length); 
int Wls_Readmessage(unsigned char *PhoneNum,unsigned char *SendTime,unsigned char *String,unsigned int *Length); 
int Wls_GetLocation(uchar mode, uchar *locationBuf, int timeoutMs);

int Wls_GetTime(uchar *BcdTime);
int Wls_SetTime(uchar *BcdTime);

int Wls_TcpSendWithTimeOut(uchar *sendData, ushort sendLen, ushort timeout);
int Wls_MTcpSendWithTimeOut(int socketNo, uchar *sendData, ushort sendLen, ushort timeout);

int Wls_GetIMEI(uchar *sn);
int Wls_GetBand(uchar *band);
int Wls_SetBand(uchar *band);

int Wls_SetOperatorName(uchar *name);
int Wls_GetModuleType(uchar *moduleType);


#ifdef __cplusplus
}
#endif


#endif
