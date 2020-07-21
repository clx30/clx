//#include "bsp.h"
#include "interlayer.h"
#include "bsp_config.h"
#include "appinit.h"
#include "lock.h"
#include "can_master.h"

const unsigned char*HARD_VER = (unsigned char *)0X803f000;
const unsigned char*BOOT_VER = (unsigned char *)0X803f040;
const unsigned char UserDev_Inf[3][64] =
{
    "F305_Ver.01.00.00_Beta",
    "A305_Ver.01.00.06_20200520-3",
    "__Debug__"					//调试版本
//	"_Release_"					//发行版本
};

const uint32_t Code_Versions __attribute__((at(0x08040800))) = 0xA12;
int main()
{
    int i;

//    sys_init();
    sys_init_IAP();
    bsp_init();
    bsp_exec();
    AppInit();
    //版本号配置
    for(i = 0; i < 40; i++)
    {
        GSR.SoftWare_Ver[i] = UserDev_Inf[1][i];
    }
    //板间通讯
    while(2 != HZ_ExCanNetStateGet())
    {
        InterLayer();
        bsp_exec();
    }
    while(1)
    {
        if(Locker() == 0 && Data.LockEn == 1 && GSS.UNLOCKEn != 1)												//0为过期，1为正常
        {
			AlarmSetBit(2,1,11);//过期
			GUW.button.RunCommand = SCRAM;
			GUW.LockGHMI = 1;
        }
        else
        {
			GUW.LockGHMI = 0;
			AlarmClearBit(2,0,12);
        }
        InterLayer();
        bsp_exec();
    }
}


int pwr_Task(void)
{
    FRam_Write(0,&GSS,8000);
    return 0;
}

s32 EnCode_GetGpioVal(u8 num, u8 pin)
{
    return 0;
}
//s32 HZ_ExAxGetStatus(u8 num)
//{
//    return 0;
//}


