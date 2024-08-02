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
    
//EEPROM内データ配置(0x00-0x3ff)　　電源落ちた後、ここから設定情報読み出す
#define AddressInit         0x00    //1バイト　　初期値FF、書き込んだら55
#define AddressDisplayMode  0x07    //1バイト
//日付
#define AddressYMD          0x08    //4バイト　曜日、日、月、年の順
#define AddressAlarm        0x0c    //3バイト　分、時、曜日の順
//#define AddressAlarmNo      0x0f     //1バイト　　アラームパターンの変更用
#define AddressTouch        0x18    //8バイト　　タッチ補正用の2か所の座標
#define AddressCustom       0x20    //カスタム情報を保持。現状20バイト/object必要(32バイト確保しておく)
    // 0x20- Date   *7オブジェクト
    // 0x40- Time
    // 0x60- Alarm
    // 0x80- Temp
    // 0xA0- Humid
    // 0xC0- Calen
    // 0xE0- Gear
#define AddressFontName    0x100    //7個のユーザフォント名 *9バイト/フォント　0x13Eまで
    
#define hi(a) ((a) >> 8)
#define lo(a) ((a) & 0xff)

//表示モード
typedef enum {
    DisplayMode0,       //0x00    //時計表示大きく、3か月カレンダー表示
    DisplayMode1,       //0x01    //カレンダー大きい表示
    DisplayMode2,       //0x02    //アナログ時計表示
    DisplayCustom,      //0x03    //カスタム表示
    MODENUM,            //        //モードの種類の数が自動的に設定される
} DispMode;

//日付表示の形式、フォーマット情報として使う
enum DateFormat {
    YYYYMMDDw,  //曜日付き
    YYYYMMDDwe,  //英語曜日付き
    YYYYMMDD,
    YYMMDDw,
    YYMMDDwe,
    YYMMDD,
    MMDDw,
    MMDDwe,
    MMDD,
    MAXDateFormat,  //要素数
};

enum TimeFormat {
    HHMM,
    HHMMss,
    HHMMSS,
    ANALOGCLOCK,
    MAXTimeFormat,  //要素数
};

enum AnalogClockFormat {
    Large = 180,
    Mid_L = 160,
    Mid_S = 140,
    Small = 120,
    Tiny  = 100,
    MAXAnalogSize,  //要素数
};

enum AlarmFormat {
    ALMAMPM,  //AM/PM表記
    ALM24,    //24時間表記
    iALAMPM,  //AM/PM表記　アイコン
    iAL24,    //24時間表記　アイコン
    MAXAlarmFormat, //要素数
};

//温度表示の形式
enum TempFormat {
    DEGC,
    DEGF,
    MAXTempFormat,  //要素数
};

//カレンダーの表示
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
    MAXCalendarFormat,  //要素数
};


//オブジェクトを表示するかどうか
typedef struct {
    uint8_t disp;       //表示させるかどうか
    int16_t x, y;       //最初の文字の左上の座標
    int16_t xw, yw;     //オブジェクトのX, Yサイズ　　基本的に不要だが、ブラウザ対応時に利用
    uint8_t fontcode;   //フォントを指定する番号　FontCodeで定義されている数値
    uint8_t fontmag;    //表示する時、フォントの倍率 attributeに相当する情報
    uint16_t color;     //色 RGB-565形式
    uint16_t format;     //表示形式
    uint16_t attribute;  //何らかの追加情報　Saturday color
    uint16_t attribute2; //追加情報　　　　　Sunday color
    int8_t xpitchAdj;    //フォントのXピッチの調整量　デフォルトピッチから変更したい時
//    int8_t ypitchAdj;    //フォントのYピッチ　基本的に1行表示なので、不要
    char *fontName;     //fontcode=EXTFONTCODE時:カスタムフォントの名前=フォルダ名へのポインタ
} ObjectRsc;

//下記オブジェクトの順にリソースを設定する
enum RscObject {
    DateObj,        //0
    TimeObj,        //1
    AlarmObj,       //2
    TempObj,        //3
    HumidityObj,    //4
    CalendarObj,    //5
    GearObj,        //6
    OBJECTNUM,      //定義されているオブジェクトの数
};

/* フォントコード:
 * 内蔵フォントは、3つ
 * 外部フォントの場合、FontExtを指定して、実際に使用するフォント名は、
 * 各オブジェクトにの1:1で対応しているCustomFontDirに格納
 */
#define EXTFONTCODE  10
enum FontCode {
    FontNormal = 1, //8x8
    FontSmall,      //6x8
    Font7like,      //5x7 7セグもどき内蔵フォント　SDカードに頼らないよう用意
    FontGear,       //32x32　ギアのみのフォント
    //ここから外部フォント
    FontExt = EXTFONTCODE,
};

extern ObjectRsc CustomRsc[];
extern ObjectRsc *RscData[];


//イベントの番号
enum EventNum {
    EventNone,      //=0
    EventTimeUpdate,    //1秒ごとに発生  時刻の更新があったら
    EventSlideSWoff,
    EventSlideSWon,
    EventAlarm,   //割込みで音鳴らすので、イベントとしては不要→ロータリー回してアラーム時刻変更した時に発生
    EventTouchDown, //最初にタッチと認識した時に発生するイベント
    EventTouchUp,   //タッチをやめた時に発生するイベント　Longかどうかで処理変更するならここか？
    EventTouchLong,
    EventTouchRepeat,   //押しっぱなしの時
    EventRotary,
    EventFormInit,
    EventFormUpdate,
    EventGetTime,
    EventDateUpdate,    //日付変更時　    日付の表示更新必要な時。カレンダーの表示も
//    EventAlarmUpdate,   //アラーム時刻変更時
    EventTemperatureUpdate, //温度の更新あったら
    EventHumidityUpdate,    //湿度の更新あったら
    EventCalendarUpate,     //カレンダー　必要あるか？
    EventColorFormInit,
    EventColorFormUpdate,
    EventXYFormInit,
    
    EventFormInitSave,  //カスタムデータの保存、読み出し用
    EventFormInitLoad,
    EventFormClose,
    
};

//RTC処理用
//Sec, Min, Hour, Wday, Day, Month, Yearを配列化
//値は、BCD10進とすることに注意。先頭は、Secのレジスタアドレス
//初期値:2021/1/1 00:00:00
// 並びは、RTCと合わせて、秒、分、時、曜日、日、月、年
extern uint8_t DateTime[7]; // = {0x00, 0x00, 0x00, 0x00, 0x01, 0x01, 0x21};
extern uint8_t AlarmTime[3]; // = {0, 0, 0x7f};    //mm, hh, wday

extern const char TouchMsg[];
//SDカードからデータを読み出す時に使うバッファ
#define DATASIZE 320    //LCDの320ドット数ライン分　40バイトの倍数が望ましい
extern uint8_t SDcardBuffer[DATASIZE];    //ローカルでは入りきらなくても、Data spaceにはまだ入った

extern int8_t Accel;   //高速回転させた時に、1クリックをAccel倍にさせる
extern int16_t TouchX, TouchY;    //ディプレイ座標に換算したタッチ座標
extern uint8_t TouchCount;     //長押し判定用に使用。タッチ直後に0になる
extern uint8_t TouchStart;
extern uint16_t PixelData[9];   //3x3のピクセルデータを保存する
extern int16_t PixelXY[2];
extern int8_t RotCount; //回した回数(クリックありのロータリーだと、何回クリックさせたか)
//SlideSWの状態変化に使う
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
