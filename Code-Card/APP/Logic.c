/*
 * File: Logic.c
 * File Created: Tuesday, 9th October 2018 2:43:29 pm
 * version: v00.00.01 8��ײ�
 * Description:�û��߼�
 * Modified By:yang
 * -----
 * Copyright 2018 - 2018, <<hzzh>>
 */

#include "Logic.h"

/*ȫ�ֱ�����ͳһ����*/
Task LogicTask = {0}; //ͳһ��������
SysData Data = {0};   //ͳһ��������
int Change[2] = {Y21, Y22};
static int RealWireNum = 0;


void IntSpeed()//��ʼ���ٶ�
{
    u8 initflag = 1;
    u8 i;
    GUS.AMakeTerminal.MakeTerminalOffer = 0;
	GUW.CurrTimeSetForUser.hour = GUW.HourForHMI;
	GUW.CurrTimeSetForUser.min = GUW.minForHMI;
	GUW.CurrTimeSetForUser.sec = GUW.SecForHMI;
	GUW.CurrDateSetForUser.year = GUW.YearForHMI - 2000;
	GUW.CurrDateSetForUser.month = GUW.MonthForHMI;
	GUW.CurrDateSetForUser.day = GUW.DayForHMI;
    if(initflag == 1)
    {
        initflag = 0;
        for(i = 0; i < 12; i++)
        {
            GSS.AxisData[i].Data.HomeSpeedSlow = 700;
        }
    }
    if(GUS.ChangeMould > GUS.BFeedWire.EndMoveFeedPos)
    {
       GUS.ChangeMould = 0; //����ģ��
    }
}

/**
* @author 20/04/16  wangh
* @Description: ��������
* @param --
* @param --
* @return --
*/
void ParaSet()
{
    int i;
    //��������
    PARAINIT(GUS.WireDistanceOffer.Data.WireDistanceOffer);
    if(ATWoPinEn)
    {
        for(i = 1; i < 10; i++)
        {
            GUS.WireDistanceOffer.Data.WireDistanceOffer[2*i] = GUS.WirePara.Data.TwoPinDistance - GUS.WirePara.Data.WireDistance;
        }
        
        for(i = 0; i < 5; i++)
        {
            if(GUS.Select.Data.SelectData[i].Num > 1 && GUS.Select.Data.SelectData[i].Num < 20)
            {
                GUS.WireDistanceOffer.Data.WireDistanceOffer[GUS.Select.Data.SelectData[i].Num] = GUS.Select.Data.SelectData[i].Distance - GUS.WirePara.Data.WireDistance;
            }
        }
    }
    //��ǲ���
	GUS.AInsertPin.Data.PressOnDelay = GUS.FeedWire.FrontOnDelay;//ѹ�߿���ʱ=ǰ�˶��߼мн���ʱ
    GUS.AInsertPin.Data.PressOffDelay = GUS.FeedWire.FrontOffDelay;
    GUS.AInsertPin.Data.EndPos_Front = GUS.Acheck.Data.CheckPos_Front;
    GUS.AInsertPin.Data.EndPos_FrontMove = GUS.Acheck.Data.CheckPos_Move;//��ǵĺ��ƽ���λ��=�����ƿ�ʼ�ƶ���λ��
    
    //���
    GUS.Acheck.Data.EndPos_FrontMove = GUS.FeedWire.FrontRFeedPos;//��������λ��=ǰ�˺��Ƶ�����λ��
    GUS.Acheck.Data.PressOn = GUS.FeedWire.FrontOnDelay;//���ʱ��ǰ�Ƶ�ʱ���߼мн�
    
    //��������
    ResetWhichBitU16(&GUS.WirePara.Data.A.Para.FunctionEn,0);//ȱ�߼����0
    ResetWhichBitU16(&GUS.WirePara.Data.A.Para.FunctionEn,1);//���ģʽ��0
    ResetWhichBitU16(&GUS.WirePara.Data.A.Para.FunctionEn,2);//��ѹ�����0
    //��ѹ���
    SetWhichBitU16(&GUS.FunctionEn,2);
    
    if(GUS.WirePara.Data.B.Para.StripLength[singleWire] > GUS.WirePara.Data.B.Para.HalfStripLength)
    {
        SetWhichBitU16(&GUS.WirePara.Data.B.Para.FunctionEn,0);//�������
    }
    else
    {
        ResetWhichBitU16(&GUS.WirePara.Data.B.Para.FunctionEn,0);
    }
    
    //�¿ؿ���
    if(BTinWarmEn)
    {
       UserOutputSet(Q_EndHot,ON); 
    }
    else
    {
       UserOutputSet(Q_EndHot,OFF); 
    }
}

/**
* @author 19/02/18  wangh
* @Description: ����������,����������
* @param --
* @param --
* @return --
*/
void FeedWire(LogicParaDef *LG)
{
    INITTASK(LogicTask.FeedWire);
    switch (LG->step)
    {
    case 1: //��ת��ˮƽλ��
        if (UserGetStatus(FrontLesserAxis) == 0 && UserGetStatus(FrontPrimaryAxis) == 0 && UserGetStatus(EndPrimaryAxis) == 0 && UserGetStatus(EndLesserAxis) == 0 && UserGetStatus(PullWireAxis) == 0)
        {
            AxMoveAbsUserUnit(FrontPrimaryAxis, GUS.SpdRatio[FrontPrimaryAxis], GUS.FeedWire.FrontRFeedPos);
            AxMoveAbsUserUnit(CutAxis, GUS.SpdRatio[CutAxis], GUS.FeedWire.CutOpenPos);
            AxMoveAbsUserUnit(EndPrimaryAxis, GUS.SpdRatio[EndPrimaryAxis], GUS.FeedWire.EndRFeedPos);
			//�����߳�
			GUS.WirePara.Data.WrieLength = GUS.WirePara.Data.WrieLengthHMI;
			
			if(Data.BadWireEn == 1)
			{
				GUS.WirePara.Data.WrieLength = GUS.PullWire.BadTerminaLength;
				Data.BadWire = 1;
			}
			else if(Data.VisionCheck != 2 
				&& AVisionCheckEn && AMakeTerminalEn && AInsertPinEn && Data.TerminalErro == 0)
			{
				GUS.WirePara.Data.WrieLength = GUS.PullWire.BadcheckLength;
				Data.BadWire = 1;
			}
			Data.BadWireEn = 0;
			Data.VisionCheck = 0;
			
            LG->step = 2;
        }
        break;

    case 2: //������λ��
        if(UserGetOutput(Q_EndClam) == ON)
        {
            AlarmSetBit(2, 1, 1); //��˼����ſ�����رպ�����
        }
        if(UserGetStatus(EndLesserAxis) == 0 && UserGetStatus(FrontPrimaryAxis) == 0 && UserGetStatus(EndPrimaryAxis) == 0 && UserGetStatus(CutAxis) == 0 && UserGetOutput(Q_EndClam) == OFF)
        {
            LG->step = 3;
            if(FrontSeparateWireEN && ShortMode == 0 && SeparateWireMode == 0)
            {
                LogicTask.CylinderSeparateWire.execute = 1;
                LG->step = 7;
            }
            else
            {
                AxMoveAbsUserUnit(FrontLesserAxis, GUS.SpdRatio[FrontLesserAxis], GUS.FeedWire.FrontFeedPos);
            }

            if(ShortMode == 0)
            {
                AxMoveAbsUserUnit(EndLesserAxis, GUS.SpdRatio[EndLesserAxis], GUS.FeedWire.EndFeedPos);
                AxMoveAbsUserUnit(PullWireAxis, GUS.SpdRatio[PullWireAxis], GUS.FeedWire.PullFeedPos);
            }
            else
            {
                AxMoveAbsUserUnit(EndLesserAxis, GUS.SpdRatio[EndLesserAxis], GUS.FeedWire.EndAxisPullPos);
            }
        }
        break;

    case 3: //���߼мн�
        if (UserGetStatus(PullWireAxis) == 0 && UserGetStatus(FrontPrimaryAxis) == 0 && UserGetStatus(FrontLesserAxis) == 0 && UserGetStatus(EndPrimaryAxis) == 0 && UserGetStatus(EndLesserAxis) == 0)
        {
            if(ShortMode == 0)
            {
                UserOutputSet(Q_PullWireClam, ON);
            }
            else
            {
                UserOutputSet(Q_EndClam, ON);
            }
            LG->step = 4;
        }
        break;

    case 4: //���߼мн���ʱ
        if (TimerCnt(LogicTask.FeedWire) > GUS.FeedWire.PullWireClamOnDelay)
        {
            LG->step = 5;
        }
        break;

    case 5: //�򿪶��߼�
        if (UserGetStatus(FrontPrimaryAxis) == 0 && UserGetStatus(CutAxis) == 0 && UserGetStatus(EndPrimaryAxis) == 0 && UserGetStatus(EndLesserAxis) == 0)
        {
            UserOutputSet(Q_FrontClam, ON);
            LG->step = 6;
        }
        break;

    case 6: //ǰ�˶��߼���ʱ�����߳�
        if (TimerCnt(LogicTask.FeedWire) > GUS.FeedWire.FrontOnDelay)
        {
            if (FrontSeparateWireEN && ShortMode == 0 && SeparateWireMode)
            {
                LogicTask.SeparateWire.execute = 1;
            }
            else
            {
                AxMoveRelUserUnit(FrontLesserAxis, GUS.SpdRatio[FrontLesserAxis], -GUS.FeedWire.FeedFrontDistance);
                if(ShortMode == 0)
                {
                    LogicTask.PullWireTwoTimes.execute = 1;
                    Data.PullTwoTimesPos =  -(GUS.WirePara.Data.WrieLength - GUS.FeedWire.FeedFrontDistance - GUS.FeedWire.PullStraightLength);
                }
                else
                {
                    Data.PullTwoTimesPos =  -(GUS.WirePara.Data.WrieLength - GUS.FeedWire.FeedFrontDistance - GUS.FeedWire.PullStraightLength);
                    AxMoveRelUserUnit(EndLesserAxis, GUS.SpdRatio[EndLesserAxis], Data.PullTwoTimesPos);
                }
            }
            LG->step = 7;
        }
        break;


    case 7: //����
        if (UserGetStatus(FrontLesserAxis) == 0  && LogicTask.CylinderSeparateWire.execute == 0 && UserGetStatus(PullWireAxis) == 0 && LogicTask.SeparateWire.execute == 0 && LogicTask.PullWireTwoTimes.execute == 0)
        {
            UserOutputSet(Q_FrontClam, OFF);
            LG->step = 8;
        }
        break;

    case 8: //������
        if (TimerCnt(LogicTask.FeedWire) > GUS.FeedWire.FrontOffDelay)
        {
            if(ShortMode == 0)
            {
                AxMoveRelUserUnit(PullWireAxis, GUS.SpdRatio[PullWireAxis], -GUS.FeedWire.PullStraightLength);
            }
            else
            {
                AxMoveRelUserUnit(EndLesserAxis, GUS.SpdRatio[EndLesserAxis], -GUS.FeedWire.PullStraightLength);
            }
            LG->step = 9;
        }
        break;

    case 9: //��мн�
        if (UserGetStatus(PullWireAxis) == 0)
        {
            LG->step = 10;
        }
        break;

    case 10: //������ʱ
        if (TimerCnt(LogicTask.FeedWire) > 15)
        {
            UserOutputSet(Q_EndClam, ON);
            LG->step = 11;
        }
        break;
        
		
	case 11: //
		if (TimerCnt(LogicTask.FeedWire) > GUS.B_Tin.B_ClamOnDelay)
		{
            if(StrepEn)
            {
                AxMoveRelUserUnit(StrepAxis, GUS.SpdRatio[StrepAxis], ((int)((GUS.Unfeed.StrepNum*GUS.WirePara.Data.WrieLength)/1000))*360);
            }
			LG->step = 12;
		}
		break;

    case 12:
		if (UserGetStatus(StrepAxis) == 0)
		{
            LG->step = ENDCASE;
		}
        break;         
		
	case ENDCASE: //
		{
			LG->step = 0;
			LG->execute = 0;
		}
		break;

    default:
        break;
    }
    if (LG->step != LG->stepBuffer)
    {
        LG->stepBuffer = LG->step;
        TimerRst(LogicTask.FeedWire);
    }
}

/**
* @author 19/02/18  wangh
* @Description: ����������
* @param --
* @param --
* @return --
*/
void FirstFeedWire()
{
//	INITTASK(LogicTask.FirstFeedWire);
//	switch (LogicTask.FirstFeedWire.step)
//	{
//	case 1: //������λ��
//		if (UserGetStatus(FrontPrimaryAxis) == 0 && UserGetStatus(EndPrimaryAxis) == 0 && UserGetStatus(EndLesserAxis) == 0)
//		{
//			AxMoveAbsUserUnit(FrontPrimaryAxis, GUS.SpdRatio[FrontPrimaryAxis], GUS.FeedWire.FrontMoveFeedPos);

//			AxMoveAbsUserUnit(EndPrimaryAxis, GUS.SpdRatio[EndPrimaryAxis], GUS.FeedWire.EndRFeedPos);
//			AxMoveAbsUserUnit(EndLesserAxis, GUS.SpdRatio[EndLesserAxis], GUS.FeedWire.EndFeedPos - GUS.WirePara.Data.B.Para.StripLength[singleWire]);
//			LogicTask.FirstFeedWire.step = 2;
//		}
//		break;

//	case 2: //���������
//		if (UserGetStatus(FrontPrimaryAxis) == 0 && UserGetStatus(CutAxis) == 0 && UserGetStatus(EndPrimaryAxis) == 0 && UserGetStatus(EndLesserAxis) == 0)
//		{
//			UserOutputSet(Q_EndClam, OFF);
//			LogicTask.FirstFeedWire.step = 3;
//		}
//		break;

//	case 3: //������ʱ
//		if (TimerCnt(LogicTask.FirstFeedWire) > GUS.B_Tin.B_ClamOffDelay)
//		{
//			LogicTask.FirstFeedWire.step = 4;
//		}
//		break;

//	case 4: //����
//		if (UserGetStatus(FeedWireAxis) == 0)
//		{
//			if (FrontSeparateWireEN == 1)
//			{
//				LogicTask.SeparateWire.execute = 1;
//			}
//			else
//			{
//				AxMoveRelUserUnit(FeedWireAxis, GUS.SpdRatio[FeedWireAxis], GUS.SperatePara.A_separateWireLength + GUS.SperatePara.A_separateWireLength + GUS.SperatePara.cutterSeparateDistance + 500);
//			}
//			LogicTask.FirstFeedWire.step = 5;
//		}
//		break;

//	case 5: //����
//		if (UserGetStatus(FeedWireAxis) == 0 && LogicTask.SeparateWire.execute == 0)
//		{
//			UserOutputSet(Q_EndClam, ON);
//			LogicTask.FirstFeedWire.step = 6;
//		}
//		break;

//	case 6: //������ʱ
//		if (TimerCnt(LogicTask.FeedWire) > GUS.B_Tin.B_ClamOnDelay)
//		{
//			LogicTask.FirstFeedWire.step = ENDCASE;
//		}
//		break;

//	case ENDCASE: //
//		if (1)
//		{
//			LogicTask.FirstFeedWire.step = 0;
//			LogicTask.FirstFeedWire.execute = 0;
//		}
//		break;

//	default:
//		break;
//	}
//	if (LogicTask.FirstFeedWire.step != LogicTask.FirstFeedWire.stepBuffer)
//	{
//		LogicTask.FirstFeedWire.stepBuffer = LogicTask.FirstFeedWire.step;
//		TimerRst(LogicTask.FeedWire);
//	}
}



/**
* @author 19/08/27  wangh
* @Description: ���׷���
* @param --
* @param --
* @return --
*/
void CylinderSeparateWire(LogicParaDef *LG)
{
    static s32 PullFirstDistance = 0;
    static s32 PullSecendDistance = 0;
    static s32 FlagDistance = 0;
    
    FlagDistance = Remainder(GUS.SperatePara.cutterSeparateDistance, GUS.WirePara.Data.WrieLength);
    if(FlagDistance < (GUS.WirePara.Data.WrieLength - GUS.SperatePara.A_separateWireLength))
    {
        PullFirstDistance = GUS.WirePara.Data.WrieLength - GUS.SperatePara.A_separateWireLength - FlagDistance;
    }
    else
    {
        PullFirstDistance = GUS.WirePara.Data.WrieLength + GUS.WirePara.Data.WrieLength - GUS.SperatePara.A_separateWireLength - FlagDistance;
    }
    Data.CylinderSeparateSecond = 0;
    
    
    
    if(PullFirstDistance < GUS.FeedWire.FeedFrontDistance)
    {
        if(FrontSeparateWireEN && ShortMode == 0 && SeparateWireMode == 0)
        {
            Data.CylinderSeparateSecond = GUS.FeedWire.FeedFrontDistance - PullFirstDistance;
        }
    }

    if (LG->step == 0 && LG->execute == 1)
    {
        LG->step = 1;
        LG->done = 0;
        LG->count = 0;
    }

    switch (LG->step)
    {
    case 1://������������ߵ����˾���
        FlagDistance = Remainder(GUS.SperatePara.cutterSeparateDistance, GUS.WirePara.Data.WrieLength);
        if(FlagDistance < (GUS.WirePara.Data.WrieLength - GUS.SperatePara.A_separateWireLength))
        {
            PullFirstDistance = GUS.WirePara.Data.WrieLength - GUS.SperatePara.A_separateWireLength - FlagDistance;
        }
        else
        {
            PullFirstDistance = GUS.WirePara.Data.WrieLength + GUS.WirePara.Data.WrieLength - GUS.SperatePara.A_separateWireLength - FlagDistance;
        }
        PullSecendDistance = GUS.WirePara.Data.WrieLength - PullFirstDistance;
        LG->step = 2;
        break;

    case 2://������һ�γ���
        if (UserGetStatus(PullWireAxis) == 0 && LogicTask.PullWireTwoTimes.execute == 0)
        {
            if(PullFirstDistance > GUS.FeedWire.FeedFrontDistance)
            {
                AxMoveAbsUserUnit(FrontLesserAxis, GUS.SpdRatio[FrontLesserAxis], GUS.FeedWire.FrontFeedPos);
                LG->step = 9;
            }
            else
            {
                AxMoveAbsUserUnit(FrontLesserAxis, GUS.SpdRatio[FrontLesserAxis], GUS.FeedWire.FrontFeedPos - Data.CylinderSeparateSecond);
                 LG->step = 3;
           }
        }
        break;
        
    case 9: //���߼мн�
        if (UserGetStatus(PullWireAxis) == 0 && UserGetStatus(FrontPrimaryAxis) == 0 && UserGetStatus(FrontLesserAxis) == 0 && UserGetStatus(EndPrimaryAxis) == 0 && UserGetStatus(EndLesserAxis) == 0)
        {
            UserOutputSet(Q_PullWireClam, ON);
            LG->step = 10;
        }
        break;

    case 10: //���߼мн���ʱ
        if (TimerCnt(LogicTask.CylinderSeparateWire) > GUS.FeedWire.PullWireClamOnDelay)
        {
            LG->step = 11;
        }
        break;

    case 11: //�򿪶��߼�
        if (UserGetStatus(FrontPrimaryAxis) == 0 && UserGetStatus(CutAxis) == 0 && UserGetStatus(EndPrimaryAxis) == 0 && UserGetStatus(EndLesserAxis) == 0)
        {
            UserOutputSet(Q_FrontClam, ON);
            LG->step = 12;
        }
        break;
        
    case 12: //ǰ�˶��߼���ʱ�����߳�
        if(TimerCnt(LogicTask.CylinderSeparateWire) > GUS.FeedWire.FrontOnDelay)
        {
            if(PullFirstDistance > GUS.FeedWire.FeedFrontDistance)
            {
                LogicTask.PullWireTwoTimes.execute = 1;
                Data.PullTwoTimesPos =  -(PullFirstDistance - GUS.FeedWire.FeedFrontDistance);
                LG->step = 3;
            }
            else
            {
                LG->step = 8;
            }
        }
        break;
        


    case 3://����ģ��
        if (UserGetStatus(PullWireAxis) == 0 && UserGetStatus(FrontLesserAxis) == 0 && LogicTask.PullWireTwoTimes.execute == 0)
        {
            UserOutputSet(Q_SeparateWireDown, ON);
            LG->step = 4;
        }
        break;

    case 4://����ģ����
        if(TimerCnt(LogicTask.CylinderSeparateWire) > GUS.SperatePara.Down_ON_Delay)
        {
            UserOutputSet(Q_SeparateWireFront, ON);
            LG->step = 5;
        }
        break;

    case 5://����ģ̧��
        if(TimerCnt(LogicTask.CylinderSeparateWire) > GUS.SperatePara.Front_ON_Delay)
        {
            UserOutputSet(Q_SeparateWireDown, OFF);
            LG->step = 6;
        }
        break;

    case 6://����ģ���ڸ�λ
        if(TimerCnt(LogicTask.CylinderSeparateWire) > GUS.SperatePara.Down_OFF_Delay)
        {
            UserOutputSet(Q_SeparateWireFront, OFF);
            LG->step = 7;
        }
        break;

    case 7://����ģ���ڶ��γ���
        if (UserGetStatus(PullWireAxis) == 0 && LogicTask.PullWireTwoTimes.execute == 0)
        {
            if(PullFirstDistance > GUS.FeedWire.FeedFrontDistance)
            {
                LG->step = 8;
            }
            else
            {
                AxMoveAbsUserUnit(FrontLesserAxis, GUS.SpdRatio[FrontLesserAxis], GUS.FeedWire.FrontFeedPos);
                LG->step = 9;
            }
            
        }
        break;

    case 8://�ڶ���
        if (UserGetStatus(PullWireAxis) == 0 && LogicTask.PullWireTwoTimes.execute == 0)
        {
            LogicTask.PullWireTwoTimes.execute = 1;
            Data.PullTwoTimesPos =  -PullSecendDistance;
            AxMoveRelUserUnit(FrontLesserAxis, GUS.SpdRatio[FrontLesserAxis], -GUS.FeedWire.FeedFrontDistance);
            LG->step = ENDCASE;
        }
        break;

    case ENDCASE://
        if (UserGetStatus(PullWireAxis) == 0 && UserGetStatus(FrontLesserAxis) == 0 && LogicTask.PullWireTwoTimes.execute == 0)
        {
            LG->execute = 0;
            LG->step = 0;
            LG->done = 1;
        }
        break;


    }
    if (LG->step != LG->stepBuffer)
    {
        LG->stepBuffer = LG->step;
        TimerRst(LogicTask.CylinderSeparateWire);
    }
}


