/*
 * @Author: your name
 * @Date: 2020-05-26 14:03:11
 * @LastEditTime: 2020-05-27 14:03:30
 * @LastEditors: Please set LastEditors
 * @Description: In User Settings Edit
 * @FilePath: \YD_KZJ\Code\APP\main.c
 */ 
#include "interlayer.h"
#include "bsp_config.h"
#include "appinit.h"
#include "maintask.h"
#include "can_master.h"

const uint32_t Code_Versions __attribute__((at(0x08040800))) = 0xA03;

int main()
{
    //sys_init();
	sys_init_IAP();
    bsp_init();
    bsp_exec();

    AppInit();
	while(2 != HZ_ExCanNetStateGet())
	{
        InterLayer();
        bsp_exec();
	}
    while(1)
    {
        InterLayer();
        bsp_exec();
		MainTask();
    }
}


int pwr_Task(void)
{
    HZ_Data_Write();
    return 0;
}




