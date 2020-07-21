#ifndef _DATADEFINE_H_
#define	_DATADEFINE_H_
#include "interlayer_config.h"
#include "lock.h"

#define DATACHAECK 0x01237654
#define	SYSREADLEN		1000
#define	USERREADLEN		1000
#define	READLEN			(SYSREADLEN+USERREADLEN)

#define	SYSWRITELEN		1000
#define	USERWRITELEN 	1000
#define	WRITELEN		(SYSWRITELEN+USERWRITELEN)

#define	SYSSAVELEN		4000
#define	USERSAVELEN		6000
#define	SAVELEN			(SYSSAVELEN+USERSAVELEN+4)

#define	GLOBALDATALEN	(READLEN+WRITELEN+SAVELEN)

#define	MAXAXISNUM		50
#define	LOCALAXISNUM	12
#define	INPUTSTARTADDR	310
#define	INPUTENDADDR	389
#define	OUTPUTSTARTADDR	1020
#define	OUTPUTENDADDR	1099

/************************************************************************/
/* 用户自定义的存储数据结构区，用户存储数据按照功能分成类存放 */
typedef struct
{
    u32 StateSelect;	//按键状态显示
    u32 Move;			//动作标志
}TeachSet;

/************************************************************************/
typedef struct
{
    u8 hour;		//范围0-23
    u8 min;			//0-59
    u8 sec;			//0-59
    u8 ampm;		//这个值没用，禁止使用
} Time;

#define WireParaDataLength 200

typedef union
{
    u32	Data_32[WireParaDataLength];

    //A端参数
    struct
    {
        union
        {
            u32	Data_32[30];
            struct
            {
                s32 StripLength[5]; 		//4700 - 4709 剥线长度，预留多芯线剥线长度
                s32 HalfStripLength; 		//4710 半剥长度
                u32 MakeTerminalDelay; 	//4712 打端时间
                u32 FunctionEn;    		 	//4714 功能启用状态子
                u32 MakeTerminalpos; 		//4716 打端位置
                struct
                {
                    u32 AddRosinLength;  	//4718 加松香深度
                    u32 TiningLength;    	//4720 沾锡深度
                    u32 SmothTinTimes;   	//4722 刮锡频率
                    u32 TinTimes;        	//4724 沾锡次数
                    u32 AddTinTimes;     	//4726 送锡频率 沾锡多少次后，开始送锡
                    u32 TinDelay;        	//4728 沾锡延时
                    u32 AddTinDelay;     	//4730 加锡延时
                    u32 RosinDelay;      	//4732 沾松香延时
                    u32 TinSpeed;   		//4734 沾锡速度
                    u32 FeedRosinTimes;  	//4736 加助焊剂频率
                    u32 FeedRosinDelay;  	//4738 加助焊剂时间
                    u32 AddRosinBackLength;	//4740 加松香退回距离
                } TinPara;
                u32 TwistBackDistance;			//4742
                u32 CutDistance;			//4744刀间距
                s32 TinBackPos;				//4746
                u32 MakeTerminalEn;			//4748 打端禁用开关
                u32 SlowTinPos;				//4750 慢速沾锡位置
                u32 SlowTinSpeed;			//4752 慢速沾锡速度
                u32 TinStayTime;			//4754 沾锡停留时间
                u32 SlowTinBackSpeed;		//4756 慢速沾锡退速度
            } Para;
        } A;

        //B端参数
        union
        {
            u32	Data_32[30];
            struct
            {
                s32 StripLength[5]; 			//4760 - 4769 剥线长度，预留多芯线剥线长度
                s32 HalfStripLength; 			//4770 半剥长度
                u32 MakeTerminalDelay; 		//4772 打端时间
                u32 FunctionEn;     			//4774 功能启用状态子
                u32 MakeTerminalpos; 			//4776 打端位置
                struct
                {
                    u32 AddRosinLength;  		//4778 加松香深度
                    u32 TiningLength;    		//4780 沾锡深度
                    u32 SmothTinTimes;   		//4782 刮锡频率
                    u32 TinTimes;        		//4784 沾锡次数
                    u32 AddTinTimes;     		//4786 送锡频率 沾锡多少次后，开始送锡
                    u32 TinDelay;        		//4788 沾锡延时
                    u32 AddTinDelay;     		//4790 加锡延时
                    u32 RosinDelay;      		//4792 沾松香延时
                    u32 TinSpeed;   				//4794 沾锡速度
                    u32 FeedRosinTimes;  		//4796 加助焊剂频率
                    u32 FeedRosinDelay; 	 	//4798 加助焊剂时间
                    u32 AddRosinBackLength;			//4800 加松香退回距离
                } TinPara;
                u32 TwistBackDistance;			//4802
                u32 CutDistance;			//4804刀间距
                s32 TinBackPos;				//4806
                u32 MakeTerminalEn;			//4808 打端禁用开关
                u32 SlowTinPos;				//4810 慢速沾锡位置
                u32 SlowTinSpeed;			//4812 慢速沾锡速度
                u32 TinStayTime;			//4814 沾锡停留时间
                u32 SlowTinBackSpeed;		//4816 慢速沾锡退速度
            } Para;
        } B;
        char FileNum[8];								//4820 4822檔案编码
        u32 AddData[20];
        u32 Term1[15];
        s32 WrieLength;								//4900 线长(总长)1
        u32 TerminalFrontLimitMin;                  //4896 打端前后限制最小值
        u32 TerminalFrontLimitMax;                  //4898 打端前后限制最大值
        s32 WrieLengthHMI;								//4900 线长(总长)1
        s32 WireCompensate;          	//4902 线长补偿
        s32 WireDistance;							//4904 线距
        s32 TwoPinDistance;                         //
        s32 WireSize[4];							//4906 - 4915 线径
        s32 CutWorth[5];							//4916 - 4925 刀值
        s32 BackCut[5];               //4926 - 4935 退刀值
        u32 StopDelay;               	//4936 把数到停机延时
        u32 BlowDelay;                //4938 吹气延时
        s32 TwistCutPos;          //4940 搓刀间距
        s32 TwistBackSpeed;        		//4942 扭线后退速度
        s32 TwistPercent;        			//4944 扭线后退速度
        s32 WastageCountSet;        	//4946 切废料计数
        u32 TwistDirection;  					//4948 0:正搓，1:反搓
        u32 TwistDistance;				//4950
        struct
        {
            u32 Num;										//4952 总量
            u32 NumOf;									//4954 定量
            u32 WireNum;								//4956 跟数
        } ProductSet;
        s32	WeldFeedLength;						        //4958
        u32 FileCheak;                                  //4960 文件确认保存 1：OK保存
    }Data;
} WireParaDefine;

