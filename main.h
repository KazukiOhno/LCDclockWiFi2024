/* 
 * File:   main.h
 * Author: K.Ohno
 *
 * Created on January 3, 2021, 12:33 PM
 */

#ifndef MAIN_H
#define	MAIN_H

#ifdef	__cplusplus
extern "C" {
#endif

#include "mcc_generated_files/mcc.h"
#include "font.h"
    
//EEPROM���f�[�^�z�u(0x00-0x3ff)�@�@�d����������A��������ݒ���ǂݏo��
#define AddressInit         0x00    //1�o�C�g�@�@�����lFF�A�������񂾂�55
#define AddressDisplayMode  0x07    //1�o�C�g
//���t
#define AddressYMD          0x08    //4�o�C�g�@�j���A���A���A�N�̏�
#define AddressAlarm        0x0c    //3�o�C�g�@���A���A�j���̏�
//#define AddressAlarmNo      0x0f     //1�o�C�g�@�@�A���[���p�^�[���̕ύX�p
#define AddressTouch        0x18    //8�o�C�g�@�@�^�b�`�␳�p��2�����̍��W
#define AddressCustom       0x20    //�J�X�^������ێ��B����20�o�C�g/object�K�v(32�o�C�g�m�ۂ��Ă���)
    // 0x20- Date   *7�I�u�W�F�N�g
    // 0x40- Time
    // 0x60- Alarm
    // 0x80- Temp
    // 0xA0- Humid
    // 0xC0- Calen
    // 0xE0- Gear
#define AddressFontName    0x100    //7�̃��[�U�t�H���g�� *9�o�C�g/�t�H���g�@0x13E�܂�
    
#define hi(a) ((a) >> 8)
#define lo(a) ((a) & 0xff)

//�\�����[�h
typedef enum {
    DisplayMode0,       //0x00    //���v�\���傫���A3�����J�����_�[�\��
    DisplayMode1,       //0x01    //�J�����_�[�傫���\��
    DisplayMode2,       //0x02    //�A�i���O���v�\��
    DisplayCustom,      //0x03    //�J�X�^���\��
    MODENUM,            //        //���[�h�̎�ނ̐��������I�ɐݒ肳���
} DispMode;

//���t�\���̌`���A�t�H�[�}�b�g���Ƃ��Ďg��
enum DateFormat {
    YYYYMMDDw,  //�j���t��
    YYYYMMDDwe,  //�p��j���t��
    YYYYMMDD,
    YYMMDDw,
    YYMMDDwe,
    YYMMDD,
    MMDDw,
    MMDDwe,
    MMDD,
    MAXDateFormat,  //�v�f��
};

enum TimeFormat {
    HHMM,
    HHMMss,
    HHMMSS,
    ANALOGCLOCK,
    MAXTimeFormat,  //�v�f��
};

enum AnalogClockFormat {
    Large = 180,
    Mid_L = 160,
    Mid_S = 140,
    Small = 120,
    Tiny  = 100,
    MAXAnalogSize,  //�v�f��
};

enum AlarmFormat {
    ALMAMPM,  //AM/PM�\�L
    ALM24,    //24���ԕ\�L
    iALAMPM,  //AM/PM�\�L�@�A�C�R��
    iAL24,    //24���ԕ\�L�@�A�C�R��
    MAXAlarmFormat, //�v�f��
};

//���x�\���̌`��
enum TempFormat {
    DEGC,
    DEGF,
    MAXTempFormat,  //�v�f��
};

//�J�����_�[�̕\��
enum CalendarFormat {
    Month1  = 0,     // 0
    Month2,
    Month2v,
    Month3,
    Month3v,
    Month1e = 10,
    Month2e,
    Month2ve,
    Month3e,
    Month3ve,
    MAXCalendarFormat,  //�v�f��
};


//�I�u�W�F�N�g��\�����邩�ǂ���
typedef struct {
    uint8_t disp;       //�\�������邩�ǂ���
    int16_t x, y;       //�ŏ��̕����̍���̍��W
    int16_t xw, yw;     //�I�u�W�F�N�g��X, Y�T�C�Y�@�@��{�I�ɕs�v�����A�u���E�U�Ή����ɗ��p
    uint8_t fontcode;   //�t�H���g���w�肷��ԍ��@FontCode�Œ�`����Ă��鐔�l
    uint8_t fontmag;    //�\�����鎞�A�t�H���g�̔{�� attribute�ɑ���������
    uint16_t color;     //�F RGB-565�`��
    uint16_t format;     //�\���`��
    uint16_t attribute;  //���炩�̒ǉ����@Saturday color
    uint16_t attribute2; //�ǉ����@�@�@�@�@Sunday color
    int8_t xpitchAdj;    //�t�H���g��X�s�b�`�̒����ʁ@�f�t�H���g�s�b�`����ύX��������
//    int8_t ypitchAdj;    //�t�H���g��Y�s�b�`�@��{�I��1�s�\���Ȃ̂ŁA�s�v
    char *fontName;     //fontcode=EXTFONTCODE��:�J�X�^���t�H���g�̖��O=�t�H���_���ւ̃|�C���^
} ObjectRsc;

//���L�I�u�W�F�N�g�̏��Ƀ��\�[�X��ݒ肷��
enum RscObject {
    DateObj,        //0
    TimeObj,        //1
    AlarmObj,       //2
    TempObj,        //3
    HumidityObj,    //4
    CalendarObj,    //5
    GearObj,        //6
    OBJECTNUM,      //��`����Ă���I�u�W�F�N�g�̐�
};

/* �t�H���g�R�[�h:
 * �����t�H���g�́A3��
 * �O���t�H���g�̏ꍇ�AFontExt���w�肵�āA���ۂɎg�p����t�H���g���́A
 * �e�I�u�W�F�N�g�ɂ�1:1�őΉ����Ă���CustomFontDir�Ɋi�[
 */
#define EXTFONTCODE  10
enum FontCode {
    FontNormal = 1, //8x8
    FontSmall,      //6x8
    Font7like,      //5x7 7�Z�O���ǂ������t�H���g�@SD�J�[�h�ɗ���Ȃ��悤�p��
    FontGear,       //32x32�@�M�A�݂̂̃t�H���g
    //��������O���t�H���g
    FontExt = EXTFONTCODE,
};

extern ObjectRsc CustomRsc[];
extern ObjectRsc *RscData[];


//�C�x���g�̔ԍ�
enum EventNum {
    EventNone,      //=0
    EventTimeUpdate,    //1�b���Ƃɔ���  �����̍X�V����������
    EventSlideSWoff,
    EventSlideSWon,
    EventAlarm,   //�����݂ŉ��炷�̂ŁA�C�x���g�Ƃ��Ă͕s�v�����[�^���[�񂵂ăA���[�������ύX�������ɔ���
    EventTouchDown, //�ŏ��Ƀ^�b�`�ƔF���������ɔ�������C�x���g
    EventTouchUp,   //�^�b�`����߂����ɔ�������C�x���g�@Long���ǂ����ŏ����ύX����Ȃ炱�����H
    EventTouchLong,
    EventTouchRepeat,   //�������ςȂ��̎�
    EventRotary,
    EventFormInit,
    EventFormUpdate,
    EventGetTime,
    EventDateUpdate,    //���t�ύX���@    ���t�̕\���X�V�K�v�Ȏ��B�J�����_�[�̕\����
//    EventAlarmUpdate,   //�A���[�������ύX��
    EventTemperatureUpdate, //���x�̍X�V��������
    EventHumidityUpdate,    //���x�̍X�V��������
    EventCalendarUpate,     //�J�����_�[�@�K�v���邩�H
    EventColorFormInit,
    EventColorFormUpdate,
    EventXYFormInit,
    
    EventFormInitSave,  //�J�X�^���f�[�^�̕ۑ��A�ǂݏo���p
    EventFormInitLoad,
    EventFormClose,
    
};

//RTC�����p
//Sec, Min, Hour, Wday, Day, Month, Year��z��
//�l�́ABCD10�i�Ƃ��邱�Ƃɒ��ӁB�擪�́ASec�̃��W�X�^�A�h���X
//�����l:2021/1/1 00:00:00
// ���т́ARTC�ƍ��킹�āA�b�A���A���A�j���A���A���A�N
extern uint8_t DateTime[7]; // = {0x00, 0x00, 0x00, 0x00, 0x01, 0x01, 0x21};
extern uint8_t AlarmTime[3]; // = {0, 0, 0x7f};    //mm, hh, wday

extern const char TouchMsg[];
//SD�J�[�h����f�[�^��ǂݏo�����Ɏg���o�b�t�@
#define DATASIZE 320    //LCD��320�h�b�g�����C�����@40�o�C�g�̔{�����]�܂���
extern uint8_t SDcardBuffer[DATASIZE];    //���[�J���ł͓��肫��Ȃ��Ă��AData space�ɂ͂܂�������

extern int8_t Accel;   //������]���������ɁA1�N���b�N��Accel�{�ɂ�����
extern int16_t TouchX, TouchY;    //�f�B�v���C���W�Ɋ��Z�����^�b�`���W
extern uint8_t TouchCount;     //����������p�Ɏg�p�B�^�b�`�����0�ɂȂ�
extern uint8_t TouchStart;
extern uint16_t PixelData[9];   //3x3�̃s�N�Z���f�[�^��ۑ�����
extern int16_t PixelXY[2];
extern int8_t RotCount; //�񂵂���(�N���b�N����̃��[�^���[���ƁA����N���b�N��������)
//SlideSW�̏�ԕω��Ɏg��
#define SlideSWoff      0
#define SlideSWoff2on   1
#define SlideSWon       2
#define SlideSWon2off   3

extern uint8_t RepeatKey;



void remount(void);



#ifdef	__cplusplus
}
#endif

#endif	/* MAIN_H */
