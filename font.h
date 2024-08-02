/* 
 * File:   font.h
 * Author: K.Ohno
 *
 * Created on December 19, 2020, 5:19 PM
 */

#ifndef FONT_H
#define	FONT_H

#ifdef	__cplusplus
extern "C" {
#endif

    
#include "mcc_generated_files/mcc.h"

/*
 * �t�H���g�f�[�^�\��: �擪8�o�C�g�ɏ��
 * �\�����鎞�A�傫���A�s�b�`�A�F�͎w��ł���悤�ɂ�����A�\���Ƃ��ď\����
 *  �f�t�H���g�Ŏ��t�H���g�f�[�^�́ANormalFontData��SmallFontData��2��
 *  �����t�H���g�́ASD�J�[�h�g����ꍇ�ɑΉ�
 *  ����ɃJ�X�^���t�H���g�Ƃ����`�ŁA���[�U��SD�J�[�h�Ɋi�[�����t�@�C���f�[�^���g����
 * 
 * LCD320x240color.c�Œ�`����Ă���ϐ�
 * Cursor_x
 * Cursor_y
 * TextMagX, TextMagY
 * Wrap
 * 
 */
    
//�m�[�}���t�H���g
// 1�o�C�g�L�����N�^�f�[�^�@8x8�h�b�g
// ASCII�R�[�h�F$20-$7F�܂Œ�`�A�j���A���AF�A������ǉ�
// Font.xlsx�ɂĊG�̃f�[�^����A�\�[�X�R�[�h�ɓ\��t������R�[�h�𐶐����Ă���
// �ŐV�t�H���g�́AFont.xlsx�̐V�t�H���g�̃V�[�g
/*
 * �t�H���g�f�[�^:������8�h�b�g�P�ʂɁA����MSB�A�E��LSB�ŁA1�o�C�g�̃f�[�^
 * �@�@�@�@�@�@�@�c�����́A�z��̗v�f�Ƃ��Ď����Ă���
 * �@���̐������Ƃ킩��ɂ������A�Ⴆ�΁AK�̃t�H���g�f�[�^���ƁA
 * 0xC6, 0xCC, 0xD8, 0xF0, 0xD8, 0xCC, 0xC6, 0x00, �Ƃ���8�o�C�g�ɂȂ��Ă��āA
 * �ȉ��̂悤�ȊG�ƂȂ�B
 * ����������������
 * ����������������
 * ����������������
 * ����������������
 * ����������������
 * ����������������
 * ����������������
 * ����������������
 * 
 * ���̕����ł́AY�����̃f�[�^�͏_��Ɏ��Ă邪�A�v���|�[�V���i���t�H���g�̑Ή��͂ł��Ȃ�
 * X�����̃f�[�^��1�o�C�g�Ƃ��Ē�`���Ă���̂ŁA8�h�b�gmax�ŁA�Œ蕝�ƂȂ�
 * 
 */
// u�́Aunsign���Ӗ�����
#define FSunday 0x16u    //�t�H���g�f�[�^�N�_�̕����R�[�h ASCII�łȂ��̂�
#define FdegC   0x1Du
#define FdegF   0x1Eu
#define Ffbox   0x1Fu    //Filled Box
#define Fbox    0x7Fu    //Box
#define Fbell   0x60u   //�A���[���A�C�R��
#define Fgear   0xFF    //Gear
    
//�z����̈ʒu
enum FontData {
    Fxsize,     // 0  //X�����̃h�b�g��
    Fysize,     // 1  //Y�����̃h�b�g��
    Fxpitch,    // 2  //X�����̃s�b�`
    Fypitch,    // 3  //Y�����̃s�b�`
    Foffset,    // 4  //�f�[�^�擪�̕����R�[�h(�N�_�ƂȂ�)
    Fbyte,      // 5  //1�����̃f�[�^�T�C�Y
    
    Fstart = 8,//�t�H���g�f�[�^�̔z����̊J�n�ʒu�A���Ԃ��ƁA8�o�C�g���̃w�b�_������Ƃ�������
};
    
//�t�H���g�̏��������₷�����邽�߂ɁA�t�H���g�f�[�^�̍\�����`���Ă���
//�t�H���g�T�C�Y�A�s�b�`�A�ŏ��̕����R�[�h�́A�t�H���g�f�[�^�擪�ɓ����Ă���
typedef struct {
    uint8_t fontcode;   //�t�H���g�ԍ�
    uint8_t xsize;      //�t�H���g��X�T�C�Y
    uint8_t ysize;      //�t�H���g��Y�T�C�Y
    uint8_t xpitch;     //�t�H���g��X�s�b�`
    uint8_t ypitch;     //�t�H���g��Y�s�b�`
//    uint8_t kratio;     //���p�t�H���g�Ɣ�r�����T�C�Y��@8x8�h�b�g�ɑ΂�������16x16��������A0x12
    uint8_t *data;    //�t�H���g�f�[�^�z��ւ̃|�C���^
    uint8_t MagX, MagY; //�t�H���g�̔{��
    int8_t XpitchAdj, YpitchAdj;    //�s�b�`�̔�����
    char *fontName;
} FontStruct;


// �����t�H���g���A�t�@�C���̒�`
enum KanjiFontCode {
    Misaki = 1,     //8x8�h�b�g�t�H���g
    SJIS16,         //16x16�h�b�g�t�H���g
    SJIS24,         //24x24�h�b�g�t�H���g
};

// �t�H���g�̃t�@�C�����̒�`�B����̓v���O�����ŌŒ�
extern char KanjiFontFile[][13];

//���݂̃t�H���g
extern FontStruct CurrentFont;

extern uint8_t *InternalFontList[];

//�J�X�^���t�H���g�́A�w�b�_��񂾂����B�擪��4�����L��
//Fxsize, Fysize, Fxpitch, Fypitch, Foffset


#ifdef	__cplusplus
}
#endif

#endif	/* FONT_H */
