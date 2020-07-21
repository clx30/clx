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
/* �û��Զ���Ĵ洢���ݽṹ�����û��洢���ݰ��չ��ֳܷ����� */
typedef struct
{
    u32 StateSelect;	//����״̬��ʾ
    u32 Move;			//������־
}TeachSet;

/************************************************************************/
typedef struct
{
    u8 hour;		//��Χ0-23
    u8 min;			//0-59
    u8 sec;			//0-59
    u8 ampm;		//���ֵû�ã���ֹʹ��
} Time;

#define WireParaDataLength 200

typedef union
{
    u32	Data_32[WireParaDataLength];

    //A�˲���
    struct
    {
        union
        {
            u32	Data_32[30];
            struct
            {
                s32 StripLength[5]; 		//4700 - 4709 ���߳��ȣ�Ԥ����о�߰��߳���
                s32 HalfStripLength; 		//4710 �������
                u32 MakeTerminalDelay; 	//4712 ���ʱ��
                u32 FunctionEn;    		 	//4714 ��������״̬��
                u32 MakeTerminalpos; 		//4716 ���λ��
                struct
                {
                    u32 AddRosinLength;  	//4718 ���������
                    u32 TiningLength;    	//4720 մ�����
                    u32 SmothTinTimes;   	//4722 ����Ƶ��
                    u32 TinTimes;        	//4724 մ������
                    u32 AddTinTimes;     	//4726 ����Ƶ�� մ�����ٴκ󣬿�ʼ����
                    u32 TinDelay;        	//4728 մ����ʱ
                    u32 AddTinDelay;     	//4730 ������ʱ
                    u32 RosinDelay;      	//4732 մ������ʱ
                    u32 TinSpeed;   		//4734 մ���ٶ�
                    u32 FeedRosinTimes;  	//4736 ��������Ƶ��
                    u32 FeedRosinDelay;  	//4738 ��������ʱ��
                    u32 AddRosinBackLength;	//4740 �������˻ؾ���
                } TinPara;
                u32 TwistBackDistance;			//4742
                u32 CutDistance;			//4744�����
                s32 TinBackPos;				//4746
                u32 MakeTerminalEn;			//4748 ��˽��ÿ���
                u32 SlowTinPos;				//4750 ����մ��λ��
                u32 SlowTinSpeed;			//4752 ����մ���ٶ�
                u32 TinStayTime;			//4754 մ��ͣ��ʱ��
                u32 SlowTinBackSpeed;		//4756 ����մ�����ٶ�
            } Para;
        } A;

        //B�˲���
        union
        {
            u32	Data_32[30];
            struct
            {
                s32 StripLength[5]; 			//4760 - 4769 ���߳��ȣ�Ԥ����о�߰��߳���
                s32 HalfStripLength; 			//4770 �������
                u32 MakeTerminalDelay; 		//4772 ���ʱ��
                u32 FunctionEn;     			//4774 ��������״̬��
                u32 MakeTerminalpos; 			//4776 ���λ��
                struct
                {
                    u32 AddRosinLength;  		//4778 ���������
                    u32 TiningLength;    		//4780 մ�����
                    u32 SmothTinTimes;   		//4782 ����Ƶ��
                    u32 TinTimes;        		//4784 մ������
                    u32 AddTinTimes;     		//4786 ����Ƶ�� մ�����ٴκ󣬿�ʼ����
                    u32 TinDelay;        		//4788 մ����ʱ
                    u32 AddTinDelay;     		//4790 ������ʱ
                    u32 RosinDelay;      		//4792 մ������ʱ
                    u32 TinSpeed;   				//4794 մ���ٶ�
                    u32 FeedRosinTimes;  		//4796 ��������Ƶ��
                    u32 FeedRosinDelay; 	 	//4798 ��������ʱ��
                    u32 AddRosinBackLength;			//4800 �������˻ؾ���
                } TinPara;
                u32 TwistBackDistance;			//4802
                u32 CutDistance;			//4804�����
                s32 TinBackPos;				//4806
                u32 MakeTerminalEn;			//4808 ��˽��ÿ���
                u32 SlowTinPos;				//4810 ����մ��λ��
                u32 SlowTinSpeed;			//4812 ����մ���ٶ�
                u32 TinStayTime;			//4814 մ��ͣ��ʱ��
                u32 SlowTinBackSpeed;		//4816 ����մ�����ٶ�
            } Para;
        } B;
        char FileNum[8];								//4820 4822�n������
        u32 AddData[20];
        u32 Term1[15];
        s32 WrieLength;								//4900 �߳�(�ܳ�)1
        u32 TerminalFrontLimitMin;                  //4896 ���ǰ��������Сֵ
        u32 TerminalFrontLimitMax;                  //4898 ���ǰ���������ֵ
        s32 WrieLengthHMI;								//4900 �߳�(�ܳ�)1
        s32 WireCompensate;          	//4902 �߳�����
        s32 WireDistance;							//4904 �߾�
        s32 TwoPinDistance;                         //
        s32 WireSize[4];							//4906 - 4915 �߾�
        s32 CutWorth[5];							//4916 - 4925 ��ֵ
        s32 BackCut[5];               //4926 - 4935 �˵�ֵ
        u32 StopDelay;               	//4936 ������ͣ����ʱ
        u32 BlowDelay;                //4938 ������ʱ
        s32 TwistCutPos;          //4940 �굶���
        s32 TwistBackSpeed;        		//4942 Ť�ߺ����ٶ�
        s32 TwistPercent;        			//4944 Ť�ߺ����ٶ�
        s32 WastageCountSet;        	//4946 �з��ϼ���
        u32 TwistDirection;  					//4948 0:���꣬1:����
        u32 TwistDistance;				//4950
        struct
        {
            u32 Num;										//4952 ����
            u32 NumOf;									//4954 ����
            u32 WireNum;								//4956 ����
        } ProductSet;
        s32	WeldFeedLength;						        //4958
        u32 FileCheak;                                  //4960 �ļ�ȷ�ϱ��� 1��OK����
    }Data;
} WireParaDefine;

