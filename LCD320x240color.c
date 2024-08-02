/*
 * File:   LCD320x240color.c
 * Author: K.Ohno
 *
 * �uPIC�̃u���O�vhttp://picceri.blogspot.com/2019/01/320240-lcd.html
 * ���烉�C�u���������������A������x�[�X��MCC�p�A����8bit PIC�p�ɏ��������܂����B
 * 2020/11/2
 * 
 * MCC��SPI�̃��[�`�����Ăяo���悤�ɕύX
 * SLR maximum�ɕύX���A16MHz���삪�\��
 */
///////////////////////////////////////////////////////////////////////////
////                                                                   ////
////                            GFX_Library.c                          ////
////                                                                   ////
////                 Graphics library for mikroC compiler.             ////
////                                                                   ////
///////////////////////////////////////////////////////////////////////////
////                                                                   ////
////               This is a free software with NO WARRANTY.           ////
////                     https://simple-circuit.com/                   ////
////                                                                   ////
///////////////////////////////////////////////////////////////////////////

#include "LCD320x240color.h"
#include "mcc_generated_files/drivers/spi_master.h"
#include "Draw.h"
#include "debug.h"

#ifndef _swap_int16_t
#define _swap_int16_t(a, b) { int16_t t = a; a = b; b = t; }
#endif

// ��Βl���擾����֐��}�N����`
#define ABS(x) ((x) < 0 ? -(x) : (x))

#define hi(a) ((a) >> 8)
#define lo(a) ((a) & 0xff)

#define EXIT_FAILURE 1
#define EXIT_SUCCESS 0
    

/*
 * �e�L�X�g��\�������邽�߂̊�{�ݒ�ϐ�
 * Cursor_x,y: �����\���̋N�_�ƂȂ���W
 * TextColor, TextgbColor: �����Ɣw�i�F�@�@�@�����l���ݒ肳��Ă���ƁA�w�i�͓����Ƃ��ď��u
 * Wrap: �������s���邩�ǂ���
 * �����t�H���g�́A�����Œ�
 * �@�����ڂ�ύX�������ꍇ�A�����̑傫���ƍ��E�㉺�̕����Ƃ̊Ԋu��ύX����
 */
int16_t 
  Cursor_x = 0,          // x location to start print()ing text
  Cursor_y = 0;          // y location to start print()ing text
uint16_t 
  TextColor   = WHITE,    // 16-bit background color for print()
  TextbgColor = BLACK;    // 16-bit text color for print()
bool 
  Wrap = false;           ///< If set, 'Wrap' text at right edge of display

//SD card
FIL FileFont;   //�t�H���g�t�@�C���p

/**************************************************************************
    @brief   Set text font color with custom background color
    @param   c   16-bit 5-6-5 Color to draw text with
    @param   b   16-bit 5-6-5 Color to draw background/fill with
 **************************************************************************/
void display_setColor(uint16_t c) {
    TextColor   = c;
}

void display_setTextColor(uint16_t c, uint16_t b) {
    TextColor   = c;
    TextbgColor = b;
}

/**************************************************************************
    @brief   Set text 'magnification' size. Each increase in s makes 1 pixel that much bigger.
    @param  s  Desired text size. 1 is default 2 is twice.
 **************************************************************************/
void display_setTextSize(uint8_t mag) {

    //Textsize�̏��4bit=X�{���A����4bit=Y�{��
    CurrentFont.MagX = mag >> 4;
    CurrentFont.MagY = mag & 0x0f;
    //�G���[����̂��߁A�Œ�T�C�Y��1�Ƃ���
    if (CurrentFont.MagX < 1) CurrentFont.MagX = 1;
    if (CurrentFont.MagY < 1) CurrentFont.MagY = 1;
}

/*
 * ���݂̃t�H���g�̃s�b�`���x�[�X�ɁA�t�H���g�̊g�嗦���悶�āA���������Z
 */
void display_setTextPitchAdj(int8_t adjX, int8_t adjY) {

    CurrentFont.XpitchAdj = adjX;
    CurrentFont.YpitchAdj = adjY;
}


void display_setWrap(bool w) {
    Wrap = w;
}


/**************************************************************************
    @brief  Set text cursor location
    @param  x    X coordinate in pixels
    @param  y    Y coordinate in pixels
 **************************************************************************/
void display_setCursor(int16_t x, int16_t y) {
    Cursor_x = x;
    Cursor_y = y;
}


/**
 * This function send data or register-information via SPI to the LC-Display
 * @param value Value to send to the LCD-Controller
 */
void write_command(uint8_t cmd) {
    LCD_DCRS_SetLow();  //�R�}���h���[�h
    SPI1_ExchangeByte(cmd);
    LCD_DCRS_SetHigh();  //�f�t�H���g���f�[�^���[�h�Ƃ�����
}

/*
 * 16bit�f�[�^(���g���G���f�B�A��)��count�񏑂�����
 */
void SPI1_ExchangeNData(uint16_t data, uint16_t count) {
    uint8_t dh, dl, dummy;
    
    dh = hi(data);
    dl = lo(data);

    while (count--) {
        SPI1TCNTL = 2;  //���M�o�C�g��
        SPI1TXB = dh;
        while (!PIR2bits.SPI1RXIF);
        dummy = SPI1RXB;   //���ꂪ�Ȃ��ƒʐM����~����
        SPI1TXB = dl;
        while (!PIR2bits.SPI1RXIF);
        dummy = SPI1RXB;   //���ꂪ�Ȃ��ƒʐM����~����
    }
}

//word�f�[�^�̏�������
// Intel�Ɠ���Lower byte first�����ASPI�ő��M���鎞��High�o�C�g���ŏ�
void write_wdata(uint16_t data) {
    SPI1_ExchangeNData(data, 1);
}


//�f�[�^�̓Ǐo��
uint8_t read_data() {
    return SPI1_ExchangeByte(0);
}

