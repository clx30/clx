/*
 * @Author: your name
 * @Date: 2020-05-28 15:18:32
 * @LastEditTime: 2020-05-29 15:00:32
 * @LastEditors: Please set LastEditors
 * @Description: In User Settings Edit
 * @FilePath: \YD_KZJ\Code\APP\Drv\Feed.c
 */ 
#include "Feed.h"
#include "USERTOOL.H"
#include "DrvCom.h"


//function   
static void FeedConfig(FeedPara p,LogicParaDef *T);
static void FeedStart(void);
static u8 FeedGetSta(void);
static void FeedClrErr(void);
static void FeedStop(void);

//����
static DrvType drv;
static FeedPara para;
FeedDrv Feed_drv = 
{
    0,
    FeedConfig,
    FeedStart,
    FeedGetSta,
	FeedClrErr,
    FeedStop
};



/**
 * �ӿں���
*/
static void FeedStart(void)
{
    if(drv.DrvSta==D_REDY)
	    drv.Task->execute = 1;
}

static u8 FeedGetSta(void)
{
    return drv.DrvSta;
}

static void FeedClrErr(void)
{
	drv.DrvSta = D_REDY;
}
static void FeedStop(void)
{
    if(drv.DrvSta==D_RUN)
        drv.DrvSta = D_STOP;
}

static void FeedConfig(FeedPara p,LogicParaDef *T)
{
    para = p;
    drv.Task = T;
    drv.DrvSta = D_REDY; 
}


//ʵ������loop �м乩�ϻ���,��������ǰ��һ��,�����
void Feed(FeedDrv* feeddrv)
{
    if(drv.DrvSta == 0) //��û�г�ʼ��
    {
        return;
    }

    if(drv.Task->execute == 1 && drv.Task->step == 0 )
    {
        drv.Task->step = 1;
        drv.DrvSta = D_RUN;
    }

    switch (drv.Task->step)
    {
    case 1://ȷ����Ӧ�����Ϸ�
        if(InGet(I_MidPressUp)==ON&&InGet(I_LWeldUp)==ON&&InGet(I_RWeldUp)==ON \
            && InGet(I_LClampUp)==ON&&InGet(I_RClampUp)==ON)
        {
            feeddrv->PlatformSta = 1;
            MotorMove(BEMOTOR,para.FeedSpd,para.FeedPos,RELMODE);
			drv.Task->step = 2;
        }else 
        {
            AlarmSetBit(2,0,1); //��������Ƿ��Ѿ����϶�
            drv.DrvSta = D_ERR;
            PARAINIT(*drv.Task);
        }
        break;
    case 2://
		if(HZ_AxGetStatus(BEMOTOR)==0)
		{
			if(InGet(I_MaskSensor)==ON) //������
            {
                feeddrv->PlatformSta = 2;
            }
            if(InGet(I_MaskSensor)==OFF)
            {
                feeddrv->PlatformSta = 3;   //û������
            }
			drv.Task->step = 3;
		}
        break;
    case 3://�ȴ������ּӹ����,����ѭ��
        switch(drv.DrvSta)
        {
            case D_RUN:
            if(feeddrv->PlatformSta ==0)
            {
                drv.Task->step = 1;
            }
            break;
            case D_STOP://���ⲿдֹͣ��
            PARAINIT(*drv.Task);
            drv.DrvSta = D_REDY;
            break;
        }
        break;
    default:
        break;
    }
}

