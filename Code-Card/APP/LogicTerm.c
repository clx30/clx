/*
 * File: LogicTerm.c
 * File Created: 2019.5.7
 * version: v00.00.01 12��ײ�
 * Description:�߼����飬��Ҫ���ڸ����߼��Ĳ�����Ҫһֱ���еĺ�������������
 * Modified By:NYZ
 * -----
 * Copyright 2019 - 2019, <<hzzh>>
 */


#include "Logic.h"
#include "FSM.h"
#include "USERTOOL.H"



/**
* @author�� 2019/1/9 ũҵ��
* @Description: ��������ť����
* @param �C
* @param --
* @return --
*/
void HMI_Button()
{
    //ֹͣ������һ����ֹͣ
    if (WordToBit(GUW.button.BitControlWord, StopBit) && GUR.RunStatus == RUN)
    {
        Data.StopFlag = 1;
    }


    //�ŷ�ʹ��
    if (WordToBit(GUW.button.BitControlWord, AxisPowerOn))
    {
		//�����ŷ��������ᣩ��ʹ��
        EN_SetSta(0, OFF);
        EN_SetSta(1, OFF);
        EN_SetSta(2, OFF);
        EN_SetSta(3, OFF);
        EN_SetSta(4, OFF);
        EN_SetSta(5, ON);
        EN_SetSta(6, ON);
        EN_SetSta(7, OFF);
        EN_SetSta(8, OFF);
        EN_SetSta(9, ON);
        EN_SetSta(10, OFF);
        EN_SetSta(11, ON);
    }
    else
    {
        GUR.HaveToReset = 1;
        EN_SetSta(0, ON);
        EN_SetSta(1, ON);
        EN_SetSta(2, ON);
        EN_SetSta(3, ON);
        EN_SetSta(4, ON);
        EN_SetSta(5, OFF);
        EN_SetSta(6, OFF);
        EN_SetSta(7, ON);
        EN_SetSta(8, ON);
        EN_SetSta(9, OFF);
        EN_SetSta(10, ON);
        EN_SetSta(11, OFF);
    }
}


/**
* @author 19/02/22  wangh
* @Description: ��������
* @param --
* @param --
* @return --
*/
void Alarm()
{
    int i;
	if(BTinWarmEn)
	{
		UserOutputSet(Q_EndHot,ON);
	}
	else
	{
		UserOutputSet(Q_EndHot,OFF);
	}
    if(PressCheckEn)
    {
        if(ton(20, InputGetSta(I_pressure) == ON, 10))
        {
            ResetWhichBitU16(&GUW.button.BitControlWord, AxisPowerOn);
            for(i = 0; i < 12; i++)
            {
                HZ_AxStop(i);
            }
            AlarmSetBit(2, 0, 3); //��ѹ����
            GUR.HaveToReset = 1;
        }
    }
    else
    {
        AlarmClearBit(2, 0, 3); //��ѹ����
    }

    if (NoWireCheckEn)
    {
        if (InputGetSta(I_NoWire) == ON)
        {
            AlarmSetBit(2, 0, 7); //ȱ��
        }
    }

    if (WireKnotCheckEn)
    {
//        if (InputGetSta(I_wireKnotCheck) == ON)
//        {
//            AlarmSetBit(2, 0, 2); //���
//        }
    }

	if(ton(17,InputGetSta(I_FeedNoTerminal) == OFF,100))
	{
		UserOutputSet(Q_FeedTerminal,ON);
	}
	else
	{
		UserOutputSet(Q_FeedTerminal,OFF);
	}
    if(BNoTerminalCheckEn)
    {
        if(ton(4,InputGetSta(I_FeedNoTerminal) == OFF, 5000))
        {
            AlarmSetBit(2, 0, 12); //ȱ���ӱ���
        }
    }
    else
    {
        AlarmClearBit(2, 0, 12);
		ton_et[4] = 0;
    }

    if(BPressCheckEn && GUR.RunStatus == RUN)
    {
        if (InputGetSta(I_PressCheck) == ON)
        {
			UserOutputSet(Q_PressRest,ON);
			Data.TerminalErro = 2;
        }
    }
    if(Data.TerminalErro == 1)
    {
        if(GUW.button.DeviceMode == NORMALMODE)
        {
            GUW.button.DeviceMode = STEPMODE;
        }
    }
	if(AVisionCheckEn && Data.VisionCheck == 1)
	{
		if(InputGetSta(I_VisionCheck) == ON)
		{
			Data.VisionCheck = 2;
		}
	}
	
	if(ton(5,UserGetOutput(Q_PressRest) == ON,200))
	{
		UserOutputSet(Q_PressRest,OFF);
	}
    
    //�߳�������ʱ�������߿��ر���
    if(GUS.WirePara.Data.WrieLength >= WireLengthClam)
    {
        if(ShortModeForUser)
        {
            AlarmSetBit(2, 1, 4); //��ǰ�߳��������޷�ʹ�ö���ģʽ
        }
    }
    if(NoTinEn)
    {
        if(ton(2,InputGetSta(I_EndNoTin) == ON,5000))
        {
            AlarmSetBit(2, 1, 7); //���ȱ��
        }
    }
    else
    {
        ton_et[2] = 0;
    }
	
	//���ױ���
	if(ton(6,InputGetSta(I_PinOFF) == OFF && UserGetOutput(Q_FrontPush) == OFF,3000))
	{
		GUW.button.DeviceMode = STEPMODE;
		GUW.button.RunCommand = STOP; //��״̬����д����
		AlarmSetBit(2,1,8); //�������ԭ�㱨��
	}
	if(ton(8,InputGetSta(I_PinON) == OFF && UserGetOutput(Q_FrontPush) == ON,3000))
	{
		GUW.button.DeviceMode = STEPMODE;
		GUW.button.RunCommand = STOP; //��״̬����д����
		AlarmSetBit(2,1,9);//������׵�λ����
	}
	
	//���Ǽ��
	if(PinCheackEn)
	{
		if(ton(3,((InputGetSta(I_PinCheck) == OFF || InputGetSta(I_PinCheck2) == OFF) && LogicTask.AFeedPin.step < 3),3000))
		{
			AlarmSetBit(2,1,10);//���Ǽ�ⱨ��
			if(LogicTask.BTin.execute == 0)
			{
				GUW.button.DeviceMode = STEPMODE;
				GUW.button.RunCommand = STOP; //��״̬����д����
			}
	   }
	}
	else
	{
		ton_et[3] = 0;
	}
}


