/*
 * File:   RTC8025.c
 * Author: K.Ohno
 *
 * Real Time Clock (RTC) Module
 * EPSON RX-8025NB I2C-Bus interface
 *         ______
 * FOE   1|i     |8 VDD = 1.15V - 5.5V
 * F0UT  2|o    o|7 /INTB (Need PU)
 * /INTA 3|o    i|6 SCL + PU=2.2k
 * GND   4|    io|5 SDA + PU=2.2k
 *         ------
 * 
 * I2C: 400 kHz����\�����A�Z���T�̃X�s�[�h�ɍ��킹100kHz�Ŏg�p�BDHT20�Ȃ�400kHz��
 * I2C�M���́A����RTC�{�[�h���pullup
 * /INTA�́A�{�[�h���Pullup+LED�@�@���x�����[�h��1�b���Ƃ�Low�ƂȂ郂�[�h(��������荞�ݏo��)
 * �@�{�[�h��ŐԐFLED��ڑ����A1�b���Ƃɓ_�Ń��[�h�ɐݒ�A1�b�X�V��PIC�ɒm�点��
 * /INTB�́A�I�[�v���h���C���o�́@�@�O�t����pull up�K�v�Ȃ̂ŁAWPU�w��
 * �@�@�@�@�@Alarm_W (/INTB)�@�A���[���p���荞��
 * FOE=Open(=L)�ŁAFOUT=L(�Œ�)�o�͂ƂȂ邪�A���g�p
 */
#include "RTC8025.h"
#include "mcc_generated_files/examples/i2c2_master_example.h"
#include <time.h>

#define RX8025  0x32    //RTC8025��I2C�A�h���X�@R/W�r�b�g��������7bit�A�h���X

//RTC�̃��W�X�^�ݒ�p��`
//Reg-E��field
uint8_t RTC_regE;
#define RTC_WALE 0x80   //1:Weekly Alarm�̗L����
#define RTC_DALE 0x40   //1:Weekly Alarm�̗L����
#define Time24   0x20   //1:24���Ԑ�
#define BCLEN2   0x10   //1=/CLEN2
uint8_t CT2_0 = 0x04; //1Hz�p���X���[�h�A/INTA=L

//Reg-F��field
//#define regF_default = 0b01011111; //RegF�ւ̏������݂̃f�t�H���g�l
uint8_t RTC_regF = 0b01011111; //RegF�ւ̏������݂̃f�t�H���g�l
#define RTC_VDSL 0x80u   //0:2.1V�A1:1.3V�ɓd���ቺ���o���x���ݒ�
#define RTC_VDET 0x40u   //1:�d���ቺ�̌��o����
#define RTC_XST  0x20u   //0:���M��~�̌��o����
#define RTC_PON  0x10u   //1:�p���[�I�����Z�b�g�����̌��o����
#define BCLEN1   0x08u   //1=/CLEN1
#define RTC_CTFG 0x04u   //INTA���荞�݂����x�����[�h�̎��A0 write��/INTA=Off�Ƃ���
#define RTC_WAFG 0x02u   //WALE=1�̎��A�A���[��W������=1�A0write��/INTB=Off
#define RTC_DAFG 0x01u   //DALE=1�̎��A�A���[��D������=1


/*
 * x��BCD��2���ɕϊ�
 */
uint8_t Hex2Bcd(uint8_t x) {
    return (uint8_t)(((x / 10) << 4) + (x % 10));
}

uint8_t Bcd2Hex(uint8_t x) {
    return (uint8_t)((x >> 4)*10 + (x & 0xf));
}


/*
 * RTC�̃A���[������
 * WAFG���N���A
 */
void RTC_resetAlarm(void) {

    RTC_INTB_SetLow();
    //reg-E��WALE�́A��x0�ɂ��āA������
    I2C2_Write1ByteRegister(RX8025, 0xe0, RTC_regE & ~RTC_WALE); //E:bit7=WALE
    __delay_us(80); //Stop���玟��Start�܂ŁA62us�ȏ�

    RTC_regF = RTC_regF & ~RTC_WAFG; //F:bit1=WAFG=0
    I2C2_Write1ByteRegister(RX8025, 0xf0, RTC_regF);
    __delay_us(80); //����Start�܂ŁA62us�ȏ�

}