// �f�[�^���������ރG���A�̎w��  �`��͈͂�ݒ�
// 4�̈����͂��ׂĉ�ʏ�̍��W
void lcd_addset(uint16_t x, uint16_t y, uint16_t xx, uint16_t yy) {
    
    write_command(0x2A);
    write_wdata(x);
    write_wdata(xx);
    write_command(0x2B);
    write_wdata(y);
    write_wdata(yy);
    write_command(0x2C);    
}


void addset(int16_t x, int16_t y, int16_t xx, int16_t yy) {

    if (!SPI1_Open(LCD16M_CONFIG)) return;
    //�����ƊJ������
    LCD_CS_SetLow();    //CS��Low��
    lcd_addset((uint16_t)x, (uint16_t)y, (uint16_t)xx, (uint16_t)yy);
    LCD_CS_SetHigh();    //CS��High��
    SPI1_Close();

}
/*******************************************************************************
 * Set the cursor to the coordinate (x,_)
 */
uint8_t lcd_set_cursor_x(uint16_t x) {
    
    if ( x >= LCD_WIDTH ) return EXIT_FAILURE;

    if (!SPI1_Open(LCD16M_CONFIG)) return EXIT_FAILURE;
    //�����ƊJ������
    LCD_CS_SetLow();    //CS��Low��
    write_command(0x2A);    //Column Address Set
    SPI1_ExchangeNData(x, 2);
    write_command(0x2C);
    LCD_CS_SetHigh();    //CS��High��
    SPI1_Close();
    
    return EXIT_SUCCESS;
}

/*******************************************************************************
 * Set the cursor to the coordinate (_,y)
 */
uint8_t lcd_set_cursor_y(uint16_t y) {
    
    if( y >= LCD_HEIGHT ) return EXIT_FAILURE;
    
    if (!SPI1_Open(LCD16M_CONFIG)) return EXIT_FAILURE;
    //�����ƊJ������
    LCD_CS_SetLow();    //CS��Low��
    write_command(0x2B);    // Page Address Set
    SPI1_ExchangeNData(y, 2);
    write_command(0x2C);
    LCD_CS_SetHigh();    //CS��High��
    SPI1_Close();

    return EXIT_SUCCESS;
}

/*******************************************************************************
 * Set the cursor at position (x,y)
 */
uint8_t lcd_set_cursor(uint16_t x, uint16_t y) {
    if ( lcd_set_cursor_x((uint16_t)x) || lcd_set_cursor_y((uint16_t)y) ) {
        return EXIT_FAILURE;
    }	
    return EXIT_SUCCESS;
}

/*******************************************************************************
 * Draw a Pixel at the actual position
 */
uint8_t draw_pixel(uint16_t color) {

    if (!SPI1_Open(LCD16M_CONFIG)) return EXIT_FAILURE;
    //�����ƊJ������
    LCD_CS_SetLow();    //CS��Low��
    write_wdata(color);
    LCD_CS_SetHigh();    //CS��High��
    SPI1_Close();

    return EXIT_SUCCESS;
}

/*******************************************************************************
 * This function will draw a Pixel at (x,y) with color
 */
void lcd_draw_pixel_at(int16_t x, int16_t y, uint16_t color) {
    if ( !lcd_set_cursor((uint16_t)x, (uint16_t)y) ) {
        draw_pixel(color);
    }
}


/**************************************************************************
    @brief   Given 8-bit red, green and blue values, return a 'packed'
             16-bit color value in '565' RGB format (5 bits red, 6 bits
             green, 5 bits blue). This is just a mathematical operation,
             no hardware is touched.
    @param   red    8-bit red brightnesss (0 = off, 255 = max).
    @param   green  8-bit green brightnesss (0 = off, 255 = max).
    @param   blue   8-bit blue brightnesss (0 = off, 255 = max).
    @return  'Packed' 16-bit color value (565 format).
 * red,green,blue�́A1bit �E�V�t�g���ꂽ�`�ň����n�����
 * R: bit6-2��5bit�Ƀf�[�^
 * G: bit6-1��6bit�Ƀf�[�^
 * B: bit6-2��5bit�Ƀf�[�^
 **************************************************************************/
#define Color565(red, green, blue) ((uint16_t)((red) & 0x7C) << 9) | ((uint16_t)((green) & 0x7E) << 4) | (((blue) & 0x7C) >> 2)

uint16_t Getcolor565(uint8_t red, uint8_t green, uint8_t blue) {
    return Color565(red, green, blue);
}

// 8bi tcolor�x�[�X
uint16_t Getcolor565_8(uint8_t red, uint8_t green, uint8_t blue) {
    return ((uint16_t)((red) & 0xF8) << 8) | ((uint16_t)((green) & 0xFC) << 3) | (((blue) & 0xF8) >> 3);
}


/*
 * ���݂̃A�h���X�̃f�[�^=�F����num�h�b�g���ǂݏo��
 * �擪�h�b�g�̐F��565color�t�H�[�}�b�g�ŕԂ�
 * 
 * ���̏�Ԃ��ƃG���[�����ʂł��Ȃ�
 */
uint16_t read_pixel(uint8_t *rgbdata, uint8_t num) {
    uint8_t dummy, jj;
    
    if (!SPI1_Open(LCD16M_CONFIG)) return EXIT_FAILURE;
    //�����ƊJ������
    LCD_CS_SetLow();    //CS��Low��
    write_command(0x2E);    // Memory Read
    dummy = read_data();    // �ŏ���1�o�C�g�̓_�~�[
    
    for (jj=0; jj<num; jj++) {
        SPI1_ReadBlock(&rgbdata[jj*3], 3);  //3byte��RGB�f�[�^�擾
    }
    write_command(0x2C);
    LCD_CS_SetHigh();    //CS��High��
    SPI1_Close();

    return Color565((uint16_t)rgbdata[0], (uint16_t)rgbdata[1], (uint16_t)rgbdata[2]);
}

