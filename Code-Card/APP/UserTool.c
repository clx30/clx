#include "USERTOOL.H"
#include "can_master.h"

/********************************************************/
/*
 * Created: 11/16
 * Description:
 * param:
 * Modified By:yang
 */

void SetAxisSpdRadio(u8 i, u8 spd)
{
    if(spd >= 100 || spd <= 0)
    {
        spd = 100;
    }
    HZ_AxSetPara(i, GSS.AxisData[i].Data.StartSpeed, GSS.AxisData[i].Data.Acctime,
                 GSS.AxisData[i].Data.StartSpeed + __fabs(GSS.AxisData[i].Data.RunSpeed - GSS.AxisData[i].Data.StartSpeed)*spd / 100,
                 GSS.AxisData[i].Data.Dectime, GSS.AxisData[i].Data.EndSpeed,
                 GSS.AxisData[i].Data.HomeSpeedFast, GSS.AxisData[i].Data.HomeSpeedSlow,
                 GSS.AxisData[i].Data.HomeOffset, 0, 0);
}



s32 SetSpd(u8 i, u32 RunSpeed)
{

    if (i > PULS_NUM)
    {
        return -1;
    }
    if (RunSpeed > GSS.AxisData[i].Data.RunSpeed)
    {
        RunSpeed = GSS.AxisData[i].Data.RunSpeed;
    }
    if (RunSpeed < GSS.AxisData[i].Data.StartSpeed)
    {
        RunSpeed = GSS.AxisData[i].Data.StartSpeed + 1;
    }
    if (HZ_AxSetPara(i, GSS.AxisData[i].Data.StartSpeed, GSS.AxisData[i].Data.Acctime, RunSpeed,
                     GSS.AxisData[i].Data.Dectime, GSS.AxisData[i].Data.EndSpeed,
                     GSS.AxisData[i].Data.HomeSpeedFast, GSS.AxisData[i].Data.HomeSpeedSlow,
                     GSS.AxisData[i].Data.HomeOffset, 0, 0) == 0)
    {
        return 0;
    }
    else
    {
        return -1;
    }
}
void SetSpdDefault(u8 i)
{

    if (i > PULS_NUM)
    {
        return;
    }
    HZ_AxSetPara(i, GSS.AxisData[i].Data.StartSpeed, GSS.AxisData[i].Data.Acctime, GSS.AxisData[i].Data.RunSpeed,
                 GSS.AxisData[i].Data.Dectime, GSS.AxisData[i].Data.EndSpeed,
                 GSS.AxisData[i].Data.HomeSpeedFast, GSS.AxisData[i].Data.HomeSpeedSlow,
                 GSS.AxisData[i].Data.HomeOffset, 0, 0);
}
/********检查轴位置*************/
s32 Check_AxisStatu()
{
    u8 i;
    for(i = 0; i < PULS_NUM; i++)
    {
        if(HZ_AxGetStatus(i) != 0)
        {
            return -1;
        }
    }
    return 0;
}

s32 Check_xyzMotor()
{
    u8 i;
    for(i = 0; i < 3; i++)
    {
        if(HZ_AxGetStatus(i) != 0)
        {
            return -1;
        }
    }
    return 0;
}

/******************************/