/**
* @author 19/02/18  wangh
* @Description: ����
* @param --
* @param --
* @return --
*/
void SeparateWire()
{
    /************************************
    �߳�<���ߵ����е���ľ���
    ��һ�����߳�-���߳�-���ߵ��е������߳�ȡ�ࣩ
    ************************************/

    unsigned int i;
    static s32 L_cutDistance;	  //�жϾ���
    static s32 L_separateDistance; //���߾���

    //��������
    static s32 wireLength;			   //���߳���
    static s32 A_separateWireLength;   //A�˷��߳���
    static s32 B_separateWireLength;   //B�˷��߳���
    static s32 cutterSeparateDistance; //���ߵ����е��ļ��
//	if(GUW.HadToResset == 0)
//	{
//		wireLength = GUS.SperatePara.A_separateWireLength + GUS.SperatePara.B_separateWireLength + GUS.SperatePara.cutterSeparateDistance + 300;
//	}
//	else
//	{
    wireLength = GUS.WirePara.Data.WrieLength;
//	}
    A_separateWireLength = GUS.SperatePara.A_separateWireLength - 50;
    B_separateWireLength = GUS.SperatePara.B_separateWireLength - 50;

    if (LogicTask.SeparateWire.execute == 1 && LogicTask.SeparateWire.step == 0)
    {
        LogicTask.SeparateWire.step = 1;
    }

    switch (LogicTask.SeparateWire.step)
    {
    case 1:
        //���߼е�����λ��
        //�жϾ��� = ���߳���
        cutterSeparateDistance = GUS.SperatePara.cutterSeparateDistance;
        L_cutDistance = wireLength - GUS.FeedWire.FeedFrontDistance;
        //���߾��� = �����߳��� - B�˷��߳��ȣ� - ���ߵ����е��ľ���//
        L_separateDistance = (wireLength - B_separateWireLength - GUS.FeedWire.FeedFrontDistance) - (cutterSeparateDistance);
        i = 1;
        while (L_separateDistance < 0 && abs(L_separateDistance) > (A_separateWireLength + B_separateWireLength))
        {
            L_separateDistance = (i * wireLength - B_separateWireLength - GUS.FeedWire.FeedFrontDistance) - cutterSeparateDistance;
            L_separateDistance = (i * wireLength - B_separateWireLength) - cutterSeparateDistance;
            i++;
        }
        //�жϷ��ߵ���λ��
        if (L_separateDistance <= 0)
        {
            //�����ߵ��ڷ��߷�Χ�ڣ�������߽���λ�õ����ߵ��ľ��룬���ߵ���ѹ
            L_separateDistance = A_separateWireLength + B_separateWireLength + L_separateDistance;
            UserOutputSet(Q_SeparateWireDown, ON);
            UserTimer[5] = GUS.SperatePara.Down_ON_Delay;
        }
        else
        {
            //���ߵ����ڷ��߷�Χ�ڣ����ߵ�̧��
            UserOutputSet(Q_SeparateWireDown, OFF);
            UserTimer[5] = GUS.SperatePara.Down_OFF_Delay;
        }
        LogicTask.SeparateWire.step = 2;
        break;

    case 2:
        if (UserTimer[5] == 0)
        {

            //�ж����߷��߾��뻹�������е�����
            if (L_cutDistance > L_separateDistance)
            {
//				AxisRunDefaultUserUnit(PullWireAxis, RELMODE, GUS.SperatePara.sperateSpeed, -L_separateDistance);
                LogicTask.PullWireTwoTimes.execute = 1;
                Data.PullTwoTimesPos = -L_separateDistance;

                L_cutDistance -= L_separateDistance;
                L_separateDistance = 0;
                LogicTask.SeparateWire.step = 3;
            }
            else
            {
//				AxisRunDefaultUserUnit(PullWireAxis, RELMODE, GUS.FeedWire.PullSpeed, -L_cutDistance);
                LogicTask.PullWireTwoTimes.execute = 1;
                Data.PullTwoTimesPos = -L_cutDistance;

                L_separateDistance -= L_cutDistance;
                L_cutDistance = 0;
                ton_et[0] = 0;
                LogicTask.SeparateWire.step = 10;
            }
        }
        break;

    case 3:
        if (UserGetStatus(PullWireAxis) == 0 && LogicTask.PullWireTwoTimes.execute == 0)
        {
            if (UserGetOutput(Q_SeparateWireDown) == ON)
            {
                UserOutputSet(Q_SeparateWireDown, OFF);
                UserTimer[5] = GUS.SperatePara.Down_OFF_Delay;
            }
            else
            {
                UserOutputSet(Q_SeparateWireDown, ON);
                UserTimer[5] = GUS.SperatePara.Down_ON_Delay;
            }
            LogicTask.SeparateWire.step = 4;
            ton_et[0] = 0;
        }
        break;

    case 4:
        if (UserTimer[5] == 0)
        {
            if (UserGetOutput(Q_SeparateWireDown) == ON)
            {
                L_separateDistance = A_separateWireLength + B_separateWireLength;
            }
            else
            {
                L_separateDistance = wireLength - A_separateWireLength - B_separateWireLength;
            }
            LogicTask.SeparateWire.step = 2;
        }
        break;

    /********************���߲���*********************************/
    case 10:
        if (UserGetStatus(PullWireAxis) == 0)
        {
            //������ߵ����е��ļ��
            cutterSeparateDistance = GUS.SperatePara.cutterSeparateDistance - GUS.FeedWire.EndFeedPos;
            //���߾��� = �����߳��� - B�˷��߳��ȣ� - ���ߵ����е��ľ���
            L_separateDistance = (wireLength - B_separateWireLength) - cutterSeparateDistance;
            i = 1;
            while(L_separateDistance < 0 && abs(L_separateDistance) > (A_separateWireLength + B_separateWireLength))
            {
                L_separateDistance = (i * wireLength - B_separateWireLength) - cutterSeparateDistance;
                i++;
            }
            //�жϷ��ߵ���λ��
            if(L_separateDistance <= 0)
            {
                //�����ߵ��ڷ��߷�Χ�ڣ�������߽���λ�õ����ߵ��ľ��룬���ߵ���ѹ
                L_separateDistance = A_separateWireLength + B_separateWireLength + L_separateDistance;
                UserOutputSet(Q_SeparateWireDown, ON);
                UserTimer[5] = GUS.SperatePara.Down_ON_Delay;
            }
            else
            {
                //���ߵ����ڷ��߷�Χ�ڣ����ߵ�̧��
                UserOutputSet(Q_SeparateWireDown, OFF);
                UserTimer[5] = GUS.SperatePara.Down_OFF_Delay;
            }
            ton_et[0] = 0;
            LogicTask.SeparateWire.step = 0xA0;
        }
        break;

    case 0xA0:
        if(UserTimer[5] == 0)
        {
            if(GUS.FeedWire.FeedFrontDistance > L_separateDistance)
            {
                AxisRunDefaultUserUnit(FrontLesserAxis, RELMODE, GUS.SpdRatio[FrontLesserAxis], -L_separateDistance);
                L_separateDistance =
                    LogicTask.SeparateWire.step = 0xA1;
            }
            else
            {
                AxisRunDefaultUserUnit(FrontLesserAxis, RELMODE, GUS.SpdRatio[FrontLesserAxis], -GUS.FeedWire.FeedFrontDistance);
                LogicTask.SeparateWire.step = 0xA3;
            }
        }
        break;

    case 0xA1:
        if(UserGetStatus(FrontLesserAxis) == 0)
        {
            if (UserGetOutput(Q_SeparateWireDown) == OFF)
            {
                UserOutputSet(Q_SeparateWireDown, ON);
                UserTimer[5] = GUS.SperatePara.Down_ON_Delay;
            }
            else
            {
                UserOutputSet(Q_SeparateWireDown, OFF);
                UserTimer[5] = GUS.SperatePara.Down_OFF_Delay;
            }
            LogicTask.SeparateWire.step = 0xA2;
            ton_et[0] = 0;
        }
        break;

    case 0xA2:
        if(UserTimer[5] == 0)
        {
            AxisRunDefaultUserUnit(FrontLesserAxis, RELMODE, GUS.SpdRatio[FrontLesserAxis], -(GUS.FeedWire.FeedFrontDistance - L_separateDistance));
            LogicTask.SeparateWire.step = 0xA3;
        }
        break;

    case 0xA3:
        if(UserGetStatus(FrontLesserAxis) == 0)
        {

            LogicTask.SeparateWire.step = 5;
            ton_et[0] = 0;
        }
        break;

    case 5:
        //if(ton(0,AxisPara[PullWireAxis].execute == 0,GUS.SperateDelay))
    {
        LogicTask.SeparateWire.execute = 0;
        LogicTask.SeparateWire.step = 0;
        LogicTask.SeparateWire.done = 1;
    }
    break;
    }
}

/**
* @author   20/03/09 ����
* @Description: �����߰��߹���
* @param --
* @param --
* @return --
*/
void sheathedWire(LogicParaDef *LG)
{
    if(GUS.SheathedWire.Data.SheathedWireEn == -1)
    {//Ĭ�Ϲرտ���
        GUS.SheathedWire.Data.SheathedWireEn = 0;
    }
    
    //���ڳ��ȣ����ܴ����е��������ľ���
    if(GUS.SheathedWire.Data.FrontStripLength > GUS.CutWirePara.A_CutDistance)
    {
        GUS.SheathedWire.Data.FrontStripLength = GUS.CutWirePara.A_CutDistance;
    }
    //���ڳ��ȣ����ܴ����е��������ľ���
    if(GUS.SheathedWire.Data.EndStripLength > GUS.CutWirePara.B_CutDistance)
    {
        GUS.SheathedWire.Data.EndStripLength = GUS.CutWirePara.B_CutDistance;
    }
    if(sheathedWireEn)
    {//��Ƥ��Ƥ��ҪС����Ƥ��Ƥ
        if (GUS.WirePara.Data.A.Para.StripLength[singleWire] > GUS.SheathedWire.Data.FrontStripLength)
        {
            GUS.WirePara.Data.A.Para.StripLength[singleWire] = GUS.SheathedWire.Data.FrontStripLength;
        }
        if (GUS.WirePara.Data.B.Para.StripLength[singleWire] > GUS.SheathedWire.Data.EndStripLength)
        {
            GUS.WirePara.Data.B.Para.StripLength[singleWire] = GUS.SheathedWire.Data.EndStripLength;
        }
    }
    if(sheathedChangeEn)
    {//���ֳ�����ҪС����Ƥ��Ƥ����
        if(GUS.SheathedWire.Data.ChangeLength > (GUS.SheathedWire.Data.EndStripLength - GUS.WirePara.Data.B.Para.StripLength[singleWire]))
        {
            GUS.SheathedWire.Data.ChangeLength = GUS.SheathedWire.Data.EndStripLength - GUS.WirePara.Data.B.Para.StripLength[singleWire];
        }
    }
    
    INITTASK(LogicTask.sheathedWire);
    switch (LG->step)
    {
    case 1://ǰ��˵������ߵİ��߳���
        if (UserGetStatus(FrontLesserAxis) == 0 && UserGetStatus(PullWireAxis) == 0 && UserGetStatus(EndLesserAxis) == 0 && UserGetStatus(CutAxis) == 0)
        {
            //ǰ����ֱ����
            //30-(x-2.5)
            AxMoveRelUserUnit(FrontLesserAxis, GUS.SpdRatio[FrontLesserAxis], -(GUS.CutWirePara.A_CutDistance - GUS.SheathedWire.Data.FrontStripLength));
            AxMoveRelUserUnit(EndLesserAxis, GUS.SpdRatio[EndLesserAxis], -(GUS.CutWirePara.B_CutDistance - GUS.SheathedWire.Data.EndStripLength));
            if((BHelpClamEN != 0 && ShortMode == 0 ) || sheathedChangeEn)
            {
                TwoAxMoveRelUserUnit(PullWireAxis, EndLesserAxis, GUS.SpdRatio[EndLesserAxis], -(GUS.CutWirePara.B_CutDistance - GUS.SheathedWire.Data.EndStripLength));
            }
            LG->step = 2;
        }
        break;
    
    case 2://�е��������ߵ�ֵ
        if (UserGetStatus(FrontLesserAxis) == 0 && UserGetStatus(PullWireAxis) == 0 && UserGetStatus(EndLesserAxis) == 0 && UserGetStatus(CutAxis) == 0)
        {
            AxMoveAbsUserUnit(CutAxis, GUS.SpdRatio[CutAxis], GUS.SheathedWire.Data.StripCut);
            LG->step = 3;
            if(sheathedChangeEn)
            {
                LG->step = 12;
            }
        }
        break; 

    case 12: //�ж��е���λ����ʱ����
        if (UserGetStatus(CutAxis) == 0)
        {
            LG->step = 13;
        }
        break;

    case 13: //�˵�
        if (TimerCnt(LogicTask.sheathedWire) > AxisDelay)
        {
            AxMoveRelUserUnit(CutAxis, GUS.SpdRatio[CutAxis], -GUS.SheathedWire.Data.StripCutBack);
            LG->step = 6;
        }
        break;


/***************************************************************����********************************************************************************/        
        
    case 6://��˼����ſ�
        if(UserGetStatus(FrontLesserAxis) == 0 && UserGetStatus(PullWireAxis) == 0 && UserGetStatus(EndLesserAxis) == 0)
        {
            UserOutputSet(Q_EndClam, OFF);
            LG->step = 7;
        }
        break;  
        
    case 7://��˵�����λ��
        if (TimerCnt(LogicTask.sheathedWire) > GUS.B_Tin.B_ClamOnDelay)
        {
            AxMoveAbsUserUnit(EndLesserAxis, GUS.SpdRatio[EndLesserAxis], GUS.SheathedWire.Data.EndChangePos);
            LG->step = 8;
        }
        break;    
        
    case 8://��˵�����λ��
        if(UserGetStatus(FrontLesserAxis) == 0 && UserGetStatus(PullWireAxis) == 0 && UserGetStatus(EndLesserAxis) == 0 && UserGetStatus(CutAxis) == 0)
        {
//            AxMoveRelUserUnit(EndLesserAxis, GUS.SpdRatio[EndLesserAxis], -(GUS.SheathedWire.Data.ChangeLength - GUS.FeedWire.PullStraightLength));
            AxMoveRelUserUnit(PullWireAxis, GUS.SheathedWire.Data.StripSpeed, -GUS.SheathedWire.Data.ChangeLength);
//            TwoAxMoveRelUserUnit(PullWireAxis, EndLesserAxis, GUS.SpdRatio[EndLesserAxis], -GUS.SheathedWire.Data.ChangeLength);
            LG->step = 9;
        }
        break;  
        
        
    case 9://��˼мн�
        if(UserGetStatus(FrontLesserAxis) == 0 && UserGetStatus(PullWireAxis) == 0 && UserGetStatus(EndLesserAxis) == 0)
        {
            UserOutputSet(Q_EndClam, ON);
            LG->step = 10;
        }
        break;  
        
    case 10://�������ɿ�
        if (TimerCnt(LogicTask.sheathedWire) > GUS.B_Tin.B_ClamOnDelay)
        {
            if(BHelpClamEN != 0 && ShortMode == 0 )
            {
                LG->step = 3;
            }
            else
            {               
                UserOutputSet(Q_PullWireClam, OFF); //�������ſ�
                LG->step = 11;
            }
        }
        break;     
        
    case 11://��������˱���
        if (TimerCnt(LogicTask.sheathedWire) > GUS.CutWirePara.PullWireClamOffDelay)
        {
            AxMoveAbsUserUnit(PullWireAxis, GUS.SpdRatio[PullWireAxis],
                              MaxS32(0, GUR.AxisPositionUserUnit[PullWireAxis] - GUS.CutWirePara.PullBackLength));
            LG->step = 3;
        }
        break; 
        
        
        
/**************************************************************************************************************************************************/        
    
    case 3://�����߰���
        if(UserGetStatus(CutAxis) == 0)
        {
           LG->step = 5;
            if(AHalfStripEn)
            {
                AxMoveRelUserUnit(FrontLesserAxis, GUS.SheathedWire.Data.StripSpeed, -GUS.SheathedWire.Data.FrontStripLength - 100);
                LG->step = 4;
           }
            else
            {
                AxMoveAbsUserUnit(FrontLesserAxis, GUS.SheathedWire.Data.StripSpeed, (GUS.FeedWire.FrontFeedPos - GUS.FeedWire.FeedFrontDistance) -(GUS.CutWirePara.A_CutDistance - GUS.WirePara.Data.A.Para.StripLength[singleWire]));
            }
            if(sheathedChangeEn)
            {
                if(BHalfStripEn)
                {
                    LogicTask.Blow.execute = 1; // ����
                    AxMoveRelUserUnit(EndLesserAxis, GUS.SheathedWire.Data.StripSpeed, -(GUS.SheathedWire.Data.EndStripLength-GUS.SheathedWire.Data.ChangeLength)- 100);
                    if(BHelpClamEN != 0 && ShortMode == 0 )
                    {
                        TwoAxMoveRelUserUnit(PullWireAxis, EndLesserAxis, GUS.SheathedWire.Data.StripSpeed - 1, -(GUS.SheathedWire.Data.EndStripLength - GUS.SheathedWire.Data.ChangeLength) - 100);
                    }
                   LG->step = 4;
                }
                else
                {
                    AxMoveAbsUserUnit(EndLesserAxis, GUS.SheathedWire.Data.StripSpeed,GUS.SheathedWire.Data.EndChangePos -(GUS.SheathedWire.Data.EndStripLength - GUS.SheathedWire.Data.ChangeLength - GUS.WirePara.Data.B.Para.StripLength[singleWire]));
                    if(BHelpClamEN != 0 && ShortMode == 0 )
                    {
                        TwoAxMoveRelUserUnit(PullWireAxis, EndLesserAxis, GUS.SheathedWire.Data.StripSpeed - 1,-(GUS.SheathedWire.Data.EndStripLength - GUS.SheathedWire.Data.ChangeLength - GUS.WirePara.Data.B.Para.StripLength[singleWire]));
                    }
                }
            }
            else
            {
                if(BHalfStripEn)
                {
                    LogicTask.Blow.execute = 1; // ����
                    AxMoveRelUserUnit(EndLesserAxis, GUS.SheathedWire.Data.StripSpeed, -GUS.SheathedWire.Data.EndStripLength - 100);
                    if(BHelpClamEN != 0 && ShortMode == 0 )
                    {
                        TwoAxMoveRelUserUnit(PullWireAxis, EndLesserAxis, GUS.SheathedWire.Data.StripSpeed - 1, -GUS.SheathedWire.Data.EndStripLength - 100);
                    }
                   LG->step = 4;
                }
                else
                {
                    AxMoveAbsUserUnit(EndLesserAxis, GUS.SheathedWire.Data.StripSpeed, GUS.FeedWire.EndFeedPos - (GUS.CutWirePara.B_CutDistance - GUS.WirePara.Data.B.Para.StripLength[singleWire]));
                    if(BHelpClamEN != 0 && ShortMode == 0 )
                    {
                        TwoAxMoveRelUserUnit(PullWireAxis, EndLesserAxis, GUS.SheathedWire.Data.StripSpeed - 1,GUR.AxisPositionUserUnit[EndLesserAxis] - (
                       GUS.SheathedWire.Data.EndChangePos -(GUS.SheathedWire.Data.EndStripLength - GUS.SheathedWire.Data.ChangeLength - GUS.WirePara.Data.B.Para.StripLength[singleWire])));
                    }
                }
            }
        }
        break;   
    
    case 4://�е��ſ�
        if (UserGetStatus(FrontLesserAxis) == 0 && UserGetStatus(PullWireAxis) == 0 && UserGetStatus(EndLesserAxis) == 0 && UserGetStatus(CutAxis) == 0)
        {
            LogicTask.ShakeWire.execute = 1;
            AxMoveAbsUserUnit(CutAxis, GUS.SpdRatio[CutAxis], GUS.SheathedWire.Data.StripCutOpen);
            LG->step = 5;
        }
        break; 
    
    case 5://ǰ����о���߳���
        if (UserGetStatus(FrontLesserAxis) == 0 && UserGetStatus(PullWireAxis) == 0 && UserGetStatus(EndLesserAxis) == 0 && UserGetStatus(CutAxis) == 0 &&  LogicTask.ShakeWire.execute == 0)
        {//��о��Ƥλ��=ȡ��λ�� - ������ - ���߳��ȣ�
            AxMoveAbsUserUnit(FrontLesserAxis, GUS.SpdRatio[FrontLesserAxis], (GUS.FeedWire.FrontFeedPos - GUS.FeedWire.FeedFrontDistance) -(GUS.CutWirePara.A_CutDistance - GUS.WirePara.Data.A.Para.StripLength[singleWire]));
            if(sheathedChangeEn)
            {
                AxMoveAbsUserUnit(EndLesserAxis, GUS.SpdRatio[EndLesserAxis],GUS.SheathedWire.Data.EndChangePos -(GUS.SheathedWire.Data.EndStripLength - GUS.SheathedWire.Data.ChangeLength - GUS.WirePara.Data.B.Para.StripLength[singleWire]));
                if(BHelpClamEN != 0 && ShortMode == 0 )
                {
                    TwoAxMoveRelUserUnit(PullWireAxis, EndLesserAxis, GUS.SpdRatio[EndLesserAxis] - 1,GUR.AxisPositionUserUnit[EndLesserAxis] - (
                   GUS.SheathedWire.Data.EndChangePos -(GUS.SheathedWire.Data.EndStripLength - GUS.SheathedWire.Data.ChangeLength - GUS.WirePara.Data.B.Para.StripLength[singleWire])));
                }
            }
            else
            {
                AxMoveAbsUserUnit(EndLesserAxis, GUS.SpdRatio[EndLesserAxis], GUS.FeedWire.EndFeedPos - (GUS.CutWirePara.B_CutDistance - GUS.WirePara.Data.B.Para.StripLength[singleWire]));
                if(BHelpClamEN != 0 && ShortMode == 0 )
                {
                    TwoAxMoveRelUserUnit(PullWireAxis, EndLesserAxis, GUS.SpdRatio[EndLesserAxis] - 1,GUR.AxisPositionUserUnit[EndLesserAxis] - (
                   GUS.FeedWire.EndFeedPos - (GUS.CutWirePara.B_CutDistance - GUS.WirePara.Data.B.Para.StripLength[singleWire])));
                }
            }
            LG->step = ENDCASE;
        }
        break;  
    
    case ENDCASE://����
        if (UserGetStatus(FrontLesserAxis) == 0 && UserGetStatus(PullWireAxis) == 0 && UserGetStatus(EndLesserAxis) == 0 && UserGetStatus(CutAxis) == 0)
        {
            LG->execute = 0;
            LG->step = 0;
            LG->done = 1;
        }
        break;


    }
    if (LG->step != LG->stepBuffer)
    {
        LG->stepBuffer = LG->step;
        TimerRst(LogicTask.sheathedWire);
    }
    
}