// num�h�b�g���̃s�N�Z���J���[���擾
uint16_t read_pixel2nd(uint8_t *rgbdata, uint8_t num) {
    uint8_t dummy, jj;
    
    if (!SPI1_Open(LCD16M_CONFIG)) return EXIT_FAILURE;
    //�����ƊJ������
    LCD_CS_SetLow();    //CS��Low��
    write_command(0x3E);    // Memory Read
    dummy = read_data();    // �ŏ���1�o�C�g�̓_�~�[
    
    for (jj=0; jj<num; jj++) {
        SPI1_ReadBlock(&rgbdata[jj*3], 3);
    }
    write_command(0x2C);
    LCD_CS_SetHigh();    //CS��High��
    SPI1_Close();
//    return (((uint16_t)rgbdata[0] & 0x7c) << 9) | (((uint16_t)rgbdata[1] & 0x7e) << 4) | ((rgbdata[2] & 0x7c) >> 2);
    return Color565(rgbdata[0], rgbdata[1], rgbdata[2]);
}


/**************************************************************************
    @brief  Print one byte/character of data
    @param  c  The 8-bit ascii character to write
 * CurrentFont�Ŏw�肳�ꂽ�����t�H���g��1�����`��
 * \r�ŁA���[�ɖ߂�
 * \n�ŁA���̍s�ɍs�����ǁAX���W�͕ς��Ȃ�
 * CurrentFont�̃f�[�^���g���ĕ`��
 * �`�掞�Ɏg�p����ϐ��Ƃ��āA�ȉ����Q�Ƃ���
 *   Cursor_x, Cursor_y, TextColor, TextbgColor, 
 *   CurrentFont.xpitch, CurrentFont.ypitch
 *   TextMagX, TextMagY, 
 *   XpitchAdj, YpitchAdj
 **************************************************************************/
void display_putc(char c) {
    uint8_t ii, jj;
    uint16_t pos;
    uint8_t line;
//    uint8_t fontXsize = CurrentFont.xsize;
    uint8_t xmul, ymul;
    int16_t xp, yp; //���W
    int16_t px, py; //�{������荞��pitch

//    xmul = (uint8_t) (TextMagX * (CurrentKanjiFont.kratio >> 4));      //X�����{��
//    ymul = (uint8_t) (TextMagY * (CurrentKanjiFont.kratio & 0x0f));    //Y�����{��
    xmul = CurrentFont.MagX;    //X�����{��
    ymul = CurrentFont.MagY;    //Y�����{��
    if (c == '\r') {    //���s=Carriage return�@���[�ɃJ�[�\���߂�
        Cursor_x = 0;
        return;
    }
    py = CurrentFont.ypitch * ymul + CurrentFont.YpitchAdj;
    if (c == '\n') {    //���s=Line Feed�@�C���[�W�I�ɂ͎��̍s�֍s��
        Cursor_y += py;
        return;
    }
    //�b��I��7�Z�O�t�H���g��:�̉����k�߂�
//    if ((c == ':') && (CurrentFont.data == Font7segData)) fontXsize = 3;
    
    //�t�H���g�f�[�^�̐擪�ʒu���v�Z�@�@1�����̃f�[�^�́AX�����̃o�C�g�� x Y�T�C�Y
//    pos = (uint16_t) Fstart + (c - CurrentFont.data[Foffset]) * ((fontXsize+7)/8) * fontYsize;
    pos = (uint16_t) Fstart + (c - CurrentFont.data[Foffset]) * CurrentFont.data[Fbyte];
    //�t�H���g�f�[�^��1�h�b�g���`��
    yp = Cursor_y;
    for (jj = 0; jj < CurrentFont.ysize; jj++) {
        xp = Cursor_x;
        for (ii = 0; ii < CurrentFont.xsize; ii++) {
            if ((ii % 8) == 0) {
                if (c == ' ') line = 0; //SmallFont�̋󔒃R�[�h��ASCII�ƈႤ�̂ŁA�����ő΍�
                else line = CurrentFont.data[pos++]; //8�h�b�g���Ƀf�[�^����荞��
            }
            if (line & 0x80) {
                display_fillRect(xp, yp, xmul, ymul, TextColor);
            } else {
                // 0�̕`��
                if (TextbgColor != TextColor) {
                    // Back�J���[��front�J���[�ƈႤ���́A���̐F��w�i�Ƃ��ēh��
                    display_fillRect(xp, yp, xmul, ymul, TextbgColor);
                }
            }
            line <<= 1;
            xp += xmul;
        }
        yp += ymul;
    }

    //���̕����̈ʒu��ݒ�B���s���������{
//    if ((c == ':') && (CurrentFont.data == Font7segData)) px = 4 * xmul + XpitchAdj;    //7seglike��:�̕�����������@�b��΍�
//    else 
        px = CurrentFont.xpitch * xmul + CurrentFont.XpitchAdj;
    
    Cursor_x += px;
//    DEBUG_PRINTF("%d\r\n", Cursor_x);
    if (Wrap && (Cursor_x + px > LCD_WIDTH)) {
        //���̕��������肫��Ȃ���
        Cursor_x = 0;
        Cursor_y += py;
        if (Cursor_y > LCD_HEIGHT) Cursor_y = LCD_HEIGHT;
    }
//    if (Cursor_x > LCD_WIDTH) Cursor_x = LCD_WIDTH;

}


/*
 * �ݒ肳��Ă���J�[�\���ʒu�A�F�A�T�C�Y�����̂܂܎g�����͂�����Ăяo��
 * ����������Ή�
 */