/*
 * �A���[�����Ԃ�ݒ�
 * �g�p�ϐ��Fhh(BCD10�i), mm(BCD10�i), wday (�j���w�肵�Ȃ����0x7f)
 * �A�i���O�\���̎��́AAM/PM����ʂ������A�����������ԋ߂������ŃA���[���ݒ�
 */
void RTC_setAlarmTime(uint8_t *alarmtime) {

    //Alarm_W��WALE����U���Z�b�g
    //reg-E��WALE�́A��x0�ɂ��āA�ēx�L�������Ȃ��Ƃ����Ȃ�
    I2C2_Write1ByteRegister(RX8025, 0xe0, RTC_regE & ~RTC_WALE); //E:bit7=WALE
    __delay_us(80); //����Start�܂ŁA62us�ȏ�

    I2C2_Write1ByteRegister(RX8025, 0x80, alarmtime[0]);
    __delay_us(80); //Stop���玟��Start�܂ŁA62us�ȏ�
    I2C2_Write1ByteRegister(RX8025, 0x90, alarmtime[1]);
    __delay_us(80); //Stop���玟��Start�܂ŁA62us�ȏ�
    I2C2_Write1ByteRegister(RX8025, 0xa0, alarmtime[2]);
    __delay_us(80); //Stop���玟��Start�܂ŁA62us�ȏ�

    //Alarm_W�ݒ�
    //reg-E��WALE���A�ēx�L����
    RTC_regE = RTC_regE | RTC_WALE;
    I2C2_Write1ByteRegister(RX8025, 0xe0, RTC_regE); //E:bit7=WALE
    __delay_us(80); //Stop���玟��Start�܂ŁA62us�ȏ�
    RTC_regF = RTC_regF & ~RTC_WAFG;
    I2C2_Write1ByteRegister(RX8025, 0xf0, RTC_regF); //E:bit7=WALE
    __delay_us(80); //Stop���玟��Start�܂ŁA62us�ȏ�
    
    //Alarm�����ݐݒ�
    //interrupt on change for group IOCB - flag
    RTC_INTB_SetHigh();
}

/*
 * ���P�ʂŁA������dmm��(+/-)����
 * ������dmm��傫�Ȑ��l�ɂł���悤�ɁA16bit�ϐ��ɂ���
 */
void IncDecTime(int16_t dmm, uint8_t *hh, uint8_t *mm) {
    int16_t min, hour;
    
    min = (int16_t)*mm;
    hour = (int16_t)*hh;
    min = min + dmm;
    
    if (dmm >= 0) {
        //�C���N����
        while (min > 59) {
            min = min - 60;
            hour = hour + 1;
        }
    } else {
        //�f�N����
        while (min < 0) {
            min = min + 60;
            hour = hour -1;
        }
    }
    if (hour > 23) hour = hour - 24;
    else if (hour < 0) hour = 24 + hour;
    *hh = (uint8_t)hour;
    *mm = (uint8_t)min;
    
}


/*
 * RTC�ɐV���Ȏ�����ݒ�
 */
void RTC_setTime(uint8_t *datetime) {

    I2C2_Write1ByteRegister(RX8025, 0x00, datetime[0]); //�b
    __delay_us(80); //Stop���玟��Start�܂ŁA62us�ȏ�
    I2C2_Write1ByteRegister(RX8025, 0x10, datetime[1]);
    __delay_us(80); //Stop���玟��Start�܂ŁA62us�ȏ�
    I2C2_Write1ByteRegister(RX8025, 0x20, datetime[2]);
    __delay_us(80); //Stop���玟��Start�܂ŁA62us�ȏ�
}

/*
 * RTC���t�ݒ�
 */
void RTC_setDate(uint8_t *datetime) {
    I2C2_Write1ByteRegister(RX8025, 0x30, datetime[3]);
    __delay_us(80); //Stop���玟��Start�܂ŁA62us�ȏ�
    I2C2_Write1ByteRegister(RX8025, 0x40, datetime[4]);
    __delay_us(80); //Stop���玟��Start�܂ŁA62us�ȏ�
    I2C2_Write1ByteRegister(RX8025, 0x50, datetime[5]);
    __delay_us(80); //Stop���玟��Start�܂ŁA62us�ȏ�
    I2C2_Write1ByteRegister(RX8025, 0x60, datetime[6]);
    __delay_us(80); //Stop���玟��Start�܂ŁA62us�ȏ�
}


