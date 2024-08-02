/*
 * File:   Draw.c
 * Author: K.Ohno
 *
 * �`��֌W���[�`��
 */

#include "debug.h"
#include "Draw.h"
#include <string.h>
#include "main.h"
#include "RTC8025.h"
#include "LCD320x240color.h"
//#include "Customizing.h"
//#include "font.h"


// ��Βl���擾����֐��}�N����`
#define ABS(x) ((x) < 0 ? -(x) : (x))


const char WeekDays[][4] = {
    "Sun", "Mon", "Tue", "Wed", "Thu", "Fri", "Sat",
};

//�����\���́A�ω��̂������������ɂ���悤�ɂ��遨�A�i���O���v�݂̂Ŏg�p
uint8_t preTime[3]; //������ێ����Ă����A�ω����`�F�b�N�ł���悤�ɂ���

int16_t CalendarXsize, CalendarYsize;

//int16_t CustomXsize; //�J�X�^�}�C�Y�̎��A�̈���w�肷�邽�߂Ɏg���@�J�����_�[�݂̂Ɏg�p
//int16_t CustomYsize; //�f�W�^���̎����\���ɂ��g�p

char Smaller[2] = "";  //�����\���ŁAbmp�t�H���g�w�莞�A�������t�H���g���w��: "s"�A����ȊO�̎��A""



/*
 * 16bit RGB=565�J���[��bmp�t�@�C����ǂݍ��ݕ\������
 * 
 * 0x0000�@(2)	bfType		�t�@�C���^�C�v�@�ʏ��'BM'
 * 0x0002�@(4)	bfSize		�t�@�C���T�C�Y (byte)
 * 0x0006�@(2)	bfReserved1	�\��̈�@��� 0
 * 0x0008�@(2)	bfReserved2	�\��̈�@��� 0
 * 0x000A�@(4)	bfOffBits	�t�@�C���擪����摜�f�[�^�܂ł̃I�t�Z�b�g (byte)
 * 0x000E�@(4)	bcSize		�w�b�_�T�C�Y
 * 0x0012�@(4)	bcWidth		�摜�̕� (�s�N�Z��)
 * 0x0016�@(4)	bcHeight	�摜�̍��� (�s�N�Z��) �����Ȃ�C�摜�f�[�^�͉�������
 *                                               �����Ȃ�C�摜�f�[�^�͏ォ�牺��
 * 0x001A�@(2)	bcPlanes	�v���[�����@��� 1
 * 0x001C�@(2)	bcBitCount	1��f������̃f�[�^�T�C�Y (bit)
 * 
 * �����ł́A16bit�J���[�ƌ��ߑł����Ă���
 * 
 * �G���[�������������́AFR_OK = 0�ȊO��Ԃ�
 */
//BMP�w�b�_
#define bfOffBits   0x0a        //�t�@�C���擪����摜�f�[�^�܂ł̃I�t�Z�b�g (byte)
#define bcWidth     0x12        //�s�N�Z���� (�T�C�Y4byte)
#define bcHeight    0x16        //�s�N�Z������ (�T�C�Y4byte)
#define bcBitCount  0x1C        //1��f������̃f�[�^�T�C�Y (bit)
#define BUFFERSIZE  80
FRESULT ReadBmp16(char *bmpfile) {
    uint8_t buffer[BUFFERSIZE];
    int16_t x, y, xx;
    int16_t kk;
    UINT actualLength, nn;
    FRESULT res;
    uint16_t num;
    FIL FileObject;
    
    remount();
    f_chdir("/");
    //�t�@�C������8�����܂ŃT�|�[�g
    res = f_open(&FileObject, bmpfile, FA_READ);
    if (res != FR_OK) return res;
    
    //�w�b�_�Ǎ�
    res = f_read(&FileObject, buffer, 32, &actualLength); //32�o�C�g���ǂݍ���
    if (res != FR_OK) return res;
    if (buffer[bcBitCount] == 16) {
        //16bit�J���[�̃f�[�^�̎��̂ݏ���
        x = buffer[bcWidth] + (buffer[bcWidth+1]<<8);   //Width
        y = buffer[bcHeight];                           //Height
        //�V�[�N���āA�f�[�^�̐擪�܂ňړ�
        f_lseek(&FileObject, buffer[bfOffBits]);
        
        //bmp�f�[�^�́A�\������鉺�̍s����n�܂�
        for (kk = 0; kk < y; kk++) {
            //BUFFERSIZE���l�������v���O����
            num = (uint16_t)x * 2;  //1�s�̃o�C�g��=x�h�b�g x 16bit(2�o�C�g)
            xx = 0;
            while (num > 0) {
                if (num >= sizeof(buffer)) nn = sizeof(buffer);
                else nn = num;
                num -= nn;

                res = f_read(&FileObject, buffer, nn, &actualLength);
                if (res != FR_OK) return res;
                glcd_array(xx, y-1-kk, actualLength/2, 1, buffer);
//                xx = xx + (int16_t)actualLength/2;
                xx = xx + (int16_t)sizeof(buffer)/2;
            }
        }
    }
    f_close(&FileObject);
    return res;
}


/*
 * ���m�N��bmp��ǂݍ��ݕ`��
 * X,Y�s�b�`�́Abmp��X,Y�T�C�Y�A�@�g��T�|�[�g���K�v���͂��Ƃōl����
 * ����: �`��ʒu�A�F�w��
 * 
 * �t�H���g��X�T�C�Y: �ő�256�h�b�g
 * ch = "FontDir/5.bmp"�@�̂悤�Ɏw��
 * 
 */
//void DrawBmp(char *ch) {
//    uint8_t buffer[32];     //32*8=256�h�b�g��
//    int16_t fontXsize, fontYsize;
//    uint16_t len;
//    int16_t ii, jj;
//    UINT actualLength, nn;
//    FRESULT res;
//    uint16_t num;
//    uint16_t pos;
//    int16_t xp, yp; //���W
//    uint8_t line;
//    FIL FileObject;
//    
//    //�t�@�C������8�����܂ŃT�|�[�g
//    do {
//        res = f_open(&FileObject, ch, FA_READ);
//        if (res != FR_OK) remount();
//    } while (res != FR_OK);
//    
//    //�w�b�_�Ǎ�
//    f_read(&FileObject, buffer, 32, &actualLength); //32�o�C�g���ǂݍ���
//    if (buffer[bcBitCount] == 1) {
//        //���m�N���f�[�^�̎��̂ݏ���
//        fontXsize = buffer[bcWidth] + (buffer[bcWidth+1]<<8);   //Width
//        fontYsize = buffer[bcHeight];                           //Height
//        //�V�[�N���āA�f�[�^�̐擪�܂ňړ�
//        f_lseek(&FileObject, buffer[bfOffBits]);
//        
//        //��x�ɓǂݍ��ރf�[�^�T�C�Y(�o�C�g��)
//        len = (uint16_t)((fontXsize + 31)/32) *4;        //1�h�b�g�s���̃o�C�g���A4�o�C�g�P��
//        //��FfontXsize=40�h�b�g�̎��A�f�[�^��5�o�C�g�A3�o�C�gpadding���āA8�o�C�g/�s
//        //    �@�@      41�h�b�g�̎��A�f�[�^��6�o�C�g�A2�o�C�gpadding
//
//        //1�h�b�g�s���ǂݏo���O��
//        
//        //bmp�̃f�[�^�́A���������ł���ŁA�\��������͉̂�����Ƃ����̂ɒ���
//        //��ʂ��͂ݏo���Ă�����ɕ`��ł���悤�ɏ����ǉ�
//        //��ʓ��ɓ���悤�ɒ����B�`�悷�鎞�ɁA�͂ݏo���Ă��Ȃ����`�F�b�N�v
//        for (jj = 0; jj < fontYsize; jj++) {
//            yp = Cursor_y + (fontYsize - jj - 1);   //
//
//            f_read(&FileObject, buffer, len, &actualLength);
//            
//            pos = 0;
//            //addset���āA����x,y���W�w�肵�Ȃ����������`��
//            addset(Cursor_x, yp, Cursor_x + fontXsize -1, yp + 1);
//            for (ii = 0; ii < fontXsize; ii++) {
//                if ((ii % 8) == 0) line = buffer[pos++]; //8�h�b�g���̃f�[�^����荞��
//                xp = Cursor_x + ii;
//                if (line & 0x80) {
//                    // 1=�w�i�̕`��
//                    if (TextbgColor != TextColor) {
//                        // Back�J���[��front�J���[�ƈႤ���́A���̐F��w�i�Ƃ��ēh��
//                        draw_pixel(TextbgColor);
//                        //�w�i�F��h��Ȃ��ꍇ�A�A�h���X���i�܂Ȃ���肪����
//                    } else {
//                        //�`�悵�Ȃ��A�h���X���΂��čĐݒ�
//                        addset(xp+1, yp, Cursor_x + fontXsize -1, yp + 1);
//                    }
//                } else {
//                    // 0=���̕`��@�O�i
//                    draw_pixel(TextColor);
//                }
//                line <<= 1;
//            }
//        }
//    }
//    f_close(&FileObject);
//    
//    Cursor_x += fontXsize;
////    if (Wrap && (Cursor_x + fontXsize) > LCD_WIDTH) {
////        Cursor_x = 0;
////        Cursor_y += fontYsize;
////        if (Cursor_y > LCD_HEIGHT) Cursor_y = LCD_HEIGHT;
////    }
//
//}


