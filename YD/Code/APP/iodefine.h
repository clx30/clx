/*
 * @Author: your name
 * @Date: 2020-05-26 14:03:11
 * @LastEditTime: 2020-05-29 11:55:30
 * @LastEditors: Please set LastEditors
 * @Description: In User Settings Edit
 * @FilePath: \YD_KZJ\Code\APP\iodefine.h
 */
#ifndef __IODEFINE_H
#define __IODEFINE_H
typedef enum //输出口定义
{
	X1,
	X2,
	X3,
	X4,
	X5,
	X6,
	X7,
	X8,
	X9,
	X10,
	X11,
	X12,
	X13,
	X14,
	X15,
	X16,
	X17,
	X18,
	X19,
	X20,
	X21,
	X22,
	X23,
	X24,
} OutNum;

typedef enum //输入口定义
{
	Y1,
	Y2,
	Y3,
	Y4,
	Y5,
	Y6,
	Y7,
	Y8,
	Y9,
	Y10,
	Y11,
	Y12,
	Y13,
	Y14,
	Y15,
	Y16,
	Y17,
	Y18,
	Y19,
	Y20,
	Y21,
	Y22,
	Y23,
	Y24,
} InPutNum;

typedef enum
{
	EX1,
	EX2,
	EX3,
	EX4,
	EX5,
	EX6,
	EX7,
	EX8,
	EX9,
	EX10,
	EX11,
	EX12,
	EX13,
	EX14,
	EX15,
	EX16,
	EX17,
	EX18,
	EX19,
	EX20,
	EX21,
	EX22,
	EX23,
	EX24,
	EX25,
	EX26,
	EX27,
	EX28,
	EX29,
	EX30,
	EX31,
	EX32
} ExInPutNum;
typedef enum
{
	EY1,
	EY2,
	EY3,
	EY4,
	EY5,
	EY6,
	EY7,
	EY8,
	EY9,
	EY10,
	EY11,
	EY12,
	EY13,
	EY14,
	EY15,
	EY16,
	EY17,
	EY18,
	EY19,
	EY20,
	EY21,
	EY22,
	EY23,
	EY24,
	EY25,
	EY26,
	EY27,
	EY28,
	EY29,
	EY30,
	EY31,
	EY32
} ExOutPutNum;
#define CARD1 0
#define CARD2 1
#define CARD3 2

#define I_Start CARD1, X13
#define I_Stop CARD1, X14
#define I_Reset CARD1, X15

#define I_LWeldUp CARD1, X16
#define I_LWeldDw CARD1, X17
#define I_LClampUp CARD1, X18
#define I_LClampDw CARD1, X19
#define I_LPressUp CARD1, X20
#define I_LPressDw CARD1, X21

#define I_RWeldUp CARD2, EX1
#define I_RWeldDw CARD2, EX2
#define I_RClampUp CARD2, EX3
#define I_RClampDw CARD2, EX4
#define I_RPressUp CARD2, EX5
#define I_RPressDw CARD2, EX6

#define I_MaskSensor CARD2, EX10
#define I_MidPressUp CARD2, EX11
#define I_MidPressDw CARD2, EX12
#define I_LEarSensor CARD2, EX13
#define I_REarSensor CARD2, EX14

#define I_Discharge CARD2, EX18
#define I_DischargeUp CARD2, EX19
#define I_DischargeDw CARD2, EX20

#define Q_Beep		CARD1,Y1

#define Q_LWeld		CARD1,Y2
#define Q_LClampUD	CARD1,Y3
#define Q_LPressEar CARD1,Y4
#define Q_LClamp	CARD1,Y5
#define Q_LPull		CARD1,Y6
#define Q_LCut		CARD1,Y7
#define Q_LUltrasonic CARD1,Y8	//超声波

#define Q_RWeld		CARD1,Y9	//焊接
#define Q_RClampUD	CARD1,Y10	//夹子上下
#define Q_RPressEar CARD1,Y11	//压耳
#define Q_RClamp	CARD1,Y12	//夹子
#define Q_RPull		CARD1,Y13	//拉线
#define Q_RCut		CARD1,Y14	//剪线
#define Q_RUltrasonic CARD1,Y15	//超声波

#define Q_MidPress  CARD1,Y16	//中间下压
#define Q_LEarMotor CARD1,Y17	//左耳带电机
#define Q_REarMotor CARD1,Y18   //右耳带电机
#define Q_Discharge CARD1,Y19	//收料电机
#define Q_Dischargebelt CARD1,Y20 //收料皮带
#define Q_DischargeUD	CARD1,Y21 //收料上下

#endif
