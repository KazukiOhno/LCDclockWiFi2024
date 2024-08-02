/* 
 * File:   Draw.h
 * Author: K.Ohno
 *
 * Created on December 20, 2020, 3:50 PM
 */

#ifndef DRAW_H
#define	DRAW_H

#ifdef	__cplusplus
extern "C" {
#endif

/*
 * LCD��ɕ\�����镔�i�����ꂼ��`�悷��t�@���N�V�������
 * ���t�A�����A���x�A���x�A�A���[���A�J�����_�[�A����
 * �`��ɍۂ��ēK�p������́AObjectRsc����擾
 * ���W�A�T�C�Y�A�t�H���g�R�[�h�A�g�嗦�A�F�A�`���A
 */

#include "mcc_generated_files/mcc.h"
#include "main.h"
    
   
FRESULT ReadBmp16(char *bmpfile);
//void DrawBmp(char *ch);

//
uint8_t SetFont(uint8_t fontcode, char *fontname);
//void SetKanjiFont(uint8_t fontcode);

uint8_t displayCustom_puts(char *s);

void resetPreDateTime(void);

void DrawTime(uint8_t *datetime, ObjectRsc *rsc);
void DrawDate(uint8_t *datetime, ObjectRsc *rsc);
void DrawCalendar(ObjectRsc *rsc);

void DrawAlarmTime(uint8_t *alarmtime, uint8_t sw, ObjectRsc *rsc);
void DrawTemperature(int16_t temp, ObjectRsc *rsc);
void DrawHumidity(int16_t humidity, ObjectRsc *rsc);

void DrawGearIcon(ObjectRsc *rsc);


//void DrawStrCenter(MainFormRsc rsc, uint16_t color, uint16_t bcolor);
//void DrawButton(MainFormRsc rsc, uint16_t fcolor, uint16_t bcolor, uint16_t rcolor, int16_t r);
    
#ifdef	__cplusplus
}
#endif

#endif	/* DRAW_H */