/**
* @author�� 2019/2/18 ׯ��
* @Description: IO���
* @param �C
* @param --
* @return --
*/
void IOCheck()
{
//	if(ExInPut_GetSta(0,I_Line_Reaction)==ON)
//	{
//		OutPut_SetSta(Q_Line_Off,OFF);
//	}
//
//	if(R_Trig(5,ExInPut_GetSta(0,I_Line_Reaction)))
//	{
//		OutPut_SetSta(Q_Line_Off,ON);
//	}
//
//	if(ExInPut_GetSta(0,I_Knot_Alarm) == ON)
//	{
//		OutPut_SetSta(Q_Line_Off,OFF);
//		AlarmSetBit(2,0,8);//�߼ܴ��
//	}
}
/**
* @author�� 2019/08/2  nyz
* @Description:  ָʾ��
* @param --
* @param --
* @return --
*/
s32 BlinkTimer[20] = {0};
s32 BlinkFlag[20] = {0};
s8 Blink(u8 num, u32 OnDelay, u32 OffDelay)
{
    if(num >= 20)
    {
        return -1;
    }

    if(SysTim_GetUser() < BlinkTimer[num])
    {
        BlinkTimer[num] = SysTim_GetUser();
    }

    if(BlinkFlag[num] == 0)
    {

        if((SysTim_GetUser() - BlinkTimer[num]) > OnDelay * 10)
        {
            BlinkTimer[num] = SysTim_GetUser();
            BlinkFlag[num]  = 1;
        }
    }
    else
    {
        if((SysTim_GetUser() - BlinkTimer[num]) > OffDelay * 10)
        {
            BlinkTimer[num] = SysTim_GetUser();
            BlinkFlag[num]  = 0;
        }
    }
    return BlinkFlag[num];
}

void LedCtrol()
{
    if(GSR.ErrorLevel > 1 && Data.InitAlarm == 1)
    {//����
       UserOutputSet(Q_Yellow,OFF); 
       UserOutputSet(Q_Green,OFF); 
       UserOutputSet(Q_Red, OFF);
        if(GUR.RunStatus != D_RESET)
        {
            if((GSR.ErrorLevel > 1 && GSR.ErrorCode[5] != 1) ||
            (GSR.ErrorLevel > 2 && GSR.ErrorCode[8] != 1) || 
            (GSR.ErrorLevel == 4))
            {
                if(Blink(1, 150, 150))
                {
                    UserOutputSet(Q_Red, ON);
                }
                else
                {
                    UserOutputSet(Q_Red, OFF);
                }
            }
        }
    }
    else
    {
		SetWhichBitU16(&GUS.FunctionEn,28);
        if(GUR.RunStatus == RUN)
        {//�̵�
           UserOutputSet(Q_Yellow,OFF); 
           UserOutputSet(Q_Green,ON); 
           UserOutputSet(Q_Red, OFF);
           if(Data.PitchNumOK == 1)
            {
                if(Blink(1, 300, 300))
                {
                    UserOutputSet(Q_Red, ON);
                }
                else
                {
                    UserOutputSet(Q_Red, OFF);
                }
            }
            else
            {
                UserOutputSet(Q_Red,OFF); 
            }
        }
        else
        {//�Ƶ�
           UserOutputSet(Q_Yellow,ON); 
           UserOutputSet(Q_Red,OFF); 
           UserOutputSet(Q_Green,OFF); 
        }
    }
	
	if(BuzzerEn == 0)
	{
	   UserOutputSet(Q_Red,OFF); 
	}
}


/**
* @author�� 2019/5/7 NYZ
* @Description: ������������Ҫ�����ڸ��߼��޹�����Ҫһֱ���еĺ���������������
* @param �C
* @param --
* @return --
*/
void AuxLogic()
{
    HMI_Button();
    Alarm();
    IOCheck();
}

