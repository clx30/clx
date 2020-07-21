#ifndef _USERTOOL_H_
#define	_USERTOOL_H_

#include "Logic.h"
#include "DataDefine.h"
#include "bsplib.h"
#include "interlayer_config.h"

#define	CylinderCheckNum	10

extern void SetAxisSpdRadio(u8 i, u8 spd);
extern s32 SetSpd(u8 i, u32 RunSpeed);
extern void SetSpdDefault(u8 i);
extern s32 Check_AxisStatu(void);
extern s32 Check_xyzMotor(void);
extern void AxisRunDefaultUserUnit(u8 i, u32 mode, u32 Ratio, s32 pos);
extern s8 SetWhichBitU16(u32 *word, u32 whichBit);
extern s8 ResetWhichBitU16(u32 *word, u32 whichBit);
extern u8 WordToBit(u32 Word, u32 whichBit);
extern s32 MaxS32(s32 a, s32 b);
extern s32 PulseToUserUnit(u8 i, s32 Dat);
extern u8 CylinderCheck(u32 Num, u32 CylinderPin, u32 OnPin, u32 OffPin, u32 OnT, u32 OffT);
extern u8 CylinderCheck_NYZ(u32 Num, u32 CylinderPin, u32 OnPin, u32 OffPin, u32 OnT, u32 OffT);
extern s32 UserGetStatus(u8 AxNum);
extern void UserGoHome(u8 AxNum);
extern void AxMoveAbsUserUnit(u8 NUM, u8 SPD, s32 POS);
extern void AxMoveRelUserUnit(u8 NUM, u8 SPD, s32 POS);
extern void TwoAxMoveAbsUserUnit(u8 RunAxis, u8 FllowAxis, u8 SPD, s32 POS);
extern void TwoAxMoveRelUserUnit(u8 RunAxis, u8 FllowAxis, u8 SPD, s32 POS);

extern s32	UserUnitToPulse(u8 i, s32 Dat);

extern s32 Remainder(s32 A, s32 B);


extern s32 InputGetSta(u8 cardAdr, u8 num);
extern s32 UserGetOutput(u8 cardAdr, u8 num);
extern s32 UserOutputSet(u8 cardAdr, u8 num, u8 value);

extern void ResetAxisEn(void);
extern void SetAxisEn(void);

extern int Locker(void);
extern int InitLocker(void);


#endif


