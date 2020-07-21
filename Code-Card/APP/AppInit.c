/*
 * File: AppInit.c
 * File Created: Tuesday, 9th October 2018 2:43:29 pm
 * version:v1.1
 * Description:用户数据初始化
 * param:
 * Modified By:yang
 * -----
 * Copyright 2018 - 2018 <<hzzh>>, <<hzzh>>
 */

#include "AppInit.h"
#include "logic.h"
#include "bsplib.h"
GlobalDataDef GlobalData = {0}; //定义用户通讯数据库
u8 uart_sendbuf[2][300];        //定义通讯接收和发送缓存
u8 uart_recvbuf[2][300];
u8 eth_txbuf[300];
u8 eth_rxbuf[300];

void AppInit()
{
    u8 i;
    init_config(&GlobalData, GSR.ErrorCode, &GlobalData.SpaceType.Save, GLOBALDATALEN, USERWRITELEN, USERREADLEN, USERSAVELEN);
//    if(HZ_Data_Read() == 0)//读取falsh值
//    {
//        HZ_BkData_Write();
//    }
//    else
//    {
//        if(HZ_BkData_Read() != 0)
//        {
//            AlarmSetBit(5,0,1); //Flash数据错误
//        }
//    }
    
    FRam_Read(0,&GSS,8000); //铁电读取
    while(FRAM_GetSta() !=0)
    {
        FRam_Exec();
    }
//    Puls_SetWidth(FrontPrimaryAxis, 5.4);
    Puls_SetWidth(EndPrimaryAxis, 5.4);
    Puls_SetWidth(EndLesserAxis, 5.4);
//    Puls_SetWidth(WeldFeedAxis, 5.4);

    for (i = 0; i < GPO_NUM; i++) //初始化 输出口
        OutPut_SetSta(i, OFF);
//    for (i = 0; i < PULS_NUM; i++) //轴口使能
//        EN_SetSta(i, OFF);

    //设定脉冲宽度
//		Puls_SetWidth(Axis_FeedWire,5);

    //设置轴的原点和正反限位
    for(i = 0; i < PULS_NUM; i++) //设置轴的原点和正反限位
        HZ_AxSetAxiSig(i, 1, i, ON, 0, 0, 0, 0, 0, 0);

///2019,07,11添加设置轴极限位置
    for(i = 0; i < PULS_NUM; i++) //设置轴的原点和正反限位
    {
        GSS.AxIOconfig[i].alarmmode = 3;
        GSS.AxIOconfig[i].HomeMode = GOHOMETYPE1;
        GSS.AxIOconfig[i].OrgNum = i;
        GSS.AxIOconfig[i].Orglev = 0;
        GSS.AxIOconfig[i].limitMode = 1;
        GSS.AxIOconfig[i].Neglimit = 0;
        GSS.AxIOconfig[i].Neglimitlev = 0;
        GSS.AxIOconfig[i].Poslimit = 0;
        GSS.AxIOconfig[i].Poslimitlev = 0;
    }
    GSS.AxIOconfig[TerminalAxis].limitMode = 0;
    GSS.AxIOconfig[StrepAxis].limitMode = 0;

    AxisConfig(GSS.AxisData, GSS.AxIOconfig);

    for (i = 0; i < PULS_NUM; i++)
        HZ_AxSetHomMod(i, GOHOMETYPE1);//设置负方向回零

    for (i = 0; i < PULS_NUM; i++) //初始化轴速度，使用gss中保存的速度
    {
        HZ_AxSetPara(i, GSS.AxisData[i].Data.StartSpeed, GSS.AxisData[i].Data.Acctime, GSS.AxisData[i].Data.RunSpeed,
                     GSS.AxisData[i].Data.Dectime, GSS.AxisData[i].Data.EndSpeed,
                     GSS.AxisData[i].Data.HomeSpeedFast, GSS.AxisData[i].Data.HomeSpeedSlow,
                     GSS.AxisData[i].Data.HomeOffset, 0, 0);
    }
    GUR.HaveToReset = 1;
    
    EN_SetSta(EndFeedTinAxis, ON);
    EN_SetSta(FrontFeedTinAxis, ON);
    UserTimer[3] = 500;
	
    //锁机相关参数初始化
    CID_init(XWD, GSR.CID);
    DeDate(&GSS.LockDate.lockdata, &GUW.CurrDateForUser);

}
