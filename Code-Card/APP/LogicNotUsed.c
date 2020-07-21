/*
 * File: LogicTerm.c
 * File Created: 2019.5.7
 * version: v00.00.01 12轴底层
 * Description:逻辑里没有使用的函数，都放置在这里
 * Modified By:NYZ
 * -----
 * Copyright 2019 - 2019, <<hzzh>>
 */


#include "Logic.h"
#include "FSM.h"
#include "USERTOOL.H"


//void SepareWire1(LogicParaDef *LG)
//{
//    static int TargLength = 0;
//    if(LG->execute == 1 && LG->step == 0)
//    {//计算当前的起始长度
//        LG->step = 1;
//        TargLength = 0;
//    }
//    switch(LG->step)
//    {
//        case 1://计算当前位置到开始分线位置
//            //线长-（分刀到中刀距离对线长取余）- 前分距离 -后分距离。
//            TargLength = SepareWireData.WireLength - Remainder(SepareWireData.cutterSeparateDistance,SepareWireData.WireLength) - SepareWireData.A_separateWireLength - SepareWireData.B_separateWireLength;
//            LG->step = 2;
//        break;
//        
//        case 2://目标长度与剩余长度比较 到达即结束
//            break;
//        
//        case 3://去到目标长度
//            break;
//        
//        case 4://分线刀向下刺破
//            break;
//        
//        case 5://计算分线长度 前端分线+后端分线
//            break;
//        
//        case 6://判断目标长度和剩余长度比较 到达即结束
//            break;
//        
//        case 7://去到目标位置
//            break;  
//        
//        case 8://分线刀抬起
//            break; 
//        
//        case 9://计算当前位置到去切断位置
//            break;  
//        
//        case 10://判断目标长度和剩余长度比较 到达即结束
//            break;
//    }
//}