/*
 * �t�H���g�t�@�C�����邩��0.bmp�̗L���Ŕ��f
 * �t�@�C������΁A�w�b�_����ǂݎ��A�O���[�o���ϐ���SDcardBuffer�Ɋi�[
 *  0.bmp�f�[�^�����邱�Ƃ��O��B�Ȃ����́A�J�X�^���t�H���g�Ȃ��Ɣ��f���AFontNormal�ŕ`��
 */
uint8_t checkFontFile(char *fontFolder) {
    char filename[30];
    FRESULT res;
    UINT actualLength;
    uint8_t xr, yr;
    FIL file;       //Opening��f�[�^�������ݗp
    
    //0.bmp������O��ŁA���̑傫�����擾����
    sprintf(filename, "/%s/0.bmp", fontFolder);
    if (f_open(&file, filename, FA_READ) != FR_OK) {
        return 1;        //�G���[�̎�
    }
    //�t�@�C�����鎞�́A�w�b�_���ǂݎ��
    res = f_read(&file, SDcardBuffer, 32, &actualLength); //32�o�C�g���ǂݍ���
    f_close(&file);

    CurrentFont.xsize = SDcardBuffer[bcWidth];     //255�h�b�g1�ȓ��Ƒz��
    CurrentFont.ysize = SDcardBuffer[bcHeight];    //255�h�b�g1�ȓ��Ƒz��
    CurrentFont.xpitch = CurrentFont.xsize;     //X�����̓v���|�[�V���i���Ȃ̂ł��̓s�x
    CurrentFont.ypitch = CurrentFont.ysize;
    
    return 0;   //�t�@�C�����������A����I��
}


/*
 * FontCode�Œ�`����Ă���t�H���g�R�[�h���w��
 * �J�X�^���t�H���g�̏ꍇ�́A�������炳��Ƀt�H���g�t�@�C�������ǂ�K�v������
 * 
 * �����SetKanjiFont�́A���p����B
 * ����������\�����s�����́A�����t�H���g�Ƃ̔{�����v�Z����s����ASetKanjiFont���Ō�Ɏ��{
 * �J�X�^���t�H���g�g�p���́A�������g��Ȃ����Ƃ�O��
 * �@�@�J�X�^���t�H���g�́A�ꕔ����������`����Ă��Ȃ����Ƃ�O��ɁAFontNormal�ŕ⊮������
 */
uint8_t SetFont(uint8_t fontcode, char *fontname) {
   
    //fontcode==0�̎��̃G���[������ǉ���NormalFont�Ƃ���̂��ǂ�����
    
    CurrentFont.fontcode = fontcode;
    CurrentFont.MagX = 1;
    CurrentFont.MagY = 1;
    CurrentFont.XpitchAdj = 0;
    CurrentFont.YpitchAdj = 0;

    if (fontcode >= EXTFONTCODE) {
        //���̑��̃J�X�^���t�H���g�̏ꍇ
        CurrentFont.data = NULL;    //�����t�H���g�łȂ����Ƃ�����
        CurrentFont.fontName = fontname;
        //�t�H���g�t�@�C���̃`�F�b�N�y�сAsize, pitch, mag���擾
        if (checkFontFile(fontname) == 0) return 0;

        //�t�H���g�t�@�C�����Ȃ����Ȃǂ́A�m�[�}���t�H���g�ɖ߂��āA�ݒ�p��
        CurrentFont.fontcode = FontNormal;        
    }
    //�J�X�^���t�H���g���G���[�����������́A�ȉ��̏��������{
    
    //�����t�H���g�̏ꍇ
    CurrentFont.data = InternalFontList[fontcode];
    CurrentFont.xsize = CurrentFont.data[Fxsize];
    CurrentFont.ysize = CurrentFont.data[Fysize];
    CurrentFont.xpitch = CurrentFont.data[Fxpitch];
    CurrentFont.ypitch = CurrentFont.data[Fypitch];
//        CurrentFont.kratio = 1;    //�{���́A�J�X�^���t�H���g�ݒ莞�ɕύX�����        

        return 0;
}


/*
 * �t�H���g�����߂��Ƃ��ɁA�w�b�_��񂩂�A��{�����擾
 * �I�t�Z�b�g�f�[�^���擾���Ă���
 * �t�@�C���̓I�[�v���̂܂܂ɂ��� FileFont����ăA�N�Z�X�\
 * 
 * fontname: 0:�����t�@�C��close������
 *    Misaki 1    //8x8�h�b�g�t�H���g
 *    SJIS16 2    //16x16�h�b�g�t�H���g
 *    SJIS24 3    //24x24�h�b�g�t�H���g
 * 
 * CurrentKanjiFont.fontcode�ŁA�����̗L�������𔻒f�Ɏg��
 */
uint8_t KanjiOffsetTable[256];  //�t�@�C�����̊����f�[�^�̂��肩�������e�[�u��

//void SetKanjiFont(uint8_t fontcode) {
//    char header[64];
//    uint16_t headerSize;
//    UINT actualLength;
//    uint8_t xr, yr;
//
//    //���ɓ����t�H���g�̃t�@�C���I�[�v�����Ă����牽�����Ȃ�
//    if (CurrentKanjiFont.fontcode == fontcode) return;
//    
//    if (CurrentKanjiFont.fontcode != 0) {
//        //���Ƀt�@�C�����J���Ă�����A��U����
//        f_close(&FileFont);
//        CurrentKanjiFont.fontcode = 0;  //��U�A�����𖳌�����Ԃɂ���
//        CurrentKanjiFont.kratio = 0x11;
//        if (fontcode == 0) {
//            // fontcode��0�̎��́A�t�@�C�����ďI��
//            return;
//        }
//    }
//
//    //SD�J�[�h�Ȃ����=�G���[�Ȃ�A�������Ȃ�
//    // �t�@�C�����J���A�f�[�^��ǂݏo��
//    //�t�@�C������8�����܂ŃT�|�[�g
//    if (f_open(&FileFont, KanjiFontFile[fontcode], FA_READ ) == FR_OK) {
//        //Open���A�w�b�_�Ǎ�
//        f_read(&FileFont, header, 64, &actualLength); //64�o�C�g���ǂݍ���
//        headerSize = header[2] + (uint16_t)header[3]*256; //�w�b�_�̃T�C�Y=64
//        CurrentKanjiFont.xsize = header[0x10]; //�t�H���g��X�����̑傫���B�h�b�g��
//        CurrentKanjiFont.ysize = header[0x11]; //�t�H���g��Y�����̑傫���B�h�b�g��
//        CurrentKanjiFont.xpitch = CurrentKanjiFont.xsize;   //�s�b�`�́A�t�H���g�T�C�Y�Ɠ����ɂ��Ă���
//        CurrentKanjiFont.ypitch = CurrentKanjiFont.ysize;
//        //��������Ƀm�[�}���t�H���g�̔{���ݒ� .kratio�͂��̃t�H���g�̔{���ł͂Ȃ����Ƃɒ���
//        //8x8�̎�x11�A16x16�̎�x12�A24x24�̎�x23�Ƃ����������ǂ�����
//        xr = (CurrentKanjiFont.xsize/2 + CurrentFont.xsize/2) / CurrentFont.xsize;
//        if (xr < 1) xr = 1;
//        yr = CurrentKanjiFont.ysize / CurrentFont.ysize;
//        if (yr < 1) yr = 1;
//        CurrentKanjiFont.kratio = (uint8_t)((xr<<4) + yr);
//        
//        CurrentKanjiFont.data = (uint8_t *)KanjiFontFile[fontcode];
//        CurrentKanjiFont.fontcode = fontcode;
//        f_read(&FileFont, KanjiOffsetTable, 256, &actualLength); //256�o�C�g���̃e�[�u���ǂݍ���
//        //���̃f�[�^�����Ƃɕ\�������̃f�[�^�̈ʒu�����΂₭�v�Z�����邽�߁A�ێ����Ă���
//    }
//}