typedef struct
{
    u32 Num;		//跳线根数
    u32 Distance;	//跳线距离
} SelectSet;  //跳线设置

#define GUS_WirePara GUS.WirePara.Data
/************************************************************************/
/* 动作交互接口类，用于每个动作的上位机参数传递和触发			        */
/************************************************************************/

/************************************************************************/
/* 此处之后的系统区不允许更改，用户只能更改用户区数据		           */
/************************************************************************/

typedef union
{
    u32	Data_32[10];
    struct
    {
        s32	StartSpeed;		    //初速度
        s32	Acctime;			//加速时间
        s32	RunSpeed;			//点动速度
        s32	Dectime;			//减速时间
        s32	EndSpeed;			//末速度
        s32	HomeSpeedFast;		//回原点快速速度4010-
        s32	HomeSpeedSlow;		//回原点慢速速度4012-
        s32	HomeOffset;			//回原点偏移坐标4014-
        s32	SoftMinLimit;		//软负限位4016-
        s32	SoftMaxLimit;		//软正限位4018-
    } Data;
} AxisDef;


typedef struct
{
    u16 limitMode;	  	//限位模式：0 没限位 1 软件限位 2 硬件限位 3 软硬都限
    u16 Poslimit;     	//限位信号
    u16 Poslimitlev;    //限位信号电平
    u16 Neglimit;       //限位信号
    u16 Neglimitlev;    //限位信号电平
    u16 OrgNum;         //原点信号
    u16 Orglev;			    //原点信号电平
    u16 HomeMode;		    //回零模式
    u16 alarmmode;		  //轴报警电平:0：低电平有效 1：高电平有效 其他：轴报警无效
    u16 res;			      //
} AxisIOConfig;

