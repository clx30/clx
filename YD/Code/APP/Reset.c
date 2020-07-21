/*
 * @Author: your name
 * @Date: 2020-05-26 14:03:11
 * @LastEditTime: 2020-05-27 16:11:43
 * @LastEditors: Please set LastEditors
 * @Description: In User Settings Edit
 * @FilePath: \YD_KZJ\Code\APP\Reset.c
 */ 
#include "reset.h"

LogicParaDef ResetTask;

#define REST_TASK ResetTask

void reset_handle(void)
{
    REST_TASK.execute = 1;
}

extern void initIO(void);
//设备复位函数
void Reset()
{
    if(GUR.RunStatus == D_RESET)
    {
        if(REST_TASK.execute == 1&&REST_TASK.step == 0)
        {
            REST_TASK.step = 1;
        }
        switch(REST_TASK.step)
        {
        case 1:
            initIO();
            PARAINIT(LogicTask);
            REST_TASK.step = 2;
            break;
        case 2:
            for(int i=0; i<7; i++)
            {
                HZ_AxReset(i);
                HZ_AxHome(i);
            }
            REST_TASK.step = 3;
            break;
        case 3:
            if(CheckHoming(7)==0)
            {
                REST_TASK.step = 4;
            }
            break;
        case 4:
            PARAINIT(REST_TASK);
            GSW.ClearAlarm = 1;
            GUW.Button.RunCommand = STOP;
            break;
        }
    }
    else
    {
        PARAINIT(REST_TASK);
    }
}