/*
 * �J�X�^���t�H���g(bmp�`���̃f�[�^)�̕`��
 * 0x16-0x3A�܂ł̕����R�[�h�ɑΉ����邱�Ƃ�z��
 * 0x16: ��, ��, ��, ��, ��, ��, �y, ��, 'F, ��
 * 0x20-0x3A�܂ł́A�ʏ��ASCII
 * �������Abmp�t�@�C������ASCII1����������U��Ȃ�����������̂ł����Œ�`�������̂ɂ���K�v����
 * �T�|�[�g����f�[�^�́A���m�N��bmp�f�[�^�݂̂Ƃ���
 * 
 * �Ή����镶���́A�\���ɕK�v�Ȃ��̂Ɍ��肵�āA�쐬�J�͂�ߖ񂵂Ă悢�B
 * bmp�t�@�C���Ȃ����́AFontNormal�ŕ\��
 * ���t: 0-9 / () �j��
 * ����: 0-9 :    ������
 * ���x: 0-9 . ��
 * ���x: 0-9 %
 * �J�����_�[: 0-9 �j��
 * 
 * �������w��: Smaller���g���@�����\���ɂ����ĕb���������������ɂ���Ƃ��ɗ��p
 * 
 * �G���[�̎��A1
 * 
 */
uint8_t displayCustom_putc(uint8_t c) {
    uint8_t k, x, xf, yf, mm, len;
    uint8_t ii, jj;
    uint16_t pos;
    uint8_t fontXsize;
    uint8_t fontYsize;
    char filename[50], st[10];
    uint16_t datasize, remainingLine, linenum;
    uint8_t idx;
    UINT actualLength;
    uint8_t line;
    FRESULT res;
    uint8_t prev_size;
    uint8_t prevfontcode, prevmag;
    int16_t xp, yp; //���W
    int16_t px, py; //�{������荞��pitch

    //ASCII���тɁB�@FSunday=0x16����J�n
    const char *charname2[] = {
        "Sun", "Mon", "Tue", "Wed", "Thu", "Fri", "Sat",  "degC", "degF", "", 
        "space", "", "", "", "", "percent", "", "", 
        "(", ")", "", "plus", "", "-",  "dot", "slash", 
        "0", "1", "2", "3", "4", "5", "6", "7", "8", "9", "colon", "alarm", "Gear", "Bell",
    };
    
    //�ΏۊO�̕����͔r��
//    if ((c != Fgear) && (c != Fbell) && (c < FSunday || c > ':')) {
//        return 0;
//    }

    // �t�H���g�f�[�^�̃t�@�C�����J���āA�f�[�^��ǂݏo��
    // bmp�t�@�C���́A���̃v���|�[�V���i���t�H���g
    // bmp�t�@�C����ǂ�ŁAX, Y�T�C�Y�擾��A�f�[�^���擾���Ȃ���`��
    
    //�t�H���_���Ńt�H���g��ނ��w�肷��
    if ((c >= 'A') && (c <= 'Z')) sprintf(st, "%c", c);
    else if (c == Fgear) sprintf(st, "Gear");   //�M���̎���������
    else if (c == Fbell) sprintf(st, "Bell");   //�A���[���x���̎���������
    else if (c >= FSunday) strcpy(st, charname2[c - FSunday]);
    else return 1;
    
    sprintf(filename, "/%s/%s%s.bmp", CurrentFont.fontName, st, Smaller);
    res = f_open(&FileFont, filename, FA_READ);
    //3�̃G���[�̃P�[�X���z�肳���B�Ώۂ̕������Ȃ��A���������t�H���gdir���Ȃ��A���̑��̃G���[
    if (res == FR_NO_FILE) {
        //�t�H���_�͂����Ă��Ώۂ̕����̃t�@�C�����Ȃ����́A�I��
            return 0;
    } else if (res != FR_OK) {
        //Open�ł��Ȃ��P�[�X���������B�ǂ��������ɔ�������̂��s���B
        //Open�ł����ɏ��������Ă��āA�\���������̂ŁA������ǉ�
        //�G���[�͂����Ŕ������Ă�������m�F
        remount();      //�ă}�E���g
        DEBUG_PRINTF("Retry Open error: %s\r\n", filename);
        do {        
            res = f_open(&FileFont, filename, FA_READ);
        } while (res != FR_OK);   //������open�ł���܂Ń��[�v

//        if (f_open(&FileFont, filename, FA_READ) != FR_OK) return 10;   //�}�E���g�ł��Ȃ������ꍇ
    }
    
    res = f_read(&FileFont, SDcardBuffer, 32, &actualLength); //header=32�o�C�g���ǂݍ���
    if (res != FR_OK) {
        f_close(&FileFont);
        return 0; //�G���[�ɂ������^�[��
    }
    //CurrentFont�ɂ�0.bmp�̑傫�����ݒ肳��Ă��邪�A����`�F�b�N���ăv���|�[�V���i���Ή�
    fontXsize = SDcardBuffer[bcWidth]; //�t�H���g�̑傫���́A�c���Ƃ�255�܂�
    fontYsize = SDcardBuffer[bcHeight];
    
    //�V�[�N���āA�f�[�^�̐擪�܂ňړ�
    res = f_lseek(&FileFont, SDcardBuffer[bfOffBits]);

    if (res != FR_OK) {
        f_close(&FileFont);
        return 3;
    }

    len = ((fontXsize+31)/32)*4;        //1�h�b�g�s���̃o�C�g���A4�o�C�g�P��
    //��FfontXsize=40�h�b�g�̎��A�f�[�^��5�o�C�g�A3�o�C�gpadding���āA8�o�C�g/�s
    //    �@�@      41�h�b�g�̎��A�f�[�^��6�o�C�g�A2�o�C�gpadding
    
    linenum = (sizeof SDcardBuffer) / len;   //�o�b�t�@�T�C�Y�ɓ���͉̂��h�b�g�s����
    //�o�b�t�@��320�o�C�g����A2560�h�b�g��������̂ŁAlinenum==0�ɂȂ炸�A�Œ�ł�8�s���͈�x�ɓǂݍ��߂�
    if (linenum >= fontYsize) {
        //�o�b�t�@�ɓ��肫��ꍇ�́A�h�b�g�s��=Y�h�b�g�T�C�Y
        linenum = fontYsize;
    }
    datasize = len * linenum;   //��x�ɓǂݍ��ރf�[�^�T�C�Y(�o�C�g��)
    
    //bmp�̃f�[�^�́A���������ł���ŁA�\��������͉̂�����Ƃ����̂ɒ���
    //��ʂ��͂ݏo���Ă�����ɕ`��ł���悤�ɏ����ǉ�
    //��ʓ��ɓ���悤�ɒ����B�`�悷�鎞�ɁA�͂ݏo���Ă��Ȃ����`�F�b�N�v
    
    //������`�悳�������B�o�b�t�@�ɓ��肫��Ȃ�bmp���Ή�
    remainingLine = 0;   //�ǂݍ��ގc��s��
    for (jj = 0; jj < fontYsize; jj++) {
        yp = Cursor_y + (fontYsize - jj - 1) * CurrentFont.MagY;
        if (remainingLine == 0) {
            //�ǂݍ��񂾃f�[�^���g���؂�����A���̃f�[�^��ǂݍ���
            res = f_read(&FileFont, SDcardBuffer, datasize, &actualLength);    //�f�[�^��ǂݍ���
            if (res != FR_OK) {
                f_close(&FileFont);
                return 44;
            }
            remainingLine = actualLength / len;    //�ǂݍ��񂾃f�[�^�̍s��
            pos = 0;
        }
        //addset���āA����x,y���W�w�肵�Ȃ����������`��
        addset(Cursor_x, yp, Cursor_x + fontXsize * CurrentFont.MagX -1, yp + CurrentFont.MagY);
        for (ii = 0; ii < fontXsize; ii++) {
            if ((ii % 8) == 0) line = SDcardBuffer[pos++]; //8�h�b�g���Ƀf�[�^����荞��
            xp = Cursor_x + ii * CurrentFont.MagX;
            if (line & 0x80) {
                // 1�̕`�� �w�i
                if (TextbgColor != TextColor) {
                    // Back�J���[��front�J���[�ƈႤ���́A���̐F��w�i�Ƃ��ēh��
                    if (CurrentFont.MagX * CurrentFont.MagY == 1)
                        draw_pixel(TextbgColor);
                    else
                        display_fillRect(xp, yp, CurrentFont.MagX, CurrentFont.MagY, TextbgColor);
                    //�w�i�F��h��Ȃ��ꍇ�A�A�h���X���i�܂Ȃ���肪����
                } else {
                    //�`�悵�Ȃ��A�h���X���΂��čĐݒ�
                    addset(xp+CurrentFont.MagX, yp, Cursor_x + fontXsize * CurrentFont.MagX -1, yp + CurrentFont.MagY);
                }
            } else {
                // 0=���̕`��@�O�i
                if (CurrentFont.MagX * CurrentFont.MagY == 1) //�g�債�Ă��Ȃ���
                    draw_pixel(TextColor);
                else
                    display_fillRect(xp, yp, CurrentFont.MagX, CurrentFont.MagY, TextColor);
            }
            line <<= 1;
        }
        remainingLine--;
        pos = pos + 3-((pos-1) % 4); //4�o�C�g�P�ʂȂ̂ŁA�؂�グ��
    }    
    
    f_close(&FileFont);
    
    px = fontXsize * CurrentFont.MagX + CurrentFont.XpitchAdj; //XpitchAdj���t�H���g�̃T�C�Y�ɒǉ�
    Cursor_x += px;
    py = fontYsize * CurrentFont.MagY + CurrentFont.YpitchAdj;
    if (Wrap && (Cursor_x + px) > LCD_WIDTH) {
        Cursor_x = 0;
        Cursor_y += py;
        if (Cursor_y > LCD_HEIGHT) Cursor_y = LCD_HEIGHT;
    }
//    DEBUG_PRINTF("  x=%d (%d, %d, %d)\r\n", Cursor_x, fontXsize, CurrentFont.MagX, CurrentFont.XpitchAdj);

    return 0;
}