/**
* @author： 2018/08/20  Administrator
* @Description: 初始化速度
* @param –
* @param --
* @return --
*/
void SpeedParaInit()
{
    u8 i;
//	if(GSW.JogPositionLevel == 0)
//	{
//		GSW.JogPositionLevel = 1;
//	}
	if(GSW.JogMode == 1)
	{
		GSW.JogMode = 2;
	}
	if(GUS.AInsertPin.Data.InsertDoneSpeed >100)
	{
		GUS.AInsertPin.Data.InsertDoneSpeed = 50;
	}
    if(GUS.BFeedWire.EndAxisUnfeedPos > GUS.WirePara.Data.TerminalFrontLimitMin && GUS.BFeedWire.EndAxisUnfeedPos < GUS.WirePara.Data.TerminalFrontLimitMax)
    {
        GUS.BFeedWire.EndAxisUnfeedPos = GUS.WirePara.Data.TerminalFrontLimitMin - 1;
    }
    if(GUS.WirePara.Data.ProductSet.WireNum > 1)
    {
        if(((GUS.WirePara.Data.ProductSet.WireNum-1) * GUS.WirePara.Data.WireDistance) > 3000)
        {
           GUS.WirePara.Data.WireDistance =  3000/(GUS.WirePara.Data.ProductSet.WireNum-1);
        }
    }
    if(SeparateWireMode == 0 && FrontSeparateWireEN)
    {
//        GUS.FeedWire.FeedFrontDistance = 0;
    }
    else
    {
        if(GUS.SperatePara.PullFeedFrontDistance != GUS.FeedWire.FeedFrontDistance && GUS.FeedWire.FeedFrontDistance == 0)
        {
            GUS.FeedWire.FeedFrontDistance = GUS.SperatePara.PullFeedFrontDistance;
        }
        GUS.SperatePara.PullFeedFrontDistance = GUS.FeedWire.FeedFrontDistance;
    }
    GUS.FeedWire.FeedFrontDistance = 0;
    SetWhichBitU16(&GUW.button.BitControlWord, AxisPowerOn);
	for(i = 0; i < PULS_NUM; i++)
	{
		GSS.AxisData[i].Data.Dectime = GSS.AxisData[i].Data.Acctime;
		GSS.AxisData[i].Data.EndSpeed = GSS.AxisData[i].Data.StartSpeed;
	}
    if (Data.initflag)
    {
        Data.initflag = 0;
        for(i = 0; i < PULS_NUM; i++)
        {
            GSS.AxisData[i].Data.StartSpeed = 2000;
            GSS.AxisData[i].Data.Acctime = 50;
            GSS.AxisData[i].Data.RunSpeed = 10000;
            GSS.AxisData[i].Data.Dectime = 50;
            GSS.AxisData[i].Data.EndSpeed = 2000;
            GSS.AxisData[i].Data.HomeSpeedFast = 1000;
            GSS.AxisData[i].Data.HomeSpeedSlow = 700;
            GSS.AxisData[i].Data.HomeOffset = 0;
            GSS.AxisData[i].Data.SoftMaxLimit = 99999;
            GSS.AxisData[i].Data.SoftMinLimit = -99999;
        }
        HZ_Data_Write();
    }
}


///******************************/

//void AxisRunDefault(u32 i,u32 mode,u32 Ratio,s32 pos)
//{
//	HZ_AxSetPara(i, GSS.AxisData[i].Data.StartSpeed, GSS.AxisData[i].Data.Acctime, \
//		GSS.AxisData[i].Data.StartSpeed + __fabs(GSS.AxisData[i].Data.RunSpeed - GSS.AxisData[i].Data.StartSpeed)*Ratio / 100, \
//		GSS.AxisData[i].Data.Dectime, GSS.AxisData[i].Data.EndSpeed, \
//		GSS.AxisData[i].Data.HomeSpeedFast, GSS.AxisData[i].Data.HomeSpeedSlow, \
//		GSS.AxisData[i].Data.HomeOffset, 0, 0);
//	switch(mode)
//	{
//		case ABSMODE:
//			HZ_AxMoveAbs(i,pos);
//			break;
//		case RELMODE:
//			HZ_AxMoveRel(i,pos);
//			break;
//		case SPDMODE:
//			HZ_AxMoveVelocity(i,pos);
//			break;
//		case GOHOMEMODE:
//			HZ_AxHome(i);
//			break;
//	}
//}

