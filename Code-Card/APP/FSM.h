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
    D_RESET,	//3 源码中使用出现重复定义，需要后期打包后可以正常使用
    SCRAM, //4
    PAUSE, //5
} RunStatus;

extern void FSM(void);

#endif

