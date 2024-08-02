/* 
 * File:   AlarmSound.h
 * Author: K.Ohno
 *
 * Created on December 13, 2020, 2:29 PM
 */

#ifndef ALARMSOUND_H
#define	ALARMSOUND_H

#ifdef	__cplusplus
extern "C" {
#endif
    
#include "mcc_generated_files/mcc.h"


//アラームオンのパターン指定
enum Alarm {
    AlarmSound0, // 0-2の3種類のアラーム音 内部で30秒ごとに変化させる
    AlarmSound1,
    AlarmSound2,
    AlarmSingle,
    AlarmTwice,
};

void SoundHandler();
void AlarmSoundOn(uint8_t sn);
void AlarmSoundOff(void);


#ifdef	__cplusplus
}
#endif

#endif	/* ALARMSOUND_H */