/**
* @author 19/01/21  wangh
* @Description: ���ߡ����ߡ�Ť�ߵĹ���
* @param --
* @param --
* @return --
*/
void CutWire(LogicParaDef *LG)
{
    INITTASK(LogicTask.CutWire);
    //���ڳ��ȣ����ܴ����е��������ľ���
    if (GUS.WirePara.Data.A.Para.StripLength[singleWire] > GUS.CutWirePara.A_CutDistance)
    {
        GUS.WirePara.Data.A.Para.StripLength[singleWire] = GUS.CutWirePara.A_CutDistance;
    }
    //���ڳ��ȣ����ܴ����е��������ľ���
    if (GUS.WirePara.Data.B.Para.StripLength[singleWire] > GUS.CutWirePara.B_CutDistance)
    {
        GUS.WirePara.Data.B.Para.StripLength[singleWire] = GUS.CutWirePara.B_CutDistance;
    }
    switch (LG->step)
    {
    case 1: //
        if (UserGetStatus(CutAxis) == 0)
        {
            LG->step = 2;
        }
        break;

    case 2: //�е����жϣ������ᵱǰλ����0
        if (1)
        {
            AxMoveAbsUserUnit(CutAxis, GUS.SpdRatio[CutAxis], GUS.StripInterSkin.C_CutPos);
            //�ж��߳�����̫�̵�ʱ���˲�����
            if(GUS.WirePara.Data.WrieLength < WireHelpLength || BHelpClamEN == 0)//���߼йر�
            {
                UserOutputSet(Q_PullWireClam, OFF); //�������ſ�
            }
            LG->step = 3;
            if(sheathedChangeEn && sheathedWireEn)
            {
                UserOutputSet(Q_PullWireClam, ON); //�����мн�
                 LG->step = 4;
           }
        }
        break;

    case 3: //�������ߴ�Ԥ��λ��
        if (UserGetStatus(CutAxis) == 0 && TimerCnt(LogicTask.CutWire) > GUS.CutWirePara.PullWireClamOffDelay)
        {
            if(GUS.WirePara.Data.WrieLength < WireHelpLength || BHelpClamEN == 0)//���߼йر�
            {
                if(ShortMode == 0)
                {
                    AxMoveAbsUserUnit(PullWireAxis, GUS.SpdRatio[PullWireAxis],
                                      MaxS32(0, GUR.AxisPositionUserUnit[PullWireAxis] - GUS.CutWirePara.PullBackLength));
                }
            }
            LG->step = 4;
        }
        break;

    case 4: //�е����ԭ�㣨����ò�����ֹ������ֵ��׼��
        if (UserGetStatus(CutAxis) == 0 && UserGetStatus(PullWireAxis) == 0 && LogicTask.sheathedWire.execute == 0)
        {
            if(sheathedWireEn)
            {
                AxMoveRelUserUnit(CutAxis, GUS.SpdRatio[CutAxis], -GUS.SheathedWire.Data.CutBack);
                LogicTask.sheathedWire.execute = 1; 
            }
            else
            {
                //ǰ����ֱ����
                AxMoveRelUserUnit(FrontLesserAxis, GUS.SpdRatio[FrontLesserAxis], -(GUS.CutWirePara.A_CutDistance - GUS.WirePara.Data.A.Para.StripLength[singleWire]));
                AxMoveRelUserUnit(EndLesserAxis, GUS.SpdRatio[EndLesserAxis], -(GUS.CutWirePara.B_CutDistance - GUS.WirePara.Data.B.Para.StripLength[singleWire]));
                if(BHelpClamEN != 0 && ShortMode == 0 )
                {
                    TwoAxMoveRelUserUnit(PullWireAxis, EndLesserAxis, GUS.SpdRatio[EndLesserAxis] - 1, -(GUS.CutWirePara.B_CutDistance - GUS.WirePara.Data.B.Para.StripLength[singleWire]));
                }
            }
            LG->step = 6;
        }
        break;

    case 6: //
        if (UserGetStatus(CutAxis) == 0 && UserGetStatus(FrontLesserAxis) == 0 && UserGetStatus(PullWireAxis) == 0 && UserGetStatus(EndLesserAxis) == 0 && LogicTask.sheathedWire.execute == 0)
        {
            LG->step = 7;
        }
        break;

    case 7: //�е��߰���λ��
        if (UserGetStatus(FrontLesserAxis) == 0 && UserGetStatus(PullWireAxis) == 0 && UserGetStatus(EndLesserAxis) == 0)
        {
            AxMoveAbsUserUnit(CutAxis, GUS.SpdRatio[CutAxis], GUS.StripInterSkin.C_StripPos - GUS.WirePara.Data.CutWorth[singleWire]);
            LG->step = 8;
        }
        break;

    case 8: //�ж��е���λ����ʱ����
        if (UserGetStatus(CutAxis) == 0)
        {
            LG->step = 9;
        }
        break;

    case 9: //�˵�
        if (TimerCnt(LogicTask.CutWire) > AxisDelay)
        {
            AxMoveRelUserUnit(CutAxis, GUS.SpdRatio[CutAxis], -GUS.WirePara.Data.BackCut[singleWire]);
            LG->step = 10;
        }
        break;

    case 10: //�˵����
        if (UserGetStatus(CutAxis) == 0)
        {
            LG->step = 11;
        }
        break;

    case 11: //����
        if(1)
        {
            //�ж�ǰ���Ƿ������
            if(AHalfStripEn)
            {
                AxMoveRelUserUnit(FrontLesserAxis, GUS.SpdRatio[FrontLesserAxis], -GUS.WirePara.Data.A.Para.HalfStripLength);
            }
            else//����أ���ȫ��
            {
                LogicTask.AWork.Ready = 0;
                AxMoveAbsUserUnit(FrontLesserAxis, GUS.SpdRatio[FrontLesserAxis], GUS.CutWirePara.StripBackPos_A);
                LogicTask.Blow.execute = 1; // ����
            }

            //�жϺ���Ƿ���Ҫ���
            if(BHalfStripEn)
            {
                if(ShortMode)
                {
                    AxMoveRelUserUnit(PullWireAxis, GUS.SpdRatio[PullWireAxis], -GUS.WirePara.Data.B.Para.HalfStripLength);
                }
                else
                {
                    AxMoveRelUserUnit(EndLesserAxis, GUS.SpdRatio[EndLesserAxis], -GUS.WirePara.Data.B.Para.HalfStripLength);
                    if(BHelpClamEN != 0  && ShortMode == 0 )
                    {
                        TwoAxMoveRelUserUnit(PullWireAxis, EndLesserAxis, GUS.SpdRatio[EndLesserAxis] - 1, -GUS.WirePara.Data.B.Para.HalfStripLength);
                    }
                }
            }
            else//ȫ��
            {
                LogicTask.Blow.execute = 1; // ����
                {
                    AxMoveAbsUserUnit(EndLesserAxis, GUS.SpdRatio[EndLesserAxis], GUS.CutWirePara.StripBackPos_B);
                    if(BHelpClamEN != 0 && GUS.WirePara.Data.WrieLength > WireHelpLength)
                    {
                        TwoAxMoveAbsUserUnit(PullWireAxis, EndLesserAxis, GUS.SpdRatio[EndLesserAxis] - 1,
                                             (GUS.FeedWire.PullFeedPos - GUS.WirePara.Data.WrieLength - GUS.FeedWire.EndFeedPos + GUS.CutWirePara.StripBackPos_B - GUS.FeedWire.PullStraightLength));
                    }
                }
            }
            LG->step = 12;
        }
        break;

    case 12: //�����ߺ��е��ſ�
        if (UserGetStatus(FrontLesserAxis) == 0 && UserGetStatus(EndLesserAxis) == 0  && UserGetStatus(PullWireAxis) == 0)
        {
            if(UserGetOutput(Q_Blow) == ON)
            {
                LogicTask.Blow.execute = 1; // ����
            }
            if((AHalfStripEn && ATwistWireEn == 0)|| (BHalfStripEn && BTwistWireEn == 0))
			{
                AxMoveAbsUserUnit(CutAxis, GUS.SpdRatio[CutAxis], GUS.PullWire.HalfCutWorth);
			}
			else
			{
				if((ATwistWireEn == 1 &&  AHalfStripEn == 1) || (BTwistWireEn == 1 && BHalfStripEn == 1))
				{
					AxMoveAbsUserUnit(CutAxis, GUS.SpdRatio[CutAxis], GUS.StripInterSkin.C_StripPos - GUS.WirePara.Data.TwistCutPos);
				}
			}
            LG->step = 13;
        }
        break;

    case 13: //�жϴ���
        if (UserGetStatus(CutAxis) == 0)
        {
            //�ж��Ƿ���Ҫ����
            if(ATwistWireEn == 1 && AHalfStripEn == 1)
            {
                AxMoveRelUserUnit(FrontLesserAxis, GUS.SpdRatio[FrontLesserAxis], GUS.WirePara.Data.A.Para.TwistBackDistance);
            }
            else
            {
                if(LogicTask.AWork.Ready == 1)
                {
                    AxMoveAbsUserUnit(FrontLesserAxis, GUS.SpdRatio[FrontLesserAxis], GUS.CutWirePara.StripBackPos_A);
                }
                LogicTask.AWork.Ready = 0;
            }

            //�ж��Ƿ���Ҫ����
            if(BTwistWireEn == 1 && BHalfStripEn == 1)
            {
                AxMoveRelUserUnit(EndLesserAxis, GUS.SpdRatio[EndLesserAxis], GUS.WirePara.Data.B.Para.TwistBackDistance);
            }
            else
            {
                AxMoveAbsUserUnit(EndLesserAxis, GUS.SpdRatio[EndLesserAxis], GUS.CutWirePara.StripBackPos_B);
                if(BHelpClamEN != 0  && GUS.WirePara.Data.WrieLength >= WireHelpLength)
                {
                    TwoAxMoveAbsUserUnit(PullWireAxis, EndLesserAxis, GUS.SpdRatio[EndLesserAxis] - 1,
                                         (GUS.FeedWire.PullFeedPos - GUS.WirePara.Data.WrieLength - GUS.FeedWire.EndFeedPos + GUS.CutWirePara.StripBackPos_B - GUS.FeedWire.PullStraightLength));
                }
                LogicTask.BWork.Ready = 0;
            }
            LG->step = 14;
        }
        break;

    case 14: //�е��ߵ�Ť��λ��
        if (UserGetStatus(CutAxis) == 0 && UserGetStatus(FrontLesserAxis) == 0 && UserGetStatus(EndLesserAxis) == 0 && TimerCnt(LogicTask.CutWire) > 50)
        {
            if ((ATwistWireEn == 1 &&  AHalfStripEn == 1) || (BTwistWireEn == 1 && BHalfStripEn == 1))
            {
                UserOutputSet(Q_TwistDown, ON);
                AxMoveAbsUserUnit(CutAxis, GUS.SpdRatio[CutAxis], GUS.StripInterSkin.C_StripPos - GUS.WirePara.Data.TwistCutPos);
                LG->step = 15;
            }
            else
            {
                LG->step = 16;
            }
        }
        break;

    case 15:
        ton_et[1] = 0;
        if(TimerCnt(LogicTask.CutWire) > GUS.CutWirePara.TwistDownOnDelay)
        {
            LG->step = 16;
        }
        break;


    case 16: //Ť����Ť��
        ton_et[1] = 0;
        if (UserGetStatus(CutAxis) == 0)
        {
            if ((ATwistWireEn == 1 &&  AHalfStripEn == 1) || (BTwistWireEn == 1 && BHalfStripEn == 1))
            {
                if(TwistMode)
                {
                    UserOutputSet(Q_Twist, ON);
                }
                else
                {
                    UserOutputSet(Q_Twist, OFF);
                }
                LG->step = 17;
            }
            else
            {
                LG->step = 18;
            }
        }
        break;

    case 17:
        ton_et[1] = 0;
        if(TimerCnt(LogicTask.CutWire) > GUS.CutWirePara.TwistOnDelay)
        {
            //ǰ�˱��˱ߴ�
            if(ATwistWireEn == 1 && AHalfStripEn == 1)
            {
                if(LogicTask.AWork.Ready == 1)
                {
                    AxMoveRelUserUnit(FrontLesserAxis, GUS.CutWirePara.TwistSpeed,
                                      -(GUS.WirePara.Data.A.Para.StripLength[singleWire] - GUS.WirePara.Data.A.Para.HalfStripLength + 200));
                LogicTask.Blow.execute = 1; // ����
               }
            }

            //��˱��˱ߴ�
            if(BTwistWireEn == 1 && BHalfStripEn == 1)
            {
                AxMoveRelUserUnit(EndLesserAxis, GUS.CutWirePara.TwistSpeed,
                                  -(GUS.WirePara.Data.B.Para.StripLength[singleWire] - GUS.WirePara.Data.B.Para.HalfStripLength + 200));
                LogicTask.Blow.execute = 1; // ����
           }
            LG->step = 18;
        }
        break;

    case 18: //Ť����ǰ���˳�
        if (UserGetStatus(EndLesserAxis) == 0 && UserGetStatus(FrontLesserAxis) == 0)
        {
            if(UserGetOutput(Q_Blow) == ON)
            {
                LogicTask.Blow.execute = 1; // ����
            }
            //�ж��Ƿ���Ҫ����
            if(ATwistWireEn == 1 && AHalfStripEn == 1)
            {
                if(LogicTask.AWork.Ready == 1)
                {
                    AxMoveAbsUserUnit(FrontLesserAxis, GUS.SpdRatio[FrontLesserAxis], GUS.CutWirePara.StripBackPos_A);
                }
            }

            //�ж��Ƿ���Ҫ����
            if(BTwistWireEn == 1 && BHalfStripEn == 1)
            {
                AxMoveAbsUserUnit(EndLesserAxis, GUS.SpdRatio[EndLesserAxis], GUS.CutWirePara.StripBackPos_B);
            }
            if(GUS.WirePara.Data.WrieLength >= WireHelpLength && BHelpClamEN != 0 && LogicTask.BWork.Ready == 1 )
            {
                TwoAxMoveAbsUserUnit(PullWireAxis, EndLesserAxis, GUS.SpdRatio[EndLesserAxis] - 1,
                                     (GUS.FeedWire.PullFeedPos - GUS.WirePara.Data.WrieLength - GUS.FeedWire.EndFeedPos + GUS.CutWirePara.StripBackPos_B - GUS.FeedWire.PullStraightLength));
            }
            LG->step = 19;
        }
        break;

    case 19: //ǰ��˿��Կ�ʼ������
        if (UserGetStatus(EndLesserAxis) == 0 && UserGetStatus(CutAxis) == 0  && UserGetStatus(FrontLesserAxis) == 0)
        {
            UserOutputSet(Q_TwistDown, OFF);
            if(TwistMode)
            {
                UserOutputSet(Q_Twist, OFF);
            }
            else
            {
                UserOutputSet(Q_Twist, ON);
            }
            LogicTask.AWork.Ready = 0;
            LG->step = 20;
        }
        break;

    case 20:
        if (UserGetStatus(EndLesserAxis) == 0 && UserGetStatus(CutAxis) == 0)
        {
            LogicTask.Blow.execute = 1; // ����
            if (LogicTask.BWork.Ready == 1)
                AxMoveAbsUserUnit(EndLesserAxis, GUS.SpdRatio[EndLesserAxis], GUS.CutWirePara.StripBackPos_B);
            LG->step = 21;
        }
        break;

    case 21:
        if ((LogicTask.AWork.Ready == 0) && UserGetStatus(CutAxis) == 0)
        {   
            LogicTask.AWork.Ready = 0;
            LogicTask.BWork.Ready = 0;
            LG->step = 22;
            if(DividEN)
            {
                LogicTask.DivideWire.execute = 1;
                LG->step = 23;
            }
            else
            {
                AxMoveAbsUserUnit(CutAxis, GUS.SpdRatio[CutAxis], GUS.FeedWire.ShakeWirePos_Cut);
            }
        }
        break;

    case 22: //�е���Ť�����0λ��
        if (UserGetStatus(CutAxis) == 0)
        {
            LogicTask.ShakeWire.execute = 1;
            LogicTask.Blow.execute = 1;
            LG->step = 23;
        }
        break;

    case 23: //�е���Ť�߻�ԭ��
        if (UserGetStatus(CutAxis) == 0 && LogicTask.ShakeWire.execute == 0 && LogicTask.DivideWire.execute == 0)
        {
            UserOutputSet(Q_TwistDown, OFF);
            AxMoveAbsUserUnit(CutAxis, GUS.SpdRatio[CutAxis], GUS.FeedWire.CutOpenPos);
            LG->step = 24;
        }
        break;

    case 24:
        if(GUR.AxisPositionUserUnit[CutAxis] <  GUS.StripInterSkin.C_CutPos / 2)
        {
            LG->done = 1;
        }
        if (UserGetStatus(CutAxis) == 0)
        {
            LG->step = ENDCASE;
        }
        break;

    case ENDCASE: //
        if (1)
        {
            LG->step = 0;
            LG->execute = 0;
        }
        break;

    default:
        break;
    }
    if (LG->step != LG->stepBuffer)
    {
        LG->stepBuffer = LG->step;
        TimerRst(LogicTask.CutWire);
    }
}

