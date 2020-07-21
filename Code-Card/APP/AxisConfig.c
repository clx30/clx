#include "axisconfig.h"
#include "USERTOOL.H"

void AxisConfig(AxisDef AxisSpeed[],
                AxisIOConfig ioconfig[])
{
    int i;
    for(i = 0; i < PULS_NUM; i++)
    {
        switch(ioconfig[i].limitMode)
        {
        case 0: //û����λ
            HZ_AxSetAxiSig(i, 1, ioconfig[i].OrgNum, ioconfig[i].Orglev, 0, 0, 0, 0, 0, 0);
            HZ_AxSetLimSoft(i, 0, UserUnitToPulse(i,AxisSpeed[i].Data.SoftMaxLimit), 0, UserUnitToPulse(i,AxisSpeed[i].Data.SoftMinLimit));
            break;
        case 1: //ֻ�������λ
            HZ_AxSetAxiSig(i, 1, ioconfig[i].OrgNum, ioconfig[i].Orglev, 0, 0, 0, 0, 0, 0);
            HZ_AxSetLimSoft(i, 1, UserUnitToPulse(i,AxisSpeed[i].Data.SoftMaxLimit), 1, UserUnitToPulse(i,AxisSpeed[i].Data.SoftMinLimit));
            break;
        case 2:  //ֻ��Ӳ����λ
            HZ_AxSetAxiSig(i, 1, ioconfig[i].OrgNum, ioconfig[i].Orglev, \
                           1, ioconfig[i].Poslimit, ioconfig[i].Poslimitlev, \
                           1, ioconfig[i].Neglimit, ioconfig[i].Neglimitlev);
            HZ_AxSetLimSoft(i, 0, UserUnitToPulse(i,AxisSpeed[i].Data.SoftMaxLimit), 0, UserUnitToPulse(i,AxisSpeed[i].Data.SoftMinLimit));
            break;
        case 3:  //������λ����Ч
            HZ_AxSetAxiSig(i, 1, ioconfig[i].OrgNum, ioconfig[i].Orglev, \
                           1, ioconfig[i].Poslimit, ioconfig[i].Poslimitlev, \
                           1, ioconfig[i].Neglimit, ioconfig[i].Neglimitlev);
            HZ_AxSetLimSoft(i, 1, UserUnitToPulse(i,AxisSpeed[i].Data.SoftMaxLimit), 1, UserUnitToPulse(i,AxisSpeed[i].Data.SoftMinLimit));
            break;
        }
        HZ_AxSetHomMod(i, ioconfig[i].HomeMode);
        HZ_AxSetAlm(i, ioconfig[i].alarmmode);
    }

}

