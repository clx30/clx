/*
 * File: LogicTerm.c
 * File Created: 2019.5.7
 * version: v00.00.01 12��ײ�
 * Description:�߼���û��ʹ�õĺ�����������������
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
//    {//���㵱ǰ����ʼ����
//        LG->step = 1;
//        TargLength = 0;
//    }
//    switch(LG->step)
//    {
//        case 1://���㵱ǰλ�õ���ʼ����λ��
//            //�߳�-���ֵ����е�������߳�ȡ�ࣩ- ǰ�־��� -��־��롣
//            TargLength = SepareWireData.WireLength - Remainder(SepareWireData.cutterSeparateDistance,SepareWireData.WireLength) - SepareWireData.A_separateWireLength - SepareWireData.B_separateWireLength;
//            LG->step = 2;
//        break;
//        
//        case 2://Ŀ�곤����ʣ�೤�ȱȽ� ���Ｔ����
//            break;
//        
//        case 3://ȥ��Ŀ�곤��
//            break;
//        
//        case 4://���ߵ����´���
//            break;
//        
//        case 5://������߳��� ǰ�˷���+��˷���
//            break;
//        
//        case 6://�ж�Ŀ�곤�Ⱥ�ʣ�೤�ȱȽ� ���Ｔ����
//            break;
//        
//        case 7://ȥ��Ŀ��λ��
//            break;  
//        
//        case 8://���ߵ�̧��
//            break; 
//        
//        case 9://���㵱ǰλ�õ�ȥ�ж�λ��
//            break;  
//        
//        case 10://�ж�Ŀ�곤�Ⱥ�ʣ�೤�ȱȽ� ���Ｔ����
//            break;
//    }
//}



