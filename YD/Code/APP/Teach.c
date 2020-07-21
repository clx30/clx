#include "Teach.h"

void Teach()
{
    u8 i;
    if(GUR.RunStatus==RUN && GUW.TeachWord != 2)
    {
        return;
    }
    switch(GUW.TeachWord) //1520
    {

    case 5: //±¨¾¯Çå³ý
        for(i = 0; i < 20; i++)
        {
            GSR.ErrorCode[i] = 0;
        }
        GSW.ClearAlarm = 1;
        break;
    default:
        break;
    }
    GUW.TeachWord = 0;
}