/**
* @author 19/12/10  wangh
* @Description:  ���ײ��߹���
* @param --
* @param --
* @return --
*/
void DivideWire(LogicParaDef *LG)
{
    if (LG->step == 0 && LG->execute == 1)
    {
        LG->step = 1;
        LG->done = 0;
        LG->count = 0;
    }

    switch (LG->step)
    {
        case 1: //�е�������λ��
            if (UserGetStatus(CutAxis) == 0)
            {
                AxMoveAbsUserUnit(CutAxis, GUS.SpdRatio[CutAxis], GUS.FeedWire.DividPos_Cut);
                LG->step = 2;
            }
            break;
        
        case 2: //ǰ�����ǰ�󵽲���λ��
            if (UserGetStatus(CutAxis) == 0 && UserGetStatus(FrontLesserAxis) == 0)
            {
                AxMoveAbsUserUnit(FrontLesserAxis, GUS.SpdRatio[FrontLesserAxis], GUS.FeedWire.DividPos_End);
                LG->step = 3;
            }
            break;
        
        case 3: //�������ײ���
            if (UserGetStatus(CutAxis) == 0 && UserGetStatus(FrontLesserAxis) == 0)
            {
                UserOutputSet(Q_TwistDown, ON);
                LG->step = 4;
            }
            break;
            
        case 4://���������ſ�
            if (UserGetStatus(CutAxis) == 0 && UserGetStatus(FrontLesserAxis) == 0 && TimerCnt(LogicTask.DivideWire) > GUS.CutWirePara.TwistDownOnDelay)
            {
                UserOutputSet(Q_TwistDown, OFF);
                LG->step = 5;
            }
            break;
        
        case 5: //ǰ��������˳���������
            if (UserGetStatus(CutAxis) == 0 && UserGetStatus(FrontLesserAxis) == 0 && TimerCnt(LogicTask.DivideWire) > GUS.CutWirePara.TwistDownOnDelay)
            {
                AxMoveAbsUserUnit(FrontLesserAxis, GUS.SpdRatio[FrontLesserAxis], GUS.FeedWire.DividPos_EndOut);
                LG->step = 6;
            }
            break;
        
        case 6: //
            if (UserGetStatus(CutAxis) == 0 && UserGetStatus(FrontLesserAxis) == 0)
            {
                LG->step = 7;
            }
            break;

        case 7:
            LG->step = 0;
            LG->execute = 0;
            LG->done = 1;
        break;
    }
    if (LG->step != LG->stepBuffer)
    {
        LG->stepBuffer = LG->step;
        TimerRst(LogicTask.DivideWire);
    }
}


/**
* @author 19/02/21  wangh
* @Description: ȥ��Ƥ����������
* @param --
* @param --
* @return --
*/
void ShakeWire()
{
    INITTASK(LogicTask.ShakeWire);
    switch (LogicTask.ShakeWire.step)
    {
    case 1: //
        if (1)
        {
            UserOutputSet(Q_TwistDown, OFF);
            if(TwistMode)
            {
                UserOutputSet(Q_Twist, OFF);
            }
            else
            {
                UserOutputSet(Q_Twist, ON);
            }
            LogicTask.ShakeWire.step = 4;
            LogicTask.ShakeWire.count++;
            if (LogicTask.ShakeWire.count > GUS.CutWirePara.ShakeWire)
            {
                LogicTask.ShakeWire.count = 0;
                LogicTask.ShakeWire.step = ENDCASE;
            }
        }
        break;

    case 2: //
        if (TimerCnt(LogicTask.ShakeWire) > 100)
        {
            if(TwistMode)
            {
                UserOutputSet(Q_Twist, ON);
            }
            else
            {
                UserOutputSet(Q_Twist, OFF);
            }
            LogicTask.ShakeWire.step = 4;
        }
        break;

    case 3: //
        if (TimerCnt(LogicTask.ShakeWire) > 100)
        {
            LogicTask.ShakeWire.step = 4;
        }
        break;


    case 4: //
        if (TimerCnt(LogicTask.ShakeWire) > 100)
        {
            UserOutputSet(Q_TwistDown, ON);
            LogicTask.Blow.execute = 1;
            LogicTask.ShakeWire.step = 5;
        }
        break;

    case 5: //
        if (TimerCnt(LogicTask.ShakeWire) > 100)
        {
            UserOutputSet(Q_TwistDown, OFF);
            if(TwistMode)
            {
                UserOutputSet(Q_Twist, OFF);
            }
            else
            {
                UserOutputSet(Q_Twist, ON);
            }
            LogicTask.ShakeWire.count++;
            LogicTask.ShakeWire.step = 4;
            if (LogicTask.ShakeWire.count > GUS.CutWirePara.ShakeWire)
            {
                LogicTask.ShakeWire.count = 0;
                LogicTask.ShakeWire.step = ENDCASE;
            }
        }
        break;

    case ENDCASE: //
        if (1)
        {
            LogicTask.ShakeWire.step = 0;
            LogicTask.ShakeWire.execute = 0;
        }
        break;

    default:
        break;
    }
    if (LogicTask.ShakeWire.step != LogicTask.ShakeWire.stepBuffer)
    {
        LogicTask.ShakeWire.stepBuffer = LogicTask.ShakeWire.step;
        TimerRst(LogicTask.ShakeWire);
    }
}

///**
//* @author 19/02/18  wangh
//* @Description: ����
//* @param --
//* @param --
//* @return --
//*/
//void SeparateWire()
//{
////	/************************************
////	�߳�<���ߵ����е���ľ���
////	��һ�����߳�-���߳�-���ߵ��е������߳�ȡ�ࣩ
////	************************************/

////	unsigned int i;
////	static s32 L_cutDistance;	  //�жϾ���
////	static s32 L_separateDistance; //���߾���

////	//��������
////	static s32 wireLength;			   //���߳���
////	static s32 A_separateWireLength;   //A�˷��߳���
////	static s32 B_separateWireLength;   //B�˷��߳���
////	static s32 cutterSeparateDistance; //���ߵ����е��ļ��
////	if(GUW.HadToResset == 0)
////	{
////		wireLength = GUS.SperatePara.A_separateWireLength + GUS.SperatePara.B_separateWireLength + GUS.SperatePara.cutterSeparateDistance + 300;
////	}
////	else
////	{
////		wireLength = GUS.WirePara.Data.WrieLength;
////	}
////	A_separateWireLength = GUS.SperatePara.A_separateWireLength - 100;
////	B_separateWireLength = GUS.SperatePara.B_separateWireLength - 100;

////	if (LogicTask.SeparateWire.execute == 1 && LogicTask.SeparateWire.step == 0)
////	{
////		LogicTask.SeparateWire.step = 1;
////	}

////	switch (LogicTask.SeparateWire.step)
////	{
////	case 1:
////		//���߼е�����λ��
////		//�жϾ��� = ���߳���
////		cutterSeparateDistance = GUS.SperatePara.cutterSeparateDistance;
////		L_cutDistance = wireLength;
////		//���߾��� = �����߳��� - B�˷��߳��ȣ� - ���ߵ����е��ľ���//
////		L_separateDistance = (wireLength - B_separateWireLength) - (cutterSeparateDistance);
////		i = 1;
////		while (L_separateDistance < 0 && abs(L_separateDistance) > (A_separateWireLength + B_separateWireLength))
////		{
////			L_separateDistance = (i * wireLength - B_separateWireLength) - cutterSeparateDistance;
////			i++;
////		}
////		//�жϷ��ߵ���λ��
////		if (L_separateDistance <= 0)
////		{
////			//�����ߵ��ڷ��߷�Χ�ڣ�������߽���λ�õ����ߵ��ľ��룬���ߵ���ѹ
////			L_separateDistance = A_separateWireLength + B_separateWireLength + L_separateDistance;
////			UserOutputSet(Q_separate_wire, ON);
////			ton_et[0] = 0;
////		}
////		else
////		{
////			//���ߵ����ڷ��߷�Χ�ڣ����ߵ�̧��
////			UserOutputSet(Q_separate_wire, OFF);
////			ton_et[0] = GUS.SperatePara.SperateOnDelay;
////		}
////		LogicTask.SeparateWire.step = 2;
////		break;

////	case 2:
////		if (ton(0, 1, GUS.SperatePara.SperateOnDelay))
////		{

////			//�ж����߷��߾��뻹�������е�����
////			if (L_cutDistance > L_separateDistance)
////			{
////				AxisRunDefaultUserUnit(FeedWireAxis, RELMODE, GUS.SperatePara.sperateSpeed, L_separateDistance);
////				L_cutDistance -= L_separateDistance;
////				L_separateDistance = 0;
////				LogicTask.SeparateWire.step = 3;
////			}
////			else
////			{
////				AxisRunDefaultUserUnit(FeedWireAxis, RELMODE, GUS.SpdRatio[FeedWireAxis], L_cutDistance);
////				L_separateDistance -= L_cutDistance;
////				L_cutDistance = 0;
////				ton_et[0] = 0;
////				LogicTask.SeparateWire.step = 5;
////			}
////		}
////		break;

////	case 3:
////		if (UserGetStatus(FeedWireAxis) == 0)
////		{
////			if (UserGetOutput(Q_separate_wire) == ON)
////			{
////				UserOutputSet(Q_separate_wire, OFF);
////			}
////			else
////			{
////				UserOutputSet(Q_separate_wire, ON);
////			}
////			LogicTask.SeparateWire.step = 4;
////			ton_et[0] = 0;
////		}
////		break;

////	case 4:
////		if (ton(0, 1, GUS.SperatePara.SperateOffDelay))
////		{
////			if (UserGetOutput(Q_separate_wire) == ON)
////			{
////				L_separateDistance = A_separateWireLength + B_separateWireLength;
////			}
////			else
////			{
////				L_separateDistance = wireLength - A_separateWireLength - B_separateWireLength;
////			}
////			LogicTask.SeparateWire.step = 2;
////		}
////		break;

////	case 5:
////		//if(ton(0,AxisPara[PullWireAxis].execute == 0,GUS.SperateDelay))
////		{
////			UserOutputSet(Q_separate_wire, OFF);
////			LogicTask.SeparateWire.execute = 0;
////			LogicTask.SeparateWire.step = 0;
////			LogicTask.SeparateWire.done = 1;
////		}
////		break;
////	}
//}

///**
//* @author 19/01/21  wangh
//* @Description: A�˴����
//* @param --
//* @param --
//* @return --
//*/
//void AMakeTeminal()
//{
//	int i;
//	if(GUS.Product.NoTerminalNum > 31)
//	{
//		GUS.Product.NoTerminalNum = 31;
//	}
//	INITTASK(LogicTask.AMakeTeminal);
//	switch (LogicTask.AMakeTeminal.step)
//	{
//	case 1: //
//		GUW.makeTerminalCountForUser = 0;
//		if (UserGetStatus(FeedWireAxis) == 0)
//		{
//			//����ģʽ���ߵ����λ�ã�������Լ��λ��
//			if (GUW.button.DeviceMode == STEPMODE)
//			{
//				if (AMakeTerminalEn == 1)
//				{
//					AxMoveAbsUserUnit(FrontPrimaryAxis, GUS.SpdRatio[FrontPrimaryAxis], GUS.AMakeTerminalPara.FrontMoveCheckPos); //���Ƶ����λ��
//				}
//			}
//			LogicTask.AMakeTeminal.step = 2;
//		}
//		break;

//	case 2: //
//		if (UserGetStatus(FrontPrimaryAxis) == 0)
//		{
//			//�ж��費��Ҫ�����
//			if (AMakeTerminalEn == 1)
//			{
//				AxMoveAbsUserUnit(FrontPrimaryAxis, GUS.SpdRatio[FrontPrimaryAxis], GUS.AMakeTerminalPara.FrontMoveMakeTerminalPos); //���Ƶ����λ
//				for (i = 0; i < GUS.WirePara.Data.ProductSet.WireNum; i++)
//				{
//					if (WordToBit(GUS.AMakeTerminalPara.MakeTerminalEn, i))
//					{
//						AxMoveAbsUserUnit(FrontPrimaryAxis, GUS.SpdRatio[FrontPrimaryAxis], GUS.AMakeTerminalPara.FrontMoveMakeTerminalPos - GUS.WirePara.Data.WireDistance * i); //���Ƶ����λ
//						GUW.makeTerminalCount = i;
//						i = GUS.WirePara.Data.ProductSet.WireNum;
//					}
//				}
//				LogicTask.AMakeTeminal.step = 3;
//			}
//			else
//			{
//				LogicTask.AMakeTeminal.step = 8;
//			}
//		}
//		break;

//	case 3: //�жϺ����ᳬ�����λ�ú�ǰ������ǰ�����λ��
//		if (((GUR.AxisPositionUserUnit[FrontPrimaryAxis] > GUS.AMakeTerminalPara.FrontMoveCheckPos) || UserGetStatus(FrontPrimaryAxis) == 0) && UserGetStatus(FeedWireAxis) == 0)
//		{
//			if (UserGetStatus(FeedWireAxis) == 0)
//			{ //ǰ������ǰ�����λ��
//				AxMoveAbsUserUnit(FeedWireAxis, 1, GUS.AMakeTerminalPara.FrontMakeTerminalPos);
//			}
//			if (UserGetStatus(FrontPrimaryAxis) == 0)
//			{
//				GUW.RealWireNum = 0;
//				for (i = 0; i < GUS.WirePara.Data.ProductSet.WireNum; i++)
//				{
//					if (WordToBit(GUS.AMakeTerminalPara.MakeTerminalEn, i))
//					{ //����ʵ���߸�����û�д�˵Ĳ�����
//						GUW.RealWireNum++;
//					}
//				}

//				LogicTask.AMakeTeminal.step = 4;
//				GUW.NoTermanal = 0;
//			}
//		}
//		break;

//	case 4:
//		if (UserGetStatus(FrontPrimaryAxis) == 0 && UserGetStatus(FeedWireAxis) == 0 && TimerCnt(LogicTask.AMakeTeminal) > 50)
//		{
//			if (TerminalEn == 1)
//			{
//				UserOutputSet(Q_MakeTerminal, ON);
//				LogicTask.AddOil.execute = 1;
//				if (NoTerminalCheckEn == 1)
//				{
//					if(WordToBit(GUW.WireNoTerminalState,GUW.WireNoTerminalCount) == 1)
//					{
//						GUW.NoTermanal = 1;
//					}
//					if (InputGetSta(I_B_NoTerminal) == OFF)
//					{
//						SetWhichBitU16(&GUW.WireNoTerminalState,GUW.WireNoTerminalCount);
//					}
//					else
//					{
//						ResetWhichBitU16(&GUW.WireNoTerminalState,GUW.WireNoTerminalCount);
//					}
//				}
//			}
//			LogicTask.AMakeTeminal.step = 5;
//		}
//		break;

//	case 5: //
//		if (TimerCnt(LogicTask.AMakeTeminal) > 15)
//		{
//			UserOutputSet(Q_MakeTerminal, OFF);
//			LogicTask.AMakeTeminal.step = 6;
//		}
//		break;

//	case 6: //
//		if (TimerCnt(LogicTask.AMakeTeminal) > MaxS32(GUS.WirePara.Data.A.Para.MakeTerminalDelay, 200))
//		{
//			if (GUW.NoTermanal == 0)
//			{
//				GUW.NoTermanalCount = 0;
//				GUW.makeTerminalCount++;
//				GUW.WireNoTerminalCount++;
//				if(GUW.WireNoTerminalCount > GUS.Product.NoTerminalNum)
//				{
//					GUW.WireNoTerminalCount = 0;
//				}
//				if (GUW.makeTerminalCount < GUS.WirePara.Data.ProductSet.WireNum)
//				{
//					if (WordToBit(GUS.AMakeTerminalPara.MakeTerminalEn, GUW.makeTerminalCount))
//					{
//						AxMoveAbsUserUnit(FrontPrimaryAxis, 9, GUS.AMakeTerminalPara.FrontMoveMakeTerminalPos - GUS.WirePara.Data.WireDistance * GUW.makeTerminalCount);
//						AxMoveAbsUserUnit(FeedWireAxis, 1, GUS.AMakeTerminalPara.FrontMakeTerminalPos);
//						LogicTask.AMakeTeminal.step = 3;
//					}
//				}
//				else
//				{
//					LogicTask.AMakeTeminal.step = 7;
//				}
//			}
//			else
//			{
//				LogicTask.AMakeTeminal.step = 4;
//				GUW.NoTermanalCount++;
//				if (GUW.NoTermanalCount > 2)
//				{
//					GUW.NoTermanal = 0;
//					AlarmSetBit(2, 0, 8);
//					LogicTask.AMakeTeminal.step = 7;
//				}
//			}
//		}
//		break;

//	case 7: //�з���
//		if (UserGetStatus(FrontPrimaryAxis) == 0)
//		{
//			if (TerminalEn == 1)
//			{
//				LogicTask.CutWastage.execute = 1;
//			}
//			LogicTask.AMakeTeminal.step = 8;
//		}
//		break;

//	case 8: //
//		//�е����ſ���Ȼ������Ƶ�����λ����ֹ����ײ���е�
//		if (UserGetStatus(FeedWireAxis) == 0 && LogicTask.CutWire.execute == 0)
//		{
//			if (GUW.button.DeviceMode == STEPMODE)
//			{
//				if (AMakeTerminalEn == 1)
//				{
//					if (GUR.AxisPositionUserUnit[FrontPrimaryAxis] > GUS.AMakeTerminalPara.FrontMoveCheckPos)
//						AxMoveAbsUserUnit(FrontPrimaryAxis, GUS.SpdRatio[FrontPrimaryAxis], GUS.AMakeTerminalPara.FrontMoveCheckPos); //���Ƶ����λ��
//				}
//			}
//			LogicTask.AMakeTeminal.step = 9;
//		}
//		break;

//	case 9: //
//		if (UserGetStatus(FrontPrimaryAxis) == 0 && UserGetStatus(FeedWireAxis) == 0)
//		{
//			AxMoveAbsUserUnit(FrontPrimaryAxis, GUS.SpdRatio[FrontPrimaryAxis], GUS.FeedWire.FrontRFeedPos); //��������λ
//		}
//		if (GUR.AxisPositionUserUnit[FrontPrimaryAxis] < GUS.AMakeTerminalPara.FrontMoveCheckPos / 2 && LogicTask.CutWire.execute == 0)
//		{
//			LogicTask.AMakeTeminal.step = 10;
//		}
//		break;

//	case 10:
//		if (UserGetStatus(FrontPrimaryAxis) == 0)
//		{
//			GUW.RealWireNum = 0;
//			for (i = 0; i < GUS.WirePara.Data.ProductSet.WireNum; i++)
//			{
//				if (WordToBit(GUS.AMakeTerminalPara.MakeTerminalEn, i))
//				{
//					GUW.RealWireNum++;
//				}
//			}

//			if (GUW.makeTerminalCountForUser < GUW.RealWireNum)
//			{
//				if (TerminalEn == 1 && AMakeTerminalEn == 1 && AMakeTerminalCheck == 1 )
//				{
//					AlarmSetBit(2, 0, 1); //���Ӽ�ⱨ��������
//				}
//			}
//			AxMoveAbsUserUnit(FeedWireAxis, GUS.SpdRatio[FeedWireAxis], 0); //�������ߵ�0λ�ã�׼����һ������
//			LogicTask.AMakeTeminal.step = 11;
//		}
//		break;

//	case 11:
//		if (UserGetStatus(FeedWireAxis) == 0 && LogicTask.CutWire.execute == 0)
//		{
//			LogicTask.AMakeTeminal.step = ENDCASE;
//		}
//		break;

//	case ENDCASE: //
//		if (1)
//		{
//			LogicTask.AMakeTeminal.step = 0;
//			LogicTask.AMakeTeminal.execute = 0;
//		}
//		break;

//	default:
//		break;
//	}
//	if (LogicTask.AMakeTeminal.step != LogicTask.AMakeTeminal.stepBuffer)
//	{
//		LogicTask.AMakeTeminal.stepBuffer = LogicTask.AMakeTeminal.step;
//		TimerRst(LogicTask.AMakeTeminal);
//	}
//}

