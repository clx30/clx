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
#define TimerCnt(a)		((SysTim_GetUser()-a.tm)/10) 	//计时转成ms
#define TimerRst1(a)		 a->tm = SysTim_GetUser()		  //0.1 ms
#define TimerCnt1(a)		((SysTim_GetUser()-a->tm)/10) 	//计时转成ms


//test
#define SPD_MAX (200*1000)

#define	ON	0
#define	OFF	1

#define PARAINIT(a)  memset(&a,0,sizeof(a));
#define INITTASK(a) if(a.execute==1&&a.step==0){a.step = 1;}

#define CHECK_NUM 1
#define	VACUUM_NUM 1
#define	DOWN_NUM 1



#define	NORMALMODE	0		//正常模式
#define	STEPMODE	1		//单步调试
#define	CYCLEMODE	2		//单次调试
#define	AGINGMODE	3		//老化模式

#define AXISDELAY	5		//停轴延时
#define ENDCASE     0xff	//最后一个case

#define Null 0xFF


#define AxisDelay 30


//时间编号
#define YEAR 0
#define MONTH 1
#define DAY 2
#define HOUR 3
#define MINUTE 4
#define SECOND 5

//焊接工位状态
#define Free 0 //空闲状态 无料
#define Done 1 //完成状态 上料完成
#define Feed 2 //上料状态 正在上料
#define Work 3 //焊接状态 正在工作
//线类别
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
    GOHOMETYPE1,	     //直接回原点(平常模式)
    GOHOMETYPE2,		//先到上限位再回原点
    GOHOMETYPE3			//先回下限位再回原点
};

typedef enum
{
    AX_ERR_AX_ALM = (1 << 0),		//轴报警
    AX_ERR_AX_TMOUT = (1 << 1),		//超时（回原点）
    AX_ERR_UP_LIM_H = (1 << 8),		//硬上限位
    AX_ERR_UP_LIM_S = (1 << 9),		//软上限位
    AX_ERR_DW_LIM_H = (1 << 12),		//硬下限位
    AX_ERR_DW_LIM_S = (1 << 13),		//软下限位
} AxErrBitTp;

typedef enum
{
    HM_GO_DW = (1 << 0),			//回下限
    HM_GO_UP = (1 << 1),			//回上限
    HM_GO_HM = (1 << 2),			//回原点
} AxHomBitTp;



//GUW.button.BitControlWord   //1506
typedef enum
{
    StartBit,							//bit0  开始按钮
    StopBit,							//bit1  停止按钮
    ResetBit,							//bit2  复位按钮
    HoldBit,							//bit3  暂停按钮
    ClearPruductNumBit,  	//bit4  产量清0按钮
    ProductNumSub1Bit,  	//bit5	产量减1按钮
    AxisPowerOn,  				//bit6	伺服使能
} HMIButton;

//A端功能启用4714
#define	AHalfStripEn 				WordToBit(GUS.WirePara.Data.A.Para.FunctionEn,0)		//bit0  半剥启用
#define	ATwistWireEn				WordToBit(GUS.WirePara.Data.A.Para.FunctionEn,1)		//bit1  扭线启用
#define	ATinEn						WordToBit(GUS.WirePara.Data.A.Para.FunctionEn,2)		//bit2  沾锡启用
#define	AMakeTerminalEn				WordToBit(GUS.WirePara.Data.A.Para.FunctionEn,3)		//bit3  打端子启用
#define	AWireCheckEn  				WordToBit(GUS.WirePara.Data.A.Para.FunctionEn,4)		//bit4  剥线检测启用
#define	AMakeTerminalCheck  		WordToBit(GUS.WirePara.Data.A.Para.FunctionEn,5)		//bit5	端子压着检测启用
#define	APressCheckEn  				WordToBit(GUS.WirePara.Data.A.Para.FunctionEn,6)		//bit6	端子机压力检测
#define ANoTerminalCheckEn			WordToBit(GUS.WirePara.Data.A.Para.FunctionEn,7)		//bit7  缺端子检测	
#define ASlowTinEn					WordToBit(GUS.WirePara.Data.A.Para.FunctionEn,8)		//bit8  慢速沾锡开关	
#define	ARosinMuchEn				WordToBit(GUS.WirePara.Data.A.Para.FunctionEn,9)		//bit9  沾松香多次启用
#define	AVisionCheckEn				WordToBit(GUS.WirePara.Data.A.Para.FunctionEn,10)		//bit10 视觉检测开关
#define	AInsertPinEn				WordToBit(GUS.WirePara.Data.A.Para.FunctionEn,11)		//bit11 插胶壳开关
#define	ATWoPinEn				    WordToBit(GUS.WirePara.Data.A.Para.FunctionEn,12)		//bit12 跳线开关