void display_puts(char *s) {
    uint16_t code;

    if (CurrentFont.data) {
        //�����t�H���g�A�����t�H���g�̕`��
        while (*s) {
//            if (*s < 0x80) {
                display_putc(*s++);  //
//            }
        }
    } else {
        //�J�X�^���t�H���g�̕`��
        displayCustom_puts(s);
    }

}


/**************************************************************************
   @brief   Draw a single character
    @param    x   Bottom left corner x coordinate
    @param    y   Bottom left corner y coordinate
    @param    c   The 8-bit font-indexed character (likely ascii)  shift jis can be accepted
    @param    color 16-bit 5-6-5 Color to draw chraracter with
    @param    bg 16-bit 5-6-5 Color to fill background with (if same as color, no background)
    @param    size  Font magnification level, 0x11 is 'original' size
                          �c�� x�{������p�����[�^
 
 * ������Ŏw��̏ꏊ�ɕ�����\�����������ɗ��p
 * �J�[�\���ʒu�A�F�A�T�C�Y�́A�ʊ֐��ňӎ��I�ɐݒ肵�����̂��A�����ŏ���ɕς���Ă��܂�Ȃ��悤�Ɍ��ɖ߂��Ă���
 * �s�b�`�����ύX���Ă��Ȃ�
 **************************************************************************/
void display_drawChars(int16_t x, int16_t y, char *s, uint16_t color, uint16_t bg, uint8_t size) {
  int16_t prev_x     = Cursor_x,
          prev_y     = Cursor_y;
  uint16_t prev_color = TextColor,
           prev_bg    = TextbgColor;
  uint8_t prev_mag  = (uint8_t)((CurrentFont.MagX << 4) + CurrentFont.MagY);

  display_setCursor(x, y);
  display_setTextColor(color, bg);
  display_setTextSize(size);
  
  display_puts(s);  //������ύX
  
  display_setCursor(prev_x, prev_y);
  display_setTextColor(prev_color, prev_bg);
  display_setTextSize(prev_mag);
}

/**************************************************************************
   @brief    Draw a perfectly horizontal line
   @param    x   Left-most x coordinate
   @param    y   Left-most y coordinate
   @param    w   Width in pixels
   @param    color 16-bit 5-6-5 Color to fill with
 **************************************************************************/
void drawHLine(int16_t x, int16_t y, int16_t w, uint16_t color) {
   
    if ( (x < LCD_WIDTH) && (y < LCD_WIDTH) && w) {
        if ((x + w - 1) >= LCD_WIDTH) w = LCD_WIDTH  - x;
        
        if (!SPI1_Open(LCD16M_CONFIG)) return;
        //�����ƊJ������
        LCD_CS_SetLow();    //CS��Low��
        lcd_addset((uint16_t)x, (uint16_t)y,(uint16_t)(x + w -1), (uint16_t)y);
        SPI1_ExchangeNData(color, (uint16_t)w);
        LCD_CS_SetHigh();    //CS��High��
        SPI1_Close();
    }
}

/**************************************************************************
   @brief    Draw a perfectly vertical line
   @param    x   Top-most x coordinate
   @param    y   Top-most y coordinate
   @param    h   Height in pixels
   @param    color 16-bit 5-6-5 Color to fill with
 **************************************************************************/
void drawVLine(int16_t x, int16_t y, int16_t h, uint16_t color) {

    if ( (x < LCD_WIDTH) && (y < LCD_WIDTH) && h) {
        if ((y + h - 1) >= LCD_WIDTH) h = LCD_WIDTH - y;

        if (!SPI1_Open(LCD16M_CONFIG)) return;
        //�����ƊJ������
        LCD_CS_SetLow();    //CS��Low��
        lcd_addset((uint16_t)x, (uint16_t)y, (uint16_t)x, (uint16_t)(y + h -1));
        SPI1_ExchangeNData(color, (uint16_t)h);
        LCD_CS_SetHigh();    //CS��High��
        SPI1_Close();
    }
}

/**************************************************************************
   @brief    Write a line.  Bresenham's algorithm - thx wikpedia
   @param    x0  Start point x coordinate
   @param    y0  Start point y coordinate
   @param    x1  End point x coordinate
   @param    y1  End point y coordinate
   @param    color 16-bit 5-6-5 Color to draw with
 **************************************************************************/
void writeLine(int16_t x0, int16_t y0, int16_t x1, int16_t y1, uint16_t color) {
    bool steep = ABS((int16_t)(y1 - y0)) > ABS((int16_t)(x1 - x0));
    int16_t dx, dy, err, ystep;
    
    if (steep) {
        _swap_int16_t(x0, y0);
        _swap_int16_t(x1, y1);
    }

    if (x0 > x1) {
        _swap_int16_t(x0, x1);
        _swap_int16_t(y0, y1);
    }

    dx = x1 - x0;
    dy = ABS((int16_t)(y1 - y0));

    err = dx / 2;

    if (y0 < y1) {
        ystep = 1;
    } else {
        ystep = -1;
    }

    for (; x0<=x1; x0++) {
        if (steep) {
            lcd_draw_pixel_at(y0, x0, color);
        } else {
            lcd_draw_pixel_at(x0, y0, color);
        }
        err -= dy;
        if (err < 0) {
            y0 += ystep;
            err += dx;
        }
    }
}

/**************************************************************************
   @brief    Draw a line
   @param    x0  Start point x coordinate
   @param    y0  Start point y coordinate
   @param    x1  End point x coordinate
   @param    y1  End point y coordinate
   @param    color 16-bit 5-6-5 Color to draw with
 **************************************************************************/
void display_drawLine(int16_t x0, int16_t y0, int16_t x1, int16_t y1, uint16_t color) {

    if (x0 == x1) {
        if (y0 > y1) _swap_int16_t(y0, y1);
        drawVLine(x0, y0, y1 - y0 + 1, color);
    } else if (y0 == y1) {
        if (x0 > x1) _swap_int16_t(x0, x1);
        drawHLine(x0, y0, x1 - x0 + 1, color);
    } else {
        writeLine(x0, y0, x1, y1, color);
    }
}