/**
* @author 19/04/28  wangh
* @Description: A��մ��
* @param --
* @param --
* @return --
*/
void ATin(LogicParaDef *LG)
{
    INITTASK(LogicTask.ATin);
    switch (LG->step)
    {
    case 1: //�ж��Ƿ�մ����������λ��
        if (UserGetStatus(FrontPrimaryAxis) == 0 && UserGetStatus(FrontLesserAxis) == 0 && LogicTask.DivideWire.execute == 0 && LogicTask.AWork.Ready == 0 && (DividEN == 0 || LogicTask.DivideWire.done == 1))
        {
            LogicTask.DivideWire.done = 0;
            if (ATinEn == 1 && Data.DivideFirstWire == 0)
            {
                AxMoveAbsUserUnit(FrontPrimaryAxis, GUS.SpdRatio[FrontPrimaryAxis], GUS.A_Tin.A_R_RosinPos);
                AxMoveAbsUserUnit(FrontLesserAxis, GUS.SpdRatio[FrontLesserAxis], GUS.WirePara.Data.A.Para.TinBackPos);
                LG->step = 2;
            }
            else
            {
                LG->step = 10;
            }
        }
        break;

    case 2: //���� ��մ�������
        if (UserGetStatus(FrontPrimaryAxis) == 0)
        {
            //20190730
            if(LG->count > 0)
            {
                AxMoveAbsUserUnit(FrontLesserAxis, GUS.SpdRatio[FrontLesserAxis], GUS.SecondTin.Data.SecondRosinLength_A);
            }
            else
            {
                AxMoveAbsUserUnit(FrontLesserAxis, GUS.SpdRatio[FrontLesserAxis], GUS.WirePara.Data.A.Para.TinPara.AddRosinLength);
            }
            LG->step = 3;
        }
        break;

    case 3: //�ж��ߵ�������ȵ�λ����ʱ��������
        if (UserGetStatus(FrontLesserAxis) == 0)
        {
            LG->step = 4;
        }
        break;

    case 4: //������ʱ������0λ
        if (TimerCnt(LogicTask.ATin) > GUS.WirePara.Data.A.Para.TinPara.RosinDelay)
        {
            if(GUS.WirePara.Data.A.Para.TinPara.AddRosinLength - GUS.WirePara.Data.A.Para.TinBackPos > GUS.WirePara.Data.A.Para.StripLength[singleWire] + 1000)
            {
                AxMoveRelUserUnit(FrontLesserAxis, GUS.WirePara.Data.A.Para.TinPara.TinSpeed, -GUS.WirePara.Data.A.Para.StripLength[singleWire] - 1000);
            }
            else
            {
                AxMoveAbsUserUnit(FrontLesserAxis, GUS.SpdRatio[FrontLesserAxis],  GUS.WirePara.Data.A.Para.TinBackPos);
            }
            LG->step = 5;
        }
        break;

    case 5: //��ת��մ��λ��
        if (UserGetStatus(FrontLesserAxis) == 0)
        {
            AxMoveAbsUserUnit(FrontPrimaryAxis, GUS.SpdRatio[FrontPrimaryAxis], GUS.A_Tin.A_R_TinPos);
            LG->step = 6;
        }
        break;

    case 6: //��մ�����
        if (UserGetStatus(FrontPrimaryAxis) == 0 && UserGetStatus(FrontLesserAxis) == 0)
        {
            if(ASlowTinEn)
            {
                //20190730 ��մ��λ��
                if(LG->count > 0)
                {
                    AxMoveAbsUserUnit(FrontLesserAxis, GUS.SecondTin.Data.SecondTinSpeed_A, GUS.SecondTin.Data.SecondSlowTinPos_A);
                }
                else
                {
                    AxMoveAbsUserUnit(FrontLesserAxis, GUS.WirePara.Data.A.Para.TinPara.TinSpeed, GUS.WirePara.Data.A.Para.SlowTinPos);
                }
                LG->step = 12;
            }
            else
            {
                if(LG->count > 0)
                {
                    AxMoveAbsUserUnit(FrontLesserAxis, GUS.SecondTin.Data.SecondTinSpeed_A, GUS.SecondTin.Data.SecondTinLength_A);
                }
                else
                {
                    AxMoveAbsUserUnit(FrontLesserAxis, GUS.WirePara.Data.A.Para.TinPara.TinSpeed, GUS.WirePara.Data.A.Para.TinPara.TiningLength);
                }
                LG->step = 7;
            }
        }
        break;

    case 12:
        if (UserGetStatus(FrontLesserAxis) == 0)
        {
            if(LG->count == 0)
            {
                AxMoveAbsUserUnit(FrontLesserAxis, GUS.WirePara.Data.A.Para.SlowTinSpeed, GUS.WirePara.Data.A.Para.TinPara.TiningLength);
            }
            else
            {
                AxMoveAbsUserUnit(FrontLesserAxis, GUS.SecondTin.Data.SecondSlowTinSpeed_A, GUS.SecondTin.Data.SecondTinLength_A);
            }
            LG->step = 7;
        }
        break;


    case 7: //�ж��ߵ�����ȵ�λ����ʱ��������
        if (UserGetStatus(FrontLesserAxis) == 0)
        {
            LG->step = 13;
        }
        break;

    case 13:
        if (TimerCnt(LogicTask.ATin) > GUS.WirePara.Data.A.Para.TinPara.TinDelay)
        {
            if(ASlowTinEn)
            {
                if(LG->count == 0)
                {
                    AxMoveAbsUserUnit(FrontLesserAxis, GUS.WirePara.Data.A.Para.SlowTinBackSpeed, GUS.WirePara.Data.A.Para.SlowTinPos);
                }
                else
                {
                    AxMoveAbsUserUnit(FrontLesserAxis, GUS.SecondTin.Data.SecondSlowTinBackSpeed_A, GUS.SecondTin.Data.SecondSlowTinPos_A);
                }
            }
            LG->step = 8;
        }
        break;

    case 8: //մ����ʱ������
        if (UserGetStatus(FrontLesserAxis) == 0)
        {
            AxMoveAbsUserUnit(FrontLesserAxis, GUS.WirePara.Data.A.Para.TinPara.TinSpeed, GUS.WirePara.Data.A.Para.TinBackPos);
            LG->step = 9;
        }
        break;

    case 9: //���մ��
        if (UserGetStatus(FrontLesserAxis) == 0)
        {
            LG->count++;
            LG->step = 10;
            if (LG->count < GUS.WirePara.Data.A.Para.TinPara.TinTimes)
            {
                //20190730
                if(ARosinMuchEn)
                {
                    LG->step = 1;
                }
                else
                {
                    LG->step = 6;
                }
            }
        }
        break;

    case 10: //��ת��ˮƽλ�á�մ��������
        if (UserGetStatus(FrontPrimaryAxis) == 0 && UserGetStatus(FrontLesserAxis) == 0 )
        {
            if (ATinEn == 1)
            {
                AxMoveAbsUserUnit(FrontLesserAxis, GUS.SpdRatio[FrontLesserAxis], GUS.WirePara.Data.A.Para.TinBackPos);
            }
            LG->step = 11;
        }
        break;

    case 11: //��ת��ˮƽλ��
        if (UserGetStatus(FrontPrimaryAxis) == 0 && UserGetStatus(FrontLesserAxis) == 0
                && (TimerCnt(LogicTask.ATin) > GUS.WirePara.Data.A.Para.TinStayTime || ATinEn == 0) && LogicTask.CutWire.execute == 0)
        {
            if (ATinEn == 1)
            {
                LogicTask.AFeedTin.execute = 1;
                LogicTask.ASmoothTin.execute = 1;
            }
            AxMoveAbsUserUnit(FrontPrimaryAxis, GUS.SpdRatio[FrontPrimaryAxis], GUS.FeedWire.FrontRFeedPos);
            LG->step = ENDCASE;
        }
        break;

    case ENDCASE: //
        if (UserGetStatus(FrontPrimaryAxis) == 0 && UserGetStatus(FrontPrimaryAxis) == 0)
        {
            LG->count = 0;
            LG->step = 0;
            LG->execute = 0;
        }
        break;

    default:
        break;
    }
    if (LG->step != LG->stepBuffer)
    {
        LG->stepBuffer = LG->step;
        TimerRst(LogicTask.ATin);
    }
}



/**
* @author 19/02/19  wangh
* @Description: B��մ��
* @param --
* @param --
* @return --
*/
void BTin(LogicParaDef *LG)
{
    GUS.WirePara.Data.B.Para.TinBackPos = 0; //մ�����˵�0λ��
    
    INITTASK(LogicTask.BTin);
    switch (LG->step)
    {
    case 1: //�ж��Ƿ�մ����������λ��
        if (UserGetStatus(EndPrimaryAxis) == 0 && LogicTask.BWork.Ready == 0 && UserGetStatus(PullWireAxis) == 0)
        {
            if (BTinEn == 1 && Data.DivideFirstWire == 0 )
            {
//                LogicTask.DivideWire.done = 0;
                AxMoveAbsUserUnit(EndPrimaryAxis, GUS.SpdRatio[EndPrimaryAxis], GUS.B_Tin.B_R_RosinPos);
                AxMoveAbsUserUnit(EndLesserAxis, GUS.SpdRatio[EndLesserAxis], GUS.WirePara.Data.B.Para.TinBackPos);
                if(BHelpClamEN != 0 && ShortMode == 0)
                {
                    AxMoveRelUserUnit(PullWireAxis, GUS.SpdRatio[PullWireAxis]-1, GUS.PullWire.PullHelp);
                }
                LG->step = 2;
            }
            else
            {
                LG->step = 9;
            }
        }
        break;

    case 2: //���� ��մ�������
        if (UserGetStatus(EndPrimaryAxis) == 0 && UserGetStatus(EndLesserAxis) == 0 && UserGetStatus(PullWireAxis) == 0)
        {
            //20190730
            if(LG->count > 0)
            {
                AxMoveAbsUserUnit(EndLesserAxis, GUS.SpdRatio[EndLesserAxis], GUS.SecondTin.Data.SecondRosinLength_B);
            }
            else
            {
                AxMoveAbsUserUnit(EndLesserAxis, GUS.SpdRatio[EndLesserAxis], GUS.WirePara.Data.B.Para.TinPara.AddRosinLength);
            }
            LG->step = 3;

        }
        break;

    case 3: //�ж��ߵ�������ȵ�λ����ʱ��������
        if (UserGetStatus(EndLesserAxis) == 0)
        {
            LG->step = 4;
        }
        break;

    case 4: //������ʱ������0λ
        if (TimerCnt(LogicTask.BTin) > GUS.WirePara.Data.B.Para.TinPara.RosinDelay)
        {
//			AxMoveRelUserUnit(EndLesserAxis, GUS.SpdRatio[EndLesserAxis], -GUS.WirePara.Data.B.Para.TinPara.AddRosinBackLength);
            if(GUS.WirePara.Data.B.Para.TinPara.AddRosinLength - GUS.WirePara.Data.B.Para.TinBackPos > GUS.WirePara.Data.B.Para.StripLength[singleWire] + 600)
            {
                AxMoveRelUserUnit(EndLesserAxis, GUS.WirePara.Data.B.Para.TinPara.TinSpeed, -GUS.WirePara.Data.B.Para.StripLength[singleWire] - 800);
            }
            else
            {
                AxMoveAbsUserUnit(EndLesserAxis, GUS.SpdRatio[EndLesserAxis], GUS.WirePara.Data.B.Para.TinBackPos);
            }
            LG->step = 5;
        }
        break;

    case 5: //��ת��մ��λ��
        if (UserGetStatus(EndLesserAxis) == 0)
        {
            AxMoveAbsUserUnit(EndPrimaryAxis, GUS.SpdRatio[EndPrimaryAxis], GUS.B_Tin.B_R_TinPos);
            if(GUS.WirePara.Data.B.Para.TinPara.TiningLength - GUS.WirePara.Data.B.Para.TinBackPos > GUS.WirePara.Data.B.Para.StripLength[singleWire] + 1000)
            {
                AxMoveAbsUserUnit(EndLesserAxis, GUS.WirePara.Data.B.Para.TinPara.TinSpeed, GUS.WirePara.Data.B.Para.TinPara.TiningLength - GUS.WirePara.Data.B.Para.StripLength[singleWire] - 1000);
            }
            else
            {
                AxMoveAbsUserUnit(EndLesserAxis, GUS.SpdRatio[EndLesserAxis],  GUS.WirePara.Data.B.Para.TinBackPos);
            }
            LG->step = 6;
        }
        break;

    case 6: //��մ�����
        if (UserGetStatus(EndPrimaryAxis) == 0 && UserGetStatus(EndLesserAxis) == 0)
        {
            if(BSlowTinEn)
            {
                //20190730 ��մ��λ��
                if(LG->count > 0)
                {
                    AxMoveAbsUserUnit(EndLesserAxis, GUS.SecondTin.Data.SecondTinSpeed_B, GUS.SecondTin.Data.SecondSlowTinPos_B);
                }
                else
                {
                    AxMoveAbsUserUnit(EndLesserAxis, GUS.WirePara.Data.B.Para.TinPara.TinSpeed, GUS.WirePara.Data.B.Para.SlowTinPos);
                }
                LG->step = 12;
            }
            else
            {
                if(LG->count > 0)
                {
                    AxMoveAbsUserUnit(EndLesserAxis, GUS.SecondTin.Data.SecondTinSpeed_B, GUS.SecondTin.Data.SecondTinLength_B);
                }
                else
                {
                    AxMoveAbsUserUnit(EndLesserAxis, GUS.WirePara.Data.B.Para.TinPara.TinSpeed, GUS.WirePara.Data.B.Para.TinPara.TiningLength);
                }
                LG->step = 7;
            }
        }
        break;

    case 12:
        if (UserGetStatus(EndLesserAxis) == 0)
        {
            if(LG->count == 0)
            {
                AxMoveAbsUserUnit(EndLesserAxis, GUS.WirePara.Data.B.Para.SlowTinSpeed, GUS.WirePara.Data.B.Para.TinPara.TiningLength);
            }
            else
            {
                AxMoveAbsUserUnit(EndLesserAxis, GUS.SecondTin.Data.SecondSlowTinSpeed_B, GUS.SecondTin.Data.SecondTinLength_B);
            }
            LG->step = 7;
        }
        break;

    case 7: //�ж��ߵ�����ȵ�λ����ʱ��������
        if (UserGetStatus(EndLesserAxis) == 0)
        {
            LG->step = 13;
        }
        break;

    case 13:
        if (TimerCnt(LogicTask.BTin) > GUS.WirePara.Data.B.Para.TinPara.TinDelay)
        {
            if(BSlowTinEn)
            {
                if(LG->count == 0)
                {
                    AxMoveAbsUserUnit(EndLesserAxis, GUS.WirePara.Data.B.Para.SlowTinBackSpeed, GUS.WirePara.Data.B.Para.SlowTinPos);
                }
                else
                {
                    AxMoveAbsUserUnit(EndLesserAxis, GUS.SecondTin.Data.SecondSlowTinBackSpeed_B, GUS.SecondTin.Data.SecondSlowTinPos_B);
                }
            }
            LG->step = 8;
        }
        break;


    case 8: //մ����ʱ������
        if (UserGetStatus(EndLesserAxis) == 0)
        {
            AxMoveAbsUserUnit(EndLesserAxis, GUS.WirePara.Data.B.Para.TinPara.TinSpeed, GUS.WirePara.Data.B.Para.TinBackPos);
            LG->step = 9;
        }
        break;

    case 9: //���մ��
        if (UserGetStatus(EndLesserAxis) == 0)
        {
            LG->count++;
            LG->step = 10;
            if (LG->count < GUS.WirePara.Data.B.Para.TinPara.TinTimes)
            {
                //20190730
                if(BRosinMuchEn)
                {
                    LG->step = 1;
                }
                else
                {
                    LG->step = 6;
                }
            }
        }
        break;

    case 10: //��ת��ˮƽλ�á�մ��������
        if (UserGetStatus(EndPrimaryAxis) == 0 && UserGetStatus(EndLesserAxis) == 0)
        {
            if (BTinEn == 1)
            {
                AxMoveAbsUserUnit(EndLesserAxis, GUS.SpdRatio[EndLesserAxis], GUS.WirePara.Data.B.Para.TinBackPos);
            }
            LG->step = 11;
        }
        break;


    case 11: //
        if (UserGetStatus(EndPrimaryAxis) == 0 && UserGetStatus(EndLesserAxis) == 0
                && (TimerCnt(LogicTask.BTin) > GUS.WirePara.Data.B.Para.TinStayTime || BTinEn == 0) && LogicTask.CutWire.execute == 0)
        {
            if (BTinEn == 1)
            {
                LogicTask.BFeedTin.execute = 1;
                LogicTask.BFeedRosin.execute = 1;
                LogicTask.BSmoothTin.execute = 1;
            }
            AxMoveAbsUserUnit(EndPrimaryAxis, GUS.SpdRatio[EndPrimaryAxis], GUS.FeedWire.EndRFeedPos);
            LG->step = ENDCASE;
        }
        break;

    case ENDCASE: //
        if (UserGetStatus(EndPrimaryAxis) == 0 && UserGetStatus(EndLesserAxis) == 0)
        {
            LG->count = 0;
            LG->step = 0;
            LG->execute = 0;
        }
        break;

    default:
        break;
    }
    if (LG->step != LG->stepBuffer)
    {
        LG->stepBuffer = LG->step;
        TimerRst(LogicTask.BTin);
    }
}



/**
* @author 19/04/28  wangh
* @Description: A������˿
* @param --
* @param --
* @return --
*/
void AFeedTin(LogicParaDef *LG)
{
    INITTASK(LogicTask.AFeedTin);
    switch (LG->step)
    {
    case 1: //���������
        if (1)
        {
            LG->count++;
            if (LG->count >= GUS.WirePara.Data.A.Para.TinPara.AddTinTimes)
            {
                LG->count = 0;
                UserOutputSet(Q_FeedTin, ON);
                LG->step = 2;
            }
            else
            {
                LG->step = ENDCASE;
            }
        }
        break;

    case 2: //������ʱ���ص��������
        if (TimerCnt(LogicTask.AFeedTin) > GUS.WirePara.Data.A.Para.TinPara.AddTinDelay)
        {
            UserOutputSet(Q_FeedTin, OFF);
            LG->step = ENDCASE;
        }
        break;

    case ENDCASE: //
        if (1)
        {
            LG->step = 0;
            LG->execute = 0;
        }
        break;

    default:
        break;
    }
    if (LG->step != LG->stepBuffer)
    {
        LG->stepBuffer = LG->step;
        TimerRst(LogicTask.AFeedTin);
    }
}


/**
* @author 19/04/28  wangh
* @Description: A������˿
* @param --
* @param --
* @return --
*/
void BFeedRosin(LogicParaDef *LG)
{
    INITTASK(LogicTask.BFeedRosin);
    switch (LG->step)
    {
    case 1: //���������
        if (1)
        {
            LG->count++;
            if (LG->count >= GUS.WirePara.Data.A.Para.TinPara.FeedRosinTimes)
            {
                LG->count = 0;
                if(BFeedTinEn)
                UserOutputSet(Q_FeedRosin, ON);
                LG->step = 2;
            }
            else
            {
                LG->step = ENDCASE;
            }
        }
        break;

    case 2: //������ʱ���ص��������
        if (TimerCnt(LogicTask.BFeedRosin) > GUS.WirePara.Data.A.Para.TinPara.FeedRosinDelay)
        {
            UserOutputSet(Q_FeedRosin, OFF);
            LG->step = ENDCASE;
        }
        break;

    case ENDCASE: //
        if (1)
        {
            LG->step = 0;
            LG->execute = 0;
        }
        break;

    default:
        break;
    }
    if (LG->step != LG->stepBuffer)
    {
        LG->stepBuffer = LG->step;
        TimerRst(LogicTask.BFeedRosin);
    }
}



/**
* @author 19/02/21  wangh
* @Description: B������˿
* @param --
* @param --
* @return --
*/
void BFeedTin(LogicParaDef *LG)
{
    INITTASK(LogicTask.BFeedTin);
    switch (LG->step)
    {
    case 1: //���������
        if (1)
        {
            LG->count++;
            if (LG->count >= GUS.WirePara.Data.B.Para.TinPara.AddTinTimes)
            {
                LG->count = 0;
                if(BFeedTinEn)
                UserOutputSet(Q_FeedTin,ON);
                LG->step = 2;
            }
            else
            {
                LG->step = ENDCASE;
            }
        }
        break;

    case 2: //������ʱ���ص��������
        if (TimerCnt(LogicTask.BFeedTin) > GUS.WirePara.Data.B.Para.TinPara.AddTinDelay)
        {
            UserOutputSet(Q_FeedTin, OFF);
            LG->step = ENDCASE;
        }
        break;

    case ENDCASE: //
        if (1)
        {
            LG->step = 0;
            LG->execute = 0;
        }
        break;

    default:
        break;
    }
    if (LG->step != LG->stepBuffer)
    {
        LG->stepBuffer = LG->step;
        TimerRst(LogicTask.BFeedTin);
    }
}





/**
* @author 19/04/28  wangh
* @Description: A�˹���
* @param --
* @param --
* @return --
*/
void ASmoothTin(LogicParaDef *LG)
{
    INITTASK(LogicTask.ASmoothTin);
    switch (LG->step)
    {
    case 1: //
        if (1)
        {
            LG->count++;
            if (LG->count >= GUS.WirePara.Data.A.Para.TinPara.SmothTinTimes)
            {
                LG->count = 0;
                UserOutputSet(Q_FrontScrapeTiniUp, OFF);
                UserOutputSet(Q_FrontScrapeTini, ON);
                LG->step = 2;
            }
            else
            {
                LG->step = ENDCASE;
            }
        }
        break;

    case 2: //
        if (TimerCnt(LogicTask.ASmoothTin) > GUS.SmoothinPara.SmoothTinDelay)
        {
            UserOutputSet(Q_FrontScrapeTiniUp, ON);
            LG->step = 3;
        }
        break;

    case 3: //
        if (TimerCnt(LogicTask.ASmoothTin) > GUS.SmoothinPara.SmoothDownDelay)
        {
            UserOutputSet(Q_FrontScrapeTini, OFF); //�ջ�
            LG->step = 4;
        }
        break;

    case 4: //
        if (TimerCnt(LogicTask.ASmoothTin) > GUS.SmoothinPara.SmoothTinDelay)
        {
            UserOutputSet(Q_FrontScrapeTiniUp, OFF); //̧��
            LG->step = ENDCASE;
        }
        break;

    case ENDCASE: //
        if (1)
        {
            LG->step = 0;
            LG->execute = 0;
        }
        break;

    default:
        break;
    }
    if (LG->step != LG->stepBuffer)
    {
        LG->stepBuffer = LG->step;
        TimerRst(LogicTask.ASmoothTin);
    }
}



