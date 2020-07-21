#ifndef _LOGIC_H_
#define	_LOGIC_H_

#include "DataDefine.h"
#include "bsplib.h"
#include "interlayer_config.h"
#include "FSM.h"
#include "teach.h"
#include "string.h"
#include "axislib.h"
#include "USERTOOL.H"

#include "math.h"
#include "stdlib.h"

#include "iodefine.h"

typedef struct
{
    u8 execute;
    u8 step;
    u8 done;
    u8 count;
    u8 index;
    u32 tm;
    u8 stepBuffer;
    u8 Ready;
} LogicParaDef;

#define TimerRst(a)		 a.tm = SysTim_GetUser()		  //0.1 ms
#define TimerCnt(a)		((SysTim_GetUser()-a.tm)/10) 	//��ʱת��ms
#define TimerRst1(a)		 a->tm = SysTim_GetUser()		  //0.1 ms
#define TimerCnt1(a)		((SysTim_GetUser()-a->tm)/10) 	//��ʱת��ms


//test
#define SPD_MAX (200*1000)

#define	ON	0
#define	OFF	1

#define PARAINIT(a)  memset(&a,0,sizeof(a));
#define INITTASK(a) if(a.execute==1&&a.step==0){a.step = 1;}

#define CHECK_NUM 1
#define	VACUUM_NUM 1
#define	DOWN_NUM 1



#define	NORMALMODE	0		//����ģʽ
#define	STEPMODE	1		//��������
#define	CYCLEMODE	2		//���ε���
#define	AGINGMODE	3		//�ϻ�ģʽ

#define AXISDELAY	5		//ͣ����ʱ
#define ENDCASE     0xff	//���һ��case

#define Null 0xFF


#define AxisDelay 30


//ʱ����
#define YEAR 0
#define MONTH 1
#define DAY 2
#define HOUR 3
#define MINUTE 4
#define SECOND 5

//���ӹ�λ״̬
#define Free 0 //����״̬ ����
#define Done 1 //���״̬ �������
#define Feed 2 //����״̬ ��������
#define Work 3 //����״̬ ���ڹ���
//�����
#define singleWire 0

//
#define WireLengthClam 4000
#define WireHelpLength 20000

//#define	RED					UserOutputSet(Q_Red,ON),UserOutputSet(Q_Green,OFF),UserOutputSet(Q_Yellow,OFF),UserOutputSet(Q_Buzzer,ON);
//#define	GREEN				UserOutputSet(Q_Red,OFF),UserOutputSet(Q_Green,ON),UserOutputSet(Q_Yellow,OFF),UserOutputSet(Q_Buzzer,OFF);
//#define	YELLOW				UserOutputSet(Q_Red,OFF),UserOutputSet(Q_Green,OFF),UserOutputSet(Q_Yellow,ON),UserOutputSet(Q_Buzzer,OFF);
#define	RED					UserOutputSet(Q_Buzzer,ON);
#define	GREEN				UserOutputSet(Q_Buzzer,OFF);
#define	YELLOW				UserOutputSet(Q_Buzzer,OFF);

enum {
    GOHOMETYPE1,	     //ֱ�ӻ�ԭ��(ƽ��ģʽ)
    GOHOMETYPE2,		//�ȵ�����λ�ٻ�ԭ��
    GOHOMETYPE3			//�Ȼ�����λ�ٻ�ԭ��
};

typedef enum
{
    AX_ERR_AX_ALM = (1 << 0),		//�ᱨ��
    AX_ERR_AX_TMOUT = (1 << 1),		//��ʱ����ԭ�㣩
    AX_ERR_UP_LIM_H = (1 << 8),		//Ӳ����λ
    AX_ERR_UP_LIM_S = (1 << 9),		//������λ
    AX_ERR_DW_LIM_H = (1 << 12),		//Ӳ����λ
    AX_ERR_DW_LIM_S = (1 << 13),		//������λ
} AxErrBitTp;

typedef enum
{
    HM_GO_DW = (1 << 0),			//������
    HM_GO_UP = (1 << 1),			//������
    HM_GO_HM = (1 << 2),			//��ԭ��
} AxHomBitTp;



//GUW.button.BitControlWord   //1506
typedef enum
{
    StartBit,							//bit0  ��ʼ��ť
    StopBit,							//bit1  ֹͣ��ť
    ResetBit,							//bit2  ��λ��ť
    HoldBit,							//bit3  ��ͣ��ť
    ClearPruductNumBit,  	//bit4  ������0��ť
    ProductNumSub1Bit,  	//bit5	������1��ť
    AxisPowerOn,  				//bit6	�ŷ�ʹ��
} HMIButton;