/**************************************************************************
   @brief   Draw a triangle with no fill color
    @param    x0  Vertex #0 x coordinate
    @param    y0  Vertex #0 y coordinate
    @param    x1  Vertex #1 x coordinate
    @param    y1  Vertex #1 y coordinate
    @param    x2  Vertex #2 x coordinate
    @param    y2  Vertex #2 y coordinate
    @param    color 16-bit 5-6-5 Color to draw with
 **************************************************************************/
void display_drawTriangle(int16_t x0, int16_t y0, int16_t x1, int16_t y1,
        int16_t x2, int16_t y2, uint16_t color) {
    display_drawLine(x0, y0, x1, y1, color);
    display_drawLine(x1, y1, x2, y2, color);
    display_drawLine(x2, y2, x0, y0, color);
}

/*******************************************************************************
 * This function will draw a filled rect from (x0,y0) to (x1,y1) with color
 */
void lcd_fill_rect(int16_t x0, int16_t y0, int16_t x1, int16_t y1, uint16_t color) {
    int16_t num;

    if ( (y0 >= LCD_HEIGHT) || (x0 >= LCD_WIDTH) ) return;
    if ( x1 >= LCD_WIDTH ) x1 = LCD_WIDTH -1;
    if ( y1 >= LCD_HEIGHT ) y1 = LCD_HEIGHT -1;

    if (!SPI1_Open(LCD16M_CONFIG)) return;
    //�����ƊJ������
    LCD_CS_SetLow();    //CS��Low��
    lcd_addset((uint16_t)x0, (uint16_t)y0, (uint16_t)x1, (uint16_t)y1); //����ŕ`��͈͂𐧌�����
    num = y1 - y0 +1;
    while (num > 0) {
        SPI1_ExchangeNData(color, (uint16_t)(x1 - x0 + 1));
        num--;
    }
    
    LCD_CS_SetHigh();    //CS��High��
    SPI1_Close();
} 
/**************************************************************************
   @brief    Fill a rectangle completely with one color. Update in subclasses if desired!
    @param    x   Top left corner x coordinate
    @param    y   Top left corner y coordinate
    @param    w   Width in pixels
    @param    h   Height in pixels
    @param    color 16-bit 5-6-5 Color to fill with
 **************************************************************************/
void display_fillRect(int16_t x, int16_t y, int16_t w, int16_t h, uint16_t color) {

    if (w && h) {   // Nonzero width and height?
        lcd_fill_rect(x, y, x+w-1, y+h-1, color);
    }
}

/*******************************************************************************
 * This function fill the hole TFT with a user defined Color.
 * ��ʃN���A
 */
void lcd_fill(uint16_t color) {
    uint16_t num;
    
    lcd_fill_rect(0, 0, LCD_WIDTH -1, LCD_HEIGHT -1, color);
}


/**************************************************************************
   @brief   Draw a rectangle with no fill color
    @param    x   Top left corner x coordinate
    @param    y   Top left corner y coordinate
    @param    w   Width in pixels
    @param    h   Height in pixels
    @param    color 16-bit 5-6-5 Color to draw with
 **************************************************************************/
void display_drawRect(int16_t x, int16_t y, int16_t w, int16_t h, uint16_t color) {

    drawHLine(x, y, w, color);
    drawHLine(x, y+h-1, w, color);
    drawVLine(x, y, h, color);
    drawVLine(x+w-1, y, h, color);

}

/**************************************************************************
   @brief    Quarter-circle drawer, used to do circles and roundrects
    @param    x0   Center-point x coordinate
    @param    y0   Center-point y coordinate
    @param    r   Radius of circle
    @param    cornername  Mask bit #1 or bit #2 to indicate which quarters of the circle we're doing
    @param    color 16-bit 5-6-5 Color to draw with
 **************************************************************************/
void display_drawCircleHelper(int16_t x0, int16_t y0, int16_t r, uint8_t cornername, uint16_t color) {
    int16_t f     = 1 - r;
    int16_t ddF_x = 1;
    int16_t ddF_y = -2 * r;
    int16_t x     = 0;
    int16_t y     = r;

    while (x<y) {
        if (f >= 0) {
            y--;
            ddF_y += 2;
            f     += ddF_y;
        }
        x++;
        ddF_x += 2;
        f     += ddF_x;
        if (cornername & 0x4) {
            lcd_draw_pixel_at(x0 + x, y0 + y, color);
            lcd_draw_pixel_at(x0 + y, y0 + x, color);
        }
        if (cornername & 0x2) {
            lcd_draw_pixel_at(x0 + x, y0 - y, color);
            lcd_draw_pixel_at(x0 + y, y0 - x, color);
        }
        if (cornername & 0x8) {
            lcd_draw_pixel_at(x0 - y, y0 + x, color);
            lcd_draw_pixel_at(x0 - x, y0 + y, color);
        }
        if (cornername & 0x1) {
            lcd_draw_pixel_at(x0 - y, y0 - x, color);
            lcd_draw_pixel_at(x0 - x, y0 - y, color);
        }
    }
}

/**************************************************************************
    @brief  Quarter-circle drawer with fill, used for circles and roundrects
    @param  x0       Center-point x coordinate
    @param  y0       Center-point y coordinate
    @param  r        Radius of circle
    @param  corners  Mask bits indicating which quarters we're doing
    @param  delta    Offset from center-point, used for round-rects
    @param  color    16-bit 5-6-5 Color to fill with
 **************************************************************************/
