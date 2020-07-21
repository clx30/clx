#include "FSM.h"
#include "can_master.h"

/*!
* @file  FSM.c
* @brief ��̨״̬����ͨ������̬��ֹ̬ͣ����λ̬����̬ͣ����̬ͣ��״̬�����ƻ�̨��ȫ���С�
* @version 1.1 bsp.lib 1.1 int.lib
* @author  yfs
* @date 2018/08/29
*/
extern LogicParaDef ResetTask;

void Button()
{

}
//����ı���״̬
void AxisErro()
{
    u8 i;
    for(i = 0; i < 12; i++)
    {
        if(AXSTA_ERRSTOP == HZ_AxGetStatus(i))
        {
            //����λ
            if( HZ_AxGetAxisErr(i) == AX_ERR_DW_LIM_H || AX_ERR_DW_LIM_S == HZ_AxGetAxisErr(i)) //||(AX_ERR_DW_LIM_S&HZ_AxGetAxisErr(i))==1)
            {
                HZ_AxReset(i);
                AlarmSetBit(3, 0, i + 1);
            }
            //����λ
            if( AX_ERR_UP_LIM_H == HZ_AxGetAxisErr(i) || AX_ERR_UP_LIM_S == HZ_AxGetAxisErr(i))
            {
                HZ_AxReset(i);
                AlarmSetBit(3, 1, i + 1);
            }

        }
    }

    //���ŷ������ź�
    for(i = 0; i < 12; i++)
    {
        if(i == 0 || i == 1 || i == 2 || i == 3 || i == 4 || i == 5 || i == 7 || i == 6 || i == 8)
        {
            if(OFF == ALM_GetSta(i))
            {
                HZ_AxStop(i);
                AlarmSetBit(4, 0, i + 1);
//                ResetWhichBitU16(&GUW.button.BitControlWord, AxisPowerOn);
            }
            else
            {
                AlarmClearBit(4, 0, i + 1);
            }
        }
        else
        {
            if(ON == ALM_GetSta(i))
            {
                HZ_AxStop(i);
                AlarmSetBit(4, 0, i + 1);
//                ResetWhichBitU16(&GUW.button.BitControlWord, AxisPowerOn);
            }
            else
            {
                AlarmClearBit(4, 0, i + 1);
            }
        }
    }
    
    //��ȡ��չ��״̬
    GUR.ExStatue = HZ_ExCanComErrGet();
    
//		if(OFF==ALM_GetSta(Axis_B_InterSkinCut))
//		{
//			HZ_AxStop(i);
//			AlarmSetBit(4,0,i);
//		}
//		else
//		{
//			AlarmClearBit(4,0,i);
//		}
//
//		if(OFF==ALM_GetSta(Axis_B_InterSkinCut))
//		{
//			HZ_AxStop(i);
//			AlarmSetBit(4,0,i);
//		}
//		else
//		{
//			AlarmClearBit(4,0,i);
//		}
}
//void FSM(){}
void FSM()
{

    u8 i;
    if(GUR.RunStatus == RUN && GSR.ErrorLevel == 0 && Data.Stop == 0)
    {
        ResetWhichBitU16(&GUW.button.HIM_Lock, 0);
    }
    else
    {
        SetWhichBitU16(&GUW.button.HIM_Lock, 0);
    }

    if(GSR.ErrorLevel > 1)//
    {
        if(GUR.RunStatus == RUN)
        {
            GUW.StopCache = 1;
        }
    }

    if(GSR.ErrorLevel > 3)//
    {
//		GUR.HaveToRepower = 1;
    }
    else
    {
        if(GSR.ErrorLevel == 3)//
        {
            //GSW.RunCommand = SCRAM;	//��λ���������뼱ͣ
            GUR.HaveToReset = 1;
        }
    }

    if(GUR.HaveToReset == 1)
    {
        AlarmSetBit(3, 0, 0); //
    }

    if(GUR.HaveToRepower == 1)
    {
        AlarmSetBit(4, 0, 0); //
    }

    switch(GUW.button.RunCommand)
    {
    case RUN:
        if(GUR.HaveToRepower == 0 && GUR.HaveToReset == 0 &&
                (GUR.RunStatus == STOP || GUR.RunStatus == PAUSE) && GSR.ErrorLevel < 2)
        {
            GUR.RunStatus = RUN;
            LogicTask.BMoveClamPara.execute = 1;
            ton_et[1] = 0;
        }
        else
        {
//				if(GUR.RunStatus != RUN)
//				{
//					//����ʱ�Զ��������
//					for(i=0;i<20;i++)
//					{
//						GSR.ErrorCode[i] = 0;
//					}
//					GSW.ClearAlarm = 1;
//				}
            GUW.button.RunCommand = GUR.RunStatus;
        }
        break;

    case STOP:
        GUW.StopCache = 0;
        //if(GUR.RunStatus == RUN || GUR.RunStatus == D_RESET || GUR.RunStatus==SCRAM||GUR.RunStatus==PAUSE)
        if(GUR.RunStatus != INIT)
        {
            GUR.RunStatus = STOP;
        }
        else
        {
            GUW.button.RunCommand = GUR.RunStatus;
        }
        break;
    case PAUSE:
        if(GUR.RunStatus == RUN)
        {
            GUR.RunStatus = PAUSE;
        }
        else
        {
            GUW.button.RunCommand = GUR.RunStatus;
        }
        break;
    case D_RESET:
        if( (GUR.RunStatus != RUN) && (GUR.RunStatus != PAUSE) )
        {
            GUR.RunStatus = D_RESET;
        }
        else
        {
            GUW.button.RunCommand = GUR.RunStatus;
        }
        break;

    case SCRAM:
        ResetWhichBitU16(&GUW.HIM_ButtonLock, 0);
        AlarmSetBit(4, 1, 0); //
        GUR.RunStatus = SCRAM;
        break;
    }

    switch(GUR.RunStatus)
    {
    case INIT:
        RED;
        Logic();
        break;
    case RUN:
        if(GSR.ErrorLevel > 0)
        {
            RED;
        }
        else
        {
            GREEN;
        }
        SetWhichBitU16(&GUW.HIM_ButtonLock, 0);
        Logic();
        break;
    case PAUSE:
        YELLOW;
        ResetWhichBitU16(&GUW.HIM_ButtonLock, 0);
        Logic();
        break;
    case STOP:
        ResetWhichBitU16(&GUW.HIM_ButtonLock, 0);
        Logic();
        if(GSR.ErrorLevel > 0)
        {
            RED;
        }
        else
        {
            YELLOW;
        }
        break;
    case D_RESET:	//Դ����ʹ���ظ������ˣ��ȴ�����������ʹ��
        RED;
        ResetWhichBitU16(&GUW.HIM_ButtonLock, 0);
        Reset();
        ButtonMotion();
        HMI_Button();
        break;
    case SCRAM:
        RED;
        ResetWhichBitU16(&GUW.HIM_ButtonLock, 0);
        ResetWhichBitU16(&GUW.button.BitControlWord, AxisPowerOn);
        ButtonMotion();
        HMI_Button();
        for(i = 0; i < 12; i++)
        {
            HZ_AxStop(i);
        }
        PARAINIT(LogicTask);	//�����������
        PARAINIT(ResetTask);
        GUR.HaveToReset = 1;
        AlarmSetBit(3, 0, 31);
        //Scram();
        break;
    }
    Button();
    SpeedParaInit();
    AxisErro();
    Alarm();
    LedCtrol();
}