/*
 * �ݒ肳��Ă���J�[�\���ʒu�A�F�A�T�C�Y�����̂܂܎g�����͂�����Ăяo��
 * �r���ŃG���[��������A���~
 */
uint8_t displayCustom_puts(char *s) {
    uint8_t res;
    char str[100];
//    uint16_t preColor = Textcolor;
    
    while (*s) {
        res = displayCustom_putc(*s++);
       
//        if (res) {
//            //�����̍X�V������Ȃ���Ԃ̎��A�ǂ��ŃG���[�������Ă��邩�m�F���邽��
////            DEBUG_PRINTF("err=%d\r\n", res);
//            return 1;  //������ύX   
//        }
    }
//    Textcolor = preColor;
    return 0;
    
}


/*
 * �e�L�X�g�\������O����
 */
void presetting(ObjectRsc *rsc) {

    SetFont(rsc->fontcode, rsc->fontName);   //�t�H���g�̐ݒ�
    display_setTextSize(rsc->fontmag);
    display_setTextPitchAdj(rsc->xpitchAdj, 0);
    display_setCursor(rsc->x, rsc->y);        //�`��J�n���W
    display_setColor(rsc->color);
//    CustomYsize = CurrentFont.ypitch * TextMagY + YpitchAdj;
}


// DateTime�̏��������A�O�̃��[�h�̏�ԁA���ɃA�i���O�̐j�̕\������������Ȃ��悤��
void resetPreDateTime(void) {
    uint8_t jj;
    
    for (jj = 0; jj < 3; jj++) preTime[jj] = 0xff;
}

/*
 * ���\�[�X�f�[�^����ȉ����擾
 * (x,y)���W������̋N�_
 * �����̊O�`���2�h�b�g���傫�߂̗̈��`���Ƒz��
 * format: HHMMss�Ȃ�
 * fontmag: �t�H���g�T�C�Y
 *  �ʏ��7�Z�O���w�肳�ꂽ���AHHMMss��ss�́A������7�Z�O�t�H���g���g�p
 * �@�@�@�@�@�f�t�H���g�ŏ�����7�Z�O�w�肳�ꂽ���́A�����傫��
 * �T�|�[�g����t�H���g�́A7seg�ACustom0��Custom1��3��
 * 
 */
void dispTime(uint8_t *datetime, ObjectRsc *rsc) {
    char ss[10];
//    int16_t tempYsize;

    //�x�[�X�ƂȂ�t�H���g�̃f�B���N�g�����w��
    //Segment7Num��Segment7sNum��z�肵�Ă���
    presetting(rsc);

    //�����������K�v�Ȍ������`�悵�Ă������A�`������������āA����`��ł����Ȃ��Ȃ���
    if (rsc->format == HHMM) {
        sprintf(ss, "%02x:%02x", datetime[2], datetime[1]);
        display_puts(ss);
    }
    else if (rsc->format == HHMMSS) {
        sprintf(ss, "%02x:%02x:%02x", datetime[2], datetime[1], datetime[0]);
        display_puts(ss);
    }
    else if (rsc->format == HHMMss) {
        sprintf(ss, "%02x:%02x", datetime[2], datetime[1]);
        //���̃��[�`���ŁACursor�����I�ɐi��
        display_puts(ss);
        
        //�t�H���g�T�C�Y��������
        if (rsc->fontcode >= EXTFONTCODE) {
            // User�t�H���g�ŁAsmalll�t�H���g��`�Ȃ����̏��u������
            strcpy(Smaller, "s");
        } else {
            //Y��������1�T�C�Y����������
//            display_setTextSize(rsc->fontmag - 0x01); //MagY��0�ɂȂ��Ă��A�G���[�ɂȂ�Ȃ��悤�ɂȂ��Ă���
            uint8_t xs = (rsc-> fontmag) >> 4;
            uint8_t ys = (rsc-> fontmag) & 0x0f;
            ys = ys / 2 + 1;
            xs = xs / 2 + 1;
            display_setTextSize((uint8_t)(xs << 4) + ys); //MagY��0�ɂȂ��Ă��A�G���[�ɂȂ�Ȃ��悤�ɂȂ��Ă���
            
        }
        sprintf(ss, "%02x", datetime[0]);
        display_puts(ss);
        Smaller[0] = '\0';
    }
    display_setTextPitchAdj(0, 0); //presetting�ƃy�A�Ō��ɖ߂�
}


// ��=0~179�x�܂ŁAsin�Ƃ�256�{�ɂ���sin�e�[�u��
//�@�A���[���j��5���P�ʂɂ����ꍇ�A2.5�x�P��(30�x��12����)���~�����B1�x�P�ʂőË�
//sin(��): ��=�p�x
//180�x�ȍ~�́A-sin(��-180)�Ƃ����`�ŎQ��
//cos(��)=sin(��+90)�Ōv�Z
//1�x�P�ʂ̃e�[�u��
const int16_t sin_table[] = {
    //0    1    2    3    4    5    6    7    8    9
      0,   4,   8,  13,  17,  22,  26,  31,  35,  40,   //00-
     44,  48,  53,  57,  61,  66,  70,  74,  79,  83,   //10-
     87,  91,  95, 100, 104, 108, 112, 116, 120, 124,   //20-
    128, 131, 135, 139, 143, 146, 150, 154, 157, 161,   //30-
    164, 167, 171, 174, 177, 181, 184, 187, 190, 193,   //40-
    196, 198, 201, 204, 207, 209, 212, 214, 217, 219,   //50-
    221, 223, 226, 228, 230, 232, 233, 235, 237, 238,   //60-
    240, 242, 243, 244, 246, 247, 248, 249, 250, 251,   //70-
    252, 252, 253, 254, 254, 255, 255, 255, 255, 255,   //80-
    256, 255, 255, 255, 255, 255, 254, 254, 253, 252,   //90-
    252, 251, 250, 249, 248, 247, 246, 244, 243, 242,   //100-
    240, 238, 237, 235, 233, 232, 230, 228, 226, 223,   //110-
    221, 219, 217, 214, 212, 209, 207, 204, 201, 198,   //120-
    196, 193, 190, 187, 184, 181, 177, 174, 171, 167,   //130-
    164, 161, 157, 154, 150, 146, 143, 139, 135, 131,   //140-
    128, 124, 120, 116, 112, 108, 104, 100,  95,  91,   //150-
     87,  83,  79,  74,  70,  66,  61,  57,  53,  48,   //160-
     44,  40,  35,  31,  26,  22,  17,  13,   8,   4,   //170-
};

//degree�Ŏw�肳�ꂽ�p�x=theta�ɑΉ�����sin��Ԃ�(x256)
int16_t sind(int16_t theta) {
    theta = (theta + 360) % 360;    //theta���}�C�i�X�ł��Ή������邽��

    if (theta >= 180) {
        //-sind(��-180)
        return -sin_table[(UINT)(theta-180)];          //�x�P�ʂ̃e�[�u���p
    } else {
        return sin_table[(UINT)theta];
    }
}

