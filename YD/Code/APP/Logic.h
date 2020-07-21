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



	

#define	NORMALMODE	0		//正常模式
#define	AGINGMODE	1		//老化模式
#define	STEPMODE	2		//单步调试
#define AXISDELAY	5		//停轴延时
#define ENDCASE     0xff	//最后一个case


typedef struct//系统任务队列
{
    LogicParaDef AxisMoveTask[PULS_NUM];
}Task;
extern Task LogicTask;

typedef enum
{
	IDLE,
	BUSY
}State_tpye;

typedef struct//系统程序自用变量
{
    int a;
} SysData;
extern SysData Data;

typedef enum	//触发器编号定义
{
  tg0,tg1,tg2,tg3,tg4,tg5,tg6,tg7,tg8,tg9,tg10,tg11,
}TgNum;


extern void Logic(void);

#endif