//气缸到位信号检测
//Return 0; 无错误
//ReTurn 1; 有错误
u32	CylinderCheckTimer[CylinderCheckNum];
u32	CylinderPinTemp[CylinderCheckNum];
u32	CylinderCheckFlag[CylinderCheckNum];
u8 CylinderCheck(u32 Num, u32 CylinderPin, u32 OnPin, u32 OffPin, u32 OnT, u32 OffT)
{
    if(OutPut_GetSta( CylinderPin) != CylinderPinTemp[Num])
    {
        CylinderCheckFlag[Num] = 1;
        if(OutPut_GetSta( CylinderPin) == 1)
        {
            CylinderCheckTimer[Num] = OffT;
        }
        else
        {
            CylinderCheckTimer[Num] = OnT;
        }
    }
    CylinderPinTemp[Num] = OutPut_GetSta( CylinderPin);
    if(CylinderCheckFlag[Num] == 1 && CylinderCheckTimer[Num] == 0)
    {
        CylinderCheckFlag[Num] = 0;
        if((OutPut_GetSta( CylinderPin) == 1 && OffPin == 1) || (OutPut_GetSta( CylinderPin) == 0 && OnPin == 1))
        {
            return 1;
        }
    }
    return 0;
}

//气缸到位信号检测
//Return 0; 无错误
//ReTurn 1; 有错误
//u32	CylinderCheckTimer[CylinderCheckNum];
//u32	CylinderPinTemp[CylinderCheckNum];
//u32	CylinderCheckFlag[CylinderCheckNum];
u8 CylinderCheck_NYZ(u32 Num, u32 CylinderPin, u32 OnPin, u32 OffPin, u32 OnT, u32 OffT)
{
    if(CylinderPin != CylinderPinTemp[Num])
    {
        CylinderCheckFlag[Num] = 1;
        if(OutPut_GetSta( CylinderPin) == 1)
        {
            CylinderCheckTimer[Num] = OffT;
        }
        else
        {
            CylinderCheckTimer[Num] = OnT;
        }
    }
    CylinderPinTemp[Num] = OutPut_GetSta( CylinderPin);
    if(CylinderCheckFlag[Num] == 1 && CylinderCheckTimer[Num] == 0)
    {
        CylinderCheckFlag[Num] = 0;
        if((CylinderPin == 1 && OffPin == 1) || (CylinderPin == 0 && OnPin == 1))
        {
            return 1;
        }
    }
    return 0;
}

//void AxisRunDefaultUserUnit(u8 i,u32 mode,u32 Ratio,s32 pos)
//{
//
//}
s32 PulseToUserUnit(u8 i, s32 Dat)
{
    return (s32)((Dat * (long int)GSS.OneCircleLenth[i]) / (long int)GSS.OneCirclePulse[i]);
}

s32	UserUnitToPulse(u8 i, s32 Dat)
{
    return (s32)((long int)Dat * (long int)GSS.OneCirclePulse[i] / (long int)GSS.OneCircleLenth[i]);
}

void AxisRunDefaultUserUnit(u8 i, u32 mode, u32 Ratio, s32 pos)
{
    float speedRate;
//    speedRate = (float)((Ratio + 1) * Ratio) / 100;
    speedRate = (float)Ratio/100;
    speedRate = (float)speedRate * (GUS.SpeedRatio * GUS.SpeedRatio) / 100;
    if(Ratio == 11)
    {
       speedRate = 1; 
    }
    HZ_AxReset(i);
    if (i < PULS_NUM)
    {
        HZ_AxSetPara(i, GSS.AxisData[i].Data.StartSpeed,
                     GSS.AxisData[i].Data.Acctime,
                     GSS.AxisData[i].Data.RunSpeed * speedRate,
                     GSS.AxisData[i].Data.Dectime,
                     GSS.AxisData[i].Data.EndSpeed,
                     GSS.AxisData[i].Data.HomeSpeedFast,
                     GSS.AxisData[i].Data.HomeSpeedSlow,
                     GSS.AxisData[i].Data.HomeOffset, 0, 0);
    }
    switch(mode)
    {
    case ABSMODE:
        HZ_AxMoveAbs(i, UserUnitToPulse(i, pos));
        break;
    case RELMODE:
        HZ_AxMoveRel(i, UserUnitToPulse(i, pos));
        break;
    case SPDMODE:
        HZ_AxMoveVelocity(i, UserUnitToPulse(i, pos));
        break;
    case GOHOMEMODE:
        HZ_AxReset(i);
        HZ_AxHome(i);
        break;
    }
}