//B端功能启用 4774
#define	BHalfStripEn 				WordToBit(GUS.WirePara.Data.B.Para.FunctionEn,0)		//bit0  半剥启用
#define	BTwistWireEn				WordToBit(GUS.WirePara.Data.B.Para.FunctionEn,1)		//bit1  扭线启用
#define	BTinEn						WordToBit(GUS.WirePara.Data.B.Para.FunctionEn,2)		//bit2  沾锡启用
#define	BMakeTerminalEn				WordToBit(GUS.WirePara.Data.B.Para.FunctionEn,3)		//bit3  打端子启用
#define	BWireCheckEn  				WordToBit(GUS.WirePara.Data.B.Para.FunctionEn,4)		//bit4  剥线检测启用
#define	BMakeTerminalCheck  		WordToBit(GUS.WirePara.Data.B.Para.FunctionEn,5)		//bit5	端子压着检测启用
#define	BPressCheckEn  				WordToBit(GUS.WirePara.Data.B.Para.FunctionEn,6)		//bit6	端子机压力检测
#define BNoTerminalCheckEn			WordToBit(GUS.WirePara.Data.B.Para.FunctionEn,7)		//bit7  缺端子检测	
#define BSlowTinEn					WordToBit(GUS.WirePara.Data.B.Para.FunctionEn,8)		//bit8  慢速沾锡开关	
#define	BTinWarmEn    				WordToBit(GUS.WirePara.Data.B.Para.FunctionEn,9)		//bit9  锡炉加热开关	
#define	BRosinMuchEn    			WordToBit(GUS.WirePara.Data.B.Para.FunctionEn,10)		//bit10  沾松香多次启用	
#define	BFeedTinEn    			    WordToBit(GUS.WirePara.Data.B.Para.FunctionEn,11)		//bit11  加锡开关


//开关功能 4062
#define NoWireCheckEn				WordToBit(GUS.FunctionEn,0)								//bit0   缺线检测	
#define WireKnotCheckEn				WordToBit(GUS.FunctionEn,1)								//bit1   打结检测	
#define PressCheckEn				WordToBit(GUS.FunctionEn,2)								//bit2   气压检测	
#define DoorCheckEn					WordToBit(GUS.FunctionEn,3)								//bit3   门禁检测	
#define SeparateWireMode			WordToBit(GUS.FunctionEn,4)								//bit4  分线模式	
//4063
#define TerminalEn					WordToBit(GUS.FunctionEn,16)							//bit16  端子机禁用开关
#define BHelpClamEN					WordToBit(GUS.FunctionEn,17)							//bit17  B端辅夹开关
#define FrontSeparateWireEN			WordToBit(GUS.FunctionEn,18)					//bit18  分线开关	
#define UnFeedHidEN					WordToBit(GUS.FunctionEn,19)							//bit19  下料避让开关	
#define ShortMode                   WordToBit(GUS.FunctionEn,20)              //bit20 短线模式
#define ShortModeForUser            WordToBit(GUS.FunctionEn,21)              //bit21 用户设置的短线模式 
#define TwistMode                   WordToBit(GUS.FunctionEn,22)              //bit22 扭线模式
#define DividEN                     WordToBit(GUS.FunctionEn,23)              //bit23 气缸分线开关
#define DisWire                     WordToBit(GUS.FunctionEn,24)              //bit24 
//#define FeedWireREn                 WordToBit(GUS.FunctionEn,25)              //bit25 接线角度启用开关9
#define StrepEn       			    WordToBit(GUS.FunctionEn,26)			  //bit26  绞线开关	10
#define NoTinEn       			    WordToBit(GUS.FunctionEn,27)			  //bit27  q缺锡开关	12
#define BuzzerEn       			    WordToBit(GUS.FunctionEn,28)			  //bit27  蜂鸣开关	13
#define PinCheackEn      			WordToBit(GUS.FunctionEn,29)			  //bit27  



