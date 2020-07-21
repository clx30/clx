/*
 * @Author: your name
 * @Date: 2020-05-28 18:14:02
 * @LastEditTime: 2020-05-31 23:41:47
 * @LastEditors: Please set LastEditors
 * @Description: In User Settings Edit
 * @FilePath: \YD_KZJ\Code\APP\Drv\Weld.h
 */
#ifndef __WELD_H
#define __WELD_H

#include "LogicData.h"
#include "DrvCom.h"

typedef union {
	u32 Data[10];
	struct 
	{
		float readypos;	//预备位置
		float Pullpos;	//后拉位置
		u32   pushspd;	//前进速度
		u32   pullspd;	//后拉速度
		u32   pullCloseT; //闭合时间
		u32   pullOpenT;	//张开时间
	}Para;
} PullPara;
typedef union {
	u32 Data[10];
	struct 
	{
		float readpos[2];
		float workpos[2];
		u32   workspd[2];
		u32   CloseClampT;
		u32	  OpenClampT;
		u32   CutTime;		//剪刀时间
		u32   CutOpenTime;	//剪刀张开时间
	}Para;
} ClampPara;

typedef union {
	u32 Data[10];
	struct 
	{
		u32 WeldingTime;	//焊接时间
		u32 CuringTime;	//固化时间
	}Para;
} WeldingPara;
typedef struct
{
	PullPara pull;
	ClampPara clamp;
	WeldingPara welding;
} WeldPara;

typedef struct Weld_type
{
	struct
	{
		u8 AxTurn1;
		u8 AxTurn2;
		u8 AxPull;
	} Axis;
	struct
	{
		IODef I_Weldup;
		IODef I_Welddw;
		IODef I_ClampUp;
		IODef I_ClampDw;
		IODef I_PressEarUp;
		IODef I_PressEarDw;
	} In;
	struct
	{
		IODef Q_Weld;
		IODef Q_ClampUD;
		IODef Q_PressEar;
		IODef Q_Clamp;
		IODef Q_Pull;	
		IODef Q_Cut;
		IODef Q_Ultrasonic;
	} Out;
	WeldPara  para;
	DrvStatus DrvSta;	//自己的状态机
	struct
	{
		LogicParaDef *PullTask;	 //拉线
		LogicParaDef *ClampTask; //夹线
		LogicParaDef *WeldTask;	 //焊线
	} Task;
	u8 PullSta;		//0 空闲 1,正在取线 2.取线完成  ---这个状态2是在焊完之后清0
	u8 ClampSta;	//0 空闲 1,正在取线 2.取线完成 ---在焊接气缸压住线后，这个状态写2.
	u8 WeldingSta;	//0 空闲 1,正在取线 2.取线完成 	--判断到压住线的状态是2时，开始计时焊接
	void (*Start)(struct Weld_type* drv);
	void (*Stop)(struct Weld_type* drv);
	u8 (*GetSta)(struct Weld_type* drv);
	void (*ClrErr)(struct Weld_type* drv); //清除错误
} WeldDrv;

extern WeldDrv Weld_drv_L; //外部使用实体左
extern WeldDrv Weld_drv_R; //外部使用实体右

extern void Weld(WeldDrv *drv);

#endif
