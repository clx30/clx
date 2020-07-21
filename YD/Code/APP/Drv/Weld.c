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
#include "Feed.h" //进料

#define PT drv->Task.PullTask
#define CT drv->Task.ClampTask
#define WT drv->Task.WeldTask

static u8 RunWithoutMask; //无口罩空跑

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
//待处理--外部写急停

//1.拉线
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
            if (GSR.AxisUnitPosition[Ax] != drv->para.pull.Para.readypos) //拉线轴不在预备位
            {
                MotorMove(Ax, drv->para.pull.Para.pushspd, drv->para.pull.Para.readypos, ABSMODE);
            }
            QSet(drv->Out.Q_PressEar, OFF); //默认是压住的
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
            AlarmSetBit(2, 0, 2); //压耳气缸异常
        }
        break;
    case 4:
        if (TCNT(PT) >= drv->para.pull.Para.pullCloseT)
        {
            QSet(drv->Out.Q_PressEar, ON); //压耳抬起
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
    case 6: //拉线到位
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
    case 8: //等待线被拿走
        if (drv->PullSta == 0)
        {
            PT->step = 9;
        }
        break;
    case 9: //线被取走，判断状态是否继续取线
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

//2.夹线
static void Clamp(WeldDrv *drv)
{
    u8 Ax1 = drv->Axis.AxTurn1;
    u8 Ax2 = drv->Axis.AxTurn2;
    static s32 pulltime;
    INITT(CT)
    switch (CT->step)
    {
    case 1:                                                 //等待拉线完成且有物料
        if (drv->PullSta == 2 && Feed_drv.PlatformSta == 2) //还要加入空跑开关
        {
            drv->ClampSta = 1;
            CT->step = 2;
            MotorMove(Ax1, drv->para.clamp.Para.workspd[0], drv->para.clamp.Para.readpos[0], ABSMODE);
            MotorMove(Ax2, drv->para.clamp.Para.workspd[1], drv->para.clamp.Para.readpos[1], ABSMODE);
        }
        else if (Feed_drv.PlatformSta == 3) //没有物料
        {                                   //是否允许空跑
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
    case 3: //下降到位
        if (IGet(drv->In.I_ClampDw) == ON)
        {
            QSet(drv->Out.Q_Clamp, ON);
            TRST(CT);
            CT->step = 4;
        }
        break;
    case 4: //闭合时间到
        if (TCNT(CT) >= drv->para.clamp.Para.CloseClampT)
        {
            QSet(drv->Out.Q_Cut, ON);   //剪刀
            QSet(drv->Out.Q_Pull, OFF); //拉线夹松
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
            QSet(drv->Out.Q_ClampUD, OFF); //夹子上行
            OutSet(Q_MidPress, ON);        //中间压板下压
            CT->step = 7;
        }
        break;
    case 7: //夹子已经上来了
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
            if (InGet(I_MidPressDw) == ON) //压板已经压住
            {
                QSet(drv->Out.Q_ClampUD, ON); //夹子往下压
                CT->step = 9;
            }
        }
        break;
    case 9: //夹线完成
        if (IGet(drv->In.I_ClampDw) == ON)
        {
            QSet(drv->Out.Q_Weld, ON); //焊接气缸下降
            CT->step = 10;
        }
        break;
    case 10:
        if (IGet(drv->In.I_Welddw) == ON)
        {
            drv->ClampSta = 2; //已经压住，开始焊接
            drv->Task.WeldTask->execute = 1;
            CT->step = 11;
        }
        break;
    case 11: //线已经被焊接端压紧了，就可以回去等下一次进料了，在weld中，将这个状态写0，并把夹爪抬起走开
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
            QSet(drv->Out.Q_ClampUD, OFF); //夹子往上抬
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
    case 14: //到预备位等
        if (HZ_AxGetStatus(Ax2) == 0 && HZ_AxGetStatus(Ax1) == 0)
        {
            CT->execute = 0;
            CT->step = 0;
        }
        break;
    }
}
//3.焊线
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
            drv->ClampSta = 0; //通知爪子可以走了
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
    case 5: //中间压料板上到位
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
    if (welddrv->DrvSta == D_INIT) //未初始化
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