#define sheathedWireEn              WordToBit(GUS.SheathedWire.Data.SheathedWireEn,0)              //护套线开关
#define sheathedChangeEn            WordToBit(GUS.SheathedWire.Data.SheathedWireEn,1)              //护套线后端换手


#define TinUpSpeed 8      //沾锡提起速度








typedef struct//系统任务队列
{
    LogicParaDef GoPosPara;		//走绝对位置，上位交互
    LogicParaDef FeedWire;


    LogicParaDef StripOuterSkinWirePara;
    LogicParaDef StripInterSkinWirePara;
    LogicParaDef BMoveClamPara;
    LogicParaDef BMoveWirePara;

    LogicParaDef RunProcess;

    LogicParaDef AWork;			    //A端调度
    LogicParaDef AMakeTerminal;	    //A打端
    LogicParaDef ACutWastage;	    //A切废料
    LogicParaDef ATin;			    //A沾锡
    LogicParaDef AFeedTin;		    //A送锡
    LogicParaDef AFeedRosin;		//A送锡水
    LogicParaDef ASmoothTin;	    //A刮锡
    LogicParaDef AServoTerminal;	//A伺服端子
    LogicParaDef AInsertPin;	    //A端穿壳
    LogicParaDef Acheck;            //A检测
    LogicParaDef AFeedPin;            //A检测
    
    

    LogicParaDef BWork;			    //B端调度
    LogicParaDef BMakeTerminal;	    //B打端
    LogicParaDef BCutWastage;	    //B切废料
    LogicParaDef BTin;			//B沾锡
    LogicParaDef BFeedTin;		//B送锡
    LogicParaDef BSmoothTin;	//B刮锡

    LogicParaDef BFeedRosin;		//B送锡水

    LogicParaDef Blow;			//吹气


    LogicParaDef CutWire;		//裁线
    LogicParaDef SeparateWire;	//分线
    LogicParaDef UnfeedProduct;	//收料

    LogicParaDef BFeedWire;		//后端接线
//	LogicParaDef BWeld;			//B端焊接

    LogicParaDef ShakeWire;		//抖线
    LogicParaDef AddOil;		//喷油
    LogicParaDef WeldBlow;		//焊接吹气
    LogicParaDef CutAxisBack;	//切刀轴回位
    LogicParaDef FirstFeedWire;	//切首线

    LogicParaDef PullWireTwoTimes;
    LogicParaDef Q_LittleRight_ON;
    LogicParaDef CylinderSeparateWire;
    LogicParaDef DivideWire;
    LogicParaDef sheathedWire;      //护套线剥线
	


} Task;
extern Task LogicTask;

typedef struct//系统程序自用变量
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

    u32 DivideFirstWire;      //分线去掉前面的线
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
	s32 BadWireEn; //1为打端不良  2为视觉不良
	s32 BadStopNum;		//不良计数
	s32 OKStopNum;
	u32 InitAlarm; 
	u32 VisionCheck;
	u32 AddRosinAlarm;

} SysData;

extern SysData Data;

#define LogicContinus ton_et[1]=0

typedef enum	//轴动模式选择
{
    ABSMODE,
    RELMODE,
    SPDMODE,
    GOHOMEMODE,
} MoveMode;



typedef struct//分线函数
{
    u8 PullAxis;  //拉线的轴
    
    u32 A_separateWireLength;   //前端分线长度
    u32 B_separateWireLength;   //后端分线长度
    u32 cutterSeparateDistance; //分刀到切刀距离
    
    u32 WireLength;             //本次线长
    u32 StarLength;             //开始长度
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


