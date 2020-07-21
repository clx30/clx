/*
 * @Author: your name
 * @Date: 2020-05-28 15:18:38
 * @LastEditTime: 2020-05-29 14:55:35
 * @LastEditors: Please set LastEditors
 * @Description: In User Settings Edit
 * @FilePath: \YD_KZJ\Code\APP\Drv\Feed.h
 */ 
#ifndef __FEED_H
#define __FEED_H
#include "iodefine.h"
#include "LogicData.h"
#include "Logic.h"

typedef struct 
{
	u32 FeedSpd;  //�����ٶ� �ٷֱ�
	float FeedPos;	//����λ��
} FeedPara;


typedef struct Feed_type
{
	u8   PlatformSta;								//ƽ̨״̬ 0��ǰƽ̨���� 1��ǰƽ̨�����ƶ� 2.��ǰƽ̨�ƶ���λ�������ϵ�λ3.��ǰ�ƶ���λ����û������
	void (*Config)(FeedPara para,LogicParaDef *T);
	void (*Start)(void);
	u8	 (*GetSta)(void);
	void (*ClrErr)(void);	//�������
	void (*Stop)(void);
} FeedDrv;
extern FeedDrv Feed_drv;    //�ⲿʹ��ʵ��

extern void Feed(FeedDrv* feeddrv);

#endif 
