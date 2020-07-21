#ifndef __IODEFINE_H
#define __IODEFINE_H





typedef enum	//��Ŷ���
{
    PullWireAxis,					//	AXIS1	������        1568 1570
    FrontLesserAxis,				//	AXIS2	ǰ��(ǰ��)    1572 1574
    EndLesserAxis,					//	AXIS3	���(ǰ��)    1576 1578
    CutAxis,						//	AXIS4	�е���        1580 1582
    FrontPrimaryAxis,				//	AXIS5	ǰ��(����)    1584 1586
    EndPrimaryAxis,					//	AXIS6	���(��ת)    1588 1590
    TerminalAxis,                   //  AXIS7   ���ӻ���      1592 1594
    InsertTerminalAxis,             //  AXIS8   �����        1596 1598
    PinFeedAxis,                    //  AXIS9   �ͽ���        1600 1602
    StrepAxis,					    //	AXIS10  ������
    
    FrontFeedTinAxis,				//	AXIS11	ǰ��������
    EndFeedTinAxis,					//	AXIS12	���������
} AxisNum;



typedef enum	//����ڶ���
{
    X1,  X2, X3, X4, X5, X6, X7, X8, X9, X10, X11, X12, X13, X14, X15, X16, X17, X18, X19, X20, X21, X22, X23, X24, X25,	X26, X27, X28, X29, X30, X31, X32,
} InPutNum;

typedef enum	//����ڶ���
{
    Y1,  Y2, Y3, Y4, Y5, Y6, Y7, Y8, Y9, Y10, Y11, Y12, Y13, Y14, Y15, Y16, Y17, Y18, Y19, Y20, Y21, Y22, Y23, Y24, Y25,	Y26, Y27, Y28, Y29, Y30, Y31, Y32,
} OutNum;


#define LOCAL 0     //�������
#define CARD1 1     //��չ��1���
#define CARD2 2     //��չ��2���

//�������
#define	Q_ProductClam		LOCAL,Y1		//	Y1	�ų�����
#define	Q_PullWireClam		LOCAL,Y2 		//	Y2	���߼�
#define	Q_EndClam			LOCAL,Y3 		//	Y3	�����
#define	Q_FrontClam 		LOCAL,Y18 		//	Y4	ǰ�˼���
#define	Q_LittleClam		LOCAL,Y5 		//	Y5	���ϼ�
#define	Q_TwistDown			LOCAL,Y6		//	Y6	Ť������
#define	Q_Twist				LOCAL,Y7 		//	Y7	Ť��ǰ��
#define	Q_EndScrapeTini		LOCAL,Y8		//	Y8	��˹���
#define	Q_EndScrapeTiniUp	LOCAL,Y9		//	Y9	�������
#define	Q_BCutWastage		LOCAL,Y10		//	Y10	�з���
#define	Q_EndHot			LOCAL,Y11		//	Y11	��¯
#define	Q_FeedTerminal		LOCAL,Y12		//	Y12	�Ͷ���
#define	Q_FeedRosin	        LOCAL,Y13		//	Y13	������
#define	Q_PinBlow	        LOCAL,Y14		//	Y14	���Ǵ���
#define	Q_Blow				LOCAL,Y15		//	Y15	��������
#define	Q_Light     		LOCAL,Y16		//	Y16	������
#define	Q_VisionTrig		LOCAL,Y17		//	Y17	�Ӿ�����
#define	Q_FrontPress 		LOCAL,Y4		//	Y18	ǰ��ѹ��
#define	Q_FrontPush 		LOCAL,Y19		//	Y19	ǰ���Ƴ�
#define	Q_PinLocatUp 		LOCAL,Y20		//	Y20	���Ӷ�λ������
#define	Q_PinLocatDown 		LOCAL,Y21		//	Y21	���Ӷ�λ������
#define	Q_FeedPin 		    LOCAL,Y22		//	Y22	�Ͻ���
#define	Q_BadProduct 	    LOCAL,Y23		//	Y23	
#define	Q_PressRest			LOCAL,Y24		//	Y24	ѹ����λ
#define	Q_  				LOCAL,Y25		//	Y25	
#define	Q_FeedTin			LOCAL,Y26		//	Y26	�������



