#include "Teach.h"

//λ��ȷ��
void PosTeach()
{
	if(GUR.RunStatus==RUN)
	{
		GUW.PosTeachWord = 0;
		return;
	}
    if(GUW.ChangeEn  != 0)
    {
        //���ÿ���״̬
        GUW.TeachSetData[GUW.ChangeEn - 1].StateSelect = 0;
        SetWhichBitU16(&GUW.TeachSetData[GUW.ChangeEn - 1].StateSelect,GUW.TeachSetData[GUW.ChangeEn-1].Move - 1);
		GUW.ChangeEn = 0;
	}
    switch(GUW.PosTeachWord)
    {
        case 1://������
            switch(GUW.TeachSetData[GUW.PosTeachWord-1].Move)
            {
                case 1://����λ��
                    GUS.FeedWire.PullFeedPos = GUR.AxisPositionUserUnit[GUW.PosTeachWord-1];
                    break;
                
                default:
                    break;
            }
            break;
            
        
        case 2://ǰ�˰�����
            switch(GUW.TeachSetData[GUW.PosTeachWord-1].Move)
            {
                case 1://������λ
                    GUS.CutWirePara.StripBackPos_A = GUR.AxisPositionUserUnit[GUW.PosTeachWord-1];
                    break;
                
                case 2://���λ��
                    GUS.AMakeTerminal.FrontMakeTerminalPos = GUR.AxisPositionUserUnit[GUW.PosTeachWord-1];
                    break;  
                
                case 3://���λ��1
                    GUS.AInsertPin.Data.FrontAxisInsertPinPos = GUR.AxisPositionUserUnit[GUW.PosTeachWord-1];
                    break;
                
                case 4://���λ��2
                    GUS.AInsertPin.Data.SecondPinDistance = GUR.AxisPositionUserUnit[GUW.PosTeachWord-1];
                    break;
                
                case 5://���λ��
                    GUS.Acheck.Data.CheckPos_Front = GUR.AxisPositionUserUnit[GUW.PosTeachWord-1];
                    break;  
                
                case 6://�����λ
                    GUS.Acheck.Data.CheckBackPos = GUR.AxisPositionUserUnit[GUW.PosTeachWord-1];
                    break;
                
                case 7://����ǰλ
                    GUS.FeedWire.FrontFeedPos = GUR.AxisPositionUserUnit[GUW.PosTeachWord-1];
                    break;
                
                
                
                default:
                    break;
            }
            break;
        
        case 3://��˰�����
            switch(GUW.TeachSetData[GUW.PosTeachWord-1].Move)
            {
                case 1://����ǰλ
                    GUS.FeedWire.EndFeedPos = GUR.AxisPositionUserUnit[GUW.PosTeachWord-1];
                    break;
                
                case 2://������λ
                    GUS.CutWirePara.StripBackPos_B = GUR.AxisPositionUserUnit[GUW.PosTeachWord-1];
                    break;  
                
                case 3://մ�������
                    GUS.WirePara.Data.B.Para.TinPara.AddRosinLength = GUR.AxisPositionUserUnit[GUW.PosTeachWord-1];
                    break;
                
                case 4://մ�����
                    GUS.WirePara.Data.B.Para.TinPara.TiningLength = GUR.AxisPositionUserUnit[GUW.PosTeachWord-1];
                    break;
                
                case 5://����λ��
                    GUS.Unfeed.EndAxisUnFeedPos = GUR.AxisPositionUserUnit[GUW.PosTeachWord-1];
                    break;  
                
                default:
                    break;
            }
            break;  
            
        case 4://�е���
            switch(GUW.TeachSetData[GUW.PosTeachWord-1].Move)
            {
                case 1://�е��ſ�λ��
                    GUS.FeedWire.CutOpenPos = GUR.AxisPositionUserUnit[GUW.PosTeachWord-1];
                    break;
                
                case 2://�ж�λ��
                    GUS.StripInterSkin.C_CutPos = GUR.AxisPositionUserUnit[GUW.PosTeachWord-1];
                    break;  
                
                case 3://�Ե���ֵ
                    GUS.StripInterSkin.C_StripPos = GUR.AxisPositionUserUnit[GUW.PosTeachWord-1];
                    break;
                
                default:
                    break;
            }
            break;
            
        case 5://ǰ������
            switch(GUW.TeachSetData[GUW.PosTeachWord-1].Move)
            {
                case 1://X����λ��
                    GUS.AMakeTerminal.FrontMoveMakeTerminalPos = GUR.AxisPositionUserUnit[GUW.PosTeachWord-1];
                    break;
                
                case 2://X�ᴩ��λ��
                    GUS.AInsertPin.Data.FrontMoveInsertPinPos = GUR.AxisPositionUserUnit[GUW.PosTeachWord-1];
                    break;  
                
                case 3://������λ��
                    GUS.Acheck.Data.CheckPos_Move = GUR.AxisPositionUserUnit[GUW.PosTeachWord-1];
                    break;
                
                case 4://X��ԭ��λ��
                    GUS.FeedWire.FrontRFeedPos = GUR.AxisPositionUserUnit[GUW.PosTeachWord-1];
                    break;
                
                default:
                    break;
            }
            break;
            
        case 6://�ڱ���
            switch(GUW.TeachSetData[GUW.PosTeachWord-1].Move)
            {
                case 1://ˮƽλ��
                    GUS.FeedWire.EndRFeedPos = GUR.AxisPositionUserUnit[GUW.PosTeachWord-1];
                    break;
                
                case 2://մ����λ��
                    GUS.B_Tin.B_R_RosinPos = GUR.AxisPositionUserUnit[GUW.PosTeachWord-1];
                    break;  
                
                case 3://մ��λ��
                    GUS.B_Tin.B_R_TinPos = GUR.AxisPositionUserUnit[GUW.PosTeachWord-1];
                    break;
                
                default:
                    break;
            }
            break;
            
        case 8://�������
            switch(GUW.TeachSetData[GUW.PosTeachWord-1].Move)
            {
                case 1://ȡ����λ��
                    GUS.AFeedPin.Data.TakePinPos = GUR.AxisPositionUserUnit[GUW.PosTeachWord-1];
                    break;
                
                case 2://�����λ��
                    GUS.AInsertPin.Data.FirstPinDistance = GUR.AxisPositionUserUnit[GUW.PosTeachWord-1];
                    break;  
                
                default:
                    break;
            }
            break;     
			
				case 9://�Ͷ�����
            switch(GUW.TeachSetData[GUW.PosTeachWord-1].Move)
            {
                case 1://
                    GUS.AFeedPin.Data.FeedBackPos = GUR.AxisPositionUserUnit[GUW.PosTeachWord-1];
                    break;
				
                case 2://
                    GUS.AFeedPin.Data.FeedPos[0] = GUR.AxisPositionUserUnit[GUW.PosTeachWord-1];
                    break;
				
                case 3://
                    GUS.AFeedPin.Data.FeedPos[1] = GUR.AxisPositionUserUnit[GUW.PosTeachWord-1];
                    break;
				
                case 4://
                    GUS.AFeedPin.Data.FeedPos[2] = GUR.AxisPositionUserUnit[GUW.PosTeachWord-1];
                    break;
				
                case 5://
                    GUS.AFeedPin.Data.FeedPos[3] = GUR.AxisPositionUserUnit[GUW.PosTeachWord-1];
                    break;                
				
                case 6://
                    GUS.AFeedPin.Data.FeedPos[4] = GUR.AxisPositionUserUnit[GUW.PosTeachWord-1];
                    break;                
                
                default:
                    break;
            }
            break;
        
        default:
            break;
    }
	GUW.PosTeachWord = 0;    
}

