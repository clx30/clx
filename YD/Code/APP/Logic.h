/*
 * @Descripttion: 
 * @version: 
 * @Author: yfs
 * @Date: 2020-02-24 10:31:18
 * @LastEditors: Please set LastEditors
 * @LastEditTime: 2020-05-29 15:39:17
 */
#ifndef _LOGIC_H_
#define	_LOGIC_H_

#include "DataDefine.h"
#include "bsplib.h"
#include "interlayer_config.h"
#include "FSM.h"
#include "string.h"
#include "logicdata.h"
#include "iodefine.h"
#include "AxisMove.h"



	

#define	NORMALMODE	0		//����ģʽ
#define	AGINGMODE	1		//�ϻ�ģʽ
#define	STEPMODE	2		//��������
#define AXISDELAY	5		//ͣ����ʱ
#define ENDCASE     0xff	//���һ��case


typedef struct//ϵͳ�������
{
    LogicParaDef AxisMoveTask[PULS_NUM];
}Task;
extern Task LogicTask;

typedef enum
{
	IDLE,
	BUSY
}State_tpye;

typedef struct//ϵͳ�������ñ���
{
    int a;
} SysData;
extern SysData Data;

typedef enum	//��������Ŷ���
{
  tg0,tg1,tg2,tg3,tg4,tg5,tg6,tg7,tg8,tg9,tg10,tg11,
}TgNum;


extern void Logic(void);

#endif


