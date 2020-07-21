/*
 * @Author: your name
 * @Date: 2020-05-28 18:13:56
 * @LastEditTime: 2020-06-01 00:09:47
 * @LastEditors: Please set LastEditors
 * @Description: In User Settings Edit
 * @FilePath: \YD_KZJ\Code\APP\Drv\Weld.c
 */
#include "Weld.h"
#include "Interlayer_config.h"
#include "DataDefine.h"
#include "USERTOOL.H"
#include "AxisMove.h"
#include "Feed.h" //����

#define PT drv->Task.PullTask
#define CT drv->Task.ClampTask
#define WT drv->Task.WeldTask

static u8 RunWithoutMask; //�޿��ֿ���

s32 IGet(IODef io)
{
    return InGet(io.card, io.num);
}
s32 QGet(IODef io)
{
    return OutGet(io.card, io.num);
}
s32 QSet(IODef io, u8 val)
{
    return OutSet(io.card, io.num, val);
}
//������--�ⲿд��ͣ

//1.����
static void Pull(WeldDrv *drv)
{
    u8 Ax = drv->Axis.AxPull;
    INITT(PT)
    switch (PT->step)
    {
    case 1:
        if (IGet(drv->In.I_ClampUp) == ON && IGet(drv->In.I_Weldup) == ON)
        {
            drv->PullSta = 1;
            if (GSR.AxisUnitPosition[Ax] != drv->para.pull.Para.readypos) //�����᲻��Ԥ��λ
            {
                MotorMove(Ax, drv->para.pull.Para.pushspd, drv->para.pull.Para.readypos, ABSMODE);
            }
            QSet(drv->Out.Q_PressEar, OFF); //Ĭ����ѹס��
            PT->step = 2;
        }
        break;
    case 2:
        if (HZ_AxGetStatus(Ax) == 0 && TAR(Ax, drv->para.pull.Para.readypos))
        {
            PT->step = 3;
        }
        else if (HZ_AxGetStatus(Ax) == 0)
        {
            PT->step = 1;
        }
        break;
    case 3:
        if (IGet(drv->In.I_PressEarDw) == ON)
        {
            QSet(drv->Out.Q_Pull, ON);
            TRST(PT);
            PT->step = 4;
        }
        else
        {
            AlarmSetBit(2, 0, 2); //ѹ�������쳣
        }
        break;
    case 4:
        if (TCNT(PT) >= drv->para.pull.Para.pullCloseT)
        {
            QSet(drv->Out.Q_PressEar, ON); //ѹ��̧��
            PT->step = 5;
        }
        break;
    case 5:
        if (IGet(drv->In.I_PressEarUp) == ON)
        {
            MotorMove(Ax, drv->para.pull.Para.pullspd, drv->para.pull.Para.Pullpos, ABSMODE);
            PT->step = 6;
        }
        break;
    case 6: //���ߵ�λ
        if (HZ_AxGetStatus(Ax) == 0 && TAR(Ax, drv->para.pull.Para.Pullpos))
        {
            QSet(drv->Out.Q_PressEar, OFF);
            PT->step = 7;
        }
        else if (HZ_AxGetStatus(Ax) == 0)
        {
            PT->step = 5;
        }
        break;
    case 7:
        if (IGet(drv->In.I_PressEarDw) == ON)
        {
            drv->PullSta = 2;
            drv->Task.ClampTask->execute = 1;
            PT->step = 8;
        }
        break;
    case 8: //�ȴ��߱�����
        if (drv->PullSta == 0)
        {
            PT->step = 9;
        }
        break;
    case 9: //�߱�ȡ�ߣ��ж�״̬�Ƿ����ȡ��
        switch (drv->DrvSta)
        {
        case D_RUN:
            PT->step = 1;
            break;
        case D_STOP:
            PARAINIT(*PT);
            break;
        }
        break;
    }
}

