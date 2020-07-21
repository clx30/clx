#include "InterLayer.h"
#include "usertool.h"
#include "can_master.h"

void Axis_pos(void);
void CurPosToUserUnit(void);

void InterLayer()
{
    //��ȡrtcʱ��
    RTC_Get_Time(&GSR.time.hour, &GSR.time.min, &GSR.time.sec, &GSR.time.ampm);	
    RTC_Get_Date(&GSR.date.year, &GSR.date.month, &GSR.date.day, &GSR.date.week);
	
    AxisConfig(GSS.AxisData, GSS.AxIOconfig);
    HZ_AxMotion();
    HZ_ModBus(uart_232, 1);
    HZ_ModBus(eth, 1);
    HZ_TimerThread();
    HZ_FlashOperate();
    HZ_JogOperate(12);
    HZ_Alarm();
    Axis_pos();
    FSM();
    CurPosToUserUnit();
    Teach();
}

void Axis_pos()
{
    int i;
    for(i = 0; i < PULS_NUM; i++)
    {
        GSR.AxisPosition[i] = HZ_AxGetCurPos(i);//�ᵱǰλ��
    }

    //nyz: 2019.1.17 �ѵ�ǰ����λ��ת�����û���λ
    for(i = 0; i < PULS_NUM; i++)
    {
        GUR.AxisPositionUserUnit[i] = PulseToUserUnit( i, GSR.AxisPosition[i]);
    }

}

void JogGo(u8 axisnum, s32 pos, u8 spd)
{
    if(GUR.RunStatus == RUN)
    {
        return;
    }
	if(GUW.TeachSetData[axisnum].StateSelect == 0 && axisnum != TerminalAxis)
	{
        return;
	}
    if(axisnum == CutAxis)
    {
        if(GUR.HaveToReset == 1)
        {
            return;
        }
        if(GUR.AxisPositionUserUnit[FrontLesserAxis] != 0 || GUR.AxisPositionUserUnit[EndLesserAxis] != 0 || GUR.AxisPositionUserUnit[PullWireAxis] != 0)
        {
//            AlarmSetBit(2, 1, 0);  //ϵͳ���е㶯�������ԭ���ȷ�ϰ�ȫ������
            if(Data.Z_JOGEn != 2)
            {
                Data.Z_JOGEn = 2;
            }
            return;
        }
    }
//    AlarmSetBit(2, 1, 0);  //ϵͳ���е㶯�������ԭ���ȷ�ϰ�ȫ������
    SetAxisSpdRadio(axisnum, 1);
    if(axisnum < PULS_NUM)	//ֻ���������ֵ㶯����
    {
        if(pos > 0)	//����㶯
        {
            if(AXSTA_ERRSTOP == HZ_AxGetStatus(axisnum))
            {
                //ֻ�����޴���
                if(0 == (0x0fff & HZ_AxGetAxisErr(axisnum)))
                {
                    HZ_AxReset(axisnum);
                    AxMoveRelUserUnit(axisnum, 1, pos);//�����λ��
                }

            } else {
                //û�б���,�����˶�
                AxMoveRelUserUnit(axisnum, 1, pos);
            }
        }
        else	//����㶯
        {
            if(AXSTA_ERRSTOP == HZ_AxGetStatus(axisnum))
            {
                //ֻ�����޴���
                if(0 == (0xf0ff & HZ_AxGetAxisErr(axisnum)))
                {
                    HZ_AxReset(axisnum);
                    AxMoveRelUserUnit(axisnum, 1, pos);
                }

            } else {
                //û�б���,�����˶�
                AxMoveRelUserUnit(axisnum, 1, pos);
            }
        }
    }

}
void jogstop(u32 axisnum)
{
    HZ_AxStop(axisnum);
}
void joghome(u32 axisnum)
{
    if(GUR.RunStatus == RUN)
    {
        return;
    }
    GUR.HaveToReset = 1;
    SetAxisSpdRadio(axisnum, 1);
    HZ_AxReset(axisnum);
    HZ_AxHome(axisnum);
}
/**
* @author  yang
* @Description: ��չ���������״̬��Modbus���£���Modbus��ѯ��ָ����ַ�����
* @param --
* @param --
* @return --
*/
int EX_INPUT[2][32];
int EX_OUTPUT[2][32];
void ex_inputupdata()
{
    u8 i;
    //��չ��1
    GSR.InputStatus[4] = 0;
    GSR.InputStatus[5] = 0;
    GSR.InputStatus[6] = 0;
    GSR.InputStatus[7] = 0;

    for(i = 0; i < 16; i++)
    {
        EX_INPUT[0][i] = HZ_ExInPutGet(0, i);
    }

    for(i = 0; i < 16; i++)
    {
        GSR.InputStatus[4] |= (u32) EX_INPUT[0][i] << i;
    }
}

void ex_outputstatusupdata()
{
    u8 i;
    for(i = 0; i < 16; i ++)
    {
        EX_OUTPUT[0][i+16] = HZ_ExOutPutGet(0, i+16);
    }

    //��չ�����1
    GSW.OutputStatus[4] = 0;
    for(i = 0; i < 16; i ++)
    GSW.OutputStatus[4]	|= (u32)	EX_OUTPUT[0][i+16]	<<	i;
}

void ex_outputupdata()
{
    u8 i;
    //��չ�����1
    for(i = 0; i < 16; i ++)
        EX_OUTPUT[0][i+16] = (GSW.OutputStatus[4] >> i & 1);
    
    for(i = 0; i < 16; i ++)
    {
        HZ_ExOutPutSet(0, i+16, EX_OUTPUT[0][i+16]);
    }
}


//������λ��ת�����û���λ
void CurPosToUserUnit()//ת��mm
{
    u32 i;
    for(i = 0; i < MAXAXISNUM; i++)
    {
        GUR.AxisPositionUserUnit[i] = PulseToUserUnit( i, GSR.AxisPosition[i]);
    }
}