/**
* @author 19/02/21  wangh
* @Description: B�˹���
* @param --
* @param --
* @return --
*/
void BSmoothTin(LogicParaDef *LG)
{
    INITTASK(LogicTask.BSmoothTin);
    switch (LG->step)
    {
    case 1: //
        if (1)
        {
            LG->count++;
            if (LG->count >= GUS.WirePara.Data.B.Para.TinPara.SmothTinTimes)
            {
                LG->count = 0;
                UserOutputSet(Q_EndScrapeTiniUp, OFF);
                UserOutputSet(Q_EndScrapeTini, ON);
                LG->step = 2;
            }
            else
            {
                LG->step = ENDCASE;
            }
        }
        break;

    case 2: //
        if (TimerCnt(LogicTask.BSmoothTin) > GUS.SmoothinPara.SmoothTinDelay)
        {
            UserOutputSet(Q_EndScrapeTiniUp, ON);
            LG->step = 3;
        }
        break;

    case 3: //
        if (TimerCnt(LogicTask.BSmoothTin) > GUS.SmoothinPara.SmoothDownDelay)
        {
            UserOutputSet(Q_EndScrapeTini, OFF); //�ջ�
            LG->step = 4;
        }
        break;

    case 4: //
        if (TimerCnt(LogicTask.BSmoothTin) > GUS.SmoothinPara.SmoothTinDelay)
        {
            UserOutputSet(Q_EndScrapeTiniUp, OFF); //̧��
            LG->step = ENDCASE;
        }
        break;

    case ENDCASE: //
        if (1)
        {
            LG->step = 0;
            LG->execute = 0;
        }
        break;

    default:
        break;
    }
    if (LG->step != LG->stepBuffer)
    {
        LG->stepBuffer = LG->step;
        TimerRst(LogicTask.BSmoothTin);
    }
}

/**
* @author 19/02/20  wangh
* @Description: ���Ϻ���
* @param --
* @param --
* @return --
*/
void UnfeedProduct(LogicParaDef *LG)
{
    INITTASK(LogicTask.UnfeedProduct);
	switch (LG->step)
	{
        
    case 1:
        if(1)
        {
            LG->step = 2;
       }
        break;
           
		
	case 2://����ᵽ����λ��
		if(UserGetStatus(EndPrimaryAxis) == 0  && UserGetStatus(EndLesserAxis) == 0) 
		{
			UserOutputSet(Q_LittleClam, OFF);
            AxMoveAbsUserUnit(EndLesserAxis, GUS.SpdRatio[EndLesserAxis], GUS.Unfeed.EndAxisUnFeedPos);
            if(BHelpClamEN != 0 && GUS.WirePara.Data.WrieLength >= WireHelpLength )
            {
                TwoAxMoveRelUserUnit(PullWireAxis,EndLesserAxis, GUS.SpdRatio[EndLesserAxis]+1,(GUS.Unfeed.EndAxisUnFeedPos) - GUR.AxisPositionUserUnit[EndLesserAxis] - GUS.FeedWire.PullStraightLength-10);
            }			
			LG->step = 3;
		}
		break;
		
	case 3: //��Ʒ�������
		if (UserGetStatus(EndLesserAxis) == 0)
		{
			UserOutputSet(Q_ProductClam, ON);
			LG->step = 4;
		}
		break;	
		
	case 4: //��Ʒ��λ С���Ӽ�
		if((TimerCnt(LogicTask.UnfeedProduct) > GUS.Unfeed.ProductClamOnDelay) && UserGetStatus(PullWireAxis) == 0 && UserGetStatus(EndLesserAxis) == 0)
		{
			UserOutputSet(Q_LittleClam, ON);
			LG->step = 5;
		}
		break;
		
	case 5: //��˼����ſ�
		if(TimerCnt(LogicTask.UnfeedProduct) > 30)
		{
			UserOutputSet(Q_EndClam, OFF);
			UserOutputSet(Q_PullWireClam, OFF);
			LG->step = 6;
		}
		break;


	case 6: //��Ʒ�����ջ�
		if(TimerCnt(LogicTask.UnfeedProduct) > GUS.B_Tin.B_ClamOffDelay)
		{
            if(BHelpClamEN != 0 && GUS.WirePara.Data.WrieLength >= WireHelpLength )
            {
                AxMoveAbsUserUnit(PullWireAxis, GUS.SpdRatio[PullWireAxis], 
                MaxS32(0,GUR.AxisPositionUserUnit[PullWireAxis]-GUS.CutWirePara.PullBackLength));
            }
			if(Data.BadWire == 0)
			{
				UserOutputSet(Q_BadProduct, OFF);
			}
			else
			{
				Data.BadStopNum++;
				UserOutputSet(Q_BadProduct, ON);
			}
			Data.BadWire = 0;
			UserOutputSet(Q_ProductClam, OFF);
			
			LG->step = 7;
		}
		break;
		
	case 7:
		if(TimerCnt(LogicTask.UnfeedProduct) > MaxS32(GUS.Unfeed.ProductClamOffDelay ,100))
		{
			LG->done = 1;
			ton_et[1] = 20;
			LG->step = 8;
		}
		break;

	case 8://��Ʒ�����ſ�
		if(1)
		{
			UserOutputSet(Q_LittleClam, OFF);
			LG->step = ENDCASE;
		}
		break;

	case ENDCASE: //
		if(TimerCnt(LogicTask.UnfeedProduct) > 50)
        {
            LG->step = 0;
            LG->execute = 0;
            LG->done = 1;            
        }
		break;

	default:
		break;
    }
    if (LG->step != LG->stepBuffer)
    {
        LG->stepBuffer = LG->step;
        TimerRst(LogicTask.UnfeedProduct);
    }
}

/**
* @author�� 2019/1/9 ũҵ��
* @Description: ����
* @param �C
* @param --
* @return --
*/
void Blow(LogicParaDef *LG)
{
    if (LG->step == 0 && LG->execute == 1)
    {
        LG->step = 1;
        LG->done = 0;
        LG->count = 0;
    }
    if(ton(11,UserGetOutput(Q_Blow) == ON,GUS.PullWire.BlowStarTime))
    {
        UserOutputSet(Q_CutBlow, ON);
    }    
    if(ton(12,UserGetOutput(Q_CutBlow) == ON,GUS.PullWire.BlowTime))
    {
        UserOutputSet(Q_CutBlow, OFF);
    }

    switch (LG->step)
    {
    case 1:
        UserOutputSet(Q_Blow, ON);
        UserTimer[9] = GUS.WirePara.Data.BlowDelay;
        LG->step = 2;
        break;

    case 2:
        if (TimerCnt(LogicTask.Blow) > GUS.PullWire.BlowTime)
        {
            UserOutputSet(Q_Blow, OFF);
            LG->step = 0;
            LG->execute = 0;
            LG->done = 0;
        }
        break;
    }
    if (LG->step != LG->stepBuffer)
    {
        LG->stepBuffer = LG->step;
        TimerRst(LogicTask.Blow);
    }
}

/**
* @author�� 2019/1/9 ����
* @Description: �������������Һ�ǰ��
* @param �C
* @param --
* @return --
*/
void Q_LittleRight_ON(LogicParaDef *LG)
{
    if(UserGetOutput(Q_LittleFront) == OFF)
    {
        LG->done = 0;
    }
    if(LG->step == 0 && LG->execute == 1)
    {
        if(LG->done == 0)
        {
            LG->step = 1;
            LG->done = 0;
            if(UserGetOutput(Q_LittleRight) == ON)
            {
                LG->step = 3;
            }
        }
        else
        {
            LG->execute = 0;
            LG->step = 0;
        }
    }

    switch (LG->step)
    {
    case 1: //����������
        UserOutputSet(Q_LittleRight, ON);
        LG->step = 2;
        break;

    case 2: //��ʱ
        if (InputGetSta(I_LittleRight) == ON)
        {
            LG->step = 3;
        }
        break;

    case 3: //��ʱ���ǰ������
    {
        UserOutputSet(Q_LittleFront, ON);
        LG->step = 4;
    }
    break;

    case 4://����
        if (InputGetSta(I_LittleFrontOn) == ON || (UnFeedHidEN == 0 && TimerCnt(LogicTask.Q_LittleRight_ON) > GUS.Unfeed.LittleFrontOnDelay))
        {
            LG->step = ENDCASE;
        }
        break;

    case ENDCASE:
        LG->execute = 0;
        LG->step = 0;
        LG->done = 1;
        break;
    }
    if (LG->step != LG->stepBuffer)
    {
        LG->stepBuffer = LG->step;
        TimerRst(LogicTask.Q_LittleRight_ON);
    }
}

/**
* @author�� 2019/07/11 ����
* @Description: ������ֶ��� ֻ�����λ��ʹ��
* @param �C
* @param --
* @return --
*/
void PullWireTwoTimes(LogicParaDef *LG)
{
    static int SlowDistance;
    if (LG->step == 0 && LG->execute == 1)
    {
        LG->step = 1;
        LG->done = 0;
        LG->count = 0;
        if(Data.PullTwoTimesPos > 0)
        {
            if(Data.PullTwoTimesPos > GUS.PullWire.PullWireSlowSpeedDistance)
            {
                SlowDistance =  GUS.PullWire.PullWireSlowSpeedDistance;
            }
            else
            {
                SlowDistance =  Data.PullTwoTimesPos;
            }
        }
        else
        {
            if(-Data.PullTwoTimesPos > GUS.PullWire.PullWireSlowSpeedDistance)
            {
                SlowDistance =  -GUS.PullWire.PullWireSlowSpeedDistance;
            }
            else
            {
                SlowDistance =  Data.PullTwoTimesPos;
            }
        }
    }

    switch (LG->step)
    {
    case 1://
        if (UserGetStatus(PullWireAxis) == 0)
        {
            AxMoveRelUserUnit(PullWireAxis, GUS.FeedWire.PullSpeed, Data.PullTwoTimesPos - SlowDistance );
            LG->step = 2;
        }
        break;

    case 2:
        if (UserGetStatus(PullWireAxis) == 0)
        {
            AxMoveRelUserUnit(PullWireAxis, GUS.PullWire.PullWireSpeed, SlowDistance);
            LG->step = 3;
        }
        break;

    case 3:
        if (UserGetStatus(PullWireAxis) == 0)
        {
            Data.PullTwoTimesPos = 0;
            LG->step = ENDCASE;
        }
        break;

    case ENDCASE:
    {
        LG->step = 0;
        LG->execute = 0;
        LG->done = 0;
    }
    break;
    }


    if (LG->step != LG->stepBuffer)
    {
        LG->stepBuffer = LG->step;
        TimerRst(LogicTask.Blow);
    }
}


/**
* @author�� 2019/1/9 ũҵ��
* @Description: B���з���
* @param �C
* @param --
* @return --
*/
void BCutWastage(LogicParaDef *LG)
{
    if(GUS.AMakeTerminal.CutWastageCountSet>50)
    {
        GUS.AMakeTerminal.CutWastageCountSet = 1;
    }
    if (LG->step == 0 && LG->execute == 1)
    {
        LG->step = 1;
        LG->done = 0;
    }

    switch (LG->step)
    {
    case 1:
        LG->step = ENDCASE;
        LG->count++;
        if(LG->count == GUS.AMakeTerminal.CutWastageCountSet)
        {
            LG->count = 0;
            UserOutputSet(Q_BCutWastage, ON);
            LG->step = 2;
        }
        break;

    case 2:
        if (TimerCnt(LogicTask.BCutWastage) > 1000)
        {
            UserOutputSet(Q_BCutWastage, OFF);
            LG->step = ENDCASE;
        }
        break;

    case ENDCASE:
        if (UserTimer[10] == 0)
        {
            LG->step = 0;
            LG->execute = 0;
            LG->done = 0;
        }
        break;
    }
    if (LG->step != LG->stepBuffer)
    {
        LG->stepBuffer = LG->step;
        TimerRst(LogicTask.BCutWastage);
    }
}



//����Ե��⣬clk��0��Ϊ1ʱ���ú�������ֵΪ1�����´���ִ�У�����ֵΪ0; �β� num�������ؼ����
unsigned char Ton_trig(char num, unsigned char clk)
{
    if(num >= 50)
        num = TIMER_NUM - 1;
    if((char)clk != (ton_et[num]))
    {
        ton_et[num] = clk;
        if (clk)
            return 1;
    }
    return 0;
}

/**
* @author�� 2019/1/9 ũҵ��
* @Description: ������
* @param �C
* @param --
* @return --
*/
void CheckPou()
{
//	int static flag = 0;
//	if(InPut_GetSta(I_A_WireCheck)== OFF)
//	{
//		flag = 0;
//	}
//	if(InPut_GetSta(I_A_WireCheck)== ON && flag == 0)
//	{
//		flag = 1;
//		GUW.makeTerminalCountForUser++;
//	}
    //���߼�⣬���ж��Ƿ���Ҫ�����
    if(Triger(3, 0, InputGetSta(I_FrontTerminalCheck_B) == ON, 0))
    {
        Data.BWireCheckFlag++;
    }
    GUW.makeTerminalCountForUser = Data.BWireCheckFlag;
}

/**
* @author 19/02/19  wangh
* @Description: A�˶�������
* @param --
* @param --
* @return --
*/
void AWork(LogicParaDef *LG)
{
    INITTASK(LogicTask.AWork);
    switch (LG->step)
    {
    case 1://���
        if (LG->Ready == 0 && LogicTask.AMakeTerminal.execute == 0)
        {
            LogicTask.AMakeTerminal.execute = 1;
            LG->step = 2;            
        }
        break;

    case 2: //����
        if (LogicTask.AMakeTerminal.execute == 0)
        {
			LogicTask.AInsertPin.done = 0;
            if(AMakeTerminalEn && AInsertPinEn && Data.TerminalErro == 0)
            {
                LogicTask.AInsertPin.execute = 1;
            }
            LG->step = 3;
        }
        break;
        
    case 3: //���
        if (LogicTask.AInsertPin.execute == 0 && LogicTask.Acheck.execute == 0)
        {
            if(LogicTask.AInsertPin.done == 1)
            {
                LogicTask.Acheck.execute = 1;
            }
            LG->step = 4;
        }
        break;

    case 4: //����
        if (LogicTask.ATin.execute == 0 && LogicTask.Acheck.execute == 0 && UserGetStatus(FrontPrimaryAxis) == 0)
        {
            AxMoveAbsUserUnit(FrontPrimaryAxis, GUS.SpdRatio[FrontPrimaryAxis], GUS.FeedWire.FrontRFeedPos);
            LG->step = ENDCASE;
        }
        break;

    case ENDCASE: //
        if(UserGetStatus(FrontPrimaryAxis) == 0)
        {
			if((Data.BadWireEn == 1) || (Data.VisionCheck != 2 
				&& AVisionCheckEn && AMakeTerminalEn && AInsertPinEn && Data.TerminalErro == 0))
			{
				Data.PressAlarmCount++;
			}
			else
			{
				Data.PressAlarmCount = 0;
			}
			
			if(Data.PressAlarmCount+1 > GUS.AMakeTerminal.PressAlarmNum)
			{
				Data.PressAlarmCount = 0;
				AlarmSetBit(2, 0, 14); //������������
				if(GUR.RunStatus == RUN)
				{
					GUW.button.DeviceMode = CYCLEMODE;		
				}
			}
			
            LG->step = 0;
            LG->execute = 0;
        }
        break;

    default:
        break;
    }
    if (LG->step != LG->stepBuffer)
    {
        LG->stepBuffer = LG->step;
        TimerRst(LogicTask.AWork);
    }
}

/**
* @author�� 2020/04/01 ����
* @Description: A�˴����
* @param �C
* @param --
* @return --
*/
void AMakeTerminal(LogicParaDef *LG)
{
    static int FrontMakeTermCount = 0;
    if(GUS.WirePara.Data.ProductSet.WireNum > 20)
    {
        GUS.WirePara.Data.ProductSet.WireNum = 20;
    }
    if(LG->step == 0 && LG->execute == 1)
    {
        LG->step = 1;
        LG->done = 0;
        LG->count = 0;
        RealWireNum = 0;
        FrontMakeTermCount = 0;
        Data.TerminalErro = 0;
    }
    switch(LG->step)
   {
        case 1://��˽���
             if(AMakeTerminalEn && Data.DivideFirstWire == 0)
            {
                LG->step = 2;
            }
            else
            {
                LG->step = 8;
            }
           break;
        
        case 2://���Ƶ����λ��
            if(UserGetStatus(FrontPrimaryAxis) == 0 && (UserGetStatus(FrontLesserAxis) == 0 || GUR.AxisPositionUserUnit[FrontLesserAxis] < 200))
            {
                AxMoveAbsUserUnit(FrontPrimaryAxis, GUS.SpdRatio[FrontPrimaryAxis], GUS.AMakeTerminal.FrontMoveMakeTerminalPos); //ǰ�˺��Ƶ����λ��
                LG->step = 3;
            }
            break;      
            
        case 3://���ǰ��
            if(((UserGetStatus(FrontPrimaryAxis) == 0 || GUR.AxisPositionUserUnit[FrontPrimaryAxis] > (GUS.AMakeTerminal.FrontMoveMakeTerminalPos - 200)) && UserGetStatus(TerminalAxis) == 0)
                && UserGetStatus(FrontLesserAxis) == 0)
            {
                AxMoveAbsUserUnit(FrontLesserAxis, GUS.SpdRatio[FrontLesserAxis], GUS.AMakeTerminal.FrontMakeTerminalPos); //ǰ���ᵽ���λ��
                LG->step = 4;
            }
            break;        

        case 4://��˺��ƣ��жϽ��ã�
            if(UserGetStatus(FrontPrimaryAxis) == 0 && UserGetStatus(FrontLesserAxis) == 0)
            {
                if(WordToBit(GUS.WirePara.Data.A.Para.MakeTerminalEn, FrontMakeTermCount) && Data.TerminalErro == 0)
                {
                    RealWireNum++;
//                    AxisRunDefaultUserUnit(FrontPrimaryAxis, ABSMODE, 11, GUS.AMakeTerminal.FrontMoveMakeTerminalPos  - GUS.WirePara.Data.WireDistance * FrontMakeTermCount
//                    - GUS.WireDistanceOffer.Data.WireDistanceOffer[FrontMakeTermCount]); //���Ƶ����λ ���Ӽ�ಹ��
					if(FrontMakeTermCount != 0)
					{
						AxisRunDefaultUserUnit(FrontPrimaryAxis, RELMODE, 11,-(GUS.WirePara.Data.WireDistance + GUS.WireDistanceOffer.Data.WireDistanceOffer[FrontMakeTermCount])); //���Ƶ����λ ���Ӽ�ಹ��
					}
                    LG->step = 5;
                }
                else
                {
                    LG->step = 6;
                }
            }
            break;
        
        case 5://�����ᵽλ��ʼ���
            if(UserGetStatus(FrontPrimaryAxis) == 0 && LogicTask.AServoTerminal.execute == 0)
            {
				Data.PressAlarmCount = 0; //��˳ɹ�����������ȡ��
				if(Data.TerminalErro == 0)
				{
					LogicTask.AServoTerminal.execute = 1;
				}
                LogicTask.AServoTerminal.done = 0;
                LG->step = 6;
           }
            break;
        
        case 6://���ӻ�̧����жϴ�˸���
            if(LogicTask.AServoTerminal.done == 1 || LogicTask.AServoTerminal.execute == 0)
            {
				ton_et[1] = 10;
                FrontMakeTermCount++;
                if(FrontMakeTermCount < GUS.WirePara.Data.ProductSet.WireNum)
                {
                    LG->step = 4;
                }
                else
                {
                    LG->step = 7;
                }
            }
            break;
        
        case 7://�з���
            if(UserGetStatus(FrontPrimaryAxis) == 0 && UserGetStatus(TerminalAxis) == 0)
            {
				if(Data.TerminalErro != 0)
				{
					Data.BadWireEn = 1;
				}
                LogicTask.BCutWastage.execute = 1;
                LG->step = 8;
            }
           break;
        
        case 8://��˺���ʱ�����Ƴ�ģ��
            if(TimerCnt(LogicTask.AMakeTerminal) > GUS.AMakeTerminal.MakeTerminaDelay && UserGetStatus(FrontLesserAxis) == 0)
            {
                LG->step = 9;
            }
            break;
        
        case 9://
                LG->step = ENDCASE;
            break;  
        
        case ENDCASE://����
            LG->step = 0;
            LG->execute = 0;
            LG->done = 1;
            break;

        default:
            break;
    }

    if (LG->step != LG->stepBuffer)
    {
        LG->stepBuffer = LG->step;
        TimerRst(LogicTask.AMakeTerminal);
    }
}