//λ���ƶ�
void PosMove()
{
	if(GUR.RunStatus==RUN)
	{
		GUW.PosMoveWord = 0;
		return;
	}
//    if(GUW.PosMoveWord)
//    {
//        AlarmSetBit(2, 1, 0);  //ϵͳ���е㶯�������ԭ���ȷ�ϰ�ȫ������
//    }
    switch(GUW.PosMoveWord)
    {
        case 1://������
            switch(GUW.TeachSetData[GUW.PosMoveWord-1].Move)
            {
                case 1://�ƶ�������λ��
                    if(UserGetStatus(PullWireAxis) == 0 && UserGetOutput(Q_EndClam) == OFF
                        && GUR.AxisPositionUserUnit[CutAxis] < GUS.FeedWire.CutOpenPos+10)
                    {
                        AxMoveAbsUserUnit(PullWireAxis, 4, GUS.FeedWire.PullFeedPos);
                    }
                    break;
                
                default:
                    break;
            }
            break;
            
        
        case 2://ǰ�˰�����
            switch(GUW.TeachSetData[GUW.PosMoveWord-1].Move)
            {
                case 1://�ƶ�������λ��
                    if(UserGetStatus(FrontLesserAxis) == 0)
                    {
                        AxMoveAbsUserUnit(FrontLesserAxis, 4, GUS.CutWirePara.StripBackPos_A);
                    }
                    break;
                    
                case 2://�ƶ������λ��
                    if(UserGetStatus(FrontLesserAxis) == 0)
                    {
                        AxMoveAbsUserUnit(FrontLesserAxis, 4, GUS.AMakeTerminal.FrontMakeTerminalPos); //ǰ���ᵽ���λ��
                    }
                    break; 
                    
                case 3://�ƶ������λ��1
                    if(UserGetStatus(FrontLesserAxis) == 0)
                    {
                        AxMoveAbsUserUnit(FrontLesserAxis,4, GUS.AInsertPin.Data.FrontAxisInsertPinPos); //
                    }
                    break;
                    
                case 4://�ƶ������λ��2
                    if(UserGetStatus(FrontLesserAxis) == 0)
                    {
                        AxMoveAbsUserUnit(FrontLesserAxis,4, GUS.AInsertPin.Data.SecondPinDistance); 
                    }
                    break;
                    
                case 5://���ǰ��
                    if(UserGetStatus(FrontLesserAxis) == 0)
                    {
                        AxMoveAbsUserUnit(FrontLesserAxis,4, GUS.Acheck.Data.CheckPos_Front); 
                    }
                    break;
                    
                case 6://�����λ
                    if(UserGetStatus(FrontLesserAxis) == 0)
                    {
                        AxMoveAbsUserUnit(FrontLesserAxis,4, GUS.Acheck.Data.CheckBackPos); 
                    }
                    break;
                    
                    
                case 7://����ǰλ
                    if(UserGetStatus(FrontLesserAxis) == 0)
                    {
                        AxMoveAbsUserUnit(FrontLesserAxis,4, GUS.FeedWire.FrontFeedPos); 
                    }
                    break;
                
                default:
                    break;
            }
            break;
        
        case 3://��˰�����
            switch(GUW.TeachSetData[GUW.PosMoveWord-1].Move)
            {
                case 1://����ǰλ
                    if(UserGetStatus(EndLesserAxis) == 0)
                    {
                        AxMoveAbsUserUnit(EndLesserAxis, 4,GUS.FeedWire.EndFeedPos);
                    }
                    break;
                    
                case 2://������λ
                    if(UserGetStatus(EndLesserAxis) == 0)
                    {
                        AxMoveAbsUserUnit(EndLesserAxis, 4,GUS.CutWirePara.StripBackPos_B);
                    }
                    break;
                
                case 3://մ�������
                    if(UserGetStatus(EndLesserAxis) == 0)
                    {
                        AxMoveAbsUserUnit(EndLesserAxis, 4, GUS.WirePara.Data.B.Para.TinPara.AddRosinLength);
                    }
                    break;
                
                case 4://մ�����
                    if(UserGetStatus(EndLesserAxis) == 0)
                    {
                        AxMoveAbsUserUnit(EndLesserAxis, 4,GUS.WirePara.Data.B.Para.TinPara.TiningLength);
                    }
                    break;
                
                case 5://����λ��
                    if(UserGetStatus(EndLesserAxis) == 0)
                    {
                         AxMoveAbsUserUnit(EndLesserAxis, 4,GUS.Unfeed.EndAxisUnFeedPos);
                   }
                    break;
                
                default:
                    break;
            }
            break;
            
        case 4://�е���
            switch(GUW.TeachSetData[GUW.PosMoveWord-1].Move)
            {
                case 1://�е��ſ�λ��
                    if(UserGetStatus(CutAxis) == 0)
                    {
                        AxMoveAbsUserUnit(CutAxis, 4,GUS.FeedWire.CutOpenPos);
                    }
                    break;
                    
                case 2://�ж�λ��
                    if(GUR.AxisPositionUserUnit[FrontLesserAxis] < 10 && GUR.AxisPositionUserUnit[EndLesserAxis] < 10 && GUR.AxisPositionUserUnit[PullWireAxis] < 10)
                    {
                        if(UserGetStatus(CutAxis) == 0)
                        {
                            AxMoveAbsUserUnit(CutAxis, 4,GUS.StripInterSkin.C_CutPos);
                        }
                    }
                    else
                    {
                        if(Data.Z_JOGEn != 2)
                        {
                            Data.Z_JOGEn = 2;
                        }
                    }
                    break;
                
                case 3://�Ե���ֵ
                     if(GUR.AxisPositionUserUnit[FrontLesserAxis] < 10 && GUR.AxisPositionUserUnit[EndLesserAxis] < 10 && GUR.AxisPositionUserUnit[PullWireAxis] < 10)
                    {
                        if(UserGetStatus(CutAxis) == 0)
                        {
                            AxMoveAbsUserUnit(CutAxis, 4, GUS.StripInterSkin.C_StripPos);
                        }
                    }
                    else
                    {
                        if(Data.Z_JOGEn != 2)
                        {
                            Data.Z_JOGEn = 2;
                        }
                    }
                    break;
                
                default:
                    break;
            }
            break;

            
        case 5://ǰ������
            switch(GUW.TeachSetData[GUW.PosMoveWord-1].Move)
            {
                case 1://X����λ��
                    if(UserGetStatus(FrontPrimaryAxis) == 0 && GUR.AxisPositionUserUnit[FrontLesserAxis] < GUS.CutWirePara.StripBackPos_A + 10)
                    {
                        AxMoveAbsUserUnit(FrontPrimaryAxis, 4,GUS.AMakeTerminal.FrontMoveMakeTerminalPos);
                    }
                    break;
                    
                case 2://X�ᴩ��λ��
                    if(UserGetStatus(FrontPrimaryAxis) == 0 && GUR.AxisPositionUserUnit[FrontLesserAxis] < GUS.CutWirePara.StripBackPos_A + 10)
                    {
                        AxMoveAbsUserUnit(FrontPrimaryAxis, 4,GUS.AInsertPin.Data.FrontMoveInsertPinPos);
                    }
                    break;
                
                case 3://������λ��
                    if(UserGetStatus(FrontPrimaryAxis) == 0 && GUR.AxisPositionUserUnit[FrontLesserAxis] < GUS.CutWirePara.StripBackPos_A + 10)
                    {
                        AxMoveAbsUserUnit(FrontPrimaryAxis, 4,GUS.Acheck.Data.CheckPos_Move);
                    }
                    break;
                
                case 4://X��ԭ��λ��
                    if(UserGetStatus(FrontPrimaryAxis) == 0 && GUR.AxisPositionUserUnit[FrontLesserAxis] < GUS.CutWirePara.StripBackPos_A + 10)
                    {
                        AxMoveAbsUserUnit(FrontPrimaryAxis, 4,GUS.FeedWire.FrontRFeedPos);
                    }
                    break;
                
                default:
                    break;
            }
            break;  
            
        case 6://�ڱ���
            switch(GUW.TeachSetData[GUW.PosMoveWord-1].Move)
            {
                case 1://ˮƽλ��
                    if(UserGetStatus(EndPrimaryAxis) == 0 && GUR.AxisPositionUserUnit[EndLesserAxis] < GUS.CutWirePara.StripBackPos_B + 10)
                    {
                        AxMoveAbsUserUnit(EndPrimaryAxis, 4,GUS.FeedWire.EndRFeedPos);
                    }
                    break;
                    
                case 2://����λ��
                    if(UserGetStatus(EndPrimaryAxis) == 0 && GUR.AxisPositionUserUnit[EndLesserAxis] < GUS.CutWirePara.StripBackPos_B + 10)
                    {
                        AxMoveAbsUserUnit(EndPrimaryAxis, 4,GUS.B_Tin.B_R_RosinPos);
                    }
                    break;
                
                case 3://մ��λ��
                    if(UserGetStatus(EndPrimaryAxis) == 0 && GUR.AxisPositionUserUnit[EndLesserAxis] < GUS.CutWirePara.StripBackPos_B + 10)
                    {
                        AxMoveAbsUserUnit(EndPrimaryAxis, 4,GUS.B_Tin.B_R_TinPos);
                    }
                    break;
                
                default:
                    break;
            }
            break;
            
        case 8://�������
            switch(GUW.TeachSetData[GUW.PosMoveWord-1].Move)
            {
                
                case 1://ȡ����λ��
                    if(UserGetStatus(InsertTerminalAxis) == 0)
                    {
                        AxMoveAbsUserUnit(InsertTerminalAxis, 4,GUS.AFeedPin.Data.TakePinPos);
                    }
                    break;
                    
                case 2://�����λ��
                    if(UserGetStatus(InsertTerminalAxis) == 0)
                    {
                        AxMoveAbsUserUnit(InsertTerminalAxis, 4,GUS.AInsertPin.Data.FirstPinDistance);
                    }
                    break;
                
                default:
                    break;
            }
            break;        

            
		case 9://�ͽ�����
            switch(GUW.TeachSetData[GUW.PosMoveWord-1].Move)
            {
                
                case 1://
                    if(UserGetStatus(PinFeedAxis) == 0)
                    {
                        AxMoveAbsUserUnit(PinFeedAxis, 4,GUS.AFeedPin.Data.FeedBackPos);
                    }
                    break;
                    
                case 2://
                    if(UserGetStatus(PinFeedAxis) == 0)
                    {
                        AxMoveAbsUserUnit(PinFeedAxis, 4,GUS.AFeedPin.Data.FeedPos[0]);
                    }
                    break;
                    
                case 3://
                    if(UserGetStatus(PinFeedAxis) == 0)
                    {
                        AxMoveAbsUserUnit(PinFeedAxis, 4,GUS.AFeedPin.Data.FeedPos[1]);
                    }
                    break;
                    
                case 4://
                    if(UserGetStatus(PinFeedAxis) == 0)
                    {
                        AxMoveAbsUserUnit(PinFeedAxis, 4,GUS.AFeedPin.Data.FeedPos[2]);
                    }
                    break;
                    
                case 5://
                    if(UserGetStatus(PinFeedAxis) == 0)
                    {
                        AxMoveAbsUserUnit(PinFeedAxis, 4,GUS.AFeedPin.Data.FeedPos[3]);
                    }
                    break;
                    
                case 6://
                    if(UserGetStatus(PinFeedAxis) == 0)
                    {
                        AxMoveAbsUserUnit(PinFeedAxis, 4,GUS.AFeedPin.Data.FeedPos[4]);
                    }
                    break;
                    
                default:
                    break;
            }
            break;        

            
        default:
            break;
    }
	GUW.PosMoveWord = 0;    
}