//degree�Ŏw�肳�ꂽ�p�x=theta�ɑΉ�����cos��Ԃ�(x256)
int16_t cosd(int16_t theta) {
    theta = theta+90;
    return sind(theta);
}

/*
 * �A�i���O���v�\��
 * mode: 
 * datetime: ���t����
 * xx, yy: ���S���W
 * size: �O�`�~�̑傫��
 * color: �j�̐F�w��
 */
#define SizeMin  3      //���j�̑���
#define SizeHour 4      //���j�̑���
//#define ColorHour   GREY
//#define ColorMin    GREY
#define ColorSec    RED
#define ColorAlarm  YELLOW

/*
 * �A�i���O���v�̃A���[���j��`��/����
 */
void drawAlarmNeedle(int16_t *x, int16_t *y, uint16_t color) {
    int16_t xd, yd;
    
    //�A���[���j�`��
    display_drawLine(x[0], y[0], x[1], y[1], color);
    
    xd = (int16_t)(x[1]-x[0]);
    yd = (int16_t)(y[1]-y[0]);
    if ( ABS(yd) > ABS(xd)) {
        //�p�x�ɂ��A�ړ����������ς��A������������
        display_drawLine(x[0]+1, y[0], x[1]+1, y[1], color);
        display_drawLine(x[0]-1, y[0], x[1]-1, y[1], color);
    } else {
        display_drawLine(x[0], y[0]-1, x[1], y[1]-1, color);
        display_drawLine(x[0], y[0]+1, x[1], y[1]+1, color);
    }
}

//���j�A���j�̕`��/����
void drawNeedle(int16_t *x, int16_t *y, uint16_t color, uint16_t color2) {
    display_fillTriangle(x[1], y[1], x[2], y[2], x[3], y[3], color);
    display_drawTriangle(x[1], y[1], x[2], y[2], x[3], y[3], color2);
    //���Α��̏o������
    display_fillTriangle(x[4], y[4], x[2], y[2], x[3], y[3], color);
    display_drawTriangle(x[4], y[4], x[2], y[2], x[3], y[3], color2);

}

//�b�j�̕`��/����
void drawSecNeedle(int16_t *x, int16_t *y, uint16_t color) {
    display_drawLine(x[1], y[1], x[2], y[2], color);
    display_fillTriangle(x[0], y[0], x[3], y[3], x[4], y[4], color);
   
}

        
//�A���[���j�̒����͒��j��75%
//���j�F���j�̒����́A�ڐ��̐����3�h�b�g����
//�Z�j
//�b�j
//�ڐ��̒���=3
//�h�b�g�̑傫��=2
void drawAnalogClock(uint8_t *datetime, ObjectRsc *rsc, uint8_t *alarmtime) {
    int16_t rc = rsc->attribute /2; //�O�`�̔��a
    int16_t xc = rsc->x + rc;    //���v�̒��S���W
    int16_t yc = rsc->y + rc;
    uint16_t color = rsc->color;
    uint16_t bcolor;

    int16_t rc1, rcs, rcm, rch, rca;    //���v�̖ڐ��A�b�j�A���j�A���j�A�A���[��
    int16_t rc2;
    int16_t rcs2, rcm2, rcm3, rch2, rch3, rca2;    //���v�̃T�C�Y�A�ڐ��A�b�j�A���j�A���j�A�A���[��
    int16_t x[5], y[5];    //���W�v�Z�������ʂ��i�[
    int16_t angle;
    uint8_t hh, mm, ss, jj, kk;
    uint8_t almhh, almmm;
    //�O��̍��W�@���j�A���j�A�b�j�A�A���[����`�悷�邽�߂̒��_�̍��W��ێ�
    static int16_t phx[5], phy[5], pmx[5], pmy[5], psx[5], psy[5], pax[2], pay[2];
    //����̍��W
    int16_t chx[5], chy[5], cmx[5], cmy[5], csx[5], csy[5], cax[2], cay[2];
    char str[3];
    int8_t minupdate = 0;

    rsc->xw = (int16_t)rsc->attribute; //�O�`�T�C�Y�X�V
    rsc->yw = rsc->xw;

    //�O�`�~�̕`��
    display_drawCircle(xc, yc, rc, color);  //size�Ŏw�肳�ꂽ�~
    rc = rc -3;
    display_drawCircle(xc, yc, rc, color);  //���菬���߂̉~

    rc1 = rc - 3;        // ��Ԓ������j�̒���=�ڐ��������ɐݒ�
    rca = rc1 *3 /4;    //�A���[���p�̐j�̒����́A���j��75%
    rch = rc1 *7/10;    //�Z�j�̒����́A70%
    rcm = rc1 -5;       //���j�̒����́A�ڐ��̐����3�h�b�g����
    rcs = rc1 -4;       //�b�j�̒����́A����

    //�A���[���j�̕`�揀��----------------------------------------------------
    //�ŐV�̃A���[���j�̍��W���v�Z
    almmm = Bcd2Hex(alarmtime[0]);
    almhh = Bcd2Hex(alarmtime[1]) % 12;
    angle = almhh * 30 + almmm/2;   //�p�x�ɕϊ�
    cax[0] = xc;    //���S���W
    cay[0] = yc;
    cax[1] = xc + rca * sind(angle)/256;    //�A���[���j�̐�̍��W
    cay[1] = yc - rca * cosd(angle)/256;
    
    //��������Ȃ������߁A�\���ɕω�����j�������������A����ȊO�͏㏑������
    //�O�̕\��������
    if ((cax[1] == pax[1]) && (cay[1] == pay[1])) {
        //�O�ƍ��W���ς���Ă��Ȃ����́A�����Ȃ��B�ŏ��̕`����������W�ɂ��Ă���̂ŏ������Ȃ�
    } else {
        drawAlarmNeedle(pax, pay, TextbgColor);   //���̕b�j������
        //�`��p�̍��W��ۑ�
        for (jj=0; jj<2; jj++) {
            pax[jj] = cax[jj];
            pay[jj] = cay[jj];
        }
    }

    //���j�̕`�揀��--------------------------------------------------------
    //���j�̈ʒu���ς������A�ĕ`��̑O�ɁA���̎��j�A���j������
    mm = Bcd2Hex(datetime[1]);
    if (preTime[1] != datetime[1]) {
        preTime[1] = datetime[1];
        hh = Bcd2Hex(datetime[2]);

        //���j�̍��W���v�Z
        angle = mm * 6;     //1��6�x
        cmx[1] = xc + rcm * sind(angle)/256;    //�j��̍��W
        cmy[1] = yc - rcm * cosd(angle)/256;
    
        rcm2 = SizeMin;       //���j�̕�
        angle = angle +90;  //�j�ɑ΂���90�x�̊p�x
        cmx[2] = xc + rcm2 * sind(angle)/256;
        cmy[2] = yc - rcm2 * cosd(angle)/256;
        cmx[3] = xc - (cmx[2]-xc);
        cmy[3] = yc - (cmy[2]-yc);

        //���Α��̏o������
        rcm3 = 10;      //�o�����蒷��
        angle = angle +90;  //�����90�x����
        cmx[4] = xc + rcm3 * sind(angle)/256;
        cmy[4] = yc - rcm3 * cosd(angle)/256;

        //���j�̍��W�v�Z�F���j���ړ�������A���j���ړ��v�Z (���ۂ�2����)
        //�@���̃f�[�^����荞��Ŏ��j�̊p�x���߂�
        hh = hh % 12;
        angle = hh * 30 + mm/2;   //�p�x�ɕϊ� 8bit�ϐ��ł�NG
        
        chx[1] = xc + rch * sind(angle)/256;    //�j��̍��W
        chy[1] = yc - rch * cosd(angle)/256;
        
        rch2 = SizeHour;    //���j�̕�
        angle = angle +90;  //�j�ɑ΂���90�x�̊p�x
        chx[2] = xc + rch2 * sind(angle)/256;
        chy[2] = yc - rch2 * cosd(angle)/256;
        chx[3] = xc - (chx[2]-xc);
        chy[3] = yc - (chy[2]-yc);
        
        //���Α��̏o������
        rch3 = 10;      //�o�����蒷��
        angle = angle +90;  //�����90�x����
        chx[4] = xc + rch3 * sind(angle)/256;
        chy[4] = yc - rch3 * cosd(angle)/256;
        
        //�����ύX�ɂȂ莞�Ԃ��ς�鎞�́A���̕ύX�Ɠ����Ɏ��j�̏���
        drawNeedle(phx, phy, TextbgColor, TextbgColor);
        //���j�̏���
        drawNeedle(pmx, pmy, TextbgColor, TextbgColor);
        minupdate = 1;  //���jupdate����

        //�`����W��ۑ�
        for (jj=1; jj<5; jj++) {
            phx[jj] = chx[jj];
            phy[jj] = chy[jj];
            pmx[jj] = cmx[jj];
            pmy[jj] = cmy[jj];
        }
    }

    //�ŐV�̕b�j�̍��W���v�Z
    if (preTime[0] != datetime[0]) {
        preTime[0] = datetime[0];
        ss = Bcd2Hex(datetime[0]);  //0-59�̐��l
        angle = ss*6;
        csx[0] = xc;  //���S���W
        csy[0] = yc;
        csx[1] = xc + rcs * sind(angle)/256;
        csy[1] = yc - rcs * cosd(angle)/256;
        
        rcs2 = 20;   //���Α��ɏo�������
        angle = angle +180;
        csx[2] = xc + rcs2 * sind(angle)/256;
        csy[2] = yc - rcs2 * cosd(angle)/256;
        csx[3] = xc + rcs2 * sind(angle+6)/256;
        csy[3] = yc - rcs2 * cosd(angle+6)/256;
        csx[4] = xc + rcs2 * sind(angle-6)/256;
        csy[4] = yc - rcs2 * cosd(angle-6)/256;
        
        //�b�j������
        drawSecNeedle(psx, psy, TextbgColor);

        //�`����W��ۑ�
        for (jj=0; jj<5; jj++) {
            psx[jj] = csx[jj];
            psy[jj] = csy[jj];
        }
    }
    
    //�ڐ��Ɛ����̕`��------------------------------------------------------------
    SetFont(FontNormal, NULL);
    display_setTextSize(rsc->fontmag);
    display_setTextPitchAdj(rsc->xpitchAdj, 0);
    //�����Ղ̐����̈ʒu
    rc2 = rc1 - CurrentFont.xpitch * ((CurrentFont.MagX > CurrentFont.MagY) ? CurrentFont.MagX : CurrentFont.MagY);

    for (jj=0; jj<60; jj++) {
        angle = jj*6;
        x[1] = xc + (rc * sind(angle)+127)/256; //�P����256�Ŋ���Ɛ؎̂ĂɂȂ�̂�+127�Ƃ��Ďl�̌ܓ��I�ɂ��Ă݂�
        y[1] = yc - (rc * cosd(angle)+127)/256;
        x[2] = xc + (rc1 * sind(angle)+127)/256;
        y[2] = yc - (rc1 * cosd(angle)+127)/256;
        
        if (jj % 5 == 0) {
            //5�̔{���̏��ɕ����Ղ̐��l��\��
            display_fillCircle(x[2], y[2], 2, color);   //���̏��́A�h�b�g�A���a2
            //�����Օ\��
            if (!((minupdate == 0) && (jj == mm))) {
                //�������Aminupdate=0�ŁAmm��5�̔{���̎��́A�`�悵�Ȃ�(�������Ȃ��ƃ`����)
                //�܂�A���j�������ɂ������Ă��āA�����������Ȃ����́A���l�͕`�悵�Ȃ��Ƃ�������
//                x[3] = xc + (rc2 * sind(angle))/256 -4;
                x[3] = xc + (rc2 * sind(angle))/256 - CurrentFont.xsize / 2 * CurrentFont.MagX;
//                y[3] = yc - (rc2 * cosd(angle))/256 -3;
                y[3] = yc - (rc2 * cosd(angle))/256 - CurrentFont.ysize / 2 * CurrentFont.MagY;
                //�\�����鐔����ݒ�
                if (jj==0) kk = 12;   //0���̏��́A12�ɐݒ�
                else kk = jj / 5;
//                if (kk>9) x[3] = x[3] - 3;  //2���̎����̎��̈ʒu����
                if (kk>9) x[3] = x[3] - CurrentFont.xsize / 2 * CurrentFont.MagX;  //2���̎����̎��̈ʒu����
                
                sprintf(str, "%d", kk);
                //�w�i�F�𓯂��ɂ���ƁA�w�i�F��h��Ȃ�=�����Ɠ���
                //1�����l��2�����l�ňʒu�𒲐�
                display_drawChars(x[3], y[3], str, color, color, rsc->fontmag);
            }
        } else {
            display_drawLine(x[1], y[1], x[2], y[2], color);
        }
    }
    
    // AM/PM�̕\��
    //��������Ȃ������߁A�w�i�����͕ω������Ȃ��`����s�����A
    //AM/PM�̐ؑ֎������A�w�i�`����{
    if (Bcd2Hex(preTime[2])/12 != Bcd2Hex(datetime[2])/12) {
        bcolor = TextbgColor;
    }
    else {
        bcolor = color;
    }
    preTime[2] = datetime[2];
    if (datetime[2] < 0x12) strcpy(str, "AM");
    else strcpy(str, "PM");
    //�ߑO�ߌ�̕\����傫���ɉ����ĕς���@8�h�b�g�T�C�Y���x�[�X�ɂ��Ă���
    //�傫�ȃt�H���g�T�C�Y�̎����Ή�������悤�ɁA16�h�b�g�ȏ�̃t�H���g
//    int16_t size = ((rc+3) / 70) * ( (CurrentFont.xsize < 15) ? (1) : (0) ) + 1;
//    display_drawChars(xc-CurrentFont.xpitch * size, yc+rc/2, str, color, bcolor, 0x11 * size);

    display_drawChars(xc-CurrentFont.xpitch * CurrentFont.MagX, yc+rc - 3 * CurrentFont.ysize * (CurrentFont.MagY), str, color, bcolor, rsc->fontmag);
    
    //��������A�j�̕`������s
    //�A���[���j�`��------------------------------------------------------
    drawAlarmNeedle(pax, pay, ColorAlarm);  //�A���[���j�̐F

    //���j�̕`��------------------------------------------------------
    drawNeedle(phx, phy, color, WHITE);    //���j�̐F�́A����������炤

    //���j�̕`��------------------------------------------------------
    drawNeedle(pmx, pmy, color, WHITE);

    //�b�j�̕`��------------------------------------------------------
    display_fillCircle(xc, yc, 3, ColorSec);    //�b�j�̒����~
    drawSecNeedle(psx, psy, ColorSec);
    
}

