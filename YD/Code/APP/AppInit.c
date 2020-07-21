/*
 * @Descripttion: 
 * @version: 
 * @Author: yfs
 * @Date: 2019-12-27 09:17:32
 * @LastEditors: Please set LastEditors
 * @LastEditTime: 2020-05-27 15:17:09
 */
/*
 * File: AppInit.c
 * File Created: Tuesday, 9th October 2018 2:43:29 pm
 * version:v1.1
 * Description:应用程序初始化配置
 * param:
 * Modified By:yang
 * -----
 * Copyright 2018 - 2018 <<hzzh>>, <<hzzh>>
 */

#include "AppInit.h"
#include "Algorithm.h"
#include "can_master.h"
#include "UserTool.h"
#include "stdio.h"

//变量声明  所有app程序的初始变量实例及声明
GlobalDataDef GlobalData = {0}; //定义用户通讯数据库
u8 uart_sendbuf[2][300];        //定义通讯接收和发送缓存
u8 uart_recvbuf[2][300];
u8 eth_txbuf[300];
u8 eth_rxbuf[300];

/****************项目使用结构的实例或声明***************/

/****************end define***************/

/**
 * @author: yfs
 * @Date: 2020-01-14 16:50:27
 * @description: 板卡初次上电flash全-1数据需要初始化
 * @para: 
 * @return: 
 */
void DataInit()
{
	memset(&GSS,0,sizeof(GSS));
	memset(&GUS,0,sizeof(GUS));
	/*
	此处添加初始化存储区数据，保证初次上电没使用的数据安全
	*/
	for(int i=0;i<8;i++)
	{
		GSS.axis[i].Axconver.MPR = 1;
		GSS.axis[i].Axconver.PPR = 100;
		GSS.axis[i].Axhomecfg.homemode = 0;
		GSS.axis[i].Axhomecfg.homespeedfast = ConvertToUserUnit(i,10000);
		GSS.axis[i].Axhomecfg.homespeedoffset = 0;
		GSS.axis[i].Axhomecfg.homespeedslow = ConvertToUserUnit(i,500);
		GSS.axis[i].Axhomecfg.orglev = ON;
		GSS.axis[i].Axhomecfg.orgnum = i;
        GSS.axis[i].Axlimitcfg.alarmmode = 0;
        GSS.axis[i].Axlimitcfg.limitMode = 0;
        GSS.axis[i].AxSpd.acctime = 80;
        GSS.axis[i].AxSpd.dectime = 80;
        GSS.axis[i].AxSpd.startspeed = ConvertToUserUnit(i,500);
        GSS.axis[i].AxSpd.endspeed = ConvertToUserUnit(i,500);
        GSS.axis[i].AxSpd.runspeed = ConvertToUserUnit(i,10000);
        AxisSetSpd(i,GSS.axis[i].AxSpd);
	}
	GSS.axis[2].Axlimitcfg.limitMode = 2;
	GSS.axis[2].Axlimitcfg.poslimitsig = X10;
	GSS.axis[2].Axlimitcfg.neglimitsig = X9;
	GSS.axis[2].Axlimitcfg.poslimitlev = ON;
	GSS.axis[2].Axlimitcfg.neglimitlev = ON;
	GSS.axis[5].Axlimitcfg.limitMode = 2;
	GSS.axis[5].Axlimitcfg.poslimitsig = X11;
	GSS.axis[5].Axlimitcfg.neglimitsig = X12;
	GSS.axis[5].Axlimitcfg.poslimitlev = ON;
	GSS.axis[5].Axlimitcfg.neglimitlev = ON;
    GSS.SaveCheck.checkflag = 0xea;
    HZ_Data_Write();
}

void initIO(void)
{
    int i;
    for (i = 0; i < 24; i++) //初始化 输出口
        OutPut_SetSta(i, OFF);
	for (i = 0;i <32 ;i++)
		HZ_ExOutPutSet(1,i,OFF);
    for (i = 0; i < PULS_NUM; i++) //轴口使能,电平和普通输出相反
        EN_SetSta(i, 1);
}

void AppInit()
{
    //配置地址表完成modbus
    init_config(&GlobalData, GSR.ErrorCode, &GlobalData.SpaceType.Save, GLOBALDATALEN, USERWRITELEN, USERREADLEN, USERSAVELEN);
	//数据读取
	if(HZ_Data_Read()==0 )
	{
	   HZ_BkData_Write();
	}else 
	{
		if(HZ_BkData_Read()!=0)
		{
			DataInit();
		}
	}
    //通过判断flash上的某个特殊值，来进行全局flash参数初始化
    //if (GSS.SaveCheck.checkflag != 0xea)
    {
        DataInit();
    }
	AxisConfig(GSS.axis);
	
    initIO();

    GSR.HardWare_Ver[0] = 2019724;
	char softstr[40]={0};
	sprintf(softstr,"yd_%s",__DATE__);
	memcpy(GSR.SoftWare_Ver,softstr,40);
    GUR.HaveToReset = 1;
    //初始化状态机，将设备状态初始是错误停
    InitFsm(&SysFsm);
}