#define	Q_LittleFront		LOCAL,Y8 		//	Y8	ȡ�߼�
#define	Q_FrontScrapeTiniUp	LOCAL,Y10		//	Y10	ǰ������
#define	Q_FrontScrapeTini	LOCAL,Y11		//	Y11	ǰ�˹���
#define	Q_SeparateWireFront	LOCAL,Y19		//	Y19	����ǰ��
#define	Q_Y21				LOCAL,Y21		//	Y21	
#define	Q_SeparateWireDown	LOCAL,Y23		//	Y23	��������	
#define	Q_LittleRight		LOCAL,Y24		//	Y24	
#define	Q_FrontHot			LOCAL,Y25		//	Y25	

//��չ�����1
#define	Q_Green			    CARD1,Y1		//	Y1 	
#define	Q_Yellow			CARD1,Y2		//	Y2	
#define	Q_Red			    CARD1,Y3		//	Y3	
#define	Q_Ex1_4			    CARD1,Y4		//	Y4	
#define	Q_ServoReset	    CARD1,Y5		//	Y5	
#define	Q_Ex1_6			    CARD1,Y6		//	Y6	
#define	Q_Ex1_7			    CARD1,Y7		//	Y7	
#define	Q_Ex1_8			    CARD1,Y8		//	Y8	
#define	Q_Ex1_9			    CARD1,Y9		//	Y9	
#define	Q_Ex1_10			CARD1,Y10		//	Y10	
#define	Q_Ex1_11			CARD1,Y11		//	Y11	
#define	Q_Ex1_12			CARD1,Y12		//	Y12	
#define	Q_Ex1_13			CARD1,Y13		//	Y13	
#define	Q_Ex1_14			CARD1,Y14		//	Y14	
#define	Q_Buzzer			CARD1,Y15		//	Y15	������
#define	Q_CutBlow			    CARD1,Y16		//	Y16	��������



//���붨��
#define	I_AutoMode			    LOCAL,X10		//	X10���ӻ��ֶ�����
#define	I_TerinalUp			    LOCAL,X11		//	X11���ӻ�����	
#define	I_TerinalDown			LOCAL,X12		//	X12���ӻ�����

#define	I_EmergencyButton		LOCAL,X16		//	X13��ͣ��ť	
#define	I_StartButton			LOCAL,X17		//	X14��ʼ��ť	
#define	I_StepRun   			LOCAL,X18		//	X15����
#define	I_StopButton 			LOCAL,X19		//	X16ֹͣ��ť
#define	I_PinOFF            	LOCAL,X20		//	X20�������ԭ��
#define	I_PinON         		LOCAL,X21		//	X21������׵�λ
#define	I_PressCheck			LOCAL,X22		//	X22ѹ�����	
#define	I_VisionCheck			LOCAL,X23		//	X23�Ӿ����	

#define	I_PinCheck				LOCAL,X25		//	X25���Ǽ��
#define	I_PinCheck2				LOCAL,X26		//	X26���Ǽ��2

//#define	I_pressure				LOCAL,X17		//	X17��ѹ���
//#define	I_NoWire				LOCAL,X18		//	X18ȱ�߼��	
//#define	I_FeedNoTerminal	    LOCAL,X19		//	X19�Ͷ��Ӽ��
//#define	I_EndNoTin				LOCAL,X24		//	X24���ȱ��

//

//��չ���뿨
#define	I_TerminalCtrol			    CARD1,X1		//	X1	
#define	I_TerminalUp			    CARD1,X2		//	X2	
#define	I_TerminalDown			    CARD1,X3		//	X3	
#define	I_pressure			    	CARD1,X4		//	X4	
#define	I_NoWire			    	CARD1,X5		//	X5	
#define	I_FeedNoTerminal			CARD1,X6		//	X6	
#define	I_EndNoTin			    	CARD1,X7		//	X7	
#define	X_Ex1_8			    		CARD1,X8		//	X8	
#define	I_FrontNoTin			    CARD1,X9		//	X9	
#define	I_wireKnotCheck				CARD1,X10		//	X10	

#define	I_FrontTerminalCheck_B 		CARD1,X11		//	X19��˶��Ӽ��	
#define	I_TerinalUp_B				CARD1,X12		//	X20���ӻ��ϸ�Ӧ	
#define	I_LittleRight				CARD1,X13		//	X22�����Ҹ�Ӧ
#define	I_LittleFrontOff			CARD1,X14		//	X25����ǰ������˻ظ�Ӧ
#define	I_LittleFrontOn				CARD1,X15		//	X26����ǰ����������Ӧ





#endif

