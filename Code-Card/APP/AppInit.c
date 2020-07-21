/*
 * File: AppInit.c
 * File Created: Tuesday, 9th October 2018 2:43:29 pm
 * version:v1.1
 * Description:�û����ݳ�ʼ��
 * param:
 * Modified By:yang
 * -----
 * Copyright 2018 - 2018 <<hzzh>>, <<hzzh>>
 */

#include "AppInit.h"
#include "logic.h"
#include "bsplib.h"
GlobalDataDef GlobalData = {0}; //�����û�ͨѶ���ݿ�
u8 uart_sendbuf[2][300];        //����ͨѶ���պͷ��ͻ���
u8 uart_recvbuf[2][300];
u8 eth_txbuf[300];
u8 eth_rxbuf[300];

void AppInit()
{
    u8 i;
    init_config(&GlobalData, GSR.ErrorCode, &GlobalData.SpaceType.Save, GLOBALDATALEN, USERWRITELEN, USERREADLEN, USERSAVELEN);
//    if(HZ_Data_Read() == 0)//��ȡfalshֵ
//    {
//        HZ_BkData_Write();
//    }
//    else
//    {
//        if(HZ_BkData_Read() != 0)
//        {
//            AlarmSetBit(5,0,1); //Flash���ݴ���
//        }
//    }
    
    FRam_Read(0,&GSS,8000); //�����ȡ
    while(FRAM_GetSta() !=0)
    {
        FRam_Exec();
    }
//    Puls_SetWidth(FrontPrimaryAxis, 5.4);
    Puls_SetWidth(EndPrimaryAxis, 5.4);
    Puls_SetWidth(EndLesserAxis, 5.4);
//    Puls_SetWidth(WeldFeedAxis, 5.4);

    for (i = 0; i < GPO_NUM; i++) //��ʼ�� �����
        OutPut_SetSta(i, OFF);
//    for (i = 0; i < PULS_NUM; i++) //���ʹ��
//        EN_SetSta(i, OFF);

    //�趨������
//		Puls_SetWidth(Axis_FeedWire,5);

    //�������ԭ���������λ
    for(i = 0; i < PULS_NUM; i++) //�������ԭ���������λ
        HZ_AxSetAxiSig(i, 1, i, ON, 0, 0, 0, 0, 0, 0);

///2019,07,11��������Ἣ��λ��
    for(i = 0; i < PULS_NUM; i++) //�������ԭ���������λ
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
        HZ_AxSetHomMod(i, GOHOMETYPE1);//���ø��������

    for (i = 0; i < PULS_NUM; i++) //��ʼ�����ٶȣ�ʹ��gss�б�����ٶ�
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
	
    //������ز�����ʼ��
    CID_init(XWD, GSR.CID);
    DeDate(&GSS.LockDate.lockdata, &GUW.CurrDateForUser);

}
