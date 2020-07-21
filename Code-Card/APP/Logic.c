/*
 * File: Logic.c
 * File Created: Tuesday, 9th October 2018 2:43:29 pm
 * version: v00.00.01 8轴底层
 * Description:用户逻辑
 * Modified By:yang
 * -----
 * Copyright 2018 - 2018, <<hzzh>>
 */

#include "Logic.h"

/*全局变量做统一处理*/
Task LogicTask = {0}; //统一的任务类
SysData Data = {0};   //统一的数据类
int Change[2] = {Y21, Y22};
static int RealWireNum = 0;


void IntSpeed()//初始化速度
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
       GUS.ChangeMould = 0; //更换模具
    }
}

/**
* @author 20/04/16  wangh
* @Description: 参数配置
* @param --
* @param --
* @return --
*/
void ParaSet()
{
    int i;
    //跳线设置
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
    //插壳参数
	GUS.AInsertPin.Data.PressOnDelay = GUS.FeedWire.FrontOnDelay;//压线开延时=前端顶线夹夹紧延时
    GUS.AInsertPin.Data.PressOffDelay = GUS.FeedWire.FrontOffDelay;
    GUS.AInsertPin.Data.EndPos_Front = GUS.Acheck.Data.CheckPos_Front;
    GUS.AInsertPin.Data.EndPos_FrontMove = GUS.Acheck.Data.CheckPos_Move;//插壳的横移结束位置=检测横移开始移动的位置
    
    //检测
    GUS.Acheck.Data.EndPos_FrontMove = GUS.FeedWire.FrontRFeedPos;//检测结束的位置=前端横移的送线位置
    GUS.Acheck.Data.PressOn = GUS.FeedWire.FrontOnDelay;//检测时往前推的时候顶线夹夹紧
    
    //开关设置
    ResetWhichBitU16(&GUS.WirePara.Data.A.Para.FunctionEn,0);//缺线检测置0
    ResetWhichBitU16(&GUS.WirePara.Data.A.Para.FunctionEn,1);//打结模式置0
    ResetWhichBitU16(&GUS.WirePara.Data.A.Para.FunctionEn,2);//气压检测置0
    //气压检测
    SetWhichBitU16(&GUS.FunctionEn,2);
    
    if(GUS.WirePara.Data.B.Para.StripLength[singleWire] > GUS.WirePara.Data.B.Para.HalfStripLength)
    {
        SetWhichBitU16(&GUS.WirePara.Data.B.Para.FunctionEn,0);//半剥启用
    }
    else
    {
        ResetWhichBitU16(&GUS.WirePara.Data.B.Para.FunctionEn,0);
    }
    
    //温控控制
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
* @Description: 送线轴送线,拉线轴拉线
* @param --
* @param --
* @return --
*/
void FeedWire(LogicParaDef *LG)
{
    INITTASK(LogicTask.FeedWire);
    switch (LG->step)
    {
    case 1: //旋转到水平位置
        if (UserGetStatus(FrontLesserAxis) == 0 && UserGetStatus(FrontPrimaryAxis) == 0 && UserGetStatus(EndPrimaryAxis) == 0 && UserGetStatus(EndLesserAxis) == 0 && UserGetStatus(PullWireAxis) == 0)
        {
            AxMoveAbsUserUnit(FrontPrimaryAxis, GUS.SpdRatio[FrontPrimaryAxis], GUS.FeedWire.FrontRFeedPos);
            AxMoveAbsUserUnit(CutAxis, GUS.SpdRatio[CutAxis], GUS.FeedWire.CutOpenPos);
            AxMoveAbsUserUnit(EndPrimaryAxis, GUS.SpdRatio[EndPrimaryAxis], GUS.FeedWire.EndRFeedPos);
			//设置线长
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

    case 2: //到接线位置
        if(UserGetOutput(Q_EndClam) == ON)
        {
            AlarmSetBit(2, 1, 1); //后端夹子张开，请关闭后运行
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

    case 3: //拉线夹夹紧
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

    case 4: //拉线夹夹紧延时
        if (TimerCnt(LogicTask.FeedWire) > GUS.FeedWire.PullWireClamOnDelay)
        {
            LG->step = 5;
        }
        break;

    case 5: //打开顶线夹
        if (UserGetStatus(FrontPrimaryAxis) == 0 && UserGetStatus(CutAxis) == 0 && UserGetStatus(EndPrimaryAxis) == 0 && UserGetStatus(EndLesserAxis) == 0)
        {
            UserOutputSet(Q_FrontClam, ON);
            LG->step = 6;
        }
        break;

    case 6: //前端顶线夹延时，拉线长
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


    case 7: //夹线
        if (UserGetStatus(FrontLesserAxis) == 0  && LogicTask.CylinderSeparateWire.execute == 0 && UserGetStatus(PullWireAxis) == 0 && LogicTask.SeparateWire.execute == 0 && LogicTask.PullWireTwoTimes.execute == 0)
        {
            UserOutputSet(Q_FrontClam, OFF);
            LG->step = 8;
        }
        break;

    case 8: //线拉紧
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

    case 9: //后夹夹紧
        if (UserGetStatus(PullWireAxis) == 0)
        {
            LG->step = 10;
        }
        break;

    case 10: //夹线延时
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
* @Description: 送线轴送线
* @param --
* @param --
* @return --
*/
void FirstFeedWire()
{
//	INITTASK(LogicTask.FirstFeedWire);
//	switch (LogicTask.FirstFeedWire.step)
//	{
//	case 1: //到送线位置
//		if (UserGetStatus(FrontPrimaryAxis) == 0 && UserGetStatus(EndPrimaryAxis) == 0 && UserGetStatus(EndLesserAxis) == 0)
//		{
//			AxMoveAbsUserUnit(FrontPrimaryAxis, GUS.SpdRatio[FrontPrimaryAxis], GUS.FeedWire.FrontMoveFeedPos);

//			AxMoveAbsUserUnit(EndPrimaryAxis, GUS.SpdRatio[EndPrimaryAxis], GUS.FeedWire.EndRFeedPos);
//			AxMoveAbsUserUnit(EndLesserAxis, GUS.SpdRatio[EndLesserAxis], GUS.FeedWire.EndFeedPos - GUS.WirePara.Data.B.Para.StripLength[singleWire]);
//			LogicTask.FirstFeedWire.step = 2;
//		}
//		break;

//	case 2: //打开相关气缸
//		if (UserGetStatus(FrontPrimaryAxis) == 0 && UserGetStatus(CutAxis) == 0 && UserGetStatus(EndPrimaryAxis) == 0 && UserGetStatus(EndLesserAxis) == 0)
//		{
//			UserOutputSet(Q_EndClam, OFF);
//			LogicTask.FirstFeedWire.step = 3;
//		}
//		break;

//	case 3: //气缸延时
//		if (TimerCnt(LogicTask.FirstFeedWire) > GUS.B_Tin.B_ClamOffDelay)
//		{
//			LogicTask.FirstFeedWire.step = 4;
//		}
//		break;

//	case 4: //送线
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

//	case 5: //夹线
//		if (UserGetStatus(FeedWireAxis) == 0 && LogicTask.SeparateWire.execute == 0)
//		{
//			UserOutputSet(Q_EndClam, ON);
//			LogicTask.FirstFeedWire.step = 6;
//		}
//		break;

//	case 6: //夹线延时
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
* @Description: 气缸分线
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
    case 1://计算出拉线轴走的两端距离
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

    case 2://先拉出一段长度
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
        
    case 9: //拉线夹夹紧
        if (UserGetStatus(PullWireAxis) == 0 && UserGetStatus(FrontPrimaryAxis) == 0 && UserGetStatus(FrontLesserAxis) == 0 && UserGetStatus(EndPrimaryAxis) == 0 && UserGetStatus(EndLesserAxis) == 0)
        {
            UserOutputSet(Q_PullWireClam, ON);
            LG->step = 10;
        }
        break;

    case 10: //拉线夹夹紧延时
        if (TimerCnt(LogicTask.CylinderSeparateWire) > GUS.FeedWire.PullWireClamOnDelay)
        {
            LG->step = 11;
        }
        break;

    case 11: //打开顶线夹
        if (UserGetStatus(FrontPrimaryAxis) == 0 && UserGetStatus(CutAxis) == 0 && UserGetStatus(EndPrimaryAxis) == 0 && UserGetStatus(EndLesserAxis) == 0)
        {
            UserOutputSet(Q_FrontClam, ON);
            LG->step = 12;
        }
        break;
        
    case 12: //前端顶线夹延时，拉线长
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
        


    case 3://分线模下
        if (UserGetStatus(PullWireAxis) == 0 && UserGetStatus(FrontLesserAxis) == 0 && LogicTask.PullWireTwoTimes.execute == 0)
        {
            UserOutputSet(Q_SeparateWireDown, ON);
            LG->step = 4;
        }
        break;

    case 4://分线模划口
        if(TimerCnt(LogicTask.CylinderSeparateWire) > GUS.SperatePara.Down_ON_Delay)
        {
            UserOutputSet(Q_SeparateWireFront, ON);
            LG->step = 5;
        }
        break;

    case 5://分线模抬起
        if(TimerCnt(LogicTask.CylinderSeparateWire) > GUS.SperatePara.Front_ON_Delay)
        {
            UserOutputSet(Q_SeparateWireDown, OFF);
            LG->step = 6;
        }
        break;

    case 6://分线模划口复位
        if(TimerCnt(LogicTask.CylinderSeparateWire) > GUS.SperatePara.Down_OFF_Delay)
        {
            UserOutputSet(Q_SeparateWireFront, OFF);
            LG->step = 7;
        }
        break;

    case 7://分线模拉第二段长度
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

    case 8://第二段
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
* @Description: 分线
* @param --
* @param --
* @return --
*/
void SeparateWire()
{
    /************************************
    线长<分线刀到切刀间的距离
    第一步：线长-（线长-分线刀切刀间距对线长取余）
    ************************************/

    unsigned int i;
    static s32 L_cutDistance;	  //切断距离
    static s32 L_separateDistance; //分线距离

    //触摸屏给
    static s32 wireLength;			   //裁线长度
    static s32 A_separateWireLength;   //A端分线长度
    static s32 B_separateWireLength;   //B端分线长度
    static s32 cutterSeparateDistance; //分线刀与切刀的间距
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
        //拉线夹到夹线位置
        //切断距离 = 裁线长度
        cutterSeparateDistance = GUS.SperatePara.cutterSeparateDistance;
        L_cutDistance = wireLength - GUS.FeedWire.FeedFrontDistance;
        //分线距离 = （裁线长度 - B端分线长度） - 分线刀与切刀的距离//
        L_separateDistance = (wireLength - B_separateWireLength - GUS.FeedWire.FeedFrontDistance) - (cutterSeparateDistance);
        i = 1;
        while (L_separateDistance < 0 && abs(L_separateDistance) > (A_separateWireLength + B_separateWireLength))
        {
            L_separateDistance = (i * wireLength - B_separateWireLength - GUS.FeedWire.FeedFrontDistance) - cutterSeparateDistance;
            L_separateDistance = (i * wireLength - B_separateWireLength) - cutterSeparateDistance;
            i++;
        }
        //判断分线到的位置
        if (L_separateDistance <= 0)
        {
            //当分线刀在分线范围内，求出分线结束位置到分线刀的距离，分线刀下压
            L_separateDistance = A_separateWireLength + B_separateWireLength + L_separateDistance;
            UserOutputSet(Q_SeparateWireDown, ON);
            UserTimer[5] = GUS.SperatePara.Down_ON_Delay;
        }
        else
        {
            //分线刀不在分线范围内，分线刀抬起
            UserOutputSet(Q_SeparateWireDown, OFF);
            UserTimer[5] = GUS.SperatePara.Down_OFF_Delay;
        }
        LogicTask.SeparateWire.step = 2;
        break;

    case 2:
        if (UserTimer[5] == 0)
        {

            //判断先走分线距离还是先走切刀距离
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

    /********************分线部分*********************************/
    case 10:
        if (UserGetStatus(PullWireAxis) == 0)
        {
            //计算分线刀和切刀的间距
            cutterSeparateDistance = GUS.SperatePara.cutterSeparateDistance - GUS.FeedWire.EndFeedPos;
            //分线距离 = （裁线长度 - B端分线长度） - 分线刀与切刀的距离
            L_separateDistance = (wireLength - B_separateWireLength) - cutterSeparateDistance;
            i = 1;
            while(L_separateDistance < 0 && abs(L_separateDistance) > (A_separateWireLength + B_separateWireLength))
            {
                L_separateDistance = (i * wireLength - B_separateWireLength) - cutterSeparateDistance;
                i++;
            }
            //判断分线到的位置
            if(L_separateDistance <= 0)
            {
                //当分线刀在分线范围内，求出分线结束位置到分线刀的距离，分线刀下压
                L_separateDistance = A_separateWireLength + B_separateWireLength + L_separateDistance;
                UserOutputSet(Q_SeparateWireDown, ON);
                UserTimer[5] = GUS.SperatePara.Down_ON_Delay;
            }
            else
            {
                //分线刀不在分线范围内，分线刀抬起
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
* @author   20/03/09 汪豪
* @Description: 护套线剥线功能
* @param --
* @param --
* @return --
*/
void sheathedWire(LogicParaDef *LG)
{
    if(GUS.SheathedWire.Data.SheathedWireEn == -1)
    {//默认关闭开关
        GUS.SheathedWire.Data.SheathedWireEn = 0;
    }
    
    //剥口长度，不能大于切刀到剥刀的距离
    if(GUS.SheathedWire.Data.FrontStripLength > GUS.CutWirePara.A_CutDistance)
    {
        GUS.SheathedWire.Data.FrontStripLength = GUS.CutWirePara.A_CutDistance;
    }
    //剥口长度，不能大于切刀到剥刀的距离
    if(GUS.SheathedWire.Data.EndStripLength > GUS.CutWirePara.B_CutDistance)
    {
        GUS.SheathedWire.Data.EndStripLength = GUS.CutWirePara.B_CutDistance;
    }
    if(sheathedWireEn)
    {//内皮剥皮需要小于外皮剥皮
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
    {//换手长度需要小于外皮剥皮长度
        if(GUS.SheathedWire.Data.ChangeLength > (GUS.SheathedWire.Data.EndStripLength - GUS.WirePara.Data.B.Para.StripLength[singleWire]))
        {
            GUS.SheathedWire.Data.ChangeLength = GUS.SheathedWire.Data.EndStripLength - GUS.WirePara.Data.B.Para.StripLength[singleWire];
        }
    }
    
    INITTASK(LogicTask.sheathedWire);
    switch (LG->step)
    {
    case 1://前后端到护套线的剥线长度
        if (UserGetStatus(FrontLesserAxis) == 0 && UserGetStatus(PullWireAxis) == 0 && UserGetStatus(EndLesserAxis) == 0 && UserGetStatus(CutAxis) == 0)
        {
            //前后剥分别后退
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
    
    case 2://切刀到护套线刀值
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

    case 12: //判断切刀到位，延时清零
        if (UserGetStatus(CutAxis) == 0)
        {
            LG->step = 13;
        }
        break;

    case 13: //退刀
        if (TimerCnt(LogicTask.sheathedWire) > AxisDelay)
        {
            AxMoveRelUserUnit(CutAxis, GUS.SpdRatio[CutAxis], -GUS.SheathedWire.Data.StripCutBack);
            LG->step = 6;
        }
        break;


/***************************************************************换手********************************************************************************/        
        
    case 6://后端夹子张开
        if(UserGetStatus(FrontLesserAxis) == 0 && UserGetStatus(PullWireAxis) == 0 && UserGetStatus(EndLesserAxis) == 0)
        {
            UserOutputSet(Q_EndClam, OFF);
            LG->step = 7;
        }
        break;  
        
    case 7://后端到换手位置
        if (TimerCnt(LogicTask.sheathedWire) > GUS.B_Tin.B_ClamOnDelay)
        {
            AxMoveAbsUserUnit(EndLesserAxis, GUS.SpdRatio[EndLesserAxis], GUS.SheathedWire.Data.EndChangePos);
            LG->step = 8;
        }
        break;    
        
    case 8://后端到更换位置
        if(UserGetStatus(FrontLesserAxis) == 0 && UserGetStatus(PullWireAxis) == 0 && UserGetStatus(EndLesserAxis) == 0 && UserGetStatus(CutAxis) == 0)
        {
//            AxMoveRelUserUnit(EndLesserAxis, GUS.SpdRatio[EndLesserAxis], -(GUS.SheathedWire.Data.ChangeLength - GUS.FeedWire.PullStraightLength));
            AxMoveRelUserUnit(PullWireAxis, GUS.SheathedWire.Data.StripSpeed, -GUS.SheathedWire.Data.ChangeLength);
//            TwoAxMoveRelUserUnit(PullWireAxis, EndLesserAxis, GUS.SpdRatio[EndLesserAxis], -GUS.SheathedWire.Data.ChangeLength);
            LG->step = 9;
        }
        break;  
        
        
    case 9://后端夹夹紧
        if(UserGetStatus(FrontLesserAxis) == 0 && UserGetStatus(PullWireAxis) == 0 && UserGetStatus(EndLesserAxis) == 0)
        {
            UserOutputSet(Q_EndClam, ON);
            LG->step = 10;
        }
        break;  
        
    case 10://后拉夹松开
        if (TimerCnt(LogicTask.sheathedWire) > GUS.B_Tin.B_ClamOnDelay)
        {
            if(BHelpClamEN != 0 && ShortMode == 0 )
            {
                LG->step = 3;
            }
            else
            {               
                UserOutputSet(Q_PullWireClam, OFF); //后拉夹张开
                LG->step = 11;
            }
        }
        break;     
        
    case 11://后拉轴后退避让
        if (TimerCnt(LogicTask.sheathedWire) > GUS.CutWirePara.PullWireClamOffDelay)
        {
            AxMoveAbsUserUnit(PullWireAxis, GUS.SpdRatio[PullWireAxis],
                              MaxS32(0, GUR.AxisPositionUserUnit[PullWireAxis] - GUS.CutWirePara.PullBackLength));
            LG->step = 3;
        }
        break; 
        
        
        
/**************************************************************************************************************************************************/        
    
    case 3://护套线剥线
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
                    LogicTask.Blow.execute = 1; // 吹气
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
                    LogicTask.Blow.execute = 1; // 吹气
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
    
    case 4://切刀张开
        if (UserGetStatus(FrontLesserAxis) == 0 && UserGetStatus(PullWireAxis) == 0 && UserGetStatus(EndLesserAxis) == 0 && UserGetStatus(CutAxis) == 0)
        {
            LogicTask.ShakeWire.execute = 1;
            AxMoveAbsUserUnit(CutAxis, GUS.SpdRatio[CutAxis], GUS.SheathedWire.Data.StripCutOpen);
            LG->step = 5;
        }
        break; 
    
    case 5://前后到线芯剥线长度
        if (UserGetStatus(FrontLesserAxis) == 0 && UserGetStatus(PullWireAxis) == 0 && UserGetStatus(EndLesserAxis) == 0 && UserGetStatus(CutAxis) == 0 &&  LogicTask.ShakeWire.execute == 0)
        {//线芯剥皮位置=取线位置 - （刀距 - 剥线长度）
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
    
    case ENDCASE://结束
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
* @Description: 切线、剥线、扭线的功能
* @param --
* @param --
* @return --
*/
void CutWire(LogicParaDef *LG)
{
    INITTASK(LogicTask.CutWire);
    //剥口长度，不能大于切刀到剥刀的距离
    if (GUS.WirePara.Data.A.Para.StripLength[singleWire] > GUS.CutWirePara.A_CutDistance)
    {
        GUS.WirePara.Data.A.Para.StripLength[singleWire] = GUS.CutWirePara.A_CutDistance;
    }
    //剥口长度，不能大于切刀到剥刀的距离
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

    case 2: //切刀轴切断，送线轴当前位置清0
        if (1)
        {
            AxMoveAbsUserUnit(CutAxis, GUS.SpdRatio[CutAxis], GUS.StripInterSkin.C_CutPos);
            //判断线长，线太短的时候后端不夹线
            if(GUS.WirePara.Data.WrieLength < WireHelpLength || BHelpClamEN == 0)//辅线夹关闭
            {
                UserOutputSet(Q_PullWireClam, OFF); //后拉夹张开
            }
            LG->step = 3;
            if(sheathedChangeEn && sheathedWireEn)
            {
                UserOutputSet(Q_PullWireClam, ON); //后拉夹夹紧
                 LG->step = 4;
           }
        }
        break;

    case 3: //拉线轴走带预备位置
        if (UserGetStatus(CutAxis) == 0 && TimerCnt(LogicTask.CutWire) > GUS.CutWirePara.PullWireClamOffDelay)
        {
            if(GUS.WirePara.Data.WrieLength < WireHelpLength || BHelpClamEN == 0)//辅线夹关闭
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

    case 4: //切刀轴回原点（电机用步进防止丢步后刀值不准）
        if (UserGetStatus(CutAxis) == 0 && UserGetStatus(PullWireAxis) == 0 && LogicTask.sheathedWire.execute == 0)
        {
            if(sheathedWireEn)
            {
                AxMoveRelUserUnit(CutAxis, GUS.SpdRatio[CutAxis], -GUS.SheathedWire.Data.CutBack);
                LogicTask.sheathedWire.execute = 1; 
            }
            else
            {
                //前后剥分别后退
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

    case 7: //切刀走剥线位置
        if (UserGetStatus(FrontLesserAxis) == 0 && UserGetStatus(PullWireAxis) == 0 && UserGetStatus(EndLesserAxis) == 0)
        {
            AxMoveAbsUserUnit(CutAxis, GUS.SpdRatio[CutAxis], GUS.StripInterSkin.C_StripPos - GUS.WirePara.Data.CutWorth[singleWire]);
            LG->step = 8;
        }
        break;

    case 8: //判断切刀到位，延时清零
        if (UserGetStatus(CutAxis) == 0)
        {
            LG->step = 9;
        }
        break;

    case 9: //退刀
        if (TimerCnt(LogicTask.CutWire) > AxisDelay)
        {
            AxMoveRelUserUnit(CutAxis, GUS.SpdRatio[CutAxis], -GUS.WirePara.Data.BackCut[singleWire]);
            LG->step = 10;
        }
        break;

    case 10: //退刀完成
        if (UserGetStatus(CutAxis) == 0)
        {
            LG->step = 11;
        }
        break;

    case 11: //剥线
        if(1)
        {
            //判断前端是否开启半剥
            if(AHalfStripEn)
            {
                AxMoveRelUserUnit(FrontLesserAxis, GUS.SpdRatio[FrontLesserAxis], -GUS.WirePara.Data.A.Para.HalfStripLength);
            }
            else//半剥关，是全剥
            {
                LogicTask.AWork.Ready = 0;
                AxMoveAbsUserUnit(FrontLesserAxis, GUS.SpdRatio[FrontLesserAxis], GUS.CutWirePara.StripBackPos_A);
                LogicTask.Blow.execute = 1; // 吹气
            }

            //判断后端是否需要半剥
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
            else//全剥
            {
                LogicTask.Blow.execute = 1; // 吹气
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

    case 12: //剥完线后，切刀张开
        if (UserGetStatus(FrontLesserAxis) == 0 && UserGetStatus(EndLesserAxis) == 0  && UserGetStatus(PullWireAxis) == 0)
        {
            if(UserGetOutput(Q_Blow) == ON)
            {
                LogicTask.Blow.execute = 1; // 吹气
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

    case 13: //判断搓线
        if (UserGetStatus(CutAxis) == 0)
        {
            //判断是否需要搓线
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

            //判断是否需要搓线
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

    case 14: //切刀走到扭线位置
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


    case 16: //扭线轴扭线
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
            //前端边退边搓
            if(ATwistWireEn == 1 && AHalfStripEn == 1)
            {
                if(LogicTask.AWork.Ready == 1)
                {
                    AxMoveRelUserUnit(FrontLesserAxis, GUS.CutWirePara.TwistSpeed,
                                      -(GUS.WirePara.Data.A.Para.StripLength[singleWire] - GUS.WirePara.Data.A.Para.HalfStripLength + 200));
                LogicTask.Blow.execute = 1; // 吹气
               }
            }

            //后端边退边搓
            if(BTwistWireEn == 1 && BHalfStripEn == 1)
            {
                AxMoveRelUserUnit(EndLesserAxis, GUS.CutWirePara.TwistSpeed,
                                  -(GUS.WirePara.Data.B.Para.StripLength[singleWire] - GUS.WirePara.Data.B.Para.HalfStripLength + 200));
                LogicTask.Blow.execute = 1; // 吹气
           }
            LG->step = 18;
        }
        break;

    case 18: //扭完线前后退出
        if (UserGetStatus(EndLesserAxis) == 0 && UserGetStatus(FrontLesserAxis) == 0)
        {
            if(UserGetOutput(Q_Blow) == ON)
            {
                LogicTask.Blow.execute = 1; // 吹气
            }
            //判断是否需要搓线
            if(ATwistWireEn == 1 && AHalfStripEn == 1)
            {
                if(LogicTask.AWork.Ready == 1)
                {
                    AxMoveAbsUserUnit(FrontLesserAxis, GUS.SpdRatio[FrontLesserAxis], GUS.CutWirePara.StripBackPos_A);
                }
            }

            //判断是否需要搓线
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

    case 19: //前后端可以开始工作了
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
            LogicTask.Blow.execute = 1; // 吹气
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

    case 22: //切刀和扭线轴回0位置
        if (UserGetStatus(CutAxis) == 0)
        {
            LogicTask.ShakeWire.execute = 1;
            LogicTask.Blow.execute = 1;
            LG->step = 23;
        }
        break;

    case 23: //切刀和扭线回原点
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
* @Description:  气缸插线功能
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
        case 1: //切刀到插线位置
            if (UserGetStatus(CutAxis) == 0)
            {
                AxMoveAbsUserUnit(CutAxis, GUS.SpdRatio[CutAxis], GUS.FeedWire.DividPos_Cut);
                LG->step = 2;
            }
            break;
        
        case 2: //前、后端前后到插线位置
            if (UserGetStatus(CutAxis) == 0 && UserGetStatus(FrontLesserAxis) == 0)
            {
                AxMoveAbsUserUnit(FrontLesserAxis, GUS.SpdRatio[FrontLesserAxis], GUS.FeedWire.DividPos_End);
                LG->step = 3;
            }
            break;
        
        case 3: //插线气缸插下
            if (UserGetStatus(CutAxis) == 0 && UserGetStatus(FrontLesserAxis) == 0)
            {
                UserOutputSet(Q_TwistDown, ON);
                LG->step = 4;
            }
            break;
            
        case 4://插线气缸张开
            if (UserGetStatus(CutAxis) == 0 && UserGetStatus(FrontLesserAxis) == 0 && TimerCnt(LogicTask.DivideWire) > GUS.CutWirePara.TwistDownOnDelay)
            {
                UserOutputSet(Q_TwistDown, OFF);
                LG->step = 5;
            }
            break;
        
        case 5: //前、后端轴退出插线气缸
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
* @Description: 去线皮动作，搓条
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
//* @Description: 分线
//* @param --
//* @param --
//* @return --
//*/
//void SeparateWire()
//{
////	/************************************
////	线长<分线刀到切刀间的距离
////	第一步：线长-（线长-分线刀切刀间距对线长取余）
////	************************************/

////	unsigned int i;
////	static s32 L_cutDistance;	  //切断距离
////	static s32 L_separateDistance; //分线距离

////	//触摸屏给
////	static s32 wireLength;			   //裁线长度
////	static s32 A_separateWireLength;   //A端分线长度
////	static s32 B_separateWireLength;   //B端分线长度
////	static s32 cutterSeparateDistance; //分线刀与切刀的间距
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
////		//拉线夹到夹线位置
////		//切断距离 = 裁线长度
////		cutterSeparateDistance = GUS.SperatePara.cutterSeparateDistance;
////		L_cutDistance = wireLength;
////		//分线距离 = （裁线长度 - B端分线长度） - 分线刀与切刀的距离//
////		L_separateDistance = (wireLength - B_separateWireLength) - (cutterSeparateDistance);
////		i = 1;
////		while (L_separateDistance < 0 && abs(L_separateDistance) > (A_separateWireLength + B_separateWireLength))
////		{
////			L_separateDistance = (i * wireLength - B_separateWireLength) - cutterSeparateDistance;
////			i++;
////		}
////		//判断分线到的位置
////		if (L_separateDistance <= 0)
////		{
////			//当分线刀在分线范围内，求出分线结束位置到分线刀的距离，分线刀下压
////			L_separateDistance = A_separateWireLength + B_separateWireLength + L_separateDistance;
////			UserOutputSet(Q_separate_wire, ON);
////			ton_et[0] = 0;
////		}
////		else
////		{
////			//分线刀不在分线范围内，分线刀抬起
////			UserOutputSet(Q_separate_wire, OFF);
////			ton_et[0] = GUS.SperatePara.SperateOnDelay;
////		}
////		LogicTask.SeparateWire.step = 2;
////		break;

////	case 2:
////		if (ton(0, 1, GUS.SperatePara.SperateOnDelay))
////		{

////			//判断先走分线距离还是先走切刀距离
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
//* @Description: A端打端子
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
//			//单步模式先走到检测位置，方便调试检测位置
//			if (GUW.button.DeviceMode == STEPMODE)
//			{
//				if (AMakeTerminalEn == 1)
//				{
//					AxMoveAbsUserUnit(FrontPrimaryAxis, GUS.SpdRatio[FrontPrimaryAxis], GUS.AMakeTerminalPara.FrontMoveCheckPos); //横移到检测位置
//				}
//			}
//			LogicTask.AMakeTeminal.step = 2;
//		}
//		break;

//	case 2: //
//		if (UserGetStatus(FrontPrimaryAxis) == 0)
//		{
//			//判断需不需要打端子
//			if (AMakeTerminalEn == 1)
//			{
//				AxMoveAbsUserUnit(FrontPrimaryAxis, GUS.SpdRatio[FrontPrimaryAxis], GUS.AMakeTerminalPara.FrontMoveMakeTerminalPos); //横移到打端位
//				for (i = 0; i < GUS.WirePara.Data.ProductSet.WireNum; i++)
//				{
//					if (WordToBit(GUS.AMakeTerminalPara.MakeTerminalEn, i))
//					{
//						AxMoveAbsUserUnit(FrontPrimaryAxis, GUS.SpdRatio[FrontPrimaryAxis], GUS.AMakeTerminalPara.FrontMoveMakeTerminalPos - GUS.WirePara.Data.WireDistance * i); //横移到打端位
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

//	case 3: //判断横移轴超过检测位置后，前后轴提前到打端位置
//		if (((GUR.AxisPositionUserUnit[FrontPrimaryAxis] > GUS.AMakeTerminalPara.FrontMoveCheckPos) || UserGetStatus(FrontPrimaryAxis) == 0) && UserGetStatus(FeedWireAxis) == 0)
//		{
//			if (UserGetStatus(FeedWireAxis) == 0)
//			{ //前后轴提前到打端位置
//				AxMoveAbsUserUnit(FeedWireAxis, 1, GUS.AMakeTerminalPara.FrontMakeTerminalPos);
//			}
//			if (UserGetStatus(FrontPrimaryAxis) == 0)
//			{
//				GUW.RealWireNum = 0;
//				for (i = 0; i < GUS.WirePara.Data.ProductSet.WireNum; i++)
//				{
//					if (WordToBit(GUS.AMakeTerminalPara.MakeTerminalEn, i))
//					{ //计算实际线根数，没有打端的不计数
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

//	case 7: //切废料
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
//		//切刀先张开，然后才能移到送线位，防止端子撞到切刀
//		if (UserGetStatus(FeedWireAxis) == 0 && LogicTask.CutWire.execute == 0)
//		{
//			if (GUW.button.DeviceMode == STEPMODE)
//			{
//				if (AMakeTerminalEn == 1)
//				{
//					if (GUR.AxisPositionUserUnit[FrontPrimaryAxis] > GUS.AMakeTerminalPara.FrontMoveCheckPos)
//						AxMoveAbsUserUnit(FrontPrimaryAxis, GUS.SpdRatio[FrontPrimaryAxis], GUS.AMakeTerminalPara.FrontMoveCheckPos); //横移到检测位置
//				}
//			}
//			LogicTask.AMakeTeminal.step = 9;
//		}
//		break;

//	case 9: //
//		if (UserGetStatus(FrontPrimaryAxis) == 0 && UserGetStatus(FeedWireAxis) == 0)
//		{
//			AxMoveAbsUserUnit(FrontPrimaryAxis, GUS.SpdRatio[FrontPrimaryAxis], GUS.FeedWire.FrontRFeedPos); //横移送线位
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
//					AlarmSetBit(2, 0, 1); //端子检测报警，请检查
//				}
//			}
//			AxMoveAbsUserUnit(FeedWireAxis, GUS.SpdRatio[FeedWireAxis], 0); //送线轴走到0位置，准备下一次送线
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
* @Description: A端沾锡
* @param --
* @param --
* @return --
*/
void ATin(LogicParaDef *LG)
{
    INITTASK(LogicTask.ATin);
    switch (LG->step)
    {
    case 1: //判断是否沾锡，到松香位置
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

    case 2: //计算 走沾松香深度
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

    case 3: //判断走到松香深度到位，延时计数清零
        if (UserGetStatus(FrontLesserAxis) == 0)
        {
            LG->step = 4;
        }
        break;

    case 4: //松香延时，提起到0位
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

    case 5: //旋转到沾锡位置
        if (UserGetStatus(FrontLesserAxis) == 0)
        {
            AxMoveAbsUserUnit(FrontPrimaryAxis, GUS.SpdRatio[FrontPrimaryAxis], GUS.A_Tin.A_R_TinPos);
            LG->step = 6;
        }
        break;

    case 6: //走沾锡深度
        if (UserGetStatus(FrontPrimaryAxis) == 0 && UserGetStatus(FrontLesserAxis) == 0)
        {
            if(ASlowTinEn)
            {
                //20190730 慢沾锡位置
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


    case 7: //判断走到锡深度到位，延时计数清零
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

    case 8: //沾锡延时，提起到
        if (UserGetStatus(FrontLesserAxis) == 0)
        {
            AxMoveAbsUserUnit(FrontLesserAxis, GUS.WirePara.Data.A.Para.TinPara.TinSpeed, GUS.WirePara.Data.A.Para.TinBackPos);
            LG->step = 9;
        }
        break;

    case 9: //多次沾锡
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

    case 10: //旋转到水平位置、沾锡、刮锡
        if (UserGetStatus(FrontPrimaryAxis) == 0 && UserGetStatus(FrontLesserAxis) == 0 )
        {
            if (ATinEn == 1)
            {
                AxMoveAbsUserUnit(FrontLesserAxis, GUS.SpdRatio[FrontLesserAxis], GUS.WirePara.Data.A.Para.TinBackPos);
            }
            LG->step = 11;
        }
        break;

    case 11: //旋转带水平位置
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
* @Description: B端沾锡
* @param --
* @param --
* @return --
*/
void BTin(LogicParaDef *LG)
{
    GUS.WirePara.Data.B.Para.TinBackPos = 0; //沾完锡退到0位置
    
    INITTASK(LogicTask.BTin);
    switch (LG->step)
    {
    case 1: //判断是否沾锡，到松香位置
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

    case 2: //计算 走沾松香深度
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

    case 3: //判断走到松香深度到位，延时计数清零
        if (UserGetStatus(EndLesserAxis) == 0)
        {
            LG->step = 4;
        }
        break;

    case 4: //松香延时，提起到0位
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

    case 5: //旋转到沾锡位置
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

    case 6: //走沾锡深度
        if (UserGetStatus(EndPrimaryAxis) == 0 && UserGetStatus(EndLesserAxis) == 0)
        {
            if(BSlowTinEn)
            {
                //20190730 慢沾锡位置
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

    case 7: //判断走到锡深度到位，延时计数清零
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


    case 8: //沾锡延时，提起到
        if (UserGetStatus(EndLesserAxis) == 0)
        {
            AxMoveAbsUserUnit(EndLesserAxis, GUS.WirePara.Data.B.Para.TinPara.TinSpeed, GUS.WirePara.Data.B.Para.TinBackPos);
            LG->step = 9;
        }
        break;

    case 9: //多次沾锡
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

    case 10: //旋转带水平位置、沾锡、刮锡
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
* @Description: A端送锡丝
* @param --
* @param --
* @return --
*/
void AFeedTin(LogicParaDef *LG)
{
    INITTASK(LogicTask.AFeedTin);
    switch (LG->step)
    {
    case 1: //打开送锡电机
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

    case 2: //送锡延时，关掉送锡电机
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
* @Description: A端送锡丝
* @param --
* @param --
* @return --
*/
void BFeedRosin(LogicParaDef *LG)
{
    INITTASK(LogicTask.BFeedRosin);
    switch (LG->step)
    {
    case 1: //打开送锡电机
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

    case 2: //送锡延时，关掉送锡电机
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
* @Description: B端送锡丝
* @param --
* @param --
* @return --
*/
void BFeedTin(LogicParaDef *LG)
{
    INITTASK(LogicTask.BFeedTin);
    switch (LG->step)
    {
    case 1: //打开送锡电机
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

    case 2: //送锡延时，关掉送锡电机
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
* @Description: A端刮锡
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
            UserOutputSet(Q_FrontScrapeTini, OFF); //收回
            LG->step = 4;
        }
        break;

    case 4: //
        if (TimerCnt(LogicTask.ASmoothTin) > GUS.SmoothinPara.SmoothTinDelay)
        {
            UserOutputSet(Q_FrontScrapeTiniUp, OFF); //抬起
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
* @Description: B端刮锡
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
            UserOutputSet(Q_EndScrapeTini, OFF); //收回
            LG->step = 4;
        }
        break;

    case 4: //
        if (TimerCnt(LogicTask.BSmoothTin) > GUS.SmoothinPara.SmoothTinDelay)
        {
            UserOutputSet(Q_EndScrapeTiniUp, OFF); //抬起
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
* @Description: 下料函数
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
           
		
	case 2://后剥轴到下料位置
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
		
	case 3: //成品气缸伸出
		if (UserGetStatus(EndLesserAxis) == 0)
		{
			UserOutputSet(Q_ProductClam, ON);
			LG->step = 4;
		}
		break;	
		
	case 4: //成品到位 小夹子夹
		if((TimerCnt(LogicTask.UnfeedProduct) > GUS.Unfeed.ProductClamOnDelay) && UserGetStatus(PullWireAxis) == 0 && UserGetStatus(EndLesserAxis) == 0)
		{
			UserOutputSet(Q_LittleClam, ON);
			LG->step = 5;
		}
		break;
		
	case 5: //后端夹子张开
		if(TimerCnt(LogicTask.UnfeedProduct) > 30)
		{
			UserOutputSet(Q_EndClam, OFF);
			UserOutputSet(Q_PullWireClam, OFF);
			LG->step = 6;
		}
		break;


	case 6: //成品气缸收回
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

	case 8://成品夹子张开
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
* @author： 2019/1/9 农业照
* @Description: 吹气
* @param –
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
* @author： 2019/1/9 汪豪
* @Description: 下料气缸先左右后前后
* @param –
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
    case 1: //打开左右气缸
        UserOutputSet(Q_LittleRight, ON);
        LG->step = 2;
        break;

    case 2: //延时
        if (InputGetSta(I_LittleRight) == ON)
        {
            LG->step = 3;
        }
        break;

    case 3: //延时后打开前后气缸
    {
        UserOutputSet(Q_LittleFront, ON);
        LG->step = 4;
    }
    break;

    case 4://结束
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
* @author： 2019/07/11 汪豪
* @Description: 拉线轴分段跑 只能相对位置使用
* @param –
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
* @author： 2019/1/9 农业照
* @Description: B端切废料
* @param –
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



//上升缘检测，clk由0变为1时，该函数返回值为1，当下次再执行，返回值为0; 形参 num：上升沿检测编号
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
* @author： 2019/1/9 农业照
* @Description: 检测端子
* @param –
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
    //过线检测，看判断是否需要打端子
    if(Triger(3, 0, InputGetSta(I_FrontTerminalCheck_B) == ON, 0))
    {
        Data.BWireCheckFlag++;
    }
    GUW.makeTerminalCountForUser = Data.BWireCheckFlag;
}

/**
* @author 19/02/19  wangh
* @Description: A端动作调度
* @param --
* @param --
* @return --
*/
void AWork(LogicParaDef *LG)
{
    INITTASK(LogicTask.AWork);
    switch (LG->step)
    {
    case 1://打端
        if (LG->Ready == 0 && LogicTask.AMakeTerminal.execute == 0)
        {
            LogicTask.AMakeTerminal.execute = 1;
            LG->step = 2;            
        }
        break;

    case 2: //穿壳
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
        
    case 3: //检测
        if (LogicTask.AInsertPin.execute == 0 && LogicTask.Acheck.execute == 0)
        {
            if(LogicTask.AInsertPin.done == 1)
            {
                LogicTask.Acheck.execute = 1;
            }
            LG->step = 4;
        }
        break;

    case 4: //结束
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
				AlarmSetBit(2, 0, 14); //连续不良报警
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
* @author： 2020/04/01 汪豪
* @Description: A端打端子
* @param –
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
        case 1://打端禁用
             if(AMakeTerminalEn && Data.DivideFirstWire == 0)
            {
                LG->step = 2;
            }
            else
            {
                LG->step = 8;
            }
           break;
        
        case 2://横移到打端位置
            if(UserGetStatus(FrontPrimaryAxis) == 0 && (UserGetStatus(FrontLesserAxis) == 0 || GUR.AxisPositionUserUnit[FrontLesserAxis] < 200))
            {
                AxMoveAbsUserUnit(FrontPrimaryAxis, GUS.SpdRatio[FrontPrimaryAxis], GUS.AMakeTerminal.FrontMoveMakeTerminalPos); //前端横移到打端位置
                LG->step = 3;
            }
            break;      
            
        case 3://打端前后
            if(((UserGetStatus(FrontPrimaryAxis) == 0 || GUR.AxisPositionUserUnit[FrontPrimaryAxis] > (GUS.AMakeTerminal.FrontMoveMakeTerminalPos - 200)) && UserGetStatus(TerminalAxis) == 0)
                && UserGetStatus(FrontLesserAxis) == 0)
            {
                AxMoveAbsUserUnit(FrontLesserAxis, GUS.SpdRatio[FrontLesserAxis], GUS.AMakeTerminal.FrontMakeTerminalPos); //前后轴到打端位置
                LG->step = 4;
            }
            break;        

        case 4://打端横移（判断禁用）
            if(UserGetStatus(FrontPrimaryAxis) == 0 && UserGetStatus(FrontLesserAxis) == 0)
            {
                if(WordToBit(GUS.WirePara.Data.A.Para.MakeTerminalEn, FrontMakeTermCount) && Data.TerminalErro == 0)
                {
                    RealWireNum++;
//                    AxisRunDefaultUserUnit(FrontPrimaryAxis, ABSMODE, 11, GUS.AMakeTerminal.FrontMoveMakeTerminalPos  - GUS.WirePara.Data.WireDistance * FrontMakeTermCount
//                    - GUS.WireDistanceOffer.Data.WireDistanceOffer[FrontMakeTermCount]); //横移到打端位 增加间距补偿
					if(FrontMakeTermCount != 0)
					{
						AxisRunDefaultUserUnit(FrontPrimaryAxis, RELMODE, 11,-(GUS.WirePara.Data.WireDistance + GUS.WireDistanceOffer.Data.WireDistanceOffer[FrontMakeTermCount])); //横移到打端位 增加间距补偿
					}
                    LG->step = 5;
                }
                else
                {
                    LG->step = 6;
                }
            }
            break;
        
        case 5://横移轴到位开始打端
            if(UserGetStatus(FrontPrimaryAxis) == 0 && LogicTask.AServoTerminal.execute == 0)
            {
				Data.PressAlarmCount = 0; //打端成功后连续计数取消
				if(Data.TerminalErro == 0)
				{
					LogicTask.AServoTerminal.execute = 1;
				}
                LogicTask.AServoTerminal.done = 0;
                LG->step = 6;
           }
            break;
        
        case 6://端子机抬起后判断打端个数
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
        
        case 7://切废料
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
        
        case 8://打端后延时，横移出模具
            if(TimerCnt(LogicTask.AMakeTerminal) > GUS.AMakeTerminal.MakeTerminaDelay && UserGetStatus(FrontLesserAxis) == 0)
            {
                LG->step = 9;
            }
            break;
        
        case 9://
                LG->step = ENDCASE;
            break;  
        
        case ENDCASE://结束
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
* @author： 2020/04/01 汪豪
* @Description: A端端子机伺服轴打端子
* @param –
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
        case 1://打端伺服轴
            if(UserGetStatus(TerminalAxis) == 0)
            {
                AxMoveRelUserUnit(TerminalAxis, GUS.SpdRatio[TerminalAxis], 36000); //转一圈
                PosCache = GUR.AxisPositionUserUnit[TerminalAxis];
				GUS.Product.TerminalNum++;
                LG->step = 2;
            }
            break;
        
        case 2://判断位置给出横移信号
            if(GUR.AxisPositionUserUnit[TerminalAxis] > (PosCache + 34000) || UserGetStatus(TerminalAxis) == 0)
            {
                LG->done = 1;
                LG->step = 3;
            }
            break;        

        case 3://到位，打端结束
            if(UserGetStatus(TerminalAxis) == 0)
            {
                LG->done = 1;
                LG->step = 4;
            }
            break;
        
        case 4://结束
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
* @author： 2020/04/01 汪豪
* @Description: A端穿壳
* @param –
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
        case 1://横移到穿壳位置
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
        
        case 2://横移到一半位置的时候，前后轴到固定端子位置，前推气缸打开
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

        case 3://到位后，端子机定位夹夹住。
            if((Data.FeedPinDone == 1 || AInsertPinEn == 0)&& InputGetSta(I_PinON) == ON &&
				TimerCnt(LogicTask.AInsertPin) > GUS.AInsertPin.Data.PushOnDelay && UserGetStatus(FrontLesserAxis) == 0 && UserGetStatus(InsertTerminalAxis) == 0)
            {
                UserOutputSet(Q_PinLocatUp, ON);
                UserOutputSet(Q_PinLocatDown, ON);
                LG->step = 4;
            }
            break;
        
        case 4://压线松开
            if(TimerCnt(LogicTask.AInsertPin) > GUS.AInsertPin.Data.PinLocatONDelay)
            {
                //前端压线松开
                UserOutputSet(Q_FrontPress, OFF);
                //前端夹子张开
                UserOutputSet(Q_FrontClam, ON);
                LG->step = 5;
           }
            break;
        
        case 5://插壳
            if(TimerCnt(LogicTask.AInsertPin) > GUS.AInsertPin.Data.PressOnDelay)
            {
                AxMoveAbsUserUnit(InsertTerminalAxis, GUS.SpdRatio[InsertTerminalAxis], GUS.AInsertPin.Data.FirstPinDistance); 
				TwoAxMoveRelUserUnit(FrontLesserAxis, InsertTerminalAxis, GUS.SpdRatio[InsertTerminalAxis] , GUS.AFeedPin.Data.TakePinPos-GUS.AInsertPin.Data.FirstPinDistance);
                LG->step = 6;
            }
            break; 
        
        case 6://压线，前夹
            if(UserGetStatus(InsertTerminalAxis) == 0)
            {
                UserOutputSet(Q_FrontPress, ON);
                UserOutputSet(Q_FrontClam, OFF);
                LG->step = 7;
            }
            break;  
        
        case 7://端子定位松开
            if(TimerCnt(LogicTask.AInsertPin) > GUS.AInsertPin.Data.PressOffDelay)
            {
                UserOutputSet(Q_PinLocatUp, OFF);
                UserOutputSet(Q_PinLocatDown, OFF);
                LG->step = 8;
            }
            break;        
        
        case 8://二次插壳
            if(TimerCnt(LogicTask.AInsertPin) > GUS.AInsertPin.Data.PinLocatOFFDelay)
            {
                AxMoveAbsUserUnit(FrontLesserAxis, GUS.AInsertPin.Data.InsertPinSpeed, GUS.AInsertPin.Data.SecondPinDistance); 
                LG->step = 9;
            }
            break;    
        
        case 9://横移出来胶壳位置
            if(UserGetStatus(FrontLesserAxis) == 0)
            {
                AxMoveAbsUserUnit(FrontPrimaryAxis, GUS.AInsertPin.Data.InsertDoneSpeed, GUS.AInsertPin.Data.EndPos_FrontMove); 
                LG->step = 10;
            }
            break;    
            
        case 10://前后轴到结束位置
            if(UserGetStatus(FrontPrimaryAxis) == 0)
            {
                Data.FeedPinDone = 0;
                AxMoveAbsUserUnit(FrontLesserAxis, GUS.SpdRatio[FrontLesserAxis], GUS.AInsertPin.Data.EndPos_Front); 
                LG->step = 11;
            }
            break;   
            
        case 11://横移出来胶壳位置
            if(UserGetStatus(FrontLesserAxis) == 0 && UserGetStatus(FrontPrimaryAxis) == 0)
            {
                AxMoveAbsUserUnit(InsertTerminalAxis, GUS.SpdRatio[InsertTerminalAxis], GUS.AFeedPin.Data.TakePinPos); 
                LG->step = ENDCASE;
            }
            break;
        
        case ENDCASE://结束
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
* @author： 2020/04/01 汪豪
* @Description: A端检测
* @param –
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
        case 1://横移到检测位置
            if(UserGetStatus(FrontPrimaryAxis) == 0)
            {
                AxMoveAbsUserUnit(FrontPrimaryAxis, GUS.SpdRatio[FrontPrimaryAxis], GUS.Acheck.Data.CheckPos_Move); 
                LG->step = 2;
            }
            break;
        
        case 2://前后到检测位
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

        case 3://触发视觉拍照
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
        
        case 6://检测视觉结果
            if(TimerCnt(LogicTask.Acheck) > 50)
				LG->step = 7;
            break;     
        
        case 7://前后轴退到检测退位
            if(UserGetStatus(FrontPrimaryAxis) == 0)
            {
                AxMoveAbsUserUnit(FrontLesserAxis, GUS.SpdRatio[FrontLesserAxis], GUS.Acheck.Data.CheckBackPos); 
                LG->step = 8;
            }
            break;     
        
        case 8://压线夹张开
            if(1)
            {
                UserOutputSet(Q_FrontPress, OFF);
                LG->step = 9;
            }
            break;     
        
        case 9://前推收回
            if(TimerCnt(LogicTask.Acheck) > GUS.Acheck.Data.PressOn)
            {
                UserOutputSet(Q_FrontPush, OFF);
                LG->step = 10;
            }
            break;     
        
        case 10://横移轴退回到结束位置
            if(InputGetSta(I_PinOFF) == ON && UserGetStatus(FrontLesserAxis) == 0)
            {
                AxMoveAbsUserUnit(FrontPrimaryAxis, GUS.SpdRatio[FrontPrimaryAxis], GUS.Acheck.Data.EndPos_FrontMove); 
                LG->step = 11;
            }
            break;     
        
        case 11://结束
            if(UserGetStatus(FrontPrimaryAxis) == 0)
            {
                LG->step = ENDCASE;
            }
            break; 
            
        case ENDCASE://结束
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
* @author： 2020/04/01 汪豪
* @Description: A端检测
* @param –
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
        case 1://到取壳位置
            if(UserGetStatus(InsertTerminalAxis) == 0)
            {
                LG->step = 2;
            }
            break;
        
        case 2://判断上壳数量
            if(UserGetStatus(InsertTerminalAxis) == 0 && UserGetStatus(PinFeedAxis) == 0)
            {
                if(Data.FeedPinCount < GUS.AFeedPin.Data.FeedNum)
                {//上壳
                    LG->step = 3;
                }
                else
                {//上壳完成
                    LG->step = 7;
                }
            }
            break;        

        case 3://上壳
            if(UserGetStatus(PinFeedAxis) == 0)
            {
                if(PinCheackEn)
                {
                    //检测胶壳感应
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
            
        case 4://退回
            if(UserGetStatus(PinFeedAxis) == 0)
            {
                LG->step = 5;
            }
            break;  
			
        case 5://退回
            if(TimerCnt(LogicTask.AFeedPin) > 80)
            {
                AxMoveAbsUserUnit(PinFeedAxis, GUS.SpdRatio[PinFeedAxis], GUS.AFeedPin.Data.FeedBackPos);
                LG->step = 6;
            }
            break;
            
        case 6://上壳时间
            if(TimerCnt(LogicTask.AFeedPin) > GUS.AFeedPin.Data.FeedStay)
            {
                LG->step = 2;
            }
            break;
        
        case 7://结束
            if(UserGetStatus(InsertTerminalAxis) == 0 && UserGetStatus(PinFeedAxis) == 0)
            {
                LG->step = ENDCASE;
            }
            break; 
            
        case ENDCASE://结束
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
* @author： 2019/1/9 wangh
* @Description: B端打端子
* @param –
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
//    case 1://横移到打端位置
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

//    case 2://打端横移（判断打端是否禁用）
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
//                    + GUS.WireDistanceOffer.Data.WireDistanceOffer[EndMakeTermCount]); //横移到打端位 增加间距补偿
//                }
//                else
//                {                    
//                    AxisRunDefaultUserUnit(EndMoveAxis, ABSMODE, 10, GUS.BFeedWire.EndMoveTeTerminalPos  + GUS.WirePara.Data.WireDistance * EndMakeTermCount); //横移到打端位
//                }
//                LG->step = 3;
//            }
//            else
//            {
//                LG->step = 6;
//            }
//        }
//        break;

//    case 3://横移轴到位后开始打端子:打开端子机
//        if(UserGetStatus(EndMoveAxis) == 0)
//        {
//            if(TerminalEn && Data.TerminalErro == 0)
//            {
//                UserOutputSet(Q_MakeTerminal, ON);
//            }
//            LG->step = 4;
//        }
//        break;

//    case 4://感应上感应离开 打端延时
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

//    case 5://感应上感应回来
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

//    case 7://切废料
//        if(UserGetStatus(EndMoveAxis) == 0)
//        {
//            LogicTask.BCutWastage.execute = 1;
//            LG->step = 8;
//        }
//        break;

//    case 8://打端延时 20190708
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

//    case 9://到下料位置
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

//    case 10://检测
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
* @Description: B端动作调度
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
            
            //剥线完成时，后拉和剥线提前待机
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
* @Description: 总数计数
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
* @Description: 总逻辑进程调度
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
    case 1: //送线
        if (LogicTask.FeedWire.execute == 0)
        {
            GUW.StopCache = 0;
            UserTimer[0] = 100000;
            LogicTask.FeedWire.execute = 1;
            LG->step = 2;
        }
        break;

    case 2: //剥线
        if (LogicTask.FeedWire.execute == 0 && LogicTask.CutWire.execute == 0 && LogicTask.FirstFeedWire.execute == 0)
        {
            LogicTask.AWork.Ready = 1;
            LogicTask.BWork.Ready = 1;
            LogicTask.CutWire.execute = 1;
            LogicTask.CutWire.done = 0;
            LG->step = 3;
        }
        break;

    case 3: //前后
        if (1)
        {
            LogicTask.AWork.execute = 1;
            LogicTask.BWork.execute = 1;
            LG->step = 4;
        }
        break;

    case 4: //前后工作
        if (1)
        {
            LogicTask.BWork.done = 0;
            LG->step = 5;
        }
        break;

    case 5: //计数
        if (LogicTask.AWork.execute == 0 && (LogicTask.BWork.execute == 0 || (GUW.button.DeviceMode == NORMALMODE && LogicTask.BWork.done == 1 && Data.TerminalErro == 0)))
        {
            GUW.HadToResset = 1;
//            GUW.UPH = 3600000 / (100000 - UserTimer[0]);
            GUW.UPH = 100000 - UserTimer[0];
//            GUW.UPH = GUW.UPH * GUW.RealWireNum;
            LG->step = ENDCASE;
            //判断定量计数是否到达
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
            //判断定量计数是否到达
            if (GUS.Product.CurrPitchNum >= GUS.Product.PitchNum && GUS.Product.PitchNum > 0)
            {
                GUS.Product.CurrPitchNum = 0;
                GUS.Product.Pitch++;
                //如果停机延时位0，则自动停机
                if (GUS.WirePara.Data.StopDelay == 0)
                {
                    AlarmSetBit(2, 0, 5); //把数数量到
                }
                else
                {
                    Data.PitchNumOK = 1;
                    UserTimer[7] = GUS.WirePara.Data.StopDelay;
                    AlarmSetBit(1, 0, 9);  //把数到达
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
					AlarmSetBit(2, 0, 15); //不良率到达停机
				}
				
			}
            if (GUS.Product.CurrNum >= GUS.Product.NumSet)
            {
                GUS.Product.CurrNum = 0;
                AlarmSetBit(2, 0, 4); //产量到达
            }
        }
        break;

    case 6: //单把暂停时间
        GUW.CurrStopDelay = UserTimer[7];
        if (TimerCnt(LogicTask.RunProcess) > GUS.WirePara.Data.StopDelay)
        {
            Data.PitchNumOK = 0;
            AlarmClearBit(1, 0, 9);  //把数到达
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
//			AlarmSetBit(2, 0, 9); //端子收纸异常
//		}
//		else
//		{
//			Flag = 0;
//			AlarmClearBit(2, 0, 9); //端子收纸异常
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


void ButtonMotion() //按钮动作
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
				GUW.button.RunCommand = RUN; //给状态命令写运行
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
            GUW.button.RunCommand = RUN; //给状态命令写运行
            LogicTask.RunProcess.execute = 1;
            GUW.button.StepRun = 1;
        }
        GUW.button.StepRun = 1;
    }

    //停止按钮
    if (Triger(2, 0, InputGetSta(I_StopButton) == ON,50))
    {
        if (GUR.RunStatus == RUN)
        {
            //			GUW.button.RunCommand = STOP; //给状态命令写停止
            GUW.StopCache = 2;
        }
    }




    //急停按钮
    if (ton(10, InputGetSta(I_EmergencyButton) == OFF, 50))
    {
        ResetWhichBitU16(&GUW.button.BitControlWord, AxisPowerOn);
        GUW.button.RunCommand = SCRAM;
    }
}

//配方操作
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
                case 0://前后轴拉线轴复位
                    if(HZ_AxGetStatus(FrontLesserAxis) == 0 && HZ_AxGetStatus(EndLesserAxis) == 0 && HZ_AxGetStatus(PullWireAxis) == 0)
                    {
                        UserGoHome(FrontLesserAxis);
                        UserGoHome(EndLesserAxis);
                        UserGoHome(PullWireAxis);
                        step = 1;
                    }
                    break;
                    
                case 1://复位到位
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

//停机后一段时间提醒手动加松香
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
    /******工程运行逻辑*******/
    if (GUR.RunStatus == RUN && (PressCheckEn == 0 || InputGetSta(I_pressure) == OFF) && GSR.ErrorLevel < 3)//状态为运行且气压正常且错误等级小于3（安全的前提下）
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