/**
* @author�� 2020/04/01 ����
* @Description: A�˶��ӻ��ŷ�������
* @param �C
* @param --
* @return --
*/
void AServoTerminal(LogicParaDef *LG)
{
    static int PosCache = 0;
    if(LG->step == 0 && LG->execute == 1)
    {
        LG->step = 1;
        LG->done = 0;
        if(InputGetSta(I_TerminalCtrol) == ON)
        {
            LG->step = 3;
        }
    }
    switch(LG->step)
   {
        case 1://����ŷ���
            if(UserGetStatus(TerminalAxis) == 0)
            {
                AxMoveRelUserUnit(TerminalAxis, GUS.SpdRatio[TerminalAxis], 36000); //תһȦ
                PosCache = GUR.AxisPositionUserUnit[TerminalAxis];
				GUS.Product.TerminalNum++;
                LG->step = 2;
            }
            break;
        
        case 2://�ж�λ�ø��������ź�
            if(GUR.AxisPositionUserUnit[TerminalAxis] > (PosCache + 34000) || UserGetStatus(TerminalAxis) == 0)
            {
                LG->done = 1;
                LG->step = 3;
            }
            break;        

        case 3://��λ����˽���
            if(UserGetStatus(TerminalAxis) == 0)
            {
                LG->done = 1;
                LG->step = 4;
            }
            break;
        
        case 4://����
                LG->execute = 0;
                LG->done = 1;
                LG->step = 0;
            break;

        default:
            break;
    }

    if (LG->step != LG->stepBuffer)
    {
        LG->stepBuffer = LG->step;
        TimerRst(LogicTask.AServoTerminal);
    }
}

/**
* @author�� 2020/04/01 ����
* @Description: A�˴���
* @param �C
* @param --
* @return --
*/
void AInsertPin(LogicParaDef *LG)
{
    if(LG->step == 0 && LG->execute == 1)
    {
        LG->step = 1;
        LG->done = 0;
    }
    switch(LG->step)
   {
        case 1://���Ƶ�����λ��
            if(UserGetStatus(FrontPrimaryAxis) == 0 && UserGetStatus(InsertTerminalAxis) == 0)
            {
                AxMoveAbsUserUnit(FrontPrimaryAxis, GUS.SpdRatio[FrontPrimaryAxis], GUS.AInsertPin.Data.FrontMoveInsertPinPos); 
                AxMoveAbsUserUnit(InsertTerminalAxis, GUS.SpdRatio[InsertTerminalAxis], GUS.AFeedPin.Data.TakePinPos); 
                LG->step = 2;
                if(AInsertPinEn == 0 || Data.TerminalErro != 0)
                {
                    LG->step = 11;
                }
            }
            break;
        
        case 2://���Ƶ�һ��λ�õ�ʱ��ǰ���ᵽ�̶�����λ�ã�ǰ�����״�
            if((UserGetStatus(FrontPrimaryAxis) == 0 || GUR.AxisPositionUserUnit[FrontPrimaryAxis] < (GUS.AMakeTerminal.FrontMakeTerminalPos + GUS.AInsertPin.Data.FrontMoveInsertPinPos)/2))
            {
                AxMoveAbsUserUnit(FrontLesserAxis, GUS.SpdRatio[FrontLesserAxis], GUS.AInsertPin.Data.FrontAxisInsertPinPos); 
                if(AInsertPinEn == 0 || Data.TerminalErro != 0)
                {
                    LG->step = 11;
                }
                else
                {
                    UserOutputSet(Q_FrontPush, ON);
                    LG->step = 3;
                }
            }
            break;        

        case 3://��λ�󣬶��ӻ���λ�м�ס��
            if((Data.FeedPinDone == 1 || AInsertPinEn == 0)&& InputGetSta(I_PinON) == ON &&
				TimerCnt(LogicTask.AInsertPin) > GUS.AInsertPin.Data.PushOnDelay && UserGetStatus(FrontLesserAxis) == 0 && UserGetStatus(InsertTerminalAxis) == 0)
            {
                UserOutputSet(Q_PinLocatUp, ON);
                UserOutputSet(Q_PinLocatDown, ON);
                LG->step = 4;
            }
            break;
        
        case 4://ѹ���ɿ�
            if(TimerCnt(LogicTask.AInsertPin) > GUS.AInsertPin.Data.PinLocatONDelay)
            {
                //ǰ��ѹ���ɿ�
                UserOutputSet(Q_FrontPress, OFF);
                //ǰ�˼����ſ�
                UserOutputSet(Q_FrontClam, ON);
                LG->step = 5;
           }
            break;
        
        case 5://���
            if(TimerCnt(LogicTask.AInsertPin) > GUS.AInsertPin.Data.PressOnDelay)
            {
                AxMoveAbsUserUnit(InsertTerminalAxis, GUS.SpdRatio[InsertTerminalAxis], GUS.AInsertPin.Data.FirstPinDistance); 
				TwoAxMoveRelUserUnit(FrontLesserAxis, InsertTerminalAxis, GUS.SpdRatio[InsertTerminalAxis] , GUS.AFeedPin.Data.TakePinPos-GUS.AInsertPin.Data.FirstPinDistance);
                LG->step = 6;
            }
            break; 
        
        case 6://ѹ�ߣ�ǰ��
            if(UserGetStatus(InsertTerminalAxis) == 0)
            {
                UserOutputSet(Q_FrontPress, ON);
                UserOutputSet(Q_FrontClam, OFF);
                LG->step = 7;
            }
            break;  
        
        case 7://���Ӷ�λ�ɿ�
            if(TimerCnt(LogicTask.AInsertPin) > GUS.AInsertPin.Data.PressOffDelay)
            {
                UserOutputSet(Q_PinLocatUp, OFF);
                UserOutputSet(Q_PinLocatDown, OFF);
                LG->step = 8;
            }
            break;        
        
        case 8://���β��
            if(TimerCnt(LogicTask.AInsertPin) > GUS.AInsertPin.Data.PinLocatOFFDelay)
            {
                AxMoveAbsUserUnit(FrontLesserAxis, GUS.AInsertPin.Data.InsertPinSpeed, GUS.AInsertPin.Data.SecondPinDistance); 
                LG->step = 9;
            }
            break;    
        
        case 9://���Ƴ�������λ��
            if(UserGetStatus(FrontLesserAxis) == 0)
            {
                AxMoveAbsUserUnit(FrontPrimaryAxis, GUS.AInsertPin.Data.InsertDoneSpeed, GUS.AInsertPin.Data.EndPos_FrontMove); 
                LG->step = 10;
            }
            break;    
            
        case 10://ǰ���ᵽ����λ��
            if(UserGetStatus(FrontPrimaryAxis) == 0)
            {
                Data.FeedPinDone = 0;
                AxMoveAbsUserUnit(FrontLesserAxis, GUS.SpdRatio[FrontLesserAxis], GUS.AInsertPin.Data.EndPos_Front); 
                LG->step = 11;
            }
            break;   
            
        case 11://���Ƴ�������λ��
            if(UserGetStatus(FrontLesserAxis) == 0 && UserGetStatus(FrontPrimaryAxis) == 0)
            {
                AxMoveAbsUserUnit(InsertTerminalAxis, GUS.SpdRatio[InsertTerminalAxis], GUS.AFeedPin.Data.TakePinPos); 
                LG->step = ENDCASE;
            }
            break;
        
        case ENDCASE://����
            LG->execute = 0;
            LG->step = 0;
            LG->done = 1;
        break;

        default:
            break;
    }
    if (LG->step != LG->stepBuffer)
    {
        LG->stepBuffer = LG->step;
        TimerRst(LogicTask.AInsertPin);
    }
}


/**
* @author�� 2020/04/01 ����
* @Description: A�˼��
* @param �C
* @param --
* @return --
*/
void Acheck(LogicParaDef *LG)
{
    if(LG->step == 0 && LG->execute == 1)
    {
        LG->step = 1;
        LG->done = 0;
    }
    switch(LG->step)
   {
        case 1://���Ƶ����λ��
            if(UserGetStatus(FrontPrimaryAxis) == 0)
            {
                AxMoveAbsUserUnit(FrontPrimaryAxis, GUS.SpdRatio[FrontPrimaryAxis], GUS.Acheck.Data.CheckPos_Move); 
                LG->step = 2;
            }
            break;
        
        case 2://ǰ�󵽼��λ
            if(UserGetStatus(FrontPrimaryAxis) == 0|| (GUR.AxisPositionUserUnit[FrontPrimaryAxis] < (GUS.Acheck.Data.CheckPos_Move + GUS.AInsertPin.Data.FrontMoveInsertPinPos)/2))
            {
                AxMoveAbsUserUnit(FrontLesserAxis, GUS.SpdRatio[FrontLesserAxis], GUS.Acheck.Data.CheckPos_Front); 
                LG->step = 3;
                if(AVisionCheckEn == 0)
                {
                    LG->step = 7;
                }
            }
            break;        

        case 3://�����Ӿ�����
            if(UserGetStatus(FrontPrimaryAxis) == 0 && UserGetStatus(FrontLesserAxis) == 0)
            {
				Data.VisionCheck = 1;
                LG->step = 4;
            }
            break;
        
        case 4://
            if(TimerCnt(LogicTask.Acheck) > GUS.Acheck.Data.CheackDelay)
            {
                UserOutputSet(Q_VisionTrig, ON);
                LG->step = 5;
            }
            break;             
        
        case 5://
            if(TimerCnt(LogicTask.Acheck) > 10)
            {
                UserOutputSet(Q_VisionTrig, OFF);
                LG->step = 6;
            }
            break; 
        
        case 6://����Ӿ����
            if(TimerCnt(LogicTask.Acheck) > 50)
				LG->step = 7;
            break;     
        
        case 7://ǰ�����˵������λ
            if(UserGetStatus(FrontPrimaryAxis) == 0)
            {
                AxMoveAbsUserUnit(FrontLesserAxis, GUS.SpdRatio[FrontLesserAxis], GUS.Acheck.Data.CheckBackPos); 
                LG->step = 8;
            }
            break;     
        
        case 8://ѹ�߼��ſ�
            if(1)
            {
                UserOutputSet(Q_FrontPress, OFF);
                LG->step = 9;
            }
            break;     
        
        case 9://ǰ���ջ�
            if(TimerCnt(LogicTask.Acheck) > GUS.Acheck.Data.PressOn)
            {
                UserOutputSet(Q_FrontPush, OFF);
                LG->step = 10;
            }
            break;     
        
        case 10://�������˻ص�����λ��
            if(InputGetSta(I_PinOFF) == ON && UserGetStatus(FrontLesserAxis) == 0)
            {
                AxMoveAbsUserUnit(FrontPrimaryAxis, GUS.SpdRatio[FrontPrimaryAxis], GUS.Acheck.Data.EndPos_FrontMove); 
                LG->step = 11;
            }
            break;     
        
        case 11://����
            if(UserGetStatus(FrontPrimaryAxis) == 0)
            {
                LG->step = ENDCASE;
            }
            break; 
            
        case ENDCASE://����
            LG->step = 0;
            LG->execute = 0;
            break;

        default:
            break;
    }

    if (LG->step != LG->stepBuffer)
    {
        LG->stepBuffer = LG->step;
        TimerRst(LogicTask.Acheck);
    }
}


/**
* @author�� 2020/04/01 ����
* @Description: A�˼��
* @param �C
* @param --
* @return --
*/
void AFeedPin(LogicParaDef *LG)
{
    if(Data.FeedPinDone == 0 && LG->execute == 0 && AInsertPinEn)
    {
       LG->execute = 1; 
    }
    if(LG->step == 0 && LG->execute == 1)
    {
        LG->step = 1;
        LG->done = 0;
        Data.FeedPinCount = 0;
    }
    switch(LG->step)
   {
        case 1://��ȡ��λ��
            if(UserGetStatus(InsertTerminalAxis) == 0)
            {
                LG->step = 2;
            }
            break;
        
        case 2://�ж��Ͽ�����
            if(UserGetStatus(InsertTerminalAxis) == 0 && UserGetStatus(PinFeedAxis) == 0)
            {
                if(Data.FeedPinCount < GUS.AFeedPin.Data.FeedNum)
                {//�Ͽ�
                    LG->step = 3;
                }
                else
                {//�Ͽ����
                    LG->step = 7;
                }
            }
            break;        

        case 3://�Ͽ�
            if(UserGetStatus(PinFeedAxis) == 0)
            {
                if(PinCheackEn)
                {
                    //��⽺�Ǹ�Ӧ
                    if(InputGetSta(I_PinCheck) == ON)
                    {
                        AxMoveAbsUserUnit(PinFeedAxis, GUS.SpdRatio[PinFeedAxis], GUS.AFeedPin.Data.FeedPos[Data.FeedPinCount]);
                        Data.FeedPinCount++;
                        LG->step = 4;
                    }
                }
                else
                {
                    AxMoveAbsUserUnit(PinFeedAxis, GUS.SpdRatio[PinFeedAxis], GUS.AFeedPin.Data.FeedPos[Data.FeedPinCount]);
                    Data.FeedPinCount++;
                    LG->step = 4;
                }
            }
            break;
            
        case 4://�˻�
            if(UserGetStatus(PinFeedAxis) == 0)
            {
                LG->step = 5;
            }
            break;  
			
        case 5://�˻�
            if(TimerCnt(LogicTask.AFeedPin) > 80)
            {
                AxMoveAbsUserUnit(PinFeedAxis, GUS.SpdRatio[PinFeedAxis], GUS.AFeedPin.Data.FeedBackPos);
                LG->step = 6;
            }
            break;
            
        case 6://�Ͽ�ʱ��
            if(TimerCnt(LogicTask.AFeedPin) > GUS.AFeedPin.Data.FeedStay)
            {
                LG->step = 2;
            }
            break;
        
        case 7://����
            if(UserGetStatus(InsertTerminalAxis) == 0 && UserGetStatus(PinFeedAxis) == 0)
            {
                LG->step = ENDCASE;
            }
            break; 
            
        case ENDCASE://����
            LG->step = 0;
            LG->execute = 0;
            LG->done = 1;
            Data.FeedPinDone = 1;
            break;

        default:
            break;
    }

    if (LG->step != LG->stepBuffer)
    {
        LG->stepBuffer = LG->step;
        TimerRst(LogicTask.AFeedPin);
    }
}

/**
* @author�� 2019/1/9 wangh
* @Description: B�˴����
* @param �C
* @param --
* @return --
*/
void BMakeTerminal(LogicParaDef *LG)
{
////	int i;
//    static int I_TerinalUp_B_Termp;
//    static int EndMakeTermCount = 0;
//    if(GUS.WirePara.Data.ProductSet.WireNum > 20)
//    {
//        GUS.WirePara.Data.ProductSet.WireNum = 20;
//    }
//    if(LG->step == 0 && LG->execute == 1)
//    {
//        LG->step = 1;
//        LG->done = 0;
//        LG->count = 0;
//        EndMakeTermCount = 0;
//        RealWireNum = 0;
//        Data.TerminalErro = 0;
//    }

//    switch(LG->step)
//    {
//    case 1://���Ƶ����λ��
//        if(BMakeTerminalEn && Data.DivideFirstWire == 0)
//        {

//            AxMoveAbsUserUnit(EndMoveAxis, GUS.SpdRatio[EndMoveAxis], GUS.BFeedWire.EndMoveTeTerminalPos); //
//            I_TerinalUp_B_Termp = InputGetSta(I_TerinalUp_B);
//            LG->step = 2;
//        }
//        else
//        {
//            if(UnFeedHidEN)
//            {
//                AxMoveAbsUserUnit(EndMoveAxis, GUS.SpdRatio[EndMoveAxis], GUS.BMakeTerminal.UnFeedPos_EndMove - GUS.Unfeed.UnFeedHidDistance); //
//            }
//            LG->step = 8;
//        }
//        break;

//    case 2://��˺��ƣ��жϴ���Ƿ���ã�
//        if(UserGetStatus(EndMoveAxis) == 0)
//        {
//            if(UnFeedHidEN && LogicTask.UnfeedProduct.execute == 0 && GUW.button.DeviceMode == NORMALMODE)
//            {
//                LogicTask.Q_LittleRight_ON.execute = 1;
//            }
//            if(WordToBit(GUS.WirePara.Data.B.Para.MakeTerminalEn, EndMakeTermCount) && Data.TerminalErro == 0)
//            {
//                RealWireNum++;
//                if(FrontSeparateWireEN)
//                {
//                    AxisRunDefaultUserUnit(EndMoveAxis, ABSMODE, 10, GUS.BFeedWire.EndMoveTeTerminalPos  + GUS.WirePara.Data.WireDistance * EndMakeTermCount
//                    + GUS.WireDistanceOffer.Data.WireDistanceOffer[EndMakeTermCount]); //���Ƶ����λ ���Ӽ�ಹ��
//                }
//                else
//                {                    
//                    AxisRunDefaultUserUnit(EndMoveAxis, ABSMODE, 10, GUS.BFeedWire.EndMoveTeTerminalPos  + GUS.WirePara.Data.WireDistance * EndMakeTermCount); //���Ƶ����λ
//                }
//                LG->step = 3;
//            }
//            else
//            {
//                LG->step = 6;
//            }
//        }
//        break;

//    case 3://�����ᵽλ��ʼ�����:�򿪶��ӻ�
//        if(UserGetStatus(EndMoveAxis) == 0)
//        {
//            if(TerminalEn && Data.TerminalErro == 0)
//            {
//                UserOutputSet(Q_MakeTerminal, ON);
//            }
//            LG->step = 4;
//        }
//        break;

//    case 4://��Ӧ�ϸ�Ӧ�뿪 �����ʱ
//        ton_et[1] = 0;
//        if(TimerCnt(LogicTask.BMakeTerminal) > 10)
//        {
//            UserOutputSet(Q_MakeTerminal, OFF);
//        }
//        if(InputGetSta(I_TerinalUp_B) != I_TerinalUp_B_Termp || TimerCnt(LogicTask.BMakeTerminal) > 1000)
//        {
//            UserOutputSet(Q_MakeTerminal, OFF);
//            LG->step = 5;
//        }
//        break;

//    case 5://��Ӧ�ϸ�Ӧ����
//        ton_et[1] = 0;
//        if(InputGetSta(I_TerinalUp_B) == I_TerinalUp_B_Termp || TimerCnt(LogicTask.BMakeTerminal) > 1000)
//        {
//            ton_et[1] = 20;
//            LG->step = 6;
//        }
//        break;

//    case 6:
//        EndMakeTermCount++;
//        if(EndMakeTermCount < GUS.WirePara.Data.ProductSet.WireNum)
//        {
//            if(WordToBit(GUS.WirePara.Data.B.Para.MakeTerminalEn, EndMakeTermCount))
//            {
//                if(TerminalEn && GUW.button.DeviceMode != STEPMODE && Data.TerminalErro == 0)
//                {
//                    UserOutputSet(Q_MakeTerminal, ON);
//                }
//            }
//            LG->step = 2;
//        }
//        else
//        {
//            UserOutputSet(Q_MakeTerminal, OFF);
//            LG->step = 7;
//        }
//        break;

//    case 7://�з���
//        if(UserGetStatus(EndMoveAxis) == 0)
//        {
//            LogicTask.BCutWastage.execute = 1;
//            LG->step = 8;
//        }
//        break;

//    case 8://�����ʱ 20190708
//        if(UnFeedHidEN && LogicTask.UnfeedProduct.execute == 0 && GUW.button.DeviceMode == NORMALMODE && UserGetStatus(EndMoveAxis) == 0)
//        {
//            LogicTask.Q_LittleRight_ON.execute = 1;
//        }
//        if(TimerCnt(LogicTask.BMakeTerminal) > GUS.BMakeTerminal.MakeTerminaDelay)
//        {
//            Data.BWireCheckFlag = 0;
//            LG->step = 9;
//        }
//        break;

//    case 9://������λ��
//        if(UnFeedHidEN && LogicTask.UnfeedProduct.execute == 0 && GUW.button.DeviceMode == NORMALMODE && UserGetStatus(EndMoveAxis) == 0)
//        {
//            LogicTask.Q_LittleRight_ON.execute = 1;
//        }
//        if(UserGetStatus(EndMoveAxis) == 0)
//        {
//            if(InputGetSta(I_LittleFrontOn) == ON || UnFeedHidEN == 0)
//            {
//                AxMoveAbsUserUnit(EndMoveAxis, GUS.SpdRatio[EndMoveAxis], GUS.BMakeTerminal.UnFeedPos_EndMove); //
//            }
//            LG->step = 10;
//        }
//        break;

//    case 10://���
//        Data.TerminalErro = 0;       
//        LG->step = ENDCASE;
//        break;

//    case ENDCASE:
//        LG->step = 0;
//        LG->execute = 0;
//        break;

//    default:
//        break;
//    }

//    if (LG->step != LG->stepBuffer)
//    {
//        LG->stepBuffer = LG->step;
//        TimerRst(LogicTask.BMakeTerminal);
//    }
}