/*
 * ���\�[�X�Ŏw�肳�ꂽ�ݒ�Ŏ�����\��
 */
void DrawTime(uint8_t *datetime, ObjectRsc *rsc) {

    if (rsc->disp) {
        //RscID�����̃I�u�W�F�N�g�̗L��(=�\�����邩�ǂ���)�ɗ��p
        if (rsc->format == ANALOGCLOCK) {
            //�O�`�~�̍��W= (140, 130) ���a=90
            //�A�i���O���v�\���̏ꍇ�́A�A���[���j�������ɕ\��
            drawAnalogClock(datetime, rsc, AlarmTime);
            //�A�i���O���v�̑傫���́Axw�ɐݒ肳��Ă��锼�a��2�{
        } else {
            dispTime(datetime, rsc);
            rsc->xw = Cursor_x - rsc->x;
            rsc->yw = CurrentFont.ysize * (rsc->fontmag & 0x0f);
        }
    }
}

/*
 * �j���̃t�H���g�����邩�����������āA�Ȃ����Normal�t�H���g�ɐ؂�ւ���
 * �؂�ւ������́A1��Ԃ�
 */
int8_t CheckSunday(ObjectRsc *rsc) {
    char filename[30];
    FIL file;       //Opening��f�[�^�������ݗp
    FRESULT res;

    if (CurrentFont.fontcode == EXTFONTCODE) {
        //Sun.bmp������O��ŁA���̑傫�����擾����
        sprintf(filename, "/%s/Sun.bmp", CurrentFont.fontName);
        if (f_open(&file, filename, FA_READ) == FR_OK) {
            return 0;        //�j���t�H���g����
        }        
    }
    //�j���t�H���g�Ȃ����A�m�[�}���t�H���g�̗j���𗘗p
    SetFont(FontNormal, NULL); //�j���́A�����t�H���g���g���`�ɂ��Ă��邪�A�J�X�^���ŗj���̃t�H���g�������Ă���ꍇ���l������������������������������
    display_setTextSize(rsc->fontmag);    //fontsize�̏��4bit=X�{���A����4bit=Y�{��
    return 1;
}

#define SpacebtwMonths  10  //�J�����_�[�����ɕ��ׂ��Ƃ��̃X�y�[�X
#define SpacebtwDays    2  //���t�Ԃ̊Ԋu�@xpitchAdj��2���̓��t�̕����Ԋu�Ɏg�p

