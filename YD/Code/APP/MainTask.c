/*
 * @Author: your name
 * @Date: 2020-05-26 14:03:11
 * @LastEditTime: 2020-05-29 14:07:06
 * @LastEditors: Please set LastEditors
 * @Description: In User Settings Edit
 * @FilePath: \YD_KZJ\Code\APP\MainTask.c
 */ 
#include "maintask.h" 

void AppErrHandle(void)
{
	if(GUR.HaveToReset == 1)
	{
		AlarmSetBit(3,0,0);
	}
	if(GUR.HaveToRepower == 1)
	{
		AlarmSetBit(4,0,0);
	}
	if(GSR.ErrorLevel<4&&GSR.ErrorLevel>0)
	{
		if(GUR.RunStatus == RUN)
		{
			GUW.Button.RunCommand = STOP;
		}
	}
	if(GSR.ErrorLevel >=4)
	{
		GUR.HaveToRepower = 1;
		GUW.Button.RunCommand = SCRAM;
	}
	for (int i = 0; i < PULS_NUM; i++)
    {
        //下限位
        if (HZ_AxGetAxisErr(i) == AX_ERR_DW_LIM_H || AX_ERR_DW_LIM_S == HZ_AxGetAxisErr(i))//||(AX_ERR_DW_LIM_S&HZ_AxGetAxisErr(i))==1)
        {
            AlarmSetBit(1, 1, i);
        }
        else
        {
            AlarmClearBit(1, 1, i);
        }
        //上限位
        if (AX_ERR_UP_LIM_H == HZ_AxGetAxisErr(i) || AX_ERR_UP_LIM_S == HZ_AxGetAxisErr(i))
        {
            AlarmSetBit(1, 0, i);
        }
        else
        {
            AlarmClearBit(1, 0, i);
        }

		if (AX_ERR_AX_ALM == HZ_AxGetAxisErr(i))
		{
			AlarmSetBit(4, 0, i+1); //轴的驱动器报警
		}

    }
}

void stop_handle(void)
{
	for(int i=0;i<7;i++)
	{
		HZ_AxStop(i);
	}
}

void MainTask(void)
{
	AppErrHandle();
    FSM(&GUW.Button.RunCommand,&SysFsm);
	Logic();
    Reset();
    if(GUR.RunStatus != RUN && GUR.RunStatus != D_RESET && GUR.RunStatus != SCRAM)
	{
		HZ_JogOperate(7);
	}
	Feed(&Feed_drv);
	
}