void display_fillCircleHelper(int16_t x0, int16_t y0, int16_t r, 
        uint8_t corners, int16_t delta, uint16_t color) {
    
    int16_t f     = 1 - r;
    int16_t ddF_x = 1;
    int16_t ddF_y = -2 * r;
    int16_t x     = 0;
    int16_t y     = r;
    int16_t px    = x;
    int16_t py    = y;

    delta++; // Avoid some +1's in the loop

    while(x < y) {
        if (f >= 0) {
            y--;
            ddF_y += 2;
            f     += ddF_y;
        }
        x++;
        ddF_x += 2;
        f     += ddF_x;
        // These checks avoid double-drawing certain lines, important
        // for the SSD1306 library which has an INVERT drawing mode.
        if(x < (y + 1)) {
            if(corners & 1) drawVLine(x0+x, y0-y, 2*y+delta, color);
            if(corners & 2) drawVLine(x0-x, y0-y, 2*y+delta, color);
        }
        if(y != py) {
            if(corners & 1) drawVLine(x0+py, y0-px, 2*px+delta, color);
            if(corners & 2) drawVLine(x0-py, y0-px, 2*px+delta, color);
            py = y;
        }
        px = x;
    }
}

/**************************************************************************
   @brief    Draw a circle with filled color
   @param    x0   Center-point x coordinate
   @param    y0   Center-point y coordinate
   @param    r   Radius of circle
   @param    color 16-bit 5-6-5 Color to fill with
 **************************************************************************/
void display_fillCircle(int16_t x0, int16_t y0, int16_t r, uint16_t color) {
    
    drawVLine(x0, y0-r, 2*r+1, color);
    display_fillCircleHelper(x0, y0, r, 3, 0, color);
}


/**************************************************************************
   @brief    Draw a circle outline
   @param    x0   Center-point x coordinate
   @param    y0   Center-point y coordinate
   @param    r   Radius of circle
   @param    color 16-bit 5-6-5 Color to draw with
 **************************************************************************/
void display_drawCircle(int16_t x0, int16_t y0, int16_t r, uint16_t color) {
    int16_t f = 1 - r;
    int16_t ddF_x = 1;
    int16_t ddF_y = -2 * r;
    int16_t x = 0;
    int16_t y = r;

    lcd_draw_pixel_at(x0  , y0+r, color);
    lcd_draw_pixel_at(x0  , y0-r, color);
    lcd_draw_pixel_at(x0+r, y0  , color);
    lcd_draw_pixel_at(x0-r, y0  , color);

    while (x < y) {
        if (f >= 0) {
            y--;
            ddF_y += 2;
            f += ddF_y;
        }
        x++;
        ddF_x += 2;
        f += ddF_x;

        lcd_draw_pixel_at(x0 + x, y0 + y, color);
        lcd_draw_pixel_at(x0 - x, y0 + y, color);
        lcd_draw_pixel_at(x0 + x, y0 - y, color);
        lcd_draw_pixel_at(x0 - x, y0 - y, color);
        lcd_draw_pixel_at(x0 + y, y0 + x, color);
        lcd_draw_pixel_at(x0 - y, y0 + x, color);
        lcd_draw_pixel_at(x0 + y, y0 - x, color);
        lcd_draw_pixel_at(x0 - y, y0 - x, color);
    }
}


/**************************************************************************
   @brief   Draw a rounded rectangle with no fill color
    @param    x   Top left corner x coordinate
    @param    y   Top left corner y coordinate
    @param    w   Width in pixels
    @param    h   Height in pixels
    @param    r   Radius of corner rounding
    @param    color 16-bit 5-6-5 Color to draw with
 **************************************************************************/
void display_drawRoundRect(int16_t x, int16_t y, int16_t w, int16_t h, int16_t r, uint16_t color) {
    // smarter version
    int16_t max_radius = ((w < h) ? w : h) / 2; // 1/2 minor axis
    if (r > max_radius) r = max_radius;
    
    drawHLine(x+r  , y    , w-2*r, color); // Top
    drawHLine(x+r  , y+h-1, w-2*r, color); // Bottom
    drawVLine(x    , y+r  , h-2*r, color); // Left
    drawVLine(x+w-1, y+r  , h-2*r, color); // Right
    // draw four corners
    display_drawCircleHelper(x+r    , y+r    , r, 1, color);
    display_drawCircleHelper(x+w-r-1, y+r    , r, 2, color);
    display_drawCircleHelper(x+w-r-1, y+h-r-1, r, 4, color);
    display_drawCircleHelper(x+r    , y+h-r-1, r, 8, color);
    
}

/**************************************************************************
   @brief   Draw a rounded rectangle with fill color
   @param    x   Top left corner x coordinate
   @param    y   Top left corner y coordinate
   @param    w   Width in pixels
   @param    h   Height in pixels
   @param    r   Radius of corner rounding
   @param    color 16-bit 5-6-5 Color to draw/fill with
 **************************************************************************/
void display_fillRoundRect(int16_t x, int16_t y, int16_t w, int16_t h, int16_t r, uint16_t color) {
    int16_t max_radius = ((w < h) ? w : h) / 2; // 1/2 minor axis

    if (r > max_radius) r = max_radius;
    
    // smarter version
    lcd_fill_rect(x+r, y, x+r+w-2*r-1, y+h-1, color);
    // draw four corners
    display_fillCircleHelper(x+w-r-1, y+r, r, 1, h-2*r-1, color);
    display_fillCircleHelper(x+r    , y+r, r, 2, h-2*r-1, color);
    
}

//�������牺�́A�ʂ̃\�[�X�B�@����A�ǂ������ȃ��[�`������荞�ޗ\��Ƃ��āA�R�����g��
/*
This is the core graphics library for all our displays, providing a common
set of graphics primitives (points, lines, circles, etc.).  It needs to be
paired with a hardware-specific library for each display device we carry
(to handle the lower-level functions).

Adafruit invests time and resources providing this open source code, please
support Adafruit & open-source hardware by purchasing products from Adafruit!

Copyright (c) 2013 Adafruit Industries.  All rights reserved.

Redistribution and use in source and binary forms, with or without
modification, are permitted provided that the following conditions are met:

- Redistributions of source code must retain the above copyright notice,
  this list of conditions and the following disclaimer.
- Redistributions in binary form must reproduce the above copyright notice,
  this list of conditions and the following disclaimer in the documentation
  and/or other materials provided with the distribution.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE
LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
POSSIBILITY OF SUCH DAMAGE.
 */