/*
 * �J�����_�[������3�����\��
 * ����10�h�b�g���ƁA300�h�b�g��21�����\���K�v�B300/21=14�h�b�g�@���t��2�h�b�g���ƁA1����6�h�b�g�ȉ�
 * 
 * 15�h�b�g���Ƃ��Ă��A*21=315�h�b�g�Ȃ̂ŁA�Ԋu��2�h�b�g�ɂ���Γ���v�Z
 * �t�H���g�Ƃ��ẮA��7�h�b�g���ƁA���S�ɂ����t���āA���̓��Ƃ̊Ԋu��1�h�b�g�ɂ���K�v������A�����ڂɂ���������
 * ���\�[�X�Ŏw�肳�ꂽ�t�H���g�ɉ����ĕ`��
 * �@�@Small:�@���Ɨj����Normal�t�H���g�g�p����
 * �@�@Normal: �S���m�[�}���ŕ`��
 * ����̍��W: xs, ys
 * �`�悷��N��: year, month
 */
void basicDrawCalendar(uint8_t year, uint8_t month, int16_t xs, int16_t ys, ObjectRsc *rsc) {
    uint8_t jj, kk;
    int16_t xx, yy;
    char str[5];
    int8_t currentDay;    //�ŏ��̓��j�̓��t�A0�ȉ��͑O��
    int8_t maxdays;
    uint8_t yr, mm, dd, wd;
    uint8_t thismonth, targetDay;
    //�j�����Ƃ̐F�w��
    uint16_t dayColor[7];   // = {RED, WHITE, WHITE, WHITE, WHITE, WHITE, BLUE, };
    uint8_t xpitch, ypitch; // �\���s�b�`
    uint8_t mulx, muly, mul;
    char w;
    int8_t res;

    //�O���A�����̕\���̂��߁A0���A13���Ƃ����ݒ�������̂ŁA�����ŏC��
    if (month == 0) {
        month = 12;
        year--;
    }
    if (month == 13) {
        month = 1;
        year++;
    }

    yr = year;
    mm = month;
    dd = 1;
    wd = getWeekdays(&yr, &mm, &dd);  //�����߂̗j�����擾
    //�\������ŏ��̓��t�����z�I�ɉ������ݒ�
    //1�������j�Ȃ�1�̂܂܁A���j�Ȃ�ŏ��̓��j����0���A�Ηj�Ȃ�-1�A���j�Ȃ�-2�ɂȂ�
    currentDay = (int8_t)(1- wd);
    
    //���Y���̍ŏI�����擾�B���̌����������邩��c��
    dd = 31;
    getWeekdays(&yr, &mm, &dd);  //31���̗j�����擾�B���̓����Ȃ��ꍇ�A�������������
    //mm���ύX�ɂȂ�����A31���ł͂Ȃ������Ƃ킩��B
    //dd��31����1���ɕς���Ă�����A���̌���30���������Ƃ킩��B2���A���邤�N���Ή��B
    if (month != mm) {
        maxdays = (int8_t)(31 - dd);    //���̌��̍ŏI��
    } else {
        maxdays = (int8_t) dd;
    }   
    
    //�j�����Ƃ̐F�����\�[�X����擾
    for (jj = 1; jj < 6; jj++) dayColor[jj] = rsc->color;
    dayColor[0] = rsc->attribute2;   //���j���̐F
    dayColor[6] = rsc->attribute;    //�y�j���̐F
    
    //�w�肳�ꂽ��{�t�H���g�����擾
    SetFont(rsc->fontcode, rsc->fontName);
    display_setTextSize(rsc->fontmag);    //���4bit=X�{���A����4bit=Y�{��
    display_setTextPitchAdj(rsc->xpitchAdj, 0);

    xpitch = (uint8_t)(CurrentFont.xpitch * CurrentFont.MagX + CurrentFont.XpitchAdj)*2 + SpacebtwDays;  //1��=2����+�X�y�[�X
    ypitch = (uint8_t)(CurrentFont.ypitch * CurrentFont.MagY + CurrentFont.YpitchAdj);
    
//    DEBUG_PRINTF("%d, %d  %d,%d, xp=%d\r\n", xpitch, ypitch, CurrentFont.xpitch, CurrentFont.XpitchAdj, rsc->xpitchAdj);
//    DEBUG_PRINTF("font %s: %d, %d\r\n", rsc->fontName, CurrentFont.xsize, CurrentFont.ysize);

    //�Ώۗ̈���N���A: fontw��14������+7���̃X�y�[�X���AY������8�s��
    //x�̊J�n��1�h�b�g���ɂ��āA�T�C�Y�����̕��傫���B�g��t�������͂ݏo����h�~
    CalendarXsize = xpitch*7;  //2���̓��t���x�[�X�ɂȂ��Ă���xpitch��7����
    CalendarYsize = ypitch * 8;     //��+�j��+�ő��6�T��

    //1�������̗̈���N���A����
    display_fillRect(xs, ys, CalendarXsize + SpacebtwMonths, CalendarYsize, TextbgColor);

    //���̐�����`��
    sprintf(str, "%d", month);
    //�����ɕ\��������@��2�{�ŕ\�������邽�߁A������́A/2�����Ă��Ȃ�
    xx = xs + CalendarXsize/2 - (int16_t)strlen(str) * (int16_t)CurrentFont.xpitch;
    //���\���͉�����2�{�ŕ`��
    display_drawChars(xx, ys, str, rsc->color, TextbgColor, rsc->fontmag +0x10);

    //�j���̕`��
    //�j���t�H���g�����邩�ǂ���
    res = CheckSunday(rsc);
    //�o�����X�̂��߁A�j���̕���pitch�Ɏ��܂�ő�̃T�C�Y�Ɋg�傳����
    mulx = xpitch / CurrentFont.xsize;
    muly = ypitch / CurrentFont.ysize;

    yy = ys + ypitch;
    xx = xs + (xpitch - CurrentFont.xsize * mulx)/2;   //�����ɂȂ�悤��
    mul = (uint8_t)(mulx << 4) + muly;
    str[1] = '\0';
    for (jj=0; jj<7; jj++) {
        if (rsc->format >= Month1e) w = WeekDays[jj][0];  //SMTWTFS�Ƃ����\�L����
        else w = FSunday + jj;    //�����ΐ��؋��y�Ƃ����\��
        str[0] = w;
        display_drawChars(xx, yy, str, dayColor[jj], TextbgColor, mul);
        xx += xpitch;
    }
    
    //���t�̕`��
    if (res) {
        //�j���Ńt�H���g�ύX���Ă������́A�t�H���g���ɖ߂�
        SetFont(rsc->fontcode, rsc->fontName);
        display_setTextSize(rsc->fontmag);    //���4bit=X�{���A����4bit=Y�{��
        display_setTextPitchAdj(rsc->xpitchAdj, 0);
    }

    yy += ypitch;
    SetFont(rsc->fontcode, rsc->fontName);  //�w��̃t�H���g�ɖ߂�
    display_setTextSize(rsc->fontmag);    //fontsize�̏��4bit=X�{���A����4bit=Y�{��
    display_setTextPitchAdj(rsc->xpitchAdj, 0);

    //�����̃J�����_�[��\�����Ă��邩���`�F�b�N�B�����̓��t�Ɉ�����邩�ǂ������f�����邽��
    if (month == Bcd2Hex(DateTime[5])) thismonth = 1;
    else thismonth = 0;
    targetDay = Bcd2Hex(DateTime[4]);
    
    for (kk = 0; kk < 6; kk++) {
        xx = xs;
        for (jj = 0; jj < 7; jj++) {
            if (currentDay > 0 && currentDay <= maxdays) {
                display_setCursor(xx, yy);
                display_setColor(dayColor[jj]);
                sprintf(str, "%2d", currentDay);
                display_puts(str);
                
                //�����̓��t�Ɉ������
                if (thismonth && (currentDay == targetDay)) {
                    //���]���Ǝ��F���ɂ����̂Řg�ň͂�
                    display_drawRect(xx-1, yy-1, xpitch + 1, ypitch, WHITE);
                }
            }
            currentDay++;
            xx += xpitch;
        }
        yy += ypitch;
    }
}


