#ifndef __FSM_H
#define __FSM_H

#include "Logic.h"
#include "DataDefine.h"
//#include "bsp.h"
#include "reset.h"


typedef enum
{
    INIT, //0
    STOP,	//1
    RUN	,	//2
    D_RESET,	//3 Դ����ʹ�ó����ظ����壬��Ҫ���ڴ�����������ʹ��
    SCRAM, //4
    PAUSE, //5
} RunStatus;

extern void FSM(void);

#endif