typedef union
{
    u8	Data_8[GLOBALDATALEN];
    u16	Data_16[GLOBALDATALEN / 2];
    struct
    {
        struct//READ ADDR 0-999
        {
            union//SYS READ ADDR 0-499
            {
                u16	Data_16[SYSREADLEN / 2];
                struct
                {
                    u32	HardWare_Ver[10];		//0-19	硬件版本
                    char SoftWare_Ver[40];		//20-39	软件版本
                    u32	Reserve[30];			//40-99	保留
                    s32	AxisEncoder[MAXAXISNUM]; //100-199	轴编码器值
                    s32	AxisPosition[MAXAXISNUM];//200-299	轴当前位置
                    u32	AxisStatus[5];			//300-309	轴当前状态
                    u32	InputStatus[40];		//310-389	输入口状态
                    u32	ErrorCode[20];			//390-	错误码
                    u32	ErrorLevel;				//430-	错误等级
					Time time;	//432	rtc 时间
					Date date;	//434	rtc 日期
					u32 CID[2];	//436 438 本机生成的客户随机码
                } Data;
            } Sys;

            union//USER READ ADDR 500-999
            {
                u16	Data_16[USERREADLEN / 2];
                struct
                {
                    u32 RunStatus;			//500
                    u32 HaveToReset;
                    u32 HaveToRepower;
                    s32	AxisPositionUserUnit[MAXAXISNUM];//506 - 605	轴当前位置
                    u32 AxisAlmSta;
                    u32 AxisInpSta;
                    u32 AxisDirSta;
                    s32 AxisSta[12];
                    u32 ExStatue;
                } Data;
            } User;
        } Read;

        struct//WRITE ADDR 1000-1999
        {
            union//SYS WRITE ADDR 1000-1499
            {
                u16	Data_16[SYSWRITELEN / 2];
                struct
                {
                    u32 res[10];			//1000-1019 保留
                    u32	OutputStatus[40];	//1020-1099	输出口状态 outPut[0]  bit 0
                    u32	FlashOperate;		//1100-	Flash操作
                    u32	FilesOperate;		//1102-	文件操作
                    u32	FramOperate;		//1104-	铁电操作
                    u32	JogMode;			//1106-	点动模式
                    u32	JogPositionLevel;	//1108-	点动行程级别
                    u32	JogPosition;		//1110-	点动设定行程
                    u32	JogForward[2];		//1112-1115	正向点动
                    u32	JogBackward[2];		//1116-1119	反向点动
                    u32	JogGohome[2];		//1120-1123	点动回零
                    u32	JogStop[2];			//1124-1127	点动立即停止
                    u32	jogSpd;				//1128  轴点动百分比速度
                    u32	ClearAlarm;			//1130-	清除错误
					struct
					{
							u32 refreshId;	//	1132 随机数刷新
							s32 decode[4];	//1134 1136 1138 1140  4个解锁码 *6个数 一共24个数
					} LockPara;					
                    //end
                } Data;
            } Sys;

            union//USER WRITE ADDR 1500-1999
            {
                u16	Data_16[USERWRITELEN / 2];
                struct
                {
                    struct
                    {
                        u32 RunCommand;     	//1500
                        u32 DeviceMode;		  	//1502
                        u32 StepRun;			  //1504
                        u32 BitControlWord;		//1506
                        u32 HIM_Lock;
                        u32 term[5];        	//
                    } button;
                    u32 TechWord;   			//1520
                    u32 AxisEnSta;				//1522
                    u32 HIM_ButtonLock;			//1524
                    u32 makeTerminalCount;		//1526
                    u32 FrontCheckCount;		//1528
                    u32 UPH;					//1530
                    u32 StopCache;				//1532
                    u32 RealWireNum;			//1534
                    u32 NoTermanal;				//1536
                    u32 NoTermanalCount;		//1538
                    u32 WiterState;				//1540
                    u32 ReadState;				//1542
                    u32 makeTerminalCountForUser;//1544
                    u32 WireNoTerminalState;       //1546
                    u32 WireNoTerminalCount;        //1548
                    u32 HadToResset;                //1550
                    u32 ChangeEn;                  //1552
                    Date CurrDateForUser;     //1554设置的到期时间解锁后的日期
                    Date CurrDateSetForUser;  //1556设置的当前时间的日期
                    Time CurrTimeSetForUser;	//1558	rtc 时间
                    u32 LockTime;               //1560 剩余的时间（天）
                    u32 CurrStopDelay;          //1562 当前暂停时间
                    
                    //位置移动 位置确认数据
                    u32 PosTeachWord;           //1564 位置确认 示教字
                    u32 PosMoveWord;            //1566 位置移动 示教字
                    TeachSet TeachSetData[15]; 	//1568 1627
					
					u16 YearForHMI;				
					u16 MonthForHMI;
					u16 DayForHMI;
					u16 HourForHMI;
					u16 minForHMI;
					u16 SecForHMI;
					
					u32 LockGHMI;
					
					
					u16 LockYearForHMI;				
					u16 LockMonthForHMI;
					u16 LockDayForHMI;
					u16 LockWeekForHMI;
					
					u32 AddRosinHIM;	//1640
					
                } Data;
            } User;
        } Write;

        struct//SAVE ADDR 2000-4999
        {
            union//SYS SAVE ADDR 2000-3999
            {
                u16	Data_16[SYSSAVELEN / 2];
                struct
                {
                    AxisDef	AxisData[MAXAXISNUM];			            //2000-2999	50个轴，每个占20个寄存器
                    s32 OneCirclePulse[MAXAXISNUM];				        //3000 电机单圈脉冲
                    s32	OneCircleLenth[MAXAXISNUM];				        //3100 单圈行程
                    s32 DataLimit[50];							        //3200
                    AxisIOConfig AxIOconfig[MAXAXISNUM];                //3300 - 3799
					lockdataflashsave LockDate;							//3800 保存设置的锁机日期			
					s32 UNLOCKEn;										//永久解锁标志					   		
                } Data;
            } Sys;

            union//USER SAVE ADDR 4000-5999
            {
                u16	Data_16[USERSAVELEN / 2];
                struct
                {
                    u32 SpdRatio[30]; //4000 - 4059
                    u32 SpeedRatio; //4060 速度比例
                    u32 FunctionEn;	//4062 功能开关
                    s32 ChangeMould;//4064 更换模具位置
                    s32 term1[6];   //4064
                    struct
                    {

                        s32 DividPos_FrontOut;           //4078
                        s32 DividPos_Front;           //4080 
                        s32 DividPos_EndOut;           //4082 
                        s32 DividPos_End;           //4084 
                        s32 DividPos_Cut;           //4086 切刀插线位置
                        s32 ShakeWirePos_Cut;     //4088  切刀的去线皮位置
                        u32 EndAxisPullPos;       //4090 后剥轴拉线位置
                        s32 CutOpenPos;						//4092 切刀张开位置
                        u32 PullSpeed;						//4094 拉线速度
                        u32 EndOnDelay;						//4096 后端顶线夹紧延时
                        u32 EndOffDelay;					//4098 后端顶线张开延时
                        s32 FeedFrontDistance;		//4100 前端送线距离
                        s32 FrontRFeedPos;    		//4102 前端旋转送线位
                        s32 EndRFeedPos; 					//4104 后端旋转送线位
                        s32 FrontFeedPos;					//4106 前端前后送线位
                        s32 EndFeedPos;						//4108 后端前后送线位
                        s32 PullFeedPos;					//4110 拉线轴送线位
                        u32 PullWireClamOnDelay;			//4112 夹线延时
                        u32 FrontOnDelay;					//4114 前端顶线夹紧延时
                        u32 FrontOffDelay;					//4116 前端顶线张开延时
                        s32 PullStraightLength;				//4118 拉线长度
                    } FeedWire;


                    struct
                    {
                        s32 M_ReadyPos;             //4120 准备位置
                        s32 M_StripPos;             //4122 剥皮位置
                        u32 M_StripSpeed;           //4124 剥皮速度
                        u32 OuterSkinClamOnDelay;   //4126 外皮夹ON延时
                        u32 OuterSkinClamOffDelay;  //4128	外皮夹OFF延时
                        s32 CutToClamDistance;      //4130 切刀原点位置到夹子的距离
                        s32 term[4];
                    } StripOuterSkin;

                    struct
                    {
                        s32 C_ReadyPos;   			    //4140 剥刀准备位置
                        s32 C_CutPos;   				//4142 剥刀切断位置
                        s32 C_StripPos;					//4144 剥刀剥线位置
                        s32 C_PressWirePos;  		    //4146 压线位置
                        s32 M_ReadyPos; 				//4148 剥刀前后 工作位
                        s32 M_PressWirePos; 		    //4150 压线位置
                        s32 M_ClosePos; 				//4152 合拢位
                        s32 M_StripPos; 				//4154 剥皮位
                        s32 M_CutPos; 					//4156 切断位
                        s32 AdjustWireSpeed;  	//4158 整线速度
                        s32 M_CutDistance; 			//4160 刀间距
                        s32 R_ReadyPos; 				//4162 工作位置
                        s32 R_AdjustWireAngle;  //4164 整线角度
                        s32 R_TimesSet;  				//4166 旋转次数
                        u32 AdjustFitOnDelay;		//4168
                        u32 AdjustFitOffDelay;	//4170
                        u32 AdjustClamOnDelay;	//4172
                        u32 AdjustClamOffDelay;	//4174
                        s32 CutToClamDistance;  //4176 切刀原点位置到夹子的距离
                        s32 PressToClamDistance;  //4178 压线夹原点位置到夹子的距离
                    } StripInterSkin;

                    struct
                    {
                        s32 B_R_RosinPos; 						//4180 送线位置
                        s32 B_R_TinPos; 						//4182 助焊剂位置
                        s32 M2_ReadyPos; 						//4184 移动夹工作位
                        s32 M2_TakeWirePos; 				//4186 取线位置
                        s32 M2_UnfeedBackDistance; 	//4188 退料后退距离
                        s32 M2_CheckWirePos; 				//4190 检测位置
                        s32 M2_MakeTermanalPos; 		//4192 打端位置
                        s32 A_ReadyPos;							//4194 B前后准备位置
                        s32 A_TakeWirePos;					//4196 B前后取线位置
                        s32 A_MakeTermanalPos; 			//4198 B前后打端位置
                        s32 A_UnfeedPos;          	//4200 B前后退料位置
                        u32 B_ClamOnDelay;        	//4202 B夹ON延时
                        u32 B_ClamOffDelay;        	//4204 B夹OFF延时
                        s32 M2_PutWirePos;					//4206 夹线位
                        s32 term[6];
                    } B_Tin;

                    struct
                    {
                        s32 LittleFrontOnDelay; 					//4220 取线位置
                        s32 LittleFrontOffDelay; 					//4222 放线位置
                        u32 EndClamOffDelay;   						//4224 取线夹ON延时
                        u32 LittleClamOnDelay;  					//4226 取线夹ON延时
                        u32 BeltBlowDelay;							//4228 皮带吹气时间
                        u32 LittleClamOffDelay;  					//4230 取线夹OFF延时
                        u32 ProductClamOnDelay;                     //4232 成品气缸开延时
                        u32 ProductClamOffDelay;                    //4234 成品气缸关延时
                        u32 EndAxisUnFeedPos;                       //4236 
                        u32 StrepNum;                               //4238 绞线圈数
                        u32 StrepFeedPos;                           //4240 水平位置
                        s32 term[9];
                    } Unfeed;

                    struct
                    {
                        u32 NumSet; 						//4260 总产量
                        u32 CurrNum;						//4262 当前数量
                        u32 PitchNum;						//4264 每把数量
                        u32 CurrPitchNum;				    //4266 每把当前数量
                        u32 StopDelay;			            //4268 把数到停机延时
                        u32 NoTerminalNum;			        //4270 缺端子计数
                        u32 Pitch;	                        //4272
                        u32 DivideWireNum;                  //4274  排线根数
                        u32 AddNum;
                        s32 TerminalNum;
                    } Product;

                    struct
                    {
                        u32 CutWastageCountSet;  			//4280切废料间隔,单位 次
                        u32 FeedDelay;  					//4282 放端保护时间
                        u32 MakeTerminaDelay;				//4284 
                        s32 FrontMoveCheckPos;				//4286  前端横移检测位置
                        s32 FrontMoveMakeTerminalPos;		//4288  前端横移打端位置
                        s32 FrontMakeTerminalPos;			//4290 前端前后打端位置
                        s32 MakeTerminalOffer;				//4292 打端深度补偿
                        s32 PressAlarmNum;					//4294
						s32 term[2];    					//-4299
                    } AMakeTerminal;

                    struct
                    {
                        u32 A_separateWireLength;   	//4300 前分线长度
                        u32 B_separateWireLength;   	//4302 后分线长度
                        u32 cutterSeparateDistance; //4304 切刀到分线到距离
                        u32 SperateOnDelay;         //4306
                        u32 SperateOffDelay;        //4308
                        u32 SperateDelay;           //4310
                        u32 sperateSpeed;           //4312 分线速度
                        u32 SperateWireNum;			//4314
                        u32 SperateWireDistance;	//4316
                        u32 SperateUp;			//4318 分线上
                        u32 SperateDown;				//4320 分线下
                        u32 Down_ON_Delay;
                        u32 Down_OFF_Delay;
                        u32 Front_ON_Delay;
                        u32 Front_OFF_Delay;
                        u32 PullFeedFrontDistance;
                        u32 CutFeedFrontDistance;
                        s32 term[3];				//  - 4339
                    } SperatePara;

                    struct
                    {
                        s32 SumProduactCount;		//4340 累计计数
                        s32 SumProduactCount_k;		//4342 累计计数k
                        s32 SumProduactCount_kk;	//4344 累计计数kk
                        s32 SumProduactCount_kkk;	//4346 累计计数kkk
                        s32 SumProduactCount_kkkk;	//4348 累计计数kkkk
                        s32 term[5];
                    } SumCount;

                    struct
                    {
                        s32 PullWireClamOffDelay;	//4360 拉线夹张开延时
                        s32 PullBackLength;			//4362 拉线轴后退距离
                        s32 A_CutDistance;			//4364 A端刀间距
                        s32 B_CutDistance;			//4366 B端刀间距
                        s32 StripBackPos_A;			//4368 剥线退位
                        s32 StripBackPos_B;			//4370 剥线退位
                        s32 TwistDownOnDelay;		//4372 扭线下开延时
                        s32 TwistOnDelay;			//4374 扭线开延时
                        u32 TwistSpeed;
                        s32 ShakeWire;
                    } CutWirePara;

                    struct
                    {
                        s32 A_R_RosinPos; 			//4380 送线位置
                        s32 A_R_TinPos; 			//4382 助焊剂位置
                        u32 A_ClamOnDelay;        	//4384 B夹ON延时
                        u32 A_ClamOffDelay;        	//4386 B夹OFF延时
                        s32 term[6];
                    } A_Tin;


                    struct
                    {
                        s32 SmoothTinDelay; 			//4400  刮锡时间
                        s32 SmoothDownDelay; 			//4402  刮锡下时间
						s32 AddRosinTime;				//4404	加松香的时间
                        s32 term[7];
                    } SmoothinPara;

                    struct
                    {
                        s32 EndAxisUnfeedPos; 			//4420 后端轴接线位置
                        s32 UnfeedStraightLength; 		//4422 接线绷直
                        s32 EndMoveFeedPos;				//4424 后横移接线位置
                        s32 EndMoveTeTerminalPos;       //4426 后横移打端位置
                        s32 EndMoveUnFeedPos;           //4428 后横移下料位置
                        s32 FeedWire_EndR;              //4430 后端旋转的接线角度
						s32 term[4];
                    } BFeedWire;

                    struct
                    {
                        s32 UnFeedPos_EndMove; 		    //4440  后横移下料位置
                        s32 MakeTerminaDelay;			//4442	打端延时
                        u32 Term[3];
                    }BMakeTerminal;

                    union
                    {
                        u32	Data_32[15];
                        struct
                        {
                            u32 FrontMoveInsertPinPos; 	    //横移轴插壳位置   4450
                            s32 FrontAxisInsertPinPos;	    //前后轴插壳位置   4452
                            u32 PinLocatONDelay;		    //端子定位气缸开延时 4454
                            u32 PinLocatOFFDelay;		    //端子定位气缸关延时 4456
                            u32 PressOnDelay;               //压线开延时 4458
                            u32 PressOffDelay;              //压线关延时   4460
                            u32 FirstPinDistance;           //一段插壳距离 4462
                            u32 SecondPinDistance;          //二段插壳距离 4464
                            u32 PushOnDelay;                //推出延时     4466
                            u32 EndPos_FrontMove;           //横移轴结束位置 4468
                            u32 EndPos_Front;               //前后轴结束位置 4470
							u32 InsertPinSpeed;				//插壳速度 4472
							u32 InsertDoneSpeed;			//
						} Data;
                    } AInsertPin;

                    struct
                    {
                        s32 PullWireSpeed; 					//4480
                        s32 PullWireSlowSpeedDistance; 		//4482
                        u32 BlowStarTime;                   //4484吹气起始时间
                        u32 BlowTime;                       //4486吹气时长
                        u32 HalfCutWorth;                   //4488半剥刀值
                        u32 PullHelp;                       //4490拉线辅助
						u32 BadTerminaLength;				//4492
						u32 BadcheckLength;					//4494
						u32 StopNum;						//4496 停机不良率
                        u32 Term;
                    } PullWire;

                    union
                    {
                        u32	Data_32[20];
                        struct
                        {
                            u32 SecondSlowTinSpeed_A; 		//第二次沾锡速度 	4500
                            u32 SecondSlowTinBackSpeed_A; 	//第二次沾锡退回速度 4502
                            u32 SecondSlowTinSpeed_B; 		//第二次沾锡速度 	4504
                            u32 SecondSlowTinBackSpeed_B; 	//第二次沾锡退回速度 4506
                            s32 SecondRosinLength_A;		//4508 多次沾松香深度A
                            s32 SecondRosinLength_B;		//4510 多次沾松香深度B
                            s32 SecondSlowTinPos_A;			//4512 多次慢沾锡位A
                            s32 SecondSlowTinPos_B;			//4514 多次慢沾锡位B
                            s32 SecondTinSpeed_A;			//4516 多次沾锡速度A
                            s32 SecondTinSpeed_B;			//4518 多次沾锡速度B
                            s32 SecondTinLength_A;			//4520 多次沾锡深度A
                            s32 SecondTinLength_B;			//4522 多次沾锡深度B
                        } Data;
                    } SecondTin;  
                    
                    union
                    {
                        u32	Data_32[30];
                        struct
                        {
                            SelectSet SelectData[8];    //4540 跳线设置
                        } Data;
                    } Select;     
                    
                    union
                    {
                        u32	Data_32[20];
                        struct
                        {
                            s32 SheathedWireEn;         //护套线使能开关       4600
                            s32 StripCut; 		        //护套线剥皮位置       4602
                            s32 StripCutOpen; 		    //护套线切刀张开位置    4604
                            s32 FrontStripLength; 		//前端剥线长度          4606
                            s32 EndStripLength; 		//后端剥线长度          4608      
                            
                            s32 ChangeLength; 		    //换手距离              4610
                            s32 EndChangePos; 		    //换手位置              4612
                            s32 StripCutBack; 		    //护套线剥皮退刀       4614
                            s32 StripSpeed;             //护套线剥皮速度       4616
                            s32 CutBack;                //切断退刀位置         4618
                        } Data;
                    }SheathedWire;                    
                    
                    union
                    {
                        u32	Data_32[10];
                        struct
                        {
                            s32 CheckPos_Move;          //4640 检测横移位
                            s32 CheckPos_Front;         //4642 检测Y位置
                            s32 CheckBackPos;           //4644 检测退位
                            s32 PressOn;               //4646
                            s32 PushOff;                //4648 插壳收回
                            s32 EndPos_FrontMove;       //4650 结束位置 
                            s32 CheackDelay;            //4652 视觉检测时间
                        } Data;
                    }Acheck;
                    
                    union
                    {
                        u32	Data_32[15];
                        struct
                        {
                            s32 TakePinPos;             //4660 取壳位置
                            s32 FeedNum;                //4662 上壳数量
                            s32 FeedBackPos;            //4664  推壳原位
                            s32 FeedPos[5];              //4666-  胶壳位置1
                            s32 FeedStay;               //4676 上壳时间
                            s32 PushDelay;              //4678 推壳时间
                        } Data;
                    }AFeedPin;
                    
                    

                    s32 term[3];                	//4680 - 4695
                    u32 FileNum;				    //4696
                    u32 FileNumTerm;				//4698
                    WireParaDefine WirePara;
                    
                    
                    union
                    {
                        u32	Data_32[30];
                        struct
                        {
                            s32 WireDistanceOffer[30];
                        } Data;
                    } WireDistanceOffer; 

                    
                    WireParaDefine RepicData[10];
                } Data;
            } User;
        } Save;
        u32	Check;
    } SpaceType;
} GlobalDataDef;
extern GlobalDataDef GlobalData;

#define	GSR	GlobalData.SpaceType.Read.Sys.Data
#define	GSW	GlobalData.SpaceType.Write.Sys.Data
#define	GSS	GlobalData.SpaceType.Save.Sys.Data
#define	GUR	GlobalData.SpaceType.Read.User.Data
#define	GUW	GlobalData.SpaceType.Write.User.Data
#define	GUS	GlobalData.SpaceType.Save.User.Data

#endif