/*
 * Format�ɉ����ăJ�����_�[��\������
 * ����5��ނ̃t�H�[�}�b�g
 * Month1: ����1�����̃J�����_�[��\���B�t�H���g�T�C�Y�ς��邱�ƂŁA�傫�������������\����
 * Month3: �O��̌����܂߂�3�������̃J�����_�[�����z�u�ŕ\��
 * Month3v:�O��̌����܂߂�3�������̃J�����_�[���c�z�u�ŕ\��
 * �@�����̔N���́A�\�������錎���w��
 * �@BCD�ł͂Ȃ�
 * �\���ʒu�́A���\�[�X�Ŏw��
 * 
 */
void DrawCalendar(ObjectRsc *rsc) {
    uint8_t jj, kk, xx, yy;
    int8_t monthAdj;
    int16_t xp, yp;
    uint16_t f;

    if (rsc->disp) {
        f = rsc->format % 10;
        if (f == Month1) {
            // 1�����̃J�����_�[��\���@�@�傫���̓t�H���g�T�C�Y�Ŏw��
            xx = 1;
            yy = 1;
        } else if (f == Month2) {
            // 2�������̃J�����_�[��\��
            xx = 2;
            yy = 1;
        } else if (f == Month2v) {
            // 2�������̃J�����_�[���c�ɕ\��
            xx = 1;
            yy = 2;
        } else if (f == Month3) {
            // 3�������̃J�����_�[��\��
            xx = 3;
            yy = 1;
        } else if (f == Month3v) {
            // 3�������̃J�����_�[���c�ɕ\��
            xx = 1;
            yy = 3;
        }

        //CalendarXsize�ACalendarYsize�́A�ŏ��̌���`�悷��ƌv�Z�����
        if (xx * yy > 2) monthAdj = -1; //3�����\���̎��́A�O������\������
        else monthAdj = 0;
        
        yp = rsc->y;
        for (kk = 0; kk < yy; kk++) {
            xp = rsc->x;
            for (jj = 0; jj < xx; jj++) {
                basicDrawCalendar(Bcd2Hex(DateTime[6]), (uint8_t)(Bcd2Hex(DateTime[5])+monthAdj + jj + kk * xx), xp, yp, rsc);
                xp += CalendarXsize + SpacebtwMonths;
            }
            yp += CalendarYsize;
        }

        rsc->xw = xp - rsc->x -10;
        rsc->yw = yp - rsc->y;
    }
}

/*
 * �A���[��������\���@�@�X�C�b�`�̏�ԂŐF��ω�������
 * sw SlideSWoff=Alarm off, SlideSWon=Alarm on
 */
void DrawAlarmTime(uint8_t *alarmtime, uint8_t sw, ObjectRsc *rsc) {
    char str[100];
    char ampm[][3] = {"AM", "PM", ""};
    uint8_t ap;
    char onOff; //[] = {'-', '+'};    //�I���I�t��+-�ؑց@plus/minus
    uint16_t color;
    uint8_t hr;
    
    if (rsc->disp) {
        //�A���[�������̕\���F���A�X���C�hSW��On/Off�ŕς���
        if (sw == SlideSWon) {
            //On�̎�
            onOff = '+';
            color = rsc->color;
        } else {
            //Off�̎�
            onOff = '-';
            color = rsc->attribute;  //�A���[���̃I�t���̐F��attribute�Ɋi�[
        }
        
        hr = Bcd2Hex(alarmtime[1]);
        if ((rsc->format == ALMAMPM)||(rsc->format == iALAMPM)) {
            //12���ԕ\��
            if (hr >= 12) {
                ap= 1;   //12���ȍ~�Ȃ�ߌ�  
                hr -= 12;   //12���ԕ\�L�̂��߁A12��������
            }
            else {
                ap = 0;
            }
        } else {
            //24���ԕ\��
            ap = 2;
        }
        if ((rsc->format == ALMAMPM)||(rsc->format == ALM24)) {
            sprintf(str, "ALM%c %s%02d:%02x", onOff, ampm[ap], hr, alarmtime[0]);
        } else {
            sprintf(str, "%c%c %s%02d:%02x", Fbell, onOff, ampm[ap], hr, alarmtime[0]);
        }
        presetting(rsc);
        display_setColor(color);    //SW�ɑΉ����ĐF��ύX�����邽��
        display_puts(str);
        display_setTextPitchAdj(0, 0); //presetting�ƃy�A�Ō��ɖ߂�
        
        rsc->xw = Cursor_x - rsc->x;
        rsc->yw = CurrentFont.ysize * CurrentFont.MagY;
    }
}

/*
 * �����̓��t��\��
 * �\�����@�Ȃǂ́A���\�[�X���ŁA�w�肷��
 * �@�@format, �t�H���g�Ȃ�
 */
void DrawDate(uint8_t *datetime, ObjectRsc *rsc) {
    char str[100];
    
    if (rsc->disp) {
        switch (rsc->format) {
            case YYYYMMDDw:
                //NormalFont�ł́A�j���̃t�H���g��FSunday=0x17-�ɂ���
                sprintf(str, "20%02x/%02x/%02x(%c)", datetime[6], datetime[5], datetime[4], FSunday+datetime[3]);
                break;
            case YYYYMMDDwe:  //�j���̉p��\�L
                sprintf(str, "20%02x/%02x/%02x(%s)", datetime[6], datetime[5], datetime[4], WeekDays[datetime[3]]);
                break;
            case YYYYMMDD:
                sprintf(str, "20%02x/%02x/%02x", datetime[6], datetime[5], datetime[4]);
                break;
            case YYMMDDw:
                sprintf(str, "%02x/%02x/%02x(%c)", datetime[6], datetime[5], datetime[4], FSunday+datetime[3]);
                break;
            case YYMMDDwe:
                sprintf(str, "%02x/%02x/%02x(%s)", datetime[6], datetime[5], datetime[4], WeekDays[datetime[3]]);
                break;
            case YYMMDD:
                sprintf(str, "%02x/%02x/%02x", datetime[6], datetime[5], datetime[4]);
                break;
            case MMDDw:
                sprintf(str, "%02x/%02x(%c)", datetime[5], datetime[4], FSunday+datetime[3]);
                break;
            case MMDDwe:
                sprintf(str, "%02x/%02x(%s)", datetime[5], datetime[4], WeekDays[datetime[3]]);
                break;
            case MMDD:
                sprintf(str, "%02x/%02x", datetime[5], datetime[4]);
                break;
        };
        presetting(rsc);
        display_puts(str);
        display_setTextPitchAdj(0, 0); //presetting�ƃy�A�Ō��ɖ߂�
        
        rsc->xw = Cursor_x - rsc->x;
        rsc->yw = CurrentFont.ysize * CurrentFont.MagY;
    }
}


/*
 * ���x��\��
 * temp:�@�ێ���10�{�ɂ��āA0.1���܂Ő����őΉ�������
 */
void DrawTemperature(int16_t temp, ObjectRsc *rsc) {
    char str[50];
    int16_t ftemp;
    
    if (rsc->disp) {
        if (rsc->format == DEGF) {
            //temp�́A10�{�̐ێ��@�@�@10F = 9/5*(10C) + 320
            ftemp = temp * 9*2 +320 *10;
            sprintf(str, "%2d%c", ftemp / 100, FdegF);  //
        } else {
            sprintf(str, "%2d.%1d%c", temp / 10, temp % 10, FdegC);  //FdegC=���̕����R�[�h
        }
        
        presetting(rsc);
        display_puts(str);
        display_setTextPitchAdj(0, 0); //presetting�ƃy�A�Ō��ɖ߂�
        
        rsc->xw = Cursor_x - rsc->x;
        rsc->yw = CurrentFont.ysize * CurrentFont.MagY;
    }
}

/*
 * ���x��\��
 * �����_�ȉ��͕\�����Ȃ�
 */
void DrawHumidity(int16_t humidity, ObjectRsc *rsc) {
    char str[50];

    if (rsc->disp) {
        presetting(rsc);

        sprintf(str, "%2d%%", humidity / 10);
        display_puts(str);
        
        rsc->xw = Cursor_x - rsc->x;
        rsc->yw = CurrentFont.ysize * CurrentFont.MagY;
    }
}

/*
 * ���ԃA�C�R��32x32�h�b�g��(x,y)���W�ɕ`��
 * �J�X�^���t�H���g�ɂ��Ή��ł���悤
 */
void DrawGearIcon(ObjectRsc *rsc) {
    char str[50];

    if (rsc->disp) {
        if (rsc->fontcode != FontExt) rsc->fontcode = FontGear;
        presetting(rsc);

        sprintf(str, "%c", Fgear);
        display_puts(str);
        rsc->xw = Cursor_x - rsc->x;
        rsc->yw = CurrentFont.ysize * CurrentFont.MagY;
        
        if (rsc->fontcode == FontGear) rsc->fontcode = FontNormal;

    }
}