///////////////////////////////////////////////////////////////////////////////

////************************* Non User Functions *************************//
//void writeLine(uint16_t x0, uint16_t y0, uint16_t x1, uint16_t y1, uint16_t color);
//uint8_t printNumber(uint32_t n, int8_t n_width, uint8_t _flags);
//void printFloat(float float_n, int8_t f_width, int8_t decimal, uint8_t _flags);
//void v_printf(const char *fmt, va_list arp);
////////////////////////////////////////////////////////////////////////////


/**************************************************************************
   @brief     Draw a triangle with color-fill
    @param    x0  Vertex #0 x coordinate
    @param    y0  Vertex #0 y coordinate
    @param    x1  Vertex #1 x coordinate
    @param    y1  Vertex #1 y coordinate
    @param    x2  Vertex #2 x coordinate
    @param    y2  Vertex #2 y coordinate
    @param    color 16-bit 5-6-5 Color to fill/draw with
 **************************************************************************/
void display_fillTriangle(int16_t x0, int16_t y0, int16_t x1, int16_t y1,
  int16_t x2, int16_t y2, uint16_t color) {
    int16_t a, b, y, last, dx01, dy01, dx02, dy02, dx12, dy12;
    //�ȉ��́Aint32_t���������A���ʂ���������������
    //�ȉ��̂悤�ɃL���X�g�����瓮�����悤���A���Ԃ�32bit�ϐ��s�v�Ɣ��f
    //        b   = (int16_t)x0 +(int16_t) sb / dy02;
    int16_t sa   = 0;
    int16_t sb   = 0;

    // Sort coordinates by Y order (y2 >= y1 >= y0)
    if (y0 > y1) {
        _swap_int16_t(y0, y1); _swap_int16_t(x0, x1);
    }
    if (y1 > y2) {
        _swap_int16_t(y2, y1); _swap_int16_t(x2, x1);
    }
    if (y0 > y1) {
        _swap_int16_t(y0, y1); _swap_int16_t(x0, x1);
    }
    
    if(y0 == y2) { // Handle awkward all-on-same-line case as its own thing
        a = b = x0;
        if(x1 < a)      a = x1;
        else if(x1 > b) b = x1;
        if(x2 < a)      a = x2;
        else if(x2 > b) b = x2;
        drawHLine(a, y0, b-a+1, color);
        return;
    }
    
    dx01 = x1 - x0;
    dy01 = y1 - y0;
    dx02 = x2 - x0;
    dy02 = y2 - y0;
    dx12 = x2 - x1;
    dy12 = y2 - y1;
    
    // For upper part of triangle, find scanline crossings for segments
    // 0-1 and 0-2.  If y1=y2 (flat-bottomed triangle), the scanline y1
    // is included here (and second loop will be skipped, avoiding a /0
    // error there), otherwise scanline y1 is skipped here and handled
    // in the second loop...which also avoids a /0 error here if y0=y1
    // (flat-topped triangle).
    if(y1 == y2) last = y1;   // Include y1 scanline
    else         last = y1-1; // Skip it
    
    for(y=y0; y<=last; y++) {
        a   = x0 + sa / dy01;
        b   = x0 + sb / dy02;
        sa += dx01;
        sb += dx02;
        /* longhand:
         a = x0 + (x1 - x0) * (y - y0) / (y1 - y0);
         b = x0 + (x2 - x0) * (y - y0) / (y2 - y0);
         */
        if(a > b) _swap_int16_t(a,b);
        drawHLine(a, y, b-a+1, color);
    }
    // For lower part of triangle, find scanline crossings for segments
    // 0-2 and 1-2.  This loop is skipped if y1=y2.
    sa = dx12 * (y - y1);
    sb = dx02 * (y - y0);
    
    // sprintf�̏�����2�ȏ�̕ϐ��̐ݒ������ƁA2�ڈȍ~�̃f�[�^�����������\�������ꍇ����
    //    sprintf(str, "%d, y=%d", sa, sb, y);
    
    for(; y<=y2; y++) {
        a   = x1 + sa / dy12;
        b   = x0 + sb / dy02;
        sa += dx12;
        sb += dx02;
        /* longhand:
         a = x1 + (x2 - x1) * (y - y1) / (y2 - y1);
         b = x0 + (x2 - x0) * (y - y0) / (y2 - y0);
         */
        if(a > b) _swap_int16_t(a,b);
        drawHLine(a, y, b-a+1, color);
    }

}

/*
 * (x,y)���N�_�ɁAxw,yw���̔��̗̈��data����������ł���
 * 16bit�J���[�̃f�[�^��`��
 */
void glcd_array(int16_t x, int16_t y, int16_t xw, int16_t yw, uint8_t *data)
{
    uint16_t a, b;
    uint16_t *dt;
    
    if (!SPI1_Open(LCD16M_CONFIG)) return;
    //�����ƊJ������
    dt = (uint16_t *)data;
    LCD_CS_SetLow();    //CS��Low��
    lcd_addset((uint16_t)x, (uint16_t)y, (uint16_t)(x+xw-1), (uint16_t)(y+yw-1));
    b = 0;
    a = (uint16_t)(xw * yw);    //�f�[�^���[�h��
    while (b < a) {
        //bmp�f�[�^�̓��g���G���f�B�A�������ALCD�͋t�Ȃ̂ňȉ��̊֐��ŏ�������
        write_wdata(dt[b++]); //���g���G���f�B�A���̃f�[�^�ɑΉ�
    }
    LCD_CS_SetHigh();    //CS��High��
    SPI1_Close();
}