//设定单字的某位设置为1
s8 SetWhichBitU16(u32 *word, u32 whichBit)
{
    if(whichBit < 32)
    {
        *word |= 1 << whichBit;
        return 1;
    }
    else
    {
        return -1;
    }
}

//设定单字的某位设置为0
s8 ResetWhichBitU16(u32 *word, u32 whichBit)
{
    if(whichBit < 32)
    {
        *word &= ~(1 << whichBit);
        return 1;
    }
    else
    {
        return -1;
    }
}
//判断32位某一位的值
u8 WordToBit(u32 Word, u32 whichBit)
{
    if(Word & ((u32)1 << whichBit))
    {
        return 1;
    }
    else
    {
        return 0;
    }
}

//最大值
s32 MaxS32(s32 a, s32 b)
{
    if(a > b)
    {
        return a;
    }
    else
    {
        return b;
    }
}

void AxisIoUpdata()
{
    u8 i;
    static u32 AxisEnStaTerm;
    for (i = 0; i < PULS_NUM; i++) //轴口使能
    {
        GUR.AxisAlmSta |= (u32) ALM_GetSta(i) << i;
        GUR.AxisDirSta |= (u32) Dir_GetSta(i) << i;
        GUW.AxisEnSta |= (u32) EN_GetSta(i) << i;
        if(GUW.AxisEnSta != AxisEnStaTerm)
        {
            AxisEnStaTerm = GUW.AxisEnSta;
            if(GUW.AxisEnSta && ((u32)1 << i))
            {
                EN_SetSta(i, OFF);
            }
            else
            {
                EN_SetSta(i, ON);
            }
        }
    }
}

/**
* @author 18/12/14  汪豪
* @Description: 获取轴状态
* @param --
* @param --
* @return --
*/
s32 UserGetStatus(u8 AxNum)
{
//	if (AxNum < PULS_NUM)
//	{
    return HZ_AxGetStatus(AxNum);
//	}
//	else
//	{
////		return HZ_ExAxGetStatus(AxNum - 16);
//	}
}

/**
* @author 19/02/18  wangh
* @Description: 回原点函数
* @param --
* @param --
* @return --
*/
void UserGoHome(u8 AxNum)
{
    SetAxisSpdRadio(AxNum, 100);//快速回零
    if (AxNum < PULS_NUM)
    {
        HZ_AxReset(AxNum);
        HZ_AxHome(AxNum);
    }
    else
    {
//		HZ_ExAxHome(AxNum - 16);
    }
}


/**
* @author 19/07/05  汪豪
* @Description: 轴绝对值运行 速度匹配另一轴
* @param --
* @param --
* @return --
*/
void TwoAxMoveAbsUserUnit(u8 RunAxis, u8 FllowAxis, u8 SPD, s32 POS)
{
    float speedRate;
//    speedRate = (float)((SPD + 1) * SPD) / 100;
    speedRate = (float)SPD/100;
    speedRate = (float)speedRate * (GUS.SpeedRatio * GUS.SpeedRatio) / 100;
    if (RunAxis < PULS_NUM)
    {
        HZ_AxSetPara(RunAxis, GSS.AxisData[FllowAxis].Data.StartSpeed,
                     GSS.AxisData[FllowAxis].Data.Acctime,
                     GSS.AxisData[FllowAxis].Data.RunSpeed * speedRate,
                     GSS.AxisData[FllowAxis].Data.Acctime,
                     GSS.AxisData[FllowAxis].Data.EndSpeed,
                     GSS.AxisData[FllowAxis].Data.HomeSpeedFast,
                     GSS.AxisData[FllowAxis].Data.HomeSpeedSlow,
                     GSS.AxisData[FllowAxis].Data.HomeOffset, 0, 0);
        HZ_AxMoveAbs(RunAxis, (s32)((float)POS * GSS.OneCirclePulse[RunAxis] / GSS.OneCircleLenth[RunAxis]));
    }
}