/*
 * ���t�̃G���[�`�F�b�N St[5:6]:���ASt[8:9]�F��
 * ����1-31�ȊO�̎��́A01�Ƃ���
 * ����1-12�ȊO�̎��́A12�̏�]���Ƃ�@13=1��
 */
void DateErrorCheck(uint8_t *dateTime) {
    uint8_t yy, mm, dd;

    yy = Bcd2Hex(dateTime[6]);
    mm = Bcd2Hex(dateTime[5]);
    dd = Bcd2Hex(dateTime[4]);

    dateTime[3] = getWeekdays(&yy, &mm, &dd);
    
    dateTime[6] = Hex2Bcd(yy);
    dateTime[5] = Hex2Bcd(mm); //�K�؂Ȍ����ɁB4/31��5/1�Ƃ�
    dateTime[4] = Hex2Bcd(dd);
}


/*
 * ���t�̃`�F�b�N�Ɨj���̌v�Z
 * �j��(0:��-6:�y)��Ԃ�
 * year: 2000�N����ɂ���0-99�N�@�@0=2000�N�A99=2099�N
 * month: 1-12
 * day: 1-31
 * �������ȓ��t���C�����Ă����B��F4/31�@���@5/1��
 */
uint8_t getWeekdays(uint8_t *year, uint8_t *month, uint8_t *day) {
    struct tm t;
    struct tm *t2;
    time_t tim;

    t.tm_year = *year + 2000 - 1900;
    t.tm_mon = *month - 1; //�v���ӁF1��=0
    t.tm_mday = *day;
    t.tm_wday = 5; //�����l�͉��ł��悢
    t.tm_hour = 1; //�K���Ȏ��ԁ@01:00:00
    t.tm_min = 0;
    t.tm_sec = 0;

    tim = mktime(&t);
    t2 = localtime(&tim);

    *year = (uint8_t)(t2->tm_year - 100);  //1900�N���2000�N���
    *month = (uint8_t)(t2->tm_mon) + 1; //�K�؂Ȍ����ɁB4/31��5/1�Ƃ�
    *day = (uint8_t)t2->tm_mday;

    return (uint8_t)t2->tm_wday; //�j����Ԃ�
}


/*
 * RTC����N�����A�j���A�����b��datetime�ɓǂݏo��
 * 1�b���Ƃ̊����ݔ�����A������Ăяo���̂ŁAdelay�͕s�v�Ƃ���
 */
void RTC_read(uint8_t *datetime) {

    __delay_us(80); //Stop���玟��Start�܂ŁA62us�ȏ�
    I2C2_ReadDataBlock(RX8025, 0x00, datetime, 7);
}


/*
 * RegF��CTFG�����Z�b�g���āA/INTA=Hi-z(�v���A�b�v����Ă���̂�H)�ɂ���
 * ��������荞�݂��g���ꍇ�A���荞�݂����邲�ƂɁACTFG�����Z�b�g���Ȃ��Ƃ����Ȃ�
 */
void resetCTFG(void) {
    
    //Reg-F CTFG���Z�b�g
    RTC_regF = RTC_regF & ~RTC_CTFG; //F: CTFG=0�Ƃ��A����ȊO�̓��Z�b�g���Ȃ�
    I2C2_Write1ByteRegister(RX8025, 0xf0, RTC_regF);
    __delay_us(80); //Stop���玟��Start�܂ŁA62us�ȏ�
}

//void readReg(void) {
//    //�d����������Ȃ�A0x00
//    RTC_regE = I2C2_Read1ByteRegister(RX8025, 0xe0);
//    //�d����������Ȃ�A0x10
//    RTC_regF = I2C2_Read1ByteRegister(RX8025, 0xf0);
//
//}

/*****************************
 * RTC�̏����ݒ�
 * RTC�̓d�������ɓ����Ă���ꍇ�A�������͂�����g��
 * RTC_regE��RTC_regF��������(�K�v�ȏꍇ)
 * �d����x�؂�Ă��A���t���͊i�[����Ă�����̂Őݒ�
 * �d���T���v�����O�́A1�b�Ԃ�7.8ms�����Ƃ������ƂŁA�u��̌��o�ł��Ȃ��P�[�X����
 *****************************/