//A�˹�������4714
#define	AHalfStripEn 				WordToBit(GUS.WirePara.Data.A.Para.FunctionEn,0)		//bit0  �������
#define	ATwistWireEn				WordToBit(GUS.WirePara.Data.A.Para.FunctionEn,1)		//bit1  Ť������
#define	ATinEn						WordToBit(GUS.WirePara.Data.A.Para.FunctionEn,2)		//bit2  մ������
#define	AMakeTerminalEn				WordToBit(GUS.WirePara.Data.A.Para.FunctionEn,3)		//bit3  ���������
#define	AWireCheckEn  				WordToBit(GUS.WirePara.Data.A.Para.FunctionEn,4)		//bit4  ���߼������
#define	AMakeTerminalCheck  		WordToBit(GUS.WirePara.Data.A.Para.FunctionEn,5)		//bit5	����ѹ�ż������
#define	APressCheckEn  				WordToBit(GUS.WirePara.Data.A.Para.FunctionEn,6)		//bit6	���ӻ�ѹ�����
#define ANoTerminalCheckEn			WordToBit(GUS.WirePara.Data.A.Para.FunctionEn,7)		//bit7  ȱ���Ӽ��	
#define ASlowTinEn					WordToBit(GUS.WirePara.Data.A.Para.FunctionEn,8)		//bit8  ����մ������	
#define	ARosinMuchEn				WordToBit(GUS.WirePara.Data.A.Para.FunctionEn,9)		//bit9  մ����������
#define	AVisionCheckEn				WordToBit(GUS.WirePara.Data.A.Para.FunctionEn,10)		//bit10 �Ӿ���⿪��
#define	AInsertPinEn				WordToBit(GUS.WirePara.Data.A.Para.FunctionEn,11)		//bit11 �彺�ǿ���
#define	ATWoPinEn				    WordToBit(GUS.WirePara.Data.A.Para.FunctionEn,12)		//bit12 ���߿���


//B�˹������� 4774
#define	BHalfStripEn 				WordToBit(GUS.WirePara.Data.B.Para.FunctionEn,0)		//bit0  �������
#define	BTwistWireEn				WordToBit(GUS.WirePara.Data.B.Para.FunctionEn,1)		//bit1  Ť������
#define	BTinEn						WordToBit(GUS.WirePara.Data.B.Para.FunctionEn,2)		//bit2  մ������
#define	BMakeTerminalEn				WordToBit(GUS.WirePara.Data.B.Para.FunctionEn,3)		//bit3  ���������
#define	BWireCheckEn  				WordToBit(GUS.WirePara.Data.B.Para.FunctionEn,4)		//bit4  ���߼������
#define	BMakeTerminalCheck  		WordToBit(GUS.WirePara.Data.B.Para.FunctionEn,5)		//bit5	����ѹ�ż������
#define	BPressCheckEn  				WordToBit(GUS.WirePara.Data.B.Para.FunctionEn,6)		//bit6	���ӻ�ѹ�����
#define BNoTerminalCheckEn			WordToBit(GUS.WirePara.Data.B.Para.FunctionEn,7)		//bit7  ȱ���Ӽ��	
#define BSlowTinEn					WordToBit(GUS.WirePara.Data.B.Para.FunctionEn,8)		//bit8  ����մ������	
#define	BTinWarmEn    				WordToBit(GUS.WirePara.Data.B.Para.FunctionEn,9)		//bit9  ��¯���ȿ���	
#define	BRosinMuchEn    			WordToBit(GUS.WirePara.Data.B.Para.FunctionEn,10)		//bit10  մ����������	
#define	BFeedTinEn    			    WordToBit(GUS.WirePara.Data.B.Para.FunctionEn,11)		//bit11  ��������


//���ع��� 4062
#define NoWireCheckEn				WordToBit(GUS.FunctionEn,0)								//bit0   ȱ�߼��	
#define WireKnotCheckEn				WordToBit(GUS.FunctionEn,1)								//bit1   �����	
#define PressCheckEn				WordToBit(GUS.FunctionEn,2)								//bit2   ��ѹ���	
#define DoorCheckEn					WordToBit(GUS.FunctionEn,3)								//bit3   �Ž����	
#define SeparateWireMode			WordToBit(GUS.FunctionEn,4)								//bit4  ����ģʽ	
//4063
#define TerminalEn					WordToBit(GUS.FunctionEn,16)							//bit16  ���ӻ����ÿ���
#define BHelpClamEN					WordToBit(GUS.FunctionEn,17)							//bit17  B�˸��п���
#define FrontSeparateWireEN			WordToBit(GUS.FunctionEn,18)					//bit18  ���߿���	
#define UnFeedHidEN					WordToBit(GUS.FunctionEn,19)							//bit19  ���ϱ��ÿ���	
#define ShortMode                   WordToBit(GUS.FunctionEn,20)              //bit20 ����ģʽ
#define ShortModeForUser            WordToBit(GUS.FunctionEn,21)              //bit21 �û����õĶ���ģʽ 
#define TwistMode                   WordToBit(GUS.FunctionEn,22)              //bit22 Ť��ģʽ
#define DividEN                     WordToBit(GUS.FunctionEn,23)              //bit23 ���׷��߿���
#define DisWire                     WordToBit(GUS.FunctionEn,24)              //bit24 
//#define FeedWireREn                 WordToBit(GUS.FunctionEn,25)              //bit25 ���߽Ƕ����ÿ���9
#define StrepEn       			    WordToBit(GUS.FunctionEn,26)			  //bit26  ���߿���	10
#define NoTinEn       			    WordToBit(GUS.FunctionEn,27)			  //bit27  qȱ������	12
#define BuzzerEn       			    WordToBit(GUS.FunctionEn,28)			  //bit27  ��������	13
#define PinCheackEn      			WordToBit(GUS.FunctionEn,29)			  //bit27  