//2.����
static void Clamp(WeldDrv *drv)
{
    u8 Ax1 = drv->Axis.AxTurn1;
    u8 Ax2 = drv->Axis.AxTurn2;
    static s32 pulltime;
    INITT(CT)
    switch (CT->step)
    {
    case 1:                                                 //�ȴ����������������
        if (drv->PullSta == 2 && Feed_drv.PlatformSta == 2) //��Ҫ������ܿ���
        {
            drv->ClampSta = 1;
            CT->step = 2;
            MotorMove(Ax1, drv->para.clamp.Para.workspd[0], drv->para.clamp.Para.readpos[0], ABSMODE);
            MotorMove(Ax2, drv->para.clamp.Para.workspd[1], drv->para.clamp.Para.readpos[1], ABSMODE);
        }
        else if (Feed_drv.PlatformSta == 3) //û������
        {                                   //�Ƿ��������
            if (RunWithoutMask == 1)
            {
                drv->ClampSta = 1;
                CT->step = 2;
                MotorMove(Ax1, drv->para.clamp.Para.workspd[0], drv->para.clamp.Para.readpos[0], ABSMODE);
                MotorMove(Ax2, drv->para.clamp.Para.workspd[1], drv->para.clamp.Para.readpos[1], ABSMODE);
            }
        }
        break;
    case 2:
        if (HZ_AxGetStatus(Ax2) == 0 && HZ_AxGetStatus(Ax1) == 0)
        {
            QSet(drv->Out.Q_ClampUD, ON);
            CT->step = 3;
        }
        break;
    case 3: //�½���λ
        if (IGet(drv->In.I_ClampDw) == ON)
        {
            QSet(drv->Out.Q_Clamp, ON);
            TRST(CT);
            CT->step = 4;
        }
        break;
    case 4: //�պ�ʱ�䵽
        if (TCNT(CT) >= drv->para.clamp.Para.CloseClampT)
        {
            QSet(drv->Out.Q_Cut, ON);   //����
            QSet(drv->Out.Q_Pull, OFF); //���߼���
            pulltime = SysTim_GetUser();
            TRST(CT);
            CT->step = 5;
        }
        break;
    case 5:
        if (TCNT(CT) >= drv->para.clamp.Para.CutTime)
        {
            QSet(drv->Out.Q_Cut, OFF);
            TRST(CT);
            CT->step = 6;
        }
        break;
    case 6:
        if ((SysTim_GetUser() - pulltime / 10) >= drv->para.pull.Para.pullOpenT)
        {
            QSet(drv->Out.Q_ClampUD, OFF); //��������
            OutSet(Q_MidPress, ON);        //�м�ѹ����ѹ
            CT->step = 7;
        }
        break;
    case 7: //�����Ѿ�������
        if (IGet(drv->In.I_ClampUp) == ON)
        {
            MotorMove(Ax1, drv->para.clamp.Para.workspd[0], drv->para.clamp.Para.workpos[0], ABSMODE);
            MotorMove(Ax2, drv->para.clamp.Para.workspd[1], drv->para.clamp.Para.workpos[1], ABSMODE);
            CT->step = 8;
        }
        break;
    case 8:
        if (HZ_AxGetStatus(Ax2) == 0 && HZ_AxGetStatus(Ax1) == 0 &&
            TAR(Ax1, drv->para.clamp.Para.workpos[0]) && TAR(Ax2, drv->para.clamp.Para.workpos[1]))
        {
            if (InGet(I_MidPressDw) == ON) //ѹ���Ѿ�ѹס
            {
                QSet(drv->Out.Q_ClampUD, ON); //��������ѹ
                CT->step = 9;
            }
        }
        break;
    case 9: //�������
        if (IGet(drv->In.I_ClampDw) == ON)
        {
            QSet(drv->Out.Q_Weld, ON); //���������½�
            CT->step = 10;
        }
        break;
    case 10:
        if (IGet(drv->In.I_Welddw) == ON)
        {
            drv->ClampSta = 2; //�Ѿ�ѹס����ʼ����
            drv->Task.WeldTask->execute = 1;
            CT->step = 11;
        }
        break;
    case 11: //���Ѿ������Ӷ�ѹ���ˣ��Ϳ��Ի�ȥ����һ�ν����ˣ���weld�У������״̬д0�����Ѽ�צ̧���߿�
        if (drv->ClampSta == 0)
        {
            QSet(drv->Out.Q_Clamp, OFF);
            TRST(CT);
            CT->step = 12;
        }
        break;
    case 12:
        if (TCNT(CT) >= drv->para.clamp.Para.OpenClampT)
        {
            QSet(drv->Out.Q_ClampUD, OFF); //��������̧
            CT->step = 13;
        }
        break;
    case 13:
        if (IGet(drv->In.I_ClampUp) == ON)
        {
            MotorMove(Ax1, drv->para.clamp.Para.workspd[0], drv->para.clamp.Para.readpos[0], ABSMODE);
            MotorMove(Ax2, drv->para.clamp.Para.workspd[1], drv->para.clamp.Para.readpos[1], ABSMODE);
            CT->step = 14;
        }
        break;
    case 14: //��Ԥ��λ��
        if (HZ_AxGetStatus(Ax2) == 0 && HZ_AxGetStatus(Ax1) == 0)
        {
            CT->execute = 0;
            CT->step = 0;
        }
        break;
    }
}
//3.����
static void Welding(WeldDrv *drv)
{
    INITT(WT)
    switch (WT->step)
    {
    case 1:
        if (IGet(drv->In.I_Welddw) == ON)
        {
            drv->WeldingSta = 1;
            TRST(WT);
            WT->step = 2;
        }
        break;
    case 2:
        if (TCNT(WT) >= drv->para.welding.Para.WeldingTime)
        {
            TRST(WT);
            drv->ClampSta = 0; //֪ͨצ�ӿ�������
            WT->step = 3;
        }
        break;
    case 3:
        if (TCNT(CT) >= drv->para.welding.Para.CuringTime)
        {
            QSet(drv->Out.Q_Weld, OFF);
            WT->step = 4;
        }
        break;
    case 4:
        if (IGet(drv->In.I_Weldup) == ON)
        {
            drv->PullSta = 0;
            OutSet(Q_MidPress,OFF);
            WT->step = 5;
        }
        break;
    case 5: //�м�ѹ�ϰ��ϵ�λ
        if(InGet(I_MidPressUp)==ON)
        {
            Feed_drv.PlatformSta = 0;
            WT->step = 0;
            WT->execute = 0;
        }
        break;
    }
}

void WeldLoop(WeldDrv *welddrv)
{
    if (welddrv->DrvSta == D_INIT) //δ��ʼ��
    {
        return;
    }

    Pull(welddrv);
    Clamp(welddrv);
    Welding(welddrv);
}

void WeldStart(WeldDrv *drv)
{
    if (drv->DrvSta == D_REDY)
    {
        drv->DrvSta = D_RUN;
        drv->Task.ClampTask->execute = 1;
        drv->Task.PullTask->execute = 1;
        drv->Task.WeldTask->execute = 1;
    }
}