void init_RTC(uint8_t *datetime) {
    
    __delay_us(80); //Stop���玟��Start�܂ŁA62us�ȏ�

    //����RTC�����삵�Ă���(�d�������Ă���)���m�F�̂��߁A�����ŏ�Ԏ擾
    //�܂��AReg-E��F���m�F
    //�d����������Ȃ�A0x00
    RTC_regE = I2C2_Read1ByteRegister(RX8025, 0xe0);
    //�d����������Ȃ�A0x10
    RTC_regF = I2C2_Read1ByteRegister(RX8025, 0xf0);

    //bit6-4:VDET=0, /XST=1, PON=0 �̎��́ARTC�͓d�����������܂ܐ��퓮�삵�Ă��āA
    //PIC���ċN�����������Ȃ̂ŁARTC�́ACTFG�̂ݏ��������u�ጟ�o�ł��Ȃ����Ƃ�z�肵��
    //�v���O�����ύX
    //RTC_VDET 0x40   //1:�d���ቺ�̌��o����
    //RTC_XST  0x20   //0:���M��~�̌��o����@���ꂾ���ɐ��t
    //RTC_PON  0x10   //1:�p���[�I�����Z�b�g�����̌��o����
    if ((RTC_regF & (RTC_VDET | RTC_XST | RTC_PON)) == RTC_XST) {
        //�������s�v�ȃP�[�X
        RTC_read(datetime);
        //CTFG���������Z�b�g�F���x�����[�h�̎��A������CTFG�̃��Z�b�g���Ă����Ȃ��ƁA
        //����CTFG���ݒ肳��Ă��܂��Ă���ꍇ�A���삵�Ȃ��Ȃ�
        resetCTFG();
//        return;
    }

    //��L�ȊO�̎��́ARTC�̏��������K�v
    __delay_us(80); //Stop���玟��Start�܂ŁA62us�ȏ�
    //VDSL VDET /XST PON /CLEN1 CTFG WAFG DAFG
    //XST���Z�b�g���āA����̔��M��~�����o�ł���悤�ɂ���
    RTC_regF = RTC_XST | BCLEN1;
    I2C2_Write1ByteRegister(RX8025, 0xf0, RTC_regF);
    __delay_us(80); //Stop���玟��Start�܂ŁA62us�ȏ�

    //bit7 bit6  bit5   bit4  bit3  bit2-0
    //WALE DALE /12,24 /CLEN2 TEST CT2 CT1 CT0
    //E:bit7=WALE, bit5=24���Ԑ�, CT=100=���x�����[�h1s
    RTC_regE = Time24 | BCLEN2 | CT2_0;  //�R�[���h�d��������̏ꍇ�́A�����ŏ�����
    I2C2_Write1ByteRegister(RX8025, 0xe0, RTC_regE);
    __delay_us(80); //Stop���玟��Start�܂ŁA62us�ȏ�

    //���t���`�F�b�N���āA�j����ݒ�
    DateErrorCheck(datetime);

    //������RTC�ɐݒ�
    RTC_setDate(datetime);
//    datetime[0] = 0x00; //�b��00�Ƃ���
    RTC_setTime(datetime);

    I2C2_Write1ByteRegister(RX8025, 0x70, 0x00); //Digital Offset:�g�p���Ȃ��ꍇ0���Z�b�g

    I2C2_Write1ByteRegister(RX8025, 0x80, 0x00); //Alarm_W ; Minute:�g�p���Ȃ��ꍇ0���Z�b�g
    I2C2_Write1ByteRegister(RX8025, 0x90, 0x00); //Alarm_W ; Hour:�g�p���Ȃ��ꍇ0���Z�b�g
    I2C2_Write1ByteRegister(RX8025, 0xa0, 0x00); //Alarm_W ; Weekday:�g�p���Ȃ��ꍇ0���Z�b�g

    I2C2_Write1ByteRegister(RX8025, 0xb0, 0x00); //Alarm_D ; Minute:�g�p���Ȃ��ꍇ0���Z�b�g
    I2C2_Write1ByteRegister(RX8025, 0xc0, 0x00); //Alarm_D ; Hour:�g�p���Ȃ��ꍇ0���Z�b�g

    __delay_us(80); //Stop���玟��Start�܂ŁA62us�ȏ�
    
}