/**
* @author 19/07/05  汪豪
* @Description: 轴绝对值运行 速度匹配另一轴
* @param --
* @param --
* @return --
*/
void TwoAxMoveRelUserUnit(u8 RunAxis, u8 FllowAxis, u8 SPD, s32 POS)
{
    float speedRate;
//    speedRate = (float)((SPD + 1) * SPD) / 100;
    speedRate = (float)SPD/ 100;
    speedRate = (float)speedRate * (GUS.SpeedRatio * GUS.SpeedRatio) / 100;
    if (RunAxis < PULS_NUM)
    {
        
        HZ_AxSetPara(RunAxis, UserUnitToPulse(RunAxis,PulseToUserUnit(FllowAxis, GSS.AxisData[FllowAxis].Data.StartSpeed)),
                     GSS.AxisData[FllowAxis].Data.Acctime,
                     (UserUnitToPulse(RunAxis,PulseToUserUnit(FllowAxis, GSS.AxisData[FllowAxis].Data.RunSpeed))) * speedRate,
                     GSS.AxisData[FllowAxis].Data.Acctime,
                     UserUnitToPulse(RunAxis,PulseToUserUnit(FllowAxis, GSS.AxisData[FllowAxis].Data.StartSpeed)),
                     GSS.AxisData[FllowAxis].Data.HomeSpeedFast,
                     GSS.AxisData[FllowAxis].Data.HomeSpeedSlow,
                     GSS.AxisData[FllowAxis].Data.HomeOffset, 0, 0);
        HZ_AxMoveRel(RunAxis, (s32)((float)POS * GSS.OneCirclePulse[RunAxis] / GSS.OneCircleLenth[RunAxis]));
    }
}


/**
* @author 18/12/13  汪豪
* @Description: 轴按实际距离绝对位置动
* @param --
* @param --
* @return --
*/
void AxMoveAbsUserUnit(u8 NUM, u8 SPD, s32 POS)
{
    HZ_AxReset(NUM);
    float speedRate;
//    speedRate = (float)((SPD + 1) * SPD) / 100;
    speedRate = (float)SPD/100;
    speedRate = (float)speedRate * (GUS.SpeedRatio * GUS.SpeedRatio) / 100;
    if (NUM < PULS_NUM)
    {
        HZ_AxSetPara(NUM, GSS.AxisData[NUM].Data.StartSpeed,
                     GSS.AxisData[NUM].Data.Acctime,
                     GSS.AxisData[NUM].Data.RunSpeed * speedRate,
                     GSS.AxisData[NUM].Data.Acctime,
                     GSS.AxisData[NUM].Data.StartSpeed,
                     GSS.AxisData[NUM].Data.HomeSpeedFast,
                     GSS.AxisData[NUM].Data.HomeSpeedSlow,
                     GSS.AxisData[NUM].Data.HomeOffset, 0, 0);
        HZ_AxMoveAbs(NUM, (s32)((float)POS * GSS.OneCirclePulse[NUM] / GSS.OneCircleLenth[NUM]));
    }
    else
    {
//		HZ_ExAxSetPara(NUM - 16, GSS.AxisData[NUM].Data.StartSpeed,
//					   GSS.AxisData[NUM].Data.Acctime,
//					   GSS.AxisData[NUM].Data.RunSpeed * speedRate,
//					   GSS.AxisData[NUM].Data.Dectime,
//					   GSS.AxisData[NUM].Data.EndSpeed,
//					   GSS.AxisData[NUM].Data.HomeSpeedFast,
//					   GSS.AxisData[NUM].Data.HomeSpeedSlow,
//					   GSS.AxisData[NUM].Data.HomeOffset, 0, 0);
//		HZ_ExAxMoveAbs(NUM - 16, (s32)((float)POS * GSS.OneCirclePulse[NUM] / GSS.OneCircleLenth[NUM]));
    }
}


