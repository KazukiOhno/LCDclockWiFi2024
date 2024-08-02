/* 
 * File:   LCD320x240color.h
 * Author: K.Ohno
 *
 * Created on December 19, 2020, 5:18 PM
 */

#ifndef LCD320X240COLOR_H
#define	LCD320X240COLOR_H

#ifdef	__cplusplus
extern "C" {
#endif

#include "mcc_generated_files/mcc.h"
#include "font.h"

/*******************************************************************************
 * Usefull Makros for the LCD-Functions
 * ここからは、PIC-Projekte (https://pic-projekte.de/blog/ili9341-bibliothek/)
 * が提供してくれていたGithubから持ってきたもの
 * 
 * ILI9341データシートにより、SCL=100ns minということなので、SPIで10MHzまでしか
 * 対応できない。16MHzの設定で動作しなくて、1日悩んだ
 * その後、SLR (Slew Rate)の設定を変更することで16MHz動作ができたことを確認
 */

// LCDディスプレイのサイズ
#define LCD_WIDTH   320
#define LCD_HEIGHT  240

//16bitカラーの設定
#define WHITE       0xFFFF
#define SILVER      0xC618
#define BLACK       0x0000
#define GREY        0x8410
#define RED         0xF800
#define MAROON      0x8000
#define FUCHSIA     0xF81F		
#define PURPLE      0x8010
#define LIME        0x07E0
#define GREEN       0x0400
#define YELLOW      0xFFE0
#define OLIVE       0x8400
#define BLUE        0x001F
#define NAVY        0x0010
#define AQUA        0x07FF
#define TEAL        0x0410
#define DARK_BLUE   0x0002
#define ROYALBLUE   0x3b5b
#define CYAN        0x05F7  //シアン
#define MAGENTA     0xB817  //マゼンタ
#define LIGHTGREY   0xD69A
#define DARKGREY    0xA554

//フォントサイズ　　整数倍の倍率のみ対応
//16進1バイトで表現。上位4bitで横方向倍率、下位4bitで縦方向倍率
//0x11      //等倍
//0x12      //縦2倍
//0x23      //横2倍縦3倍

extern int16_t
  Cursor_x,          // x location to start printing text
  Cursor_y;          // y location to start printing text
extern uint16_t
  TextColor,    // 16-bit background color for print
  TextbgColor;  // 16-bit text color for print
//extern uint8_t TextMagX, TextMagY;
extern bool Wrap;           ///< If set, 'Wrap' text at right edge of display
//extern int8_t XpitchAdj;    //ピッチの調整
//extern int8_t YpitchAdj;

extern FIL FileFont;   //フォントファイル用
//extern uint8_t KanjiOffsetTable[256];  //ファイル内の漢字データのありかを示すテーブル


// PIC18は、Intel CPUと同様にリトルエンディアン
    
void display_setColor(uint16_t c);
void display_setTextColor(uint16_t c, uint16_t bg);
void display_setTextSize(uint8_t mag);
void display_setTextPitchAdj(int8_t adjX, int8_t adjY);
void display_setWrap(bool w);
void display_setCursor(int16_t x, int16_t y);

//内部専用は、lcd_とかのヘッダなし　　CSの設定をしないと動作しない
void addset(int16_t x, int16_t y, int16_t xx, int16_t yy);

/**
 * Set the cursor at position (x,y)
 * @param x X-Position [0..319] depend on LCD_WIDTH
 * @param y Y-Position [0..239] depend on LCD_HEIGHT
 */
uint8_t lcd_set_cursor(uint16_t x, uint16_t y);
uint8_t lcd_set_cursor_x(uint16_t x);

/*
 * Draw a Pixel at the current position
 * @param color The foreground color of the pixel
 */
uint8_t draw_pixel(uint16_t color);
uint16_t read_pixel(uint8_t *rgbdata, uint8_t num);
uint16_t read_pixel2nd(uint8_t *rgbdata, uint8_t num);

uint16_t Getcolor565(uint8_t red, uint8_t green, uint8_t blue);
uint16_t Getcolor565_8(uint8_t red, uint8_t green, uint8_t blue);

//***************************************************************************
//void drawHLine(int16_t x, int16_t y, int16_t w, uint16_t color);
//void drawVLine(int16_t x, int16_t y, int16_t h, uint16_t color);
//void writeLine(int16_t x0, int16_t y0, int16_t x1, int16_t y1, uint16_t color);
/**************************************************************************
   @brief    Draw a line
   @param    x0  Start point x coordinate
   @param    y0  Start point y coordinate
   @param    x1  End point x coordinate
   @param    y1  End point y coordinate
   @param    color 16-bit 5-6-5 Color to draw with
 **************************************************************************/
void display_drawLine(int16_t x0, int16_t y0, int16_t x1, int16_t y1, uint16_t color);

/**
 * Draw a circle with the center at (xm,ym) and the radius (r)
 * @param x0
 * @param y0
 * @param r
 * @param color
 */
void display_drawCircle(int16_t x0, int16_t y0, int16_t r, uint16_t color);

/**
 * Draw a filled circle with the center at (xm,ym) and the radius (r)
 * @param x0
 * @param y0
 * @param r
 * @param color
 */
void display_fillCircle(int16_t x0, int16_t y0, int16_t r, uint16_t color);


/**
 * This function will draw a Pixel at (x,y)
 * @param x X-Coordinate
 * @param y Y-Coordinate
 * @param color Foreground color of the Pixel
 */
void lcd_draw_pixel_at(int16_t x, int16_t y, uint16_t color);

/**
 * This function will draw a filled rect from (x0,y0) to (x1,y1)
 * @param x0 Start-X-Coordinate
 * @param y0 Start-Y-Coordinate
 * @param x1 End-X-Coordinate
 * @param y1 End-Y-Coordinate
 * @param color Foregroundcolor of the rect
 */
void lcd_fill_rect(int16_t x0, int16_t y0, int16_t x1, int16_t y1, uint16_t color);
void display_fillRect(int16_t x, int16_t y, int16_t w, int16_t h, uint16_t color);
void display_fillRoundRect(int16_t x, int16_t y, int16_t w, int16_t h, int16_t r, uint16_t color);
/**
 * This function fill the hole TFT with a user defined Color.
 * @param color color
 */
void lcd_fill(uint16_t color);

void display_putc(char c);
void display_puts(char *s); //display_drawCharsと違い文字列をどんどん書ける。改行も対応
void display_drawChars(int16_t x, int16_t y, char *s, uint16_t color, uint16_t bg, uint8_t size);
//void display_putK(uint16_t c);


//*************************** User Functions ***************************//
void display_drawRect(int16_t x, int16_t y, int16_t w, int16_t h, uint16_t color);
void display_drawTriangle(int16_t x0, int16_t y0, int16_t x1, int16_t y1, int16_t x2, int16_t y2, uint16_t color);
void display_fillTriangle(int16_t x0, int16_t y0, int16_t x1, int16_t y1, int16_t x2, int16_t y2, uint16_t color);
void display_drawRoundRect(int16_t x0, int16_t y0, int16_t w, int16_t h, int16_t radius, uint16_t color);


////void display_setTextWrap(bool w);
////void display_printf(const char *fmt, ...);

void glcd_array(int16_t x, int16_t y, int16_t xw, int16_t yw, uint8_t *data);

uint8_t lcd_init(void) ; //LCD初期化

#ifdef	__cplusplus
}
#endif

#endif	/* LCD320X240COLOR_H */
