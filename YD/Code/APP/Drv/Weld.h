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
		float readypos;	//Ԥ��λ��
		float Pullpos;	//����λ��
		u32   pushspd;	//ǰ���ٶ�
		u32   pullspd;	//�����ٶ�
		u32   pullCloseT; //�պ�ʱ��
		u32   pullOpenT;	//�ſ�ʱ��
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
		u32   CutTime;		//����ʱ��
		u32   CutOpenTime;	//�����ſ�ʱ��
	}Para;
} ClampPara;

typedef union {
	u32 Data[10];
	struct 
	{
		u32 WeldingTime;	//����ʱ��
		u32 CuringTime;	//�̻�ʱ��
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
	DrvStatus DrvSta;	//�Լ���״̬��
	struct
	{
		LogicParaDef *PullTask;	 //����
		LogicParaDef *ClampTask; //����
		LogicParaDef *WeldTask;	 //����
	} Task;
	u8 PullSta;		//0 ���� 1,����ȡ�� 2.ȡ�����  ---���״̬2���ں���֮����0
	u8 ClampSta;	//0 ���� 1,����ȡ�� 2.ȡ����� ---�ں�������ѹס�ߺ����״̬д2.
	u8 WeldingSta;	//0 ���� 1,����ȡ�� 2.ȡ����� 	--�жϵ�ѹס�ߵ�״̬��2ʱ����ʼ��ʱ����
	void (*Start)(struct Weld_type* drv);
	void (*Stop)(struct Weld_type* drv);
	u8 (*GetSta)(struct Weld_type* drv);
	void (*ClrErr)(struct Weld_type* drv); //�������
} WeldDrv;

extern WeldDrv Weld_drv_L; //�ⲿʹ��ʵ����
extern WeldDrv Weld_drv_R; //�ⲿʹ��ʵ����

extern void Weld(WeldDrv *drv);

#endif