/**
* @author 18/12/13  汪豪
* @Description: 轴按实际距离相对位置动
* @param --
* @param --
* @return --
*/
void AxMoveRelUserUnit(u8 NUM, u8 SPD, s32 POS)
{
    float speedRate;
	static int Pos;
//    speedRate = (float)((SPD + 1) * SPD) / 100;
    speedRate = (float) SPD / 100;
    if(NUM != 6)//排除端子机轴
    {
        speedRate = (float)speedRate * (GUS.SpeedRatio * GUS.SpeedRatio) / 100;
    }
    HZ_AxReset(NUM);
	Pos =(s32)((float)POS * GSS.OneCirclePulse[NUM] / GSS.OneCircleLenth[NUM]);
	if(POS >0)
	{
		if(Pos == 0)
		{
			Pos = 1;
		}
	}
	else
	{
		if(Pos == 0)
		{
			Pos = -1;
		}
	}
    if (NUM < PULS_NUM)
    {
        HZ_AxSetPara(NUM, GSS.AxisData[NUM].Data.StartSpeed,
                     GSS.AxisData[NUM].Data.Acctime,
                     GSS.AxisData[NUM].Data.RunSpeed * speedRate,
                     GSS.AxisData[NUM].Data.Acctime,
                     GSS.AxisData[NUM].Data.StartSpeed,
                     GSS.AxisData[NUM].Data.HomeSpeedFast,
                     GSS.AxisData[NUM].Data.HomeSpeedSlow,
                     GSS.AxisData[NUM].Data.HomeOffset, 0, 0);
        HZ_AxMoveRel(NUM, Pos);
    }
//	else
//	{
//		HZ_ExAxSetPara(NUM - 16, GSS.AxisData[NUM].Data.StartSpeed,
//					   GSS.AxisData[NUM].Data.Acctime,
//					   GSS.AxisData[NUM].Data.RunSpeed * speedRate,
//					   GSS.AxisData[NUM].Data.Dectime,
//					   GSS.AxisData[NUM].Data.EndSpeed,
//					   GSS.AxisData[NUM].Data.HomeSpeedFast,
//					   GSS.AxisData[NUM].Data.HomeSpeedSlow,
//					   GSS.AxisData[NUM].Data.HomeOffset, 0, 0);
//		HZ_ExAxMoveRel(NUM - 16, (s32)((float)POS * GSS.OneCirclePulse[NUM] / GSS.OneCircleLenth[NUM]));
//	}
}


//获取输入口状态
//cardAdr : 扩展卡地址  0:本体  1：从卡1 ......
//num : 输入编号
//return ：0：ON 1:OFF，其他参数错误
s32 InputGetSta(u8 cardAdr, u8 num)
{
    if(cardAdr == 0)
    {
        return InPut_GetSta(num);
    }
    else
    {
        return HZ_ExInPutGet(cardAdr - 1, num);
    }
}


//获取输入口状态
//cardAdr : 扩展卡地址  0:本体  1：从卡1 ......
//num
//return ：0：ON 1:OFF，其他参数错误
s32 UserGetOutput(u8 cardAdr, u8 num)
{
    if(cardAdr == 0)
    {
        return OutPut_GetSta(num);
    }
    else
    {
        return HZ_ExOutPutGet(cardAdr - 1, num + 16);
    }
}

//获取输入口状态
//cardAdr : 扩展卡地址  0:本体  1：从卡1 ......
//num：输入编号
//return ：0：ON 1:OFF，其他参数错误
s32 UserOutputSet(u8 cardAdr, u8 num, u8 value)
{
    if(cardAdr == 0)
    {
        return OutPut_SetSta(num, value);
    }
    else
    {
        return HZ_ExOutPutSet(cardAdr - 1, num + 16, value);
    }
}


/**
* @author 19/02/21  wangh
* @Description:
* @param --
* @param --
* @return --
*/
void SetAxisEn()
{
    EN_SetSta(0, OFF);
    EN_SetSta(1, ON);
    EN_SetSta(2, ON);
    EN_SetSta(3, ON);
    EN_SetSta(4, ON);
    EN_SetSta(5, ON);
    EN_SetSta(6, ON);
    EN_SetSta(7, ON);
    EN_SetSta(8, ON);
    EN_SetSta(9, ON);
    EN_SetSta(10, ON);
    EN_SetSta(11, ON);

}