void Teach()
{
    u32 i;
//	if(GUR.RunStatus==RUN)
//	{
//		GUW.TechWord = 0;
//		return;
//	}

    switch(GUW.TechWord)
    {

    case 1://�������
        for(i = 0; i < 20; i++)
        {
            GSR.ErrorCode[i] = 0;
        }
        GSW.ClearAlarm = 1;
        break;

    case 2://����
        if(GSR.ErrorLevel < 2)//
        {
            Data.Stop = 0;
        }
        if(GUR.RunStatus == STOP)
        {
			if(Data.AddRosinAlarm == 1)
			{
				Data.AddRosinAlarm = 2;
			}
			else
			{
				GUW.button.RunCommand = RUN; //��״̬����д����
				LogicTask.RunProcess.execute = 1;
				GUW.button.StepRun = 1;
			}
        }
        GUW.button.StepRun = 1;
        break;

    case 3://ֹͣ
        if(GUR.RunStatus == RUN)
        {
//				GUW.button.RunCommand = STOP; //��״̬����дֹͣ
            GUW.StopCache = 2;
        }
        break;

    case 4://��λ
        if(GUR.RunStatus != RUN)
        {
            GUW.button.RunCommand = D_RESET;
            Data.ChangeMould = 0;
        }
        break;

    case 5:
        GUW.button.RunCommand = SCRAM;
        break;

    case 6:
        GUS.SumCount.SumProduactCount =
            GUS.SumCount.SumProduactCount_k =
                GUS.SumCount.SumProduactCount_kk =
                    GUS.SumCount.SumProduactCount_kkk =
                        GUS.SumCount.SumProduactCount_kkkk = 0;

        break;

    case 7:
        AlarmClearBit(1, 0, 5); //
        if(File_Write(0, &GSS, 0, SAVELEN, 0))
        {
            AlarmSetBit(1, 0, 5); //SD��д��ʧ��
        }
        break;

    case 8:
        AlarmClearBit(1, 0, 4); //
        if(File_Read(0, &GSS, 0, SAVELEN, 0))
        {
            AlarmSetBit(1, 0, 4); //SD����ȡʧ��
        }
        break;

    case 9:
        GUW.WireNoTerminalCount = 0;
        GUW.WireNoTerminalState = 0;
        break;

    case 10:
        break;
    

    case 11://��λ ����ģ��
        if(GUR.RunStatus != RUN)
        {
            GUW.button.RunCommand = D_RESET;
            Data.ChangeMould = 1;
        }
        break;

    case 12://
        if(LogicTask.AFeedTin.execute == 0)
        {
            LogicTask.AFeedTin.execute = 1;
        }
        break;

    case 13:
        if(LogicTask.ASmoothTin.execute == 0)
        {
            LogicTask.ASmoothTin.execute = 1;
        }
        break;

    case 14://
        if(LogicTask.BFeedTin.execute == 0)
        {
            LogicTask.BFeedTin.execute = 1;
        }
        break;

    case 15:
        if(LogicTask.BSmoothTin.execute == 0)
        {
            LogicTask.BSmoothTin.execute = 1;
        }
        break;

    case 16:
        Data.DivideFirstWire = GUS.SperatePara.cutterSeparateDistance / GUS.WirePara.Data.WrieLength + 1;
        break;

    case 17:
        Data.TerminalErro = 1;
        if(GSR.ErrorLevel < 2)//
        {
            Data.Stop = 0;
        }
        if(GUR.RunStatus == STOP)
        {
            GUW.button.RunCommand = RUN; //��״̬����д����
            LogicTask.RunProcess.execute = 1;
            GUW.button.StepRun = 1;
        }
        GUW.button.DeviceMode = CYCLEMODE;
        GUW.button.StepRun = 1;
        break;
    
   

    case 22://���õ�ǰʱ��

		RTC_Set_Date(GUW.CurrDateSetForUser.year, GUW.CurrDateSetForUser.month, GUW.CurrDateSetForUser.day, GUW.CurrDateSetForUser.week);
        RTC_Set_Time(GUW.CurrTimeSetForUser.hour, GUW.CurrTimeSetForUser.min, GUW.CurrTimeSetForUser.sec, GUW.CurrTimeSetForUser.ampm);
        break;

    case 23:
        if(unlock(GSW.LockPara.decode, &GSS.LockDate, XWD, GSR.CID[0]) == 0)
        {
            DeDate(&GSS.LockDate.lockdata, &GUW.CurrDateForUser);
//            GSW.FlashOperate = 2;
        }
        break;

    case 24://���������
        CID_init(XWD, GSR.CID);
        break;
    
    case 25://����
        AlarmClearBit(2, 0, 13); //ѹ����ⱨ��
        Data.StopCurrWire = 1;
        Data.TerminalErro = 1;
       for(i = 0; i < 20; i++)
        {
            GSR.ErrorCode[i] = 0;
        }
        GSW.ClearAlarm = 1;
        break;
    
    case 26://���������
        AlarmClearBit(2, 0, 13); //ѹ����ⱨ��
        Data.TerminalErro = 0;
        for(i = 0; i < 20; i++)
        {
            GSR.ErrorCode[i] = 0;
        }
        Data.StopCurrWire = 1;
        GSW.ClearAlarm = 1;
        break; 
        
    case 27://������ͣ�������
        AlarmClearBit(2, 0, 5); 
        for(i = 0; i < 20; i++)
        {
            GSR.ErrorCode[i] = 0;
        }
        Data.StopCurrWire = 1;
        GSW.ClearAlarm = 1;
        break;  
        
    case 28://ȫ�Զ�-����
        if(GSR.ErrorLevel < 2)//
        {
            Data.Stop = 0;
        }
        if(GUR.RunStatus == STOP)
        {
            GUW.button.DeviceMode = NORMALMODE;//0
            GUW.button.RunCommand = RUN; //��״̬����д����
            LogicTask.RunProcess.execute = 1;
            GUW.button.StepRun = 1;
        }
        GUW.button.StepRun = 1;
        break;  
        
    case 29://����-����
        if(GSR.ErrorLevel < 2)
        {
            Data.Stop = 0;
        }
        if(GUR.RunStatus == STOP)
        {
            GUW.button.DeviceMode = CYCLEMODE;//2
            GUW.button.RunCommand = RUN; //��״̬����д����
            LogicTask.RunProcess.execute = 1;
            GUW.button.StepRun = 1;
        }
        GUW.button.StepRun = 1;
        break;    
        
    case 30://��ͣ
        if(GUR.RunStatus == RUN)
        {
            GUW.button.DeviceMode = STEPMODE;//1
            GUW.button.RunCommand = STOP; //��״̬����д����
        }
        break;
        
        
    case 31://����-����
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
        break;   
        
    case 32://���
        if(UserGetStatus(TerminalAxis) == 0)
        {
            AxMoveRelUserUnit(TerminalAxis, GUS.SpdRatio[TerminalAxis], 36000); //תһȦ
        }
        break;   
    }
    GUW.TechWord = 0;
	
	PosTeach();
	PosMove();
}