/*
 * LCD�f�B�X�v���C�̏�����
 * �G���[�Ȃ�AEXIT_FAILURE
 * OK�Ȃ�AEXIT_SUCCESS
 */
uint8_t lcd_init(void)
{
    //LCD Reset
    LCD_RESET_SetLow();
    __delay_ms(1);  //Reset pulse duration > 10us
    LCD_RESET_SetHigh();
    __delay_ms(15); // >5ms

    if (!SPI1_Open(LCD16M_CONFIG)) return EXIT_FAILURE;

    //�����ƊJ������
    LCD_CS_SetLow();    //CS��Low��
    // Power control A
    write_command(0xCB);
    SPI1_ExchangeByte(0x39);
    SPI1_ExchangeByte(0x2C);
    SPI1_ExchangeByte(0x00);
    SPI1_ExchangeByte(0x34);
    SPI1_ExchangeByte(0x02);
    
    // Power control B
    write_command(0xCF);
    SPI1_ExchangeByte(0x00);
    SPI1_ExchangeByte(0xC1);   //A2h
    SPI1_ExchangeByte(0x30);
    
    // Driver timing control A
    write_command(0xE8);
    SPI1_ExchangeByte(0x85);   //84h
    SPI1_ExchangeByte(0x00);   //11h
    SPI1_ExchangeByte(0x78);   //7Ah
    
    // Driver timing control B
    write_command(0xEA);
    SPI1_ExchangeByte(0x00);   //66h
    SPI1_ExchangeByte(0x00);
    
    // Power on sequence control
    write_command(0xED);
    SPI1_ExchangeByte(0x64);   //55h
    SPI1_ExchangeByte(0x03);   //01h
    SPI1_ExchangeByte(0x12);   //23h
    SPI1_ExchangeByte(0x81);   //01h
    
    // Pump ratio control
    write_command(0xF7);
    SPI1_ExchangeByte(0x20);   //10h
    
    // Power Control 1
    write_command(0xC0); //Power control
    SPI1_ExchangeByte(0x23); //VRH[5:0]   0x23=4.60V
    
    // Power Control 2
    write_command(0xC1); //Power control
    SPI1_ExchangeByte(0x10); //SAP[2:0];BT[3:0]
    
    // VCOM Control 1
    write_command(0xC5); //VCM control
    SPI1_ExchangeByte(0x3E);   // 4.25V
    SPI1_ExchangeByte(0x28);   // -1.5V
    
    // VCOM Control 2
    write_command(0xC7); //VCM control2
    SPI1_ExchangeByte(0x86); // VCOM offset voltage: VMH ? 58 VML ? 58
    
    // Memory Access Control
    write_command(0x36); // Memory Access Control
    SPI1_ExchangeByte(0xf8); //
    
    // COLMOD: Pixel Format Set
    write_command(0x3A);
    SPI1_ExchangeByte(0x55);   //16 bits/pixel
    
    // Frame Rate Control (In Normal Mode/Full Colors)
    write_command(0xB1);
    SPI1_ExchangeByte(0x00);
    SPI1_ExchangeByte(0x18);   // 79Hz,  0x1B=70Hz (default)
    
    // Display Function Control
    write_command(0xB6); // Display Function Control
    SPI1_ExchangeByte(0x08);
    SPI1_ExchangeByte(0x82);
    SPI1_ExchangeByte(0x27);
    
    // Enable 3G
    write_command(0xF2); // 3Gamma Function Disable
    SPI1_ExchangeByte(0x00);   // Disable 3 gamma control
    
    // Gamma Set
    write_command(0x26); //Gamma curve selected
    SPI1_ExchangeByte(0x01);   // Gammana curve 1 (G2.2))
    
    // Positive Gamma Correction
    write_command(0xE0); //Set Gamma
    SPI1_ExchangeByte(0x0F);
    SPI1_ExchangeByte(0x31);
    SPI1_ExchangeByte(0x2B);
    SPI1_ExchangeByte(0x0C);
    SPI1_ExchangeByte(0x0E);
    SPI1_ExchangeByte(0x08);
    SPI1_ExchangeByte(0x4E);
    SPI1_ExchangeByte(0xF1);
    SPI1_ExchangeByte(0x37);
    SPI1_ExchangeByte(0x07);
    SPI1_ExchangeByte(0x10);
    SPI1_ExchangeByte(0x03);
    SPI1_ExchangeByte(0x0E);
    SPI1_ExchangeByte(0x09);
    SPI1_ExchangeByte(0x00);
    
    // Negative Gamma Correction
    write_command(0xE1); //Set Gamma
    SPI1_ExchangeByte(0x00);
    SPI1_ExchangeByte(0x0E);
    SPI1_ExchangeByte(0x14);
    SPI1_ExchangeByte(0x03);
    SPI1_ExchangeByte(0x11);
    SPI1_ExchangeByte(0x07);
    SPI1_ExchangeByte(0x31);
    SPI1_ExchangeByte(0xC1);
    SPI1_ExchangeByte(0x48);
    SPI1_ExchangeByte(0x08);
    SPI1_ExchangeByte(0x0F);
    SPI1_ExchangeByte(0x0C);
    SPI1_ExchangeByte(0x31);
    SPI1_ExchangeByte(0x36);
    SPI1_ExchangeByte(0x0F);
    
    write_command(0x11); //Sleep Out
    __delay_ms(10); // It will be necessary to wait 5ms before sending next command
    write_command(0x29); //Display ON
    
//    write_command(0x2c);    //Memory Write���[�h�A�ɂ��Őݒ肷��K�v�Ȃ��̂ō폜
    
    LCD_CS_SetHigh();    //CS��High��
    SPI1_Close();
    
    return EXIT_SUCCESS;
}

// end of library code.
