#include "reset.h"
#include "AppInit.h"

LogicParaDef ResetTask = {0};
void Reset()
{
    u8 i;

    if (GUR.RunStatus == D_RESET)
    {
        switch (ResetTask.step)
        {
        case 0:
//			AppInit();
            for (i = 0; i < 12; i++)
            {
                HZ_AxReset(i);
            }
            for (i = 0; i < GPO_NUM; i++) //初始化 输出口
            {
                if(i != 15 )
                    OutPut_SetSta(i, OFF);
            }
            SetWhichBitU16(&GUW.button.BitControlWord, AxisPowerOn);\
			UserOutputSet(Q_ServoReset,ON);
            PARAINIT(LogicTask);
            GSW.ClearAlarm = 1;
            ResetTask.step = 1;
            break;

        case 1://拉线轴先回原点
            UserGoHome(TerminalAxis);
            UserGoHome(PullWireAxis);
            ResetTask.step = 2;
            break;

        case 2://切刀复位
            for (i = 0; i < 12; i++)
            {
                if (HZ_AxGetStatus(i) != 0)
                {
                    return;
                }
            }
            if(StrepEn)
            {
                UserGoHome(StrepAxis);
            }
            UserGoHome(CutAxis);
//            AxMoveAbsUserUnit(FrontLesserAxis, GUS.SpdRatio[FrontLesserAxis], 0);
            ResetTask.step = 3;

            break;

        case 3://前后剥复位
            if (HZ_AxGetStatus(FrontLesserAxis) == 0)
            {
                UserGoHome(FrontLesserAxis);
                UserGoHome(EndLesserAxis);
                ResetTask.step = 4;
            }
            break;

        case 4:
            if (HZ_AxGetStatus(FrontLesserAxis) == 0 && HZ_AxGetStatus(EndLesserAxis) == 0)
            {
				UserOutputSet(Q_ServoReset,OFF);
                ResetTask.step = 5;
            }
            break;

        case 5:
            if (HZ_AxGetStatus(PullWireAxis) == 0 && HZ_AxGetStatus(FrontLesserAxis) == 0 && HZ_AxGetStatus(EndLesserAxis) == 0)
            {
                
                UserGoHome(InsertTerminalAxis);
                UserGoHome(PinFeedAxis);
                UserGoHome(FrontPrimaryAxis);
                UserGoHome(EndPrimaryAxis);
                ResetTask.step = 6;
            }
            break;

        case 6:
            ResetTask.step = 7;
            break;

        case 7:
            for (i = 0; i < 12; i++)
            {
                if (HZ_AxGetStatus(i) != 0)
                {
                    return;
                }
            }
            if(TwistMode)
            {
                UserOutputSet(Q_Twist, OFF);
            }
            else
            {
                UserOutputSet(Q_Twist, ON);
            }
            if(StrepEn)
            {
                AxMoveAbsUserUnit(StrepAxis, GUS.SpdRatio[StrepAxis], GUS.Unfeed.StrepFeedPos);
            }
            AxMoveAbsUserUnit(CutAxis, GUS.SpdRatio[CutAxis], GUS.FeedWire.CutOpenPos);
            AxMoveAbsUserUnit(FrontPrimaryAxis, GUS.SpdRatio[FrontPrimaryAxis], GUS.FeedWire.FrontRFeedPos);
            AxMoveAbsUserUnit(EndPrimaryAxis, GUS.SpdRatio[EndPrimaryAxis], GUS.FeedWire.EndRFeedPos);
			AxMoveAbsUserUnit(InsertTerminalAxis, GUS.SpdRatio[InsertTerminalAxis], GUS.AFeedPin.Data.TakePinPos);
            AxMoveAbsUserUnit(PullWireAxis, 30, GUS.FeedWire.PullFeedPos);
            ResetTask.step = 8;
            break;

        case 8:
            if(HZ_AxGetStatus(InsertTerminalAxis) == 0)
            {
				AxMoveAbsUserUnit(PinFeedAxis, 30, 5570);
				ResetTask.step = 9;
            }
            break;
			
        case 9:
            if(HZ_AxGetStatus(PinFeedAxis) == 0)
            {
				AxMoveAbsUserUnit(PinFeedAxis, GUS.SpdRatio[PinFeedAxis], GUS.AFeedPin.Data.FeedBackPos);
				RTC_Set_Date(GUW.CurrDateSetForUser.year, GUW.CurrDateSetForUser.month, GUW.CurrDateSetForUser.day, GUW.CurrDateSetForUser.week);
				RTC_Set_Time(GUW.CurrTimeSetForUser.hour, GUW.CurrTimeSetForUser.min, GUW.CurrTimeSetForUser.sec, GUW.CurrTimeSetForUser.ampm);
				ResetTask.step = 10;
            }
            break;
            
        case 10:
            for (i = 0; i < 12; i++)
            {
                if (HZ_AxGetStatus(i) != 0)
                {
                    return;
                }
            }
			CID_init(XWD, GSR.CID);
            Data.StopCurrWire = 0;
            Data.TerminalErro = 0;
            GUR.HaveToReset = 0;
            GUR.HaveToRepower = 0;
            GUW.button.RunCommand = STOP;
            GUR.RunStatus = STOP;
            ResetTask.execute = 0;
            ResetTask.step = 0;
            ResetTask.done = 1;
            GSW.ClearAlarm = 1;
            GUW.HadToResset = 1;
            Data.BuzzerSet = 1;
			Data.BadWire = 0;
			Data.BadStopNum = 0;
			Data.OKStopNum = 0;
			Data.InitAlarm = 1;
			Data.VisionCheck = 2;
			Data.LockEn = 1;
            break;
        }
    }
}