#define sheathedWireEn              WordToBit(GUS.SheathedWire.Data.SheathedWireEn,0)              //�����߿���
#define sheathedChangeEn            WordToBit(GUS.SheathedWire.Data.SheathedWireEn,1)              //�����ߺ�˻���


#define TinUpSpeed 8      //մ�������ٶ�








typedef struct//ϵͳ�������
{
    LogicParaDef GoPosPara;		//�߾���λ�ã���λ����
    LogicParaDef FeedWire;


    LogicParaDef StripOuterSkinWirePara;
    LogicParaDef StripInterSkinWirePara;
    LogicParaDef BMoveClamPara;
    LogicParaDef BMoveWirePara;

    LogicParaDef RunProcess;

    LogicParaDef AWork;			    //A�˵���
    LogicParaDef AMakeTerminal;	    //A���
    LogicParaDef ACutWastage;	    //A�з���
    LogicParaDef ATin;			    //Aմ��
    LogicParaDef AFeedTin;		    //A����
    LogicParaDef AFeedRosin;		//A����ˮ
    LogicParaDef ASmoothTin;	    //A����
    LogicParaDef AServoTerminal;	//A�ŷ�����
    LogicParaDef AInsertPin;	    //A�˴���
    LogicParaDef Acheck;            //A���
    LogicParaDef AFeedPin;            //A���
    
    

    LogicParaDef BWork;			    //B�˵���
    LogicParaDef BMakeTerminal;	    //B���
    LogicParaDef BCutWastage;	    //B�з���
    LogicParaDef BTin;			//Bմ��
    LogicParaDef BFeedTin;		//B����
    LogicParaDef BSmoothTin;	//B����

    LogicParaDef BFeedRosin;		//B����ˮ

    LogicParaDef Blow;			//����


    LogicParaDef CutWire;		//����
    LogicParaDef SeparateWire;	//����
    LogicParaDef UnfeedProduct;	//����

    LogicParaDef BFeedWire;		//��˽���
//	LogicParaDef BWeld;			//B�˺���

    LogicParaDef ShakeWire;		//����
    LogicParaDef AddOil;		//����
    LogicParaDef WeldBlow;		//���Ӵ���
    LogicParaDef CutAxisBack;	//�е����λ
    LogicParaDef FirstFeedWire;	//������

    LogicParaDef PullWireTwoTimes;
    LogicParaDef Q_LittleRight_ON;
    LogicParaDef CylinderSeparateWire;
    LogicParaDef DivideWire;
    LogicParaDef sheathedWire;      //�����߰���
	


} Task;
extern Task LogicTask;

typedef struct//ϵͳ�������ñ���
{
    u32 initflag;
	u32 LockEn;
    u8 IsCanStrip;
    u8 IsCanClam;
    u8 StopFlag;
    u8 CleanPruduct;
    u8 AWireCheckFlag;
    u8 BWireCheckFlag;
    u8 CurrentStation;

    s32 PullTwoTimesPos;

    u32 DivideFirstWire;      //����ȥ��ǰ�����
    u32 Stop;
    u32 TerminalErro;
    u32 StopCurrWire;
    u32 ChangeMould;
    s32 CylinderSeparateSecond;
    
    s32 FeedPinCount;
    s32 FeedPinDone;
    s32 TerminalCtrol;   //
    s32 PitchNumOK;
    s32 BuzzerSet;
    s32 Z_JOGEn;
	s32 PressAlarmCount;
	s32 BadWire;
	s32 BadWireEn; //1Ϊ��˲���  2Ϊ�Ӿ�����
	s32 BadStopNum;		//��������
	s32 OKStopNum;
	u32 InitAlarm; 
	u32 VisionCheck;
	u32 AddRosinAlarm;

} SysData;

extern SysData Data;

#define LogicContinus ton_et[1]=0

typedef enum	//�ᶯģʽѡ��
{
    ABSMODE,
    RELMODE,
    SPDMODE,
    GOHOMEMODE,
} MoveMode;



typedef struct//���ߺ���
{
    u8 PullAxis;  //���ߵ���
    
    u32 A_separateWireLength;   //ǰ�˷��߳���
    u32 B_separateWireLength;   //��˷��߳���
    u32 cutterSeparateDistance; //�ֵ����е�����
    
    u32 WireLength;             //�����߳�
    u32 StarLength;             //��ʼ����
}SepareWire;
extern SepareWire SepareWireData;

extern void SpeedParaInit(void);
extern void SetSpdDefault(u8 i);
extern void Logic(void);
extern void HMI_Button(void);
extern void Alarm(void);
extern void ButtonMotion(void);
extern void LedCtrol(void);

#endif