/**
* @author 19/02/21  wangh
* @Description:
* @param --
* @param --
* @return --
*/
void ResetAxisEn()
{
    EN_SetSta(0, ON);
    EN_SetSta(1, OFF);
    EN_SetSta(2, OFF);
    EN_SetSta(3, OFF);
    EN_SetSta(4, OFF);
    EN_SetSta(5, OFF);
    EN_SetSta(6, OFF);
    EN_SetSta(7, OFF);
    EN_SetSta(8, OFF);
    EN_SetSta(9, OFF);
    EN_SetSta(10, OFF);
    EN_SetSta(11, OFF);
}


/**
* @author 19/09/27 wangh
* @Description: 取余 A对B取余
* @param --
* @param --
* @return --
*/
s32 Remainder(s32 A, s32 B)
{
    if(B == 0)
    {
        return B;
    }
    while(A > B)
    {
        A = A - B;
    }
    return A;
}

/**
* @author： 2019/7/8  hgg
* @Description: 读取时间，触摸屏上可以看到设备时间
* @param --
* @param --
* @return --
*/
void TimeRefresher()
{
//	GUW.Date[READ].year = GSR.date.year;
//	GUW.Date[READ].month = GSR.date.month;
//	GUW.Date[READ].day = GSR.date.day;
//	GUW.Date[READ].week = GSR.date.week;
//
//
//	GUW.Time[READ].hour = GSR.time.hour;
//	GUW.Time[READ].min = GSR.time.min;
//	GUW.Time[READ].sec = GSR.time.sec;
}


/******************************/

/**
* @author： 2019/7/8  hgg
* @Description: 检查是否过期
* @param 返回1为正常，0为过期
* @param --
* @return --
*/
int InitLocker()
{
    if(19 > GSR.date.year)
    {
        return 1;
    }
//		else if(7 > GSR.date.month)
//		{
//			return 1;
//		}
//		else if(9 > GSR.date.day)
//		{
//			return 1;
//		}
    else//当前年份小于19  锁机
    {
        return 0;
    }
}


int Locker()
{
    if(SysTim_GetUser() > 50000)
    {

        GUW.LockTime = 0;
        if(InitLocker() == 0)
        {
            //计算剩余时间
            if(GUW.CurrDateForUser.year > GSR.date.year)
            {
                GUW.LockTime = GUW.LockTime + (GUW.CurrDateForUser.year - GSR.date.year) * 365;
            }
            if(GUW.CurrDateForUser.month > GSR.date.month)
            {
                GUW.LockTime = GUW.LockTime + (GUW.CurrDateForUser.month - GSR.date.month) * 30;
            }
            if(GUW.CurrDateForUser.day > GSR.date.day)
            {
                GUW.LockTime = GUW.LockTime + (GUW.CurrDateForUser.day - GSR.date.day);
            }
            //永久解锁
            if(GUW.LockTime > (365*5))
            {
                GSS.UNLOCKEn = 1;
            }
			
			GUW.LockYearForHMI = 2000 + GUW.CurrDateForUser.year;
			GUW.LockMonthForHMI = GUW.CurrDateForUser.month;
			GUW.LockDayForHMI = GUW.CurrDateForUser.day;
            
            //判断是否锁机
            if(GUW.CurrDateForUser.year > GSR.date.year)
            {
                return 1;
            }
            else if(GUW.CurrDateForUser.year < GSR.date.year)//当前日期大于设置日期 锁机
            {
                return 0;
            }
            else if(GUW.CurrDateForUser.month > GSR.date.month)
            {
                return 1;
            }
            else if(GUW.CurrDateForUser.month < GSR.date.month)//当前日期大于设置日期 锁机
            {
                return 0;
            }
            else if(GUW.CurrDateForUser.day > GSR.date.day)
            {
                return 1;
            }
            else//当前日期大于设置日期 锁机
            {
                return 0;
            }
        }
        else
        {
            return 0;
        }
    }
    else
    {
        return 1;
    }
	
}