/**
* @author 19/02/19  wangh
* @Description: B�˶�������
* @param --
* @param --
* @return --
*/
void BWork(LogicParaDef *LG)
{
    INITTASK(LogicTask.BWork);
    switch (LG->step)
    {
    case 1: //
        if (LG->Ready == 0)
        {
            LG->step = 2;
        }
        break;

    case 2: //
        if (LogicTask.BTin.execute == 0 && Data.Stop == 0)
        {
            LG->step = 3;
        }
        break;

    case 3: //
        if (LogicTask.BFeedWire.execute == 0)
        {
            LogicTask.BTin.execute = 1;
            LG->step = 4;
        }
        break;


    case 4: //
		if (LogicTask.UnfeedProduct.execute == 0 && LogicTask.BMakeTerminal.execute == 0 && LogicTask.BTin.execute == 0)
        {
            LogicTask.UnfeedProduct.execute = 1;
            LogicTask.UnfeedProduct.done = 0;
            LG->step = 5;
        }
        break;

    case 5: //
        if((LogicTask.UnfeedProduct.done == 1 && GUW.button.DeviceMode == NORMALMODE && GUW.StopCache == 0) || LogicTask.UnfeedProduct.execute == 0)
        {
//            if(FrontSeparateWireEN)
//            {
//                if(GUS.Product.DivideWireNum > 20)
//                {
//                    GUS.Product.DivideWireNum = 1;
//                }
//                GUS.Product.CurrNum += GUS.Product.DivideWireNum;
//                GUS.SumCount.SumProduactCount += GUS.Product.DivideWireNum;
//            }
//            else
//            {
//                GUS.Product.CurrNum += GUW.RealWireNum;
//                GUS.SumCount.SumProduactCount += GUW.RealWireNum;
//            }
                GUS.Product.CurrNum += GUS.Product.AddNum;
                Data.OKStopNum += GUS.Product.AddNum;
				
                GUS.SumCount.SumProduactCount += GUS.Product.AddNum;
            LG->done = 1;
            GUS.Product.CurrPitchNum = Remainder(GUS.Product.CurrNum, GUS.Product.PitchNum);
            
            //�������ʱ�������Ͱ�����ǰ����
            if(LogicTask.CutWire.execute == 0 && UserGetStatus(EndLesserAxis) == 0 && UserGetStatus(PullWireAxis) == 0)
            {
                if(ShortMode == 0)
                {
                    AxMoveAbsUserUnit(EndLesserAxis, GUS.SpdRatio[EndLesserAxis], GUS.FeedWire.EndFeedPos);
                    AxMoveAbsUserUnit(PullWireAxis, GUS.SpdRatio[PullWireAxis], GUS.FeedWire.PullFeedPos);
                }
                else
                {
                    AxMoveAbsUserUnit(EndLesserAxis, GUS.SpdRatio[EndLesserAxis], GUS.FeedWire.EndAxisPullPos);
                }
            }
            LG->step = ENDCASE;
        }
        break;

    case ENDCASE: //
        if (1)
        {
            LG->step = 0;
            LG->execute = 0;
        }
        break;

    default:
        break;
    }
    if (LG->step != LG->stepBuffer)
    {
        LG->stepBuffer = LG->step;
        TimerRst(LogicTask.BWork);
    }
}

/**
* @author 19/02/25  wangh
* @Description: ��������
* @param --
* @param --
* @return --
*/
void SunCount()
{
    if (GUS.SumCount.SumProduactCount >= 1000)
    {
        GUS.SumCount.SumProduactCount_k++;
        GUS.SumCount.SumProduactCount = 0;
    }
    if (GUS.SumCount.SumProduactCount_k >= 1000)
    {
        GUS.SumCount.SumProduactCount_kk++;
        GUS.SumCount.SumProduactCount_k = 0;
    }
    if (GUS.SumCount.SumProduactCount_kk >= 1000)
    {
        GUS.SumCount.SumProduactCount_kkk++;
        GUS.SumCount.SumProduactCount_kk = 0;
    }
    if (GUS.SumCount.SumProduactCount_kkk >= 1000)
    {
        GUS.SumCount.SumProduactCount_kkkk++;
        GUS.SumCount.SumProduactCount_kkk = 0;
    }
}

/**
* @author 19/02/19  wangh
* @Description: ���߼����̵���
* @param --
* @param --
* @return --
*/
void RunProcess(LogicParaDef *LG)
{
	static float BadPercent = 0;
    INITTASK(LogicTask.RunProcess);
    switch (LG->step)
    {
    case 1: //����
        if (LogicTask.FeedWire.execute == 0)
        {
            GUW.StopCache = 0;
            UserTimer[0] = 100000;
            LogicTask.FeedWire.execute = 1;
            LG->step = 2;
        }
        break;

    case 2: //����
        if (LogicTask.FeedWire.execute == 0 && LogicTask.CutWire.execute == 0 && LogicTask.FirstFeedWire.execute == 0)
        {
            LogicTask.AWork.Ready = 1;
            LogicTask.BWork.Ready = 1;
            LogicTask.CutWire.execute = 1;
            LogicTask.CutWire.done = 0;
            LG->step = 3;
        }
        break;

    case 3: //ǰ��
        if (1)
        {
            LogicTask.AWork.execute = 1;
            LogicTask.BWork.execute = 1;
            LG->step = 4;
        }
        break;

    case 4: //ǰ����
        if (1)
        {
            LogicTask.BWork.done = 0;
            LG->step = 5;
        }
        break;

    case 5: //����
        if (LogicTask.AWork.execute == 0 && (LogicTask.BWork.execute == 0 || (GUW.button.DeviceMode == NORMALMODE && LogicTask.BWork.done == 1 && Data.TerminalErro == 0)))
        {
            GUW.HadToResset = 1;
//            GUW.UPH = 3600000 / (100000 - UserTimer[0]);
            GUW.UPH = 100000 - UserTimer[0];
//            GUW.UPH = GUW.UPH * GUW.RealWireNum;
            LG->step = ENDCASE;
            //�ж϶��������Ƿ񵽴�
            GUS.Product.CurrPitchNum = Remainder(GUS.Product.CurrNum, GUS.Product.PitchNum);
            
//            if(FrontSeparateWireEN)
//            {
//                if(GUS.Product.DivideWireNum > 20)
//                {
//                    GUS.Product.DivideWireNum = 1;
//                }
//                GUS.Product.CurrPitchNum = Remainder((GUS.Product.CurrNum + GUS.Product.DivideWireNum), GUS.Product.PitchNum);
//            }
//            else
//            {
//                GUS.Product.CurrPitchNum = Remainder((GUS.Product.CurrNum + GUW.RealWireNum), GUS.Product.PitchNum);
//            }
		    GUS.Product.CurrPitchNum = Remainder((GUS.Product.CurrNum + GUS.Product.AddNum), GUS.Product.PitchNum);
            //�ж϶��������Ƿ񵽴�
            if (GUS.Product.CurrPitchNum >= GUS.Product.PitchNum && GUS.Product.PitchNum > 0)
            {
                GUS.Product.CurrPitchNum = 0;
                GUS.Product.Pitch++;
                //���ͣ����ʱλ0�����Զ�ͣ��
                if (GUS.WirePara.Data.StopDelay == 0)
                {
                    AlarmSetBit(2, 0, 5); //����������
                }
                else
                {
                    Data.PitchNumOK = 1;
                    UserTimer[7] = GUS.WirePara.Data.StopDelay;
                    AlarmSetBit(1, 0, 9);  //��������
                    LG->step = 6;
                }
            }
			if(Data.BadStopNum > GUS.PullWire.StopNum)
			{
				BadPercent = (float)Data.BadStopNum/Data.OKStopNum;
				if((int)BadPercent*1000 > GUS.PullWire.StopNum)
				{
					Data.BadStopNum = 0;
					Data.OKStopNum = 0;
					AlarmSetBit(2, 0, 15); //�����ʵ���ͣ��
				}
				
			}
            if (GUS.Product.CurrNum >= GUS.Product.NumSet)
            {
                GUS.Product.CurrNum = 0;
                AlarmSetBit(2, 0, 4); //��������
            }
        }
        break;

    case 6: //������ͣʱ��
        GUW.CurrStopDelay = UserTimer[7];
        if (TimerCnt(LogicTask.RunProcess) > GUS.WirePara.Data.StopDelay)
        {
            Data.PitchNumOK = 0;
            AlarmClearBit(1, 0, 9);  //��������
            if(GSR.ErrorLevel == 1)//
            {
                GSR.ErrorLevel = 0;
            }
            LG->step = ENDCASE;
        }
        break;

    case ENDCASE: //
        if (GUW.button.DeviceMode == CYCLEMODE || GUW.StopCache != 0 || Data.TerminalErro == 1)
        {
            if(LogicTask.UnfeedProduct.execute == 0 && LogicTask.BWork.execute == 0)
            {
//                Data.TerminalErro = 0;
                GUW.StopCache = 0;
                GUW.button.RunCommand = STOP;
                LG->step = 1;
            }
        }
        else
        {
            LG->step = 1;
        }
        break;

    default:
        break;
    }
    if (LG->step != LG->stepBuffer)
    {
        LG->stepBuffer = LG->step;
        TimerRst(LogicTask.RunProcess);
    }
}

void UnfeedPaper()
{
//	static int Flag = 0;
//	if (InputGetSta(I_Scram) == OFF)
//	{
//		if(ton(7,InputGetSta(I_CollectPaperCheck) == ON,4000))
//		{
//			Flag = 1;
//			AlarmSetBit(2, 0, 9); //������ֽ�쳣
//		}
//		else
//		{
//			Flag = 0;
//			AlarmClearBit(2, 0, 9); //������ֽ�쳣
//		}
//
//		if (InputGetSta(I_CollectPaperCheck) == ON && Flag == 0)
//		{
//			UserOutputSet(Q_CollectPaper, ON);
//		}
//		else
//		{
//			UserOutputSet(Q_CollectPaper, OFF);
//		}
//	}
}


void ButtonMotion() //��ť����
{
    if(Triger(0, 0, InputGetSta(I_StartButton) == ON,50))
    {

        if(GSR.ErrorLevel < 2)//
        {
            Data.Stop = 0;
        }
        if (GUR.RunStatus == STOP)
        {
			if(Data.AddRosinAlarm == 1)
			{
				Data.AddRosinAlarm = 2;
				GUW.AddRosinHIM = 1;
			}
			else
			{
				GUW.button.DeviceMode = NORMALMODE;
				GUW.button.RunCommand = RUN; //��״̬����д����
				LogicTask.RunProcess.execute = 1;
				GUW.button.StepRun = 1;
			}
        }
        GUW.button.StepRun = 1;
    }

    if(Triger(1, 0, InputGetSta(I_StepRun) == ON, 50))
    {

        if(GSR.ErrorLevel < 2)//
        {
            Data.Stop = 0;
        }
        if(GUR.RunStatus == STOP)
        {
            GUW.button.DeviceMode = STEPMODE;
            GUW.button.RunCommand = RUN; //��״̬����д����
            LogicTask.RunProcess.execute = 1;
            GUW.button.StepRun = 1;
        }
        GUW.button.StepRun = 1;
    }

    //ֹͣ��ť
    if (Triger(2, 0, InputGetSta(I_StopButton) == ON,50))
    {
        if (GUR.RunStatus == RUN)
        {
            //			GUW.button.RunCommand = STOP; //��״̬����дֹͣ
            GUW.StopCache = 2;
        }
    }




    //��ͣ��ť
    if (ton(10, InputGetSta(I_EmergencyButton) == OFF, 50))
    {
        ResetWhichBitU16(&GUW.button.BitControlWord, AxisPowerOn);
        GUW.button.RunCommand = SCRAM;
    }
}

//�䷽����
void FileOperate()
{
    u32 j = 0;
    static u32 Num = 0;
    if(GUS.WirePara.Data.FileCheak > 1)
    {
       GUS.WirePara.Data.FileCheak = 0; 
    }
    if(Num > 19)
    {
        Num = 19;
    }
    if(GUS.FileNum > 19)
    {
        GUS.FileNum = Num;
    }
    if(ShortModeForUser && GUS.FileNum != 20)
    {
        Num = GUS.FileNum;
        GUS.FileNum = 20;
    }
    if(Data.DivideFirstWire > 0)
    {
        SetWhichBitU16(&GUS.FunctionEn,24);
    }
    else
    {
        ResetWhichBitU16(&GUS.FunctionEn,24);
    }
    

    if(GUS.FileNumTerm != GUS.FileNum)
    {
        if(GUS.WirePara.Data.FileCheak == 1)
        {
            for(j = 0; j < WireParaDataLength - 1; j++)
            {
                GUS.RepicData[GUS.FileNumTerm].Data_32[j] = GUS.WirePara.Data_32[j];
            }
            GUS.RepicData[GUS.FileNumTerm].Data.AddData[0] = GUS.BFeedWire.EndAxisUnfeedPos;
            GUS.RepicData[GUS.FileNumTerm].Data.AddData[1] = GUS.FeedWire.FrontFeedPos;
            GUS.RepicData[GUS.FileNumTerm].Data.AddData[2] = GUS.FeedWire.EndFeedPos;

            GUS.RepicData[GUS.FileNumTerm].Data.AddData[3] = GUS.CutWirePara.StripBackPos_A;
            GUS.RepicData[GUS.FileNumTerm].Data.AddData[4] = GUS.CutWirePara.StripBackPos_B;
            GUS.RepicData[GUS.FileNumTerm].Data.AddData[5] = GUS.StripInterSkin.C_StripPos;
            GUS.RepicData[GUS.FileNumTerm].Data.AddData[6] = GSS.DataLimit[10];
            GUS.RepicData[GUS.FileNumTerm].Data.AddData[7] = GSS.DataLimit[11];
            GUS.RepicData[GUS.FileNumTerm].Data.AddData[8] = GUS.FeedWire.FeedFrontDistance;
            GUS.RepicData[GUS.FileNumTerm].Data.AddData[9] = GUS.StripInterSkin.C_CutPos;
            GUS.RepicData[GUS.FileNumTerm].Data.AddData[10] = GUS.BFeedWire.EndMoveTeTerminalPos;
        }

        if(GUS.RepicData[GUS.FileNum].Data.FileCheak == 1)
        {
            for(j = 0; j < WireParaDataLength - 1; j++)
            {
                GUS.WirePara.Data_32[j] = GUS.RepicData[GUS.FileNum].Data_32[j] ;
            }
            GUS.BFeedWire.EndAxisUnfeedPos = GUS.RepicData[GUS.FileNum].Data.AddData[0];
            GUS.FeedWire.FrontFeedPos = GUS.RepicData[GUS.FileNum].Data.AddData[1];
            GUS.FeedWire.EndFeedPos = GUS.RepicData[GUS.FileNum].Data.AddData[2];

            GUS.CutWirePara.StripBackPos_A = GUS.RepicData[GUS.FileNum].Data.AddData[3];
            GUS.CutWirePara.StripBackPos_B = GUS.RepicData[GUS.FileNum].Data.AddData[4];
            GUS.StripInterSkin.C_StripPos = GUS.RepicData[GUS.FileNum].Data.AddData[5];
            GSS.DataLimit[10] = GUS.RepicData[GUS.FileNum].Data.AddData[6];
            GSS.DataLimit[11] = GUS.RepicData[GUS.FileNum].Data.AddData[7];
            GUS.FeedWire.FeedFrontDistance = GUS.RepicData[GUS.FileNum].Data.AddData[8];
            GUS.StripInterSkin.C_CutPos = GUS.RepicData[GUS.FileNum].Data.AddData[9];
            GUS.BFeedWire.EndMoveTeTerminalPos = GUS.RepicData[GUS.FileNum].Data.AddData[10];
        }
        GUS.WirePara.Data.FileCheak = GUS.RepicData[GUS.FileNum].Data.FileCheak;
        GUS.FileNumTerm = GUS.FileNum;
    }
}

void ShortCtr()
{
    if(GUS.WirePara.Data.WrieLength < WireLengthClam)
    {
        if(ShortModeForUser)
        {
            SetWhichBitU16(&GUS.FunctionEn, 20);
        }
        else
        {
            ResetWhichBitU16(&GUS.FunctionEn, 20);
        }
    }
    else
    {
        ResetWhichBitU16(&GUS.FunctionEn, 20);
    }
}

void ZProtect()
{
    static int step = 0;
    if(GUR.RunStatus != RUN)
    {
        if(Data.Z_JOGEn == 2)
        {
            switch(step)
            {
                case 0://ǰ���������Ḵλ
                    if(HZ_AxGetStatus(FrontLesserAxis) == 0 && HZ_AxGetStatus(EndLesserAxis) == 0 && HZ_AxGetStatus(PullWireAxis) == 0)
                    {
                        UserGoHome(FrontLesserAxis);
                        UserGoHome(EndLesserAxis);
                        UserGoHome(PullWireAxis);
                        step = 1;
                    }
                    break;
                    
                case 1://��λ��λ
                    if(HZ_AxGetStatus(FrontLesserAxis) == 0 && HZ_AxGetStatus(EndLesserAxis) == 0 && HZ_AxGetStatus(PullWireAxis) == 0)
                    {
                        Data.Z_JOGEn = 1;
                        step = 0;
                    }
                    break;
            }
        }
    }
    else
    {
        Data.Z_JOGEn = 0;
    }
}

//ͣ����һ��ʱ�������ֶ�������
void AddRosin()
{
	if(GUR.RunStatus == STOP)
	{
		if(ton_et[13] > GUS.SmoothinPara.AddRosinTime && Data.AddRosinAlarm != 2)
		{
			Data.AddRosinAlarm = 1;
		}
	}
	else
	{
		ton_et[13] = 0;
		Data.AddRosinAlarm = 0;
	}
}	

void Logic()
{
    u32 i;
    GUW.RealWireNum = GUS.WirePara.Data.ProductSet.WireNum;
//	ResetWhichBitU16(&GUS.WirePara.Data.B.Para.FunctionEn,0);
//	ResetWhichBitU16(&GUS.WirePara.Data.B.Para.FunctionEn,1);
    if(GUS.WirePara.Data.WrieLength < 15000)
    {
        ResetWhichBitU16(&GUS.WirePara.Data.B.Para.FunctionEn, 15);
    }
    if(GSR.ErrorLevel > 1)
    {
        GUW.StopCache = 1;
    }
//    if(GUS.Product.PitchNum == 0)
//    {
//        GUS.Product.PitchNum = 1;
//    }
    for (i = 0; i < 12; i++)
    {
        GUR.AxisSta[i] = HZ_AxGetStatus(i);
    }
    if (ton(1, GUR.RunStatus == RUN, 10))
    {
        if (GUW.button.DeviceMode == STEPMODE)
        {
            GUW.button.RunCommand = STOP;
        }
    }
    /******���������߼�*******/
    if (GUR.RunStatus == RUN && (PressCheckEn == 0 || InputGetSta(I_pressure) == OFF) && GSR.ErrorLevel < 3)//״̬Ϊ��������ѹ�����Ҵ���ȼ�С��3����ȫ��ǰ���£�
    {
        FeedWire(&LogicTask.FeedWire);
        RunProcess(&LogicTask.RunProcess);
        UnfeedProduct(&LogicTask.UnfeedProduct);
        BTin(&LogicTask.BTin);
        ATin(&LogicTask.ATin);
        AWork(&LogicTask.AWork);
        BWork(&LogicTask.BWork);
        CutWire(&LogicTask.CutWire);
        UnfeedProduct(&LogicTask.UnfeedProduct);
        CylinderSeparateWire(&LogicTask.CylinderSeparateWire);
        SeparateWire();
        ShakeWire();
        PullWireTwoTimes(&LogicTask.PullWireTwoTimes);
        DivideWire(&LogicTask.DivideWire);
        sheathedWire(&LogicTask.sheathedWire);
        
        AMakeTerminal(&LogicTask.AMakeTerminal);
        AInsertPin(&LogicTask.AInsertPin);
        AFeedPin(&LogicTask.AFeedPin);
        Acheck(&LogicTask.Acheck);
   }
    BCutWastage(&LogicTask.BCutWastage);
    Blow(&LogicTask.Blow);
    Q_LittleRight_ON(&LogicTask.Q_LittleRight_ON);
    HMI_Button();
    Alarm();
    ButtonMotion();
    BSmoothTin(&LogicTask.BSmoothTin);
    ASmoothTin(&LogicTask.ASmoothTin);
    BFeedTin(&LogicTask.BFeedTin);
    AFeedTin(&LogicTask.AFeedTin);
    BFeedRosin(&LogicTask.BFeedRosin);
    UnfeedPaper();
    SunCount();
    CheckPou();
    IntSpeed();
    FileOperate();
    ShortCtr();
    ParaSet();
    ZProtect();
   
    AServoTerminal(&LogicTask.AServoTerminal);
   
    if(InputGetSta(I_TerminalCtrol) == OFF)
    {
        if(Data.TerminalCtrol != 0)
        {
            ResetWhichBitU16(GSW.JogForward,TerminalAxis);
            ResetWhichBitU16(GSW.JogBackward,TerminalAxis);
        }
        Data.TerminalCtrol = 0;
    }
    else
    {
        if(InputGetSta(I_TerminalUp) == ON)
        {
            GSW.JogMode = 2;
            SetWhichBitU16(GSW.JogForward,TerminalAxis);
            Data.TerminalCtrol = 1;
			GUR.HaveToReset = 1; 
        }
        else if(InputGetSta(I_TerminalDown) == ON)
        {
            GSW.JogMode = 2;
            SetWhichBitU16(GSW.JogBackward,TerminalAxis);
            Data.TerminalCtrol = 2;
			GUR.HaveToReset = 1; 
        }
        else
        {
            ResetWhichBitU16(GSW.JogForward,TerminalAxis);
            ResetWhichBitU16(GSW.JogBackward,TerminalAxis);
            Data.TerminalCtrol = 0;
        }
    }
    

}