typedef struct
{
    u32 Num;		//���߸���
    u32 Distance;	//���߾���
} SelectSet;  //��������

#define GUS_WirePara GUS.WirePara.Data
/************************************************************************/
/* ���������ӿ��࣬����ÿ����������λ���������ݺʹ���			        */
/************************************************************************/

/************************************************************************/
/* �˴�֮���ϵͳ�����������ģ��û�ֻ�ܸ����û�������		           */
/************************************************************************/

typedef union
{
    u32	Data_32[10];
    struct
    {
        s32	StartSpeed;		    //���ٶ�
        s32	Acctime;			//����ʱ��
        s32	RunSpeed;			//�㶯�ٶ�
        s32	Dectime;			//����ʱ��
        s32	EndSpeed;			//ĩ�ٶ�
        s32	HomeSpeedFast;		//��ԭ������ٶ�4010-
        s32	HomeSpeedSlow;		//��ԭ�������ٶ�4012-
        s32	HomeOffset;			//��ԭ��ƫ������4014-
        s32	SoftMinLimit;		//������λ4016-
        s32	SoftMaxLimit;		//������λ4018-
    } Data;
} AxisDef;


typedef struct
{
    u16 limitMode;	  	//��λģʽ��0 û��λ 1 ������λ 2 Ӳ����λ 3 ��Ӳ����
    u16 Poslimit;     	//��λ�ź�
    u16 Poslimitlev;    //��λ�źŵ�ƽ
    u16 Neglimit;       //��λ�ź�
    u16 Neglimitlev;    //��λ�źŵ�ƽ
    u16 OrgNum;         //ԭ���ź�
    u16 Orglev;			    //ԭ���źŵ�ƽ
    u16 HomeMode;		    //����ģʽ
    u16 alarmmode;		  //�ᱨ����ƽ:0���͵�ƽ��Ч 1���ߵ�ƽ��Ч �������ᱨ����Ч
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
                    u32	HardWare_Ver[10];		//0-19	Ӳ���汾
                    char SoftWare_Ver[40];		//20-39	�����汾
                    u32	Reserve[30];			//40-99	����
                    s32	AxisEncoder[MAXAXISNUM]; //100-199	�������ֵ
                    s32	AxisPosition[MAXAXISNUM];//200-299	�ᵱǰλ��
                    u32	AxisStatus[5];			//300-309	�ᵱǰ״̬
                    u32	InputStatus[40];		//310-389	�����״̬
                    u32	ErrorCode[20];			//390-	������
                    u32	ErrorLevel;				//430-	����ȼ�
					Time time;	//432	rtc ʱ��
					Date date;	//434	rtc ����
					u32 CID[2];	//436 438 �������ɵĿͻ������
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
                    s32	AxisPositionUserUnit[MAXAXISNUM];//506 - 605	�ᵱǰλ��
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
                    u32 res[10];			//1000-1019 ����
                    u32	OutputStatus[40];	//1020-1099	�����״̬ outPut[0]  bit 0
                    u32	FlashOperate;		//1100-	Flash����
                    u32	FilesOperate;		//1102-	�ļ�����
                    u32	FramOperate;		//1104-	�������
                    u32	JogMode;			//1106-	�㶯ģʽ
                    u32	JogPositionLevel;	//1108-	�㶯�г̼���
                    u32	JogPosition;		//1110-	�㶯�趨�г�
                    u32	JogForward[2];		//1112-1115	����㶯
                    u32	JogBackward[2];		//1116-1119	����㶯
                    u32	JogGohome[2];		//1120-1123	�㶯����
                    u32	JogStop[2];			//1124-1127	�㶯����ֹͣ
                    u32	jogSpd;				//1128  ��㶯�ٷֱ��ٶ�
                    u32	ClearAlarm;			//1130-	�������
					struct
					{
							u32 refreshId;	//	1132 �����ˢ��
							s32 decode[4];	//1134 1136 1138 1140  4�������� *6���� һ��24����
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
                    Date CurrDateForUser;     //1554���õĵ���ʱ������������
                    Date CurrDateSetForUser;  //1556���õĵ�ǰʱ�������
                    Time CurrTimeSetForUser;	//1558	rtc ʱ��
                    u32 LockTime;               //1560 ʣ���ʱ�䣨�죩
                    u32 CurrStopDelay;          //1562 ��ǰ��ͣʱ��
                    
                    //λ���ƶ� λ��ȷ������
                    u32 PosTeachWord;           //1564 λ��ȷ�� ʾ����
                    u32 PosMoveWord;            //1566 λ���ƶ� ʾ����
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
                    AxisDef	AxisData[MAXAXISNUM];			            //2000-2999	50���ᣬÿ��ռ20���Ĵ���
                    s32 OneCirclePulse[MAXAXISNUM];				        //3000 �����Ȧ����
                    s32	OneCircleLenth[MAXAXISNUM];				        //3100 ��Ȧ�г�
                    s32 DataLimit[50];							        //3200
                    AxisIOConfig AxIOconfig[MAXAXISNUM];                //3300 - 3799
					lockdataflashsave LockDate;							//3800 �������õ���������			
					s32 UNLOCKEn;										//���ý�����־					   		
                } Data;
            } Sys;

            union//USER SAVE ADDR 4000-5999
            {
                u16	Data_16[USERSAVELEN / 2];
                struct
                {
                    u32 SpdRatio[30]; //4000 - 4059
                    u32 SpeedRatio; //4060 �ٶȱ���
                    u32 FunctionEn;	//4062 ���ܿ���
                    s32 ChangeMould;//4064 ����ģ��λ��
                    s32 term1[6];   //4064
                    struct
                    {

                        s32 DividPos_FrontOut;           //4078
                        s32 DividPos_Front;           //4080 
                        s32 DividPos_EndOut;           //4082 
                        s32 DividPos_End;           //4084 
                        s32 DividPos_Cut;           //4086 �е�����λ��
                        s32 ShakeWirePos_Cut;     //4088  �е���ȥ��Ƥλ��
                        u32 EndAxisPullPos;       //4090 ���������λ��
                        s32 CutOpenPos;						//4092 �е��ſ�λ��
                        u32 PullSpeed;						//4094 �����ٶ�
                        u32 EndOnDelay;						//4096 ��˶��߼н���ʱ
                        u32 EndOffDelay;					//4098 ��˶����ſ���ʱ
                        s32 FeedFrontDistance;		//4100 ǰ�����߾���
                        s32 FrontRFeedPos;    		//4102 ǰ����ת����λ
                        s32 EndRFeedPos; 					//4104 �����ת����λ
                        s32 FrontFeedPos;					//4106 ǰ��ǰ������λ
                        s32 EndFeedPos;						//4108 ���ǰ������λ
                        s32 PullFeedPos;					//4110 ����������λ
                        u32 PullWireClamOnDelay;			//4112 ������ʱ
                        u32 FrontOnDelay;					//4114 ǰ�˶��߼н���ʱ
                        u32 FrontOffDelay;					//4116 ǰ�˶����ſ���ʱ
                        s32 PullStraightLength;				//4118 ���߳���
                    } FeedWire;


                    struct
                    {
                        s32 M_ReadyPos;             //4120 ׼��λ��
                        s32 M_StripPos;             //4122 ��Ƥλ��
                        u32 M_StripSpeed;           //4124 ��Ƥ�ٶ�
                        u32 OuterSkinClamOnDelay;   //4126 ��Ƥ��ON��ʱ
                        u32 OuterSkinClamOffDelay;  //4128	��Ƥ��OFF��ʱ
                        s32 CutToClamDistance;      //4130 �е�ԭ��λ�õ����ӵľ���
                        s32 term[4];
                    } StripOuterSkin;

                    struct
                    {
                        s32 C_ReadyPos;   			    //4140 ����׼��λ��
                        s32 C_CutPos;   				//4142 �����ж�λ��
                        s32 C_StripPos;					//4144 ��������λ��
                        s32 C_PressWirePos;  		    //4146 ѹ��λ��
                        s32 M_ReadyPos; 				//4148 ����ǰ�� ����λ
                        s32 M_PressWirePos; 		    //4150 ѹ��λ��
                        s32 M_ClosePos; 				//4152 ��£λ
                        s32 M_StripPos; 				//4154 ��Ƥλ
                        s32 M_CutPos; 					//4156 �ж�λ
                        s32 AdjustWireSpeed;  	//4158 �����ٶ�
                        s32 M_CutDistance; 			//4160 �����
                        s32 R_ReadyPos; 				//4162 ����λ��
                        s32 R_AdjustWireAngle;  //4164 ���߽Ƕ�
                        s32 R_TimesSet;  				//4166 ��ת����
                        u32 AdjustFitOnDelay;		//4168
                        u32 AdjustFitOffDelay;	//4170
                        u32 AdjustClamOnDelay;	//4172
                        u32 AdjustClamOffDelay;	//4174
                        s32 CutToClamDistance;  //4176 �е�ԭ��λ�õ����ӵľ���
                        s32 PressToClamDistance;  //4178 ѹ�߼�ԭ��λ�õ����ӵľ���
                    } StripInterSkin;

                    struct
                    {
                        s32 B_R_RosinPos; 						//4180 ����λ��
                        s32 B_R_TinPos; 						//4182 ������λ��
                        s32 M2_ReadyPos; 						//4184 �ƶ��й���λ
                        s32 M2_TakeWirePos; 				//4186 ȡ��λ��
                        s32 M2_UnfeedBackDistance; 	//4188 ���Ϻ��˾���
                        s32 M2_CheckWirePos; 				//4190 ���λ��
                        s32 M2_MakeTermanalPos; 		//4192 ���λ��
                        s32 A_ReadyPos;							//4194 Bǰ��׼��λ��
                        s32 A_TakeWirePos;					//4196 Bǰ��ȡ��λ��
                        s32 A_MakeTermanalPos; 			//4198 Bǰ����λ��
                        s32 A_UnfeedPos;          	//4200 Bǰ������λ��
                        u32 B_ClamOnDelay;        	//4202 B��ON��ʱ
                        u32 B_ClamOffDelay;        	//4204 B��OFF��ʱ
                        s32 M2_PutWirePos;					//4206 ����λ
                        s32 term[6];
                    } B_Tin;

                    struct
                    {
                        s32 LittleFrontOnDelay; 					//4220 ȡ��λ��
                        s32 LittleFrontOffDelay; 					//4222 ����λ��
                        u32 EndClamOffDelay;   						//4224 ȡ�߼�ON��ʱ
                        u32 LittleClamOnDelay;  					//4226 ȡ�߼�ON��ʱ
                        u32 BeltBlowDelay;							//4228 Ƥ������ʱ��
                        u32 LittleClamOffDelay;  					//4230 ȡ�߼�OFF��ʱ
                        u32 ProductClamOnDelay;                     //4232 ��Ʒ���׿���ʱ
                        u32 ProductClamOffDelay;                    //4234 ��Ʒ���׹���ʱ
                        u32 EndAxisUnFeedPos;                       //4236 
                        u32 StrepNum;                               //4238 ����Ȧ��
                        u32 StrepFeedPos;                           //4240 ˮƽλ��
                        s32 term[9];
                    } Unfeed;

                    struct
                    {
                        u32 NumSet; 						//4260 �ܲ���
                        u32 CurrNum;						//4262 ��ǰ����
                        u32 PitchNum;						//4264 ÿ������
                        u32 CurrPitchNum;				    //4266 ÿ�ѵ�ǰ����
                        u32 StopDelay;			            //4268 ������ͣ����ʱ
                        u32 NoTerminalNum;			        //4270 ȱ���Ӽ���
                        u32 Pitch;	                        //4272
                        u32 DivideWireNum;                  //4274  ���߸���
                        u32 AddNum;
                        s32 TerminalNum;
                    } Product;

                    struct
                    {
                        u32 CutWastageCountSet;  			//4280�з��ϼ��,��λ ��
                        u32 FeedDelay;  					//4282 �Ŷ˱���ʱ��
                        u32 MakeTerminaDelay;				//4284 
                        s32 FrontMoveCheckPos;				//4286  ǰ�˺��Ƽ��λ��
                        s32 FrontMoveMakeTerminalPos;		//4288  ǰ�˺��ƴ��λ��
                        s32 FrontMakeTerminalPos;			//4290 ǰ��ǰ����λ��
                        s32 MakeTerminalOffer;				//4292 �����Ȳ���
                        s32 PressAlarmNum;					//4294
						s32 term[2];    					//-4299
                    } AMakeTerminal;

                    struct
                    {
                        u32 A_separateWireLength;   	//4300 ǰ���߳���
                        u32 B_separateWireLength;   	//4302 ����߳���
                        u32 cutterSeparateDistance; //4304 �е������ߵ�����
                        u32 SperateOnDelay;         //4306
                        u32 SperateOffDelay;        //4308
                        u32 SperateDelay;           //4310
                        u32 sperateSpeed;           //4312 �����ٶ�
                        u32 SperateWireNum;			//4314
                        u32 SperateWireDistance;	//4316
                        u32 SperateUp;			//4318 ������
                        u32 SperateDown;				//4320 ������
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
                        s32 SumProduactCount;		//4340 �ۼƼ���
                        s32 SumProduactCount_k;		//4342 �ۼƼ���k
                        s32 SumProduactCount_kk;	//4344 �ۼƼ���kk
                        s32 SumProduactCount_kkk;	//4346 �ۼƼ���kkk
                        s32 SumProduactCount_kkkk;	//4348 �ۼƼ���kkkk
                        s32 term[5];
                    } SumCount;

                    struct
                    {
                        s32 PullWireClamOffDelay;	//4360 ���߼��ſ���ʱ
                        s32 PullBackLength;			//4362 ��������˾���
                        s32 A_CutDistance;			//4364 A�˵����
                        s32 B_CutDistance;			//4366 B�˵����
                        s32 StripBackPos_A;			//4368 ������λ
                        s32 StripBackPos_B;			//4370 ������λ
                        s32 TwistDownOnDelay;		//4372 Ť���¿���ʱ
                        s32 TwistOnDelay;			//4374 Ť�߿���ʱ
                        u32 TwistSpeed;
                        s32 ShakeWire;
                    } CutWirePara;

                    struct
                    {
                        s32 A_R_RosinPos; 			//4380 ����λ��
                        s32 A_R_TinPos; 			//4382 ������λ��
                        u32 A_ClamOnDelay;        	//4384 B��ON��ʱ
                        u32 A_ClamOffDelay;        	//4386 B��OFF��ʱ
                        s32 term[6];
                    } A_Tin;


                    struct
                    {
                        s32 SmoothTinDelay; 			//4400  ����ʱ��
                        s32 SmoothDownDelay; 			//4402  ������ʱ��
						s32 AddRosinTime;				//4404	�������ʱ��
                        s32 term[7];
                    } SmoothinPara;

                    struct
                    {
                        s32 EndAxisUnfeedPos; 			//4420 ��������λ��
                        s32 UnfeedStraightLength; 		//4422 ���߱�ֱ
                        s32 EndMoveFeedPos;				//4424 ����ƽ���λ��
                        s32 EndMoveTeTerminalPos;       //4426 ����ƴ��λ��
                        s32 EndMoveUnFeedPos;           //4428 ���������λ��
                        s32 FeedWire_EndR;              //4430 �����ת�Ľ��߽Ƕ�
						s32 term[4];
                    } BFeedWire;

                    struct
                    {
                        s32 UnFeedPos_EndMove; 		    //4440  ���������λ��
                        s32 MakeTerminaDelay;			//4442	�����ʱ
                        u32 Term[3];
                    }BMakeTerminal;

                    union
                    {
                        u32	Data_32[15];
                        struct
                        {
                            u32 FrontMoveInsertPinPos; 	    //��������λ��   4450
                            s32 FrontAxisInsertPinPos;	    //ǰ������λ��   4452
                            u32 PinLocatONDelay;		    //���Ӷ�λ���׿���ʱ 4454
                            u32 PinLocatOFFDelay;		    //���Ӷ�λ���׹���ʱ 4456
                            u32 PressOnDelay;               //ѹ�߿���ʱ 4458
                            u32 PressOffDelay;              //ѹ�߹���ʱ   4460
                            u32 FirstPinDistance;           //һ�β�Ǿ��� 4462
                            u32 SecondPinDistance;          //���β�Ǿ��� 4464
                            u32 PushOnDelay;                //�Ƴ���ʱ     4466
                            u32 EndPos_FrontMove;           //���������λ�� 4468
                            u32 EndPos_Front;               //ǰ�������λ�� 4470
							u32 InsertPinSpeed;				//����ٶ� 4472
							u32 InsertDoneSpeed;			//
						} Data;
                    } AInsertPin;

                    struct
                    {
                        s32 PullWireSpeed; 					//4480
                        s32 PullWireSlowSpeedDistance; 		//4482
                        u32 BlowStarTime;                   //4484������ʼʱ��
                        u32 BlowTime;                       //4486����ʱ��
                        u32 HalfCutWorth;                   //4488�����ֵ
                        u32 PullHelp;                       //4490���߸���
						u32 BadTerminaLength;				//4492
						u32 BadcheckLength;					//4494
						u32 StopNum;						//4496 ͣ��������
                        u32 Term;
                    } PullWire;

                    union
                    {
                        u32	Data_32[20];
                        struct
                        {
                            u32 SecondSlowTinSpeed_A; 		//�ڶ���մ���ٶ� 	4500
                            u32 SecondSlowTinBackSpeed_A; 	//�ڶ���մ���˻��ٶ� 4502
                            u32 SecondSlowTinSpeed_B; 		//�ڶ���մ���ٶ� 	4504
                            u32 SecondSlowTinBackSpeed_B; 	//�ڶ���մ���˻��ٶ� 4506
                            s32 SecondRosinLength_A;		//4508 ���մ�������A
                            s32 SecondRosinLength_B;		//4510 ���մ�������B
                            s32 SecondSlowTinPos_A;			//4512 �����մ��λA
                            s32 SecondSlowTinPos_B;			//4514 �����մ��λB
                            s32 SecondTinSpeed_A;			//4516 ���մ���ٶ�A
                            s32 SecondTinSpeed_B;			//4518 ���մ���ٶ�B
                            s32 SecondTinLength_A;			//4520 ���մ�����A
                            s32 SecondTinLength_B;			//4522 ���մ�����B
                        } Data;
                    } SecondTin;  
                    
                    union
                    {
                        u32	Data_32[30];
                        struct
                        {
                            SelectSet SelectData[8];    //4540 ��������
                        } Data;
                    } Select;     
                    
                    union
                    {
                        u32	Data_32[20];
                        struct
                        {
                            s32 SheathedWireEn;         //������ʹ�ܿ���       4600
                            s32 StripCut; 		        //�����߰�Ƥλ��       4602
                            s32 StripCutOpen; 		    //�������е��ſ�λ��    4604
                            s32 FrontStripLength; 		//ǰ�˰��߳���          4606
                            s32 EndStripLength; 		//��˰��߳���          4608      
                            
                            s32 ChangeLength; 		    //���־���              4610
                            s32 EndChangePos; 		    //����λ��              4612
                            s32 StripCutBack; 		    //�����߰�Ƥ�˵�       4614
                            s32 StripSpeed;             //�����߰�Ƥ�ٶ�       4616
                            s32 CutBack;                //�ж��˵�λ��         4618
                        } Data;
                    }SheathedWire;                    
                    
                    union
                    {
                        u32	Data_32[10];
                        struct
                        {
                            s32 CheckPos_Move;          //4640 ������λ
                            s32 CheckPos_Front;         //4642 ���Yλ��
                            s32 CheckBackPos;           //4644 �����λ
                            s32 PressOn;               //4646
                            s32 PushOff;                //4648 ����ջ�
                            s32 EndPos_FrontMove;       //4650 ����λ�� 
                            s32 CheackDelay;            //4652 �Ӿ����ʱ��
                        } Data;
                    }Acheck;
                    
                    union
                    {
                        u32	Data_32[15];
                        struct
                        {
                            s32 TakePinPos;             //4660 ȡ��λ��
                            s32 FeedNum;                //4662 �Ͽ�����
                            s32 FeedBackPos;            //4664  �ƿ�ԭλ
                            s32 FeedPos[5];              //4666-  ����λ��1
                            s32 FeedStay;               //4676 �Ͽ�ʱ��
                            s32 PushDelay;              //4678 �ƿ�ʱ��
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