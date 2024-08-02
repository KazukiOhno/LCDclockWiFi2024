/*
 * File:   Draw.c
 * Author: K.Ohno
 *
 * 描画関係ルーチン
 */

#include "debug.h"
#include "Draw.h"
#include <string.h>
#include "main.h"
#include "RTC8025.h"
#include "LCD320x240color.h"
//#include "Customizing.h"
//#include "font.h"


// 絶対値を取得する関数マクロ定義
#define ABS(x) ((x) < 0 ? -(x) : (x))


const char WeekDays[][4] = {
    "Sun", "Mon", "Tue", "Wed", "Thu", "Fri", "Sat",
};

//時刻表示は、変化のあった所だけにするようにする→アナログ時計のみで使用
uint8_t preTime[3]; //時刻を保持しておき、変化をチェックできるようにする

int16_t CalendarXsize, CalendarYsize;

//int16_t CustomXsize; //カスタマイズの時、領域を指定するために使う　カレンダーのみに使用
//int16_t CustomYsize; //デジタルの時刻表示にも使用

char Smaller[2] = "";  //時刻表示で、bmpフォント指定時、小さいフォントを指定: "s"、それ以外の時、""



/*
 * 16bit RGB=565カラーのbmpファイルを読み込み表示する
 * 
 * 0x0000　(2)	bfType		ファイルタイプ　通常は'BM'
 * 0x0002　(4)	bfSize		ファイルサイズ (byte)
 * 0x0006　(2)	bfReserved1	予約領域　常に 0
 * 0x0008　(2)	bfReserved2	予約領域　常に 0
 * 0x000A　(4)	bfOffBits	ファイル先頭から画像データまでのオフセット (byte)
 * 0x000E　(4)	bcSize		ヘッダサイズ
 * 0x0012　(4)	bcWidth		画像の幅 (ピクセル)
 * 0x0016　(4)	bcHeight	画像の高さ (ピクセル) 正数なら，画像データは下から上へ
 *                                               負数なら，画像データは上から下へ
 * 0x001A　(2)	bcPlanes	プレーン数　常に 1
 * 0x001C　(2)	bcBitCount	1画素あたりのデータサイズ (bit)
 * 
 * ここでは、16bitカラーと決め打ちしている
 * 
 * エラーが発生した時は、FR_OK = 0以外を返す
 */
//BMPヘッダ
#define bfOffBits   0x0a        //ファイル先頭から画像データまでのオフセット (byte)
#define bcWidth     0x12        //ピクセル幅 (サイズ4byte)
#define bcHeight    0x16        //ピクセル高さ (サイズ4byte)
#define bcBitCount  0x1C        //1画素あたりのデータサイズ (bit)
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
    //ファイル名は8文字までサポート
    res = f_open(&FileObject, bmpfile, FA_READ);
    if (res != FR_OK) return res;
    
    //ヘッダ読込
    res = f_read(&FileObject, buffer, 32, &actualLength); //32バイト分読み込む
    if (res != FR_OK) return res;
    if (buffer[bcBitCount] == 16) {
        //16bitカラーのデータの時のみ処理
        x = buffer[bcWidth] + (buffer[bcWidth+1]<<8);   //Width
        y = buffer[bcHeight];                           //Height
        //シークして、データの先頭まで移動
        f_lseek(&FileObject, buffer[bfOffBits]);
        
        //bmpデータは、表示される下の行から始まる
        for (kk = 0; kk < y; kk++) {
            //BUFFERSIZEを考慮したプログラム
            num = (uint16_t)x * 2;  //1行のバイト数=xドット x 16bit(2バイト)
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
 * モノクロbmpを読み込み描画
 * X,Yピッチは、bmpのX,Yサイズ、　拡大サポートが必要かはあとで考える
 * 引数: 描画位置、色指定
 * 
 * フォントのXサイズ: 最大256ドット
 * ch = "FontDir/5.bmp"　のように指定
 * 
 */
//void DrawBmp(char *ch) {
//    uint8_t buffer[32];     //32*8=256ドット分
//    int16_t fontXsize, fontYsize;
//    uint16_t len;
//    int16_t ii, jj;
//    UINT actualLength, nn;
//    FRESULT res;
//    uint16_t num;
//    uint16_t pos;
//    int16_t xp, yp; //座標
//    uint8_t line;
//    FIL FileObject;
//    
//    //ファイル名は8文字までサポート
//    do {
//        res = f_open(&FileObject, ch, FA_READ);
//        if (res != FR_OK) remount();
//    } while (res != FR_OK);
//    
//    //ヘッダ読込
//    f_read(&FileObject, buffer, 32, &actualLength); //32バイト分読み込む
//    if (buffer[bcBitCount] == 1) {
//        //モノクロデータの時のみ処理
//        fontXsize = buffer[bcWidth] + (buffer[bcWidth+1]<<8);   //Width
//        fontYsize = buffer[bcHeight];                           //Height
//        //シークして、データの先頭まで移動
//        f_lseek(&FileObject, buffer[bfOffBits]);
//        
//        //一度に読み込むデータサイズ(バイト数)
//        len = (uint16_t)((fontXsize + 31)/32) *4;        //1ドット行分のバイト数、4バイト単位
//        //例：fontXsize=40ドットの時、データは5バイト、3バイトpaddingして、8バイト/行
//        //    　　      41ドットの時、データは6バイト、2バイトpadding
//
//        //1ドット行ずつ読み出す前提
//        
//        //bmpのデータは、下から並んでいるで、表示させるのは下からというのに注意
//        //画面をはみ出しても正常に描画できるように処理追加
//        //画面内に入るように調整。描画する時に、はみ出していないかチェック要
//        for (jj = 0; jj < fontYsize; jj++) {
//            yp = Cursor_y + (fontYsize - jj - 1);   //
//
//            f_read(&FileObject, buffer, len, &actualLength);
//            
//            pos = 0;
//            //addsetして、毎回x,y座標指定しない方が高速描画
//            addset(Cursor_x, yp, Cursor_x + fontXsize -1, yp + 1);
//            for (ii = 0; ii < fontXsize; ii++) {
//                if ((ii % 8) == 0) line = buffer[pos++]; //8ドット分のデータを取り込む
//                xp = Cursor_x + ii;
//                if (line & 0x80) {
//                    // 1=背景の描画
//                    if (TextbgColor != TextColor) {
//                        // Backカラーがfrontカラーと違う時は、その色を背景として塗る
//                        draw_pixel(TextbgColor);
//                        //背景色を塗らない場合、アドレスが進まない問題がある
//                    } else {
//                        //描画しないアドレスを飛ばして再設定
//                        addset(xp+1, yp, Cursor_x + fontXsize -1, yp + 1);
//                    }
//                } else {
//                    // 0=黒の描画　前景
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
 * フォントファイルあるかを0.bmpの有無で判断
 * ファイルあれば、ヘッダ情報を読み取り、グローバル変数のSDcardBufferに格納
 *  0.bmpデータがあることが前提。ない時は、カスタムフォントないと判断し、FontNormalで描画
 */
uint8_t checkFontFile(char *fontFolder) {
    char filename[30];
    FRESULT res;
    UINT actualLength;
    uint8_t xr, yr;
    FIL file;       //Openingやデータ書き込み用
    
    //0.bmpがある前提で、その大きさを取得する
    sprintf(filename, "/%s/0.bmp", fontFolder);
    if (f_open(&file, filename, FA_READ) != FR_OK) {
        return 1;        //エラーの時
    }
    //ファイルある時は、ヘッダ情報読み取る
    res = f_read(&file, SDcardBuffer, 32, &actualLength); //32バイト分読み込む
    f_close(&file);

    CurrentFont.xsize = SDcardBuffer[bcWidth];     //255ドット1以内と想定
    CurrentFont.ysize = SDcardBuffer[bcHeight];    //255ドット1以内と想定
    CurrentFont.xpitch = CurrentFont.xsize;     //X方向はプロポーショナルなのでその都度
    CurrentFont.ypitch = CurrentFont.ysize;
    
    return 0;   //ファイルあった時、正常終了
}


/*
 * FontCodeで定義されているフォントコードを指定
 * カスタムフォントの場合は、そこからさらにフォントファイルをたどる必要がある
 * 
 * これとSetKanjiFontは、併用する。
 * 漢字交じり表示を行う時は、内蔵フォントとの倍率を計算する都合上、SetKanjiFontを最後に実施
 * カスタムフォント使用時は、漢字を使わないことを前提
 * 　　カスタムフォントは、一部文字しか定義されていないことを前提に、FontNormalで補完させる
 */
uint8_t SetFont(uint8_t fontcode, char *fontname) {
   
    //fontcode==0の時のエラー処理を追加→NormalFontとするのが良いかも
    
    CurrentFont.fontcode = fontcode;
    CurrentFont.MagX = 1;
    CurrentFont.MagY = 1;
    CurrentFont.XpitchAdj = 0;
    CurrentFont.YpitchAdj = 0;

    if (fontcode >= EXTFONTCODE) {
        //その他のカスタムフォントの場合
        CurrentFont.data = NULL;    //内蔵フォントでないことを示す
        CurrentFont.fontName = fontname;
        //フォントファイルのチェック及び、size, pitch, mag情報取得
        if (checkFontFile(fontname) == 0) return 0;

        //フォントファイルがない時などは、ノーマルフォントに戻して、設定継続
        CurrentFont.fontcode = FontNormal;        
    }
    //カスタムフォントがエラーがあった時は、以下の処理を実施
    
    //内蔵フォントの場合
    CurrentFont.data = InternalFontList[fontcode];
    CurrentFont.xsize = CurrentFont.data[Fxsize];
    CurrentFont.ysize = CurrentFont.data[Fysize];
    CurrentFont.xpitch = CurrentFont.data[Fxpitch];
    CurrentFont.ypitch = CurrentFont.data[Fypitch];
//        CurrentFont.kratio = 1;    //倍率は、カスタムフォント設定時に変更される        

        return 0;
}


/*
 * フォントを決めたときに、ヘッダ情報から、基本情報を取得
 * オフセットデータも取得しておく
 * ファイルはオープンのままにする FileFontを介してアクセス可能
 * 
 * fontname: 0:漢字ファイルcloseさせる
 *    Misaki 1    //8x8ドットフォント
 *    SJIS16 2    //16x16ドットフォント
 *    SJIS24 3    //24x24ドットフォント
 * 
 * CurrentKanjiFont.fontcodeで、漢字の有効無効を判断に使う
 */
uint8_t KanjiOffsetTable[256];  //ファイル内の漢字データのありかを示すテーブル

//void SetKanjiFont(uint8_t fontcode) {
//    char header[64];
//    uint16_t headerSize;
//    UINT actualLength;
//    uint8_t xr, yr;
//
//    //既に同じフォントのファイルオープンしていたら何もしない
//    if (CurrentKanjiFont.fontcode == fontcode) return;
//    
//    if (CurrentKanjiFont.fontcode != 0) {
//        //既にファイルを開いていたら、一旦閉じる
//        f_close(&FileFont);
//        CurrentKanjiFont.fontcode = 0;  //一旦、漢字を無効化状態にする
//        CurrentKanjiFont.kratio = 0x11;
//        if (fontcode == 0) {
//            // fontcodeが0の時は、ファイル閉じて終了
//            return;
//        }
//    }
//
//    //SDカードなければ=エラーなら、何もしない
//    // ファイルを開き、データを読み出す
//    //ファイル名は8文字までサポート
//    if (f_open(&FileFont, KanjiFontFile[fontcode], FA_READ ) == FR_OK) {
//        //Openし、ヘッダ読込
//        f_read(&FileFont, header, 64, &actualLength); //64バイト分読み込む
//        headerSize = header[2] + (uint16_t)header[3]*256; //ヘッダのサイズ=64
//        CurrentKanjiFont.xsize = header[0x10]; //フォントのX方向の大きさ。ドット数
//        CurrentKanjiFont.ysize = header[0x11]; //フォントのY方向の大きさ。ドット数
//        CurrentKanjiFont.xpitch = CurrentKanjiFont.xsize;   //ピッチは、フォントサイズと同じにしておく
//        CurrentKanjiFont.ypitch = CurrentKanjiFont.ysize;
//        //高さを基準にノーマルフォントの倍率設定 .kratioはこのフォントの倍率ではないことに注意
//        //8x8の時x11、16x16の時x12、24x24の時x23という感じが良いかも
//        xr = (CurrentKanjiFont.xsize/2 + CurrentFont.xsize/2) / CurrentFont.xsize;
//        if (xr < 1) xr = 1;
//        yr = CurrentKanjiFont.ysize / CurrentFont.ysize;
//        if (yr < 1) yr = 1;
//        CurrentKanjiFont.kratio = (uint8_t)((xr<<4) + yr);
//        
//        CurrentKanjiFont.data = (uint8_t *)KanjiFontFile[fontcode];
//        CurrentKanjiFont.fontcode = fontcode;
//        f_read(&FileFont, KanjiOffsetTable, 256, &actualLength); //256バイト分のテーブル読み込む
//        //このデータをもとに表示文字のデータの位置をすばやく計算させるため、保持しておく
//    }
//}


/*
 * カスタムフォント(bmp形式のデータ)の描画
 * 0x16-0x3Aまでの文字コードに対応することを想定
 * 0x16: 日, 月, 火, 水, 木, 金, 土, ℃, 'F, ■
 * 0x20-0x3Aまでは、通常のASCII
 * ただし、bmpファイル名にASCII1文字を割り振れない文字もあるのでここで定義した名称にする必要あり
 * サポートするデータは、モノクロbmpデータのみとする
 * 
 * 対応する文字は、表示に必要なものに限定して、作成労力を節約してよい。
 * bmpファイルない時は、FontNormalで表示
 * 日付: 0-9 / () 曜日
 * 時刻: 0-9 :    小文字
 * 温度: 0-9 . ℃
 * 湿度: 0-9 %
 * カレンダー: 0-9 曜日
 * 
 * 小文字指定: Smallerを使う　時刻表示において秒だけ小さい文字にするときに利用
 * 
 * エラーの時、1
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
    int16_t xp, yp; //座標
    int16_t px, py; //倍率を取り込んだpitch

    //ASCII並びに。　FSunday=0x16から開始
    const char *charname2[] = {
        "Sun", "Mon", "Tue", "Wed", "Thu", "Fri", "Sat",  "degC", "degF", "", 
        "space", "", "", "", "", "percent", "", "", 
        "(", ")", "", "plus", "", "-",  "dot", "slash", 
        "0", "1", "2", "3", "4", "5", "6", "7", "8", "9", "colon", "alarm", "Gear", "Bell",
    };
    
    //対象外の文字は排除
//    if ((c != Fgear) && (c != Fbell) && (c < FSunday || c > ':')) {
//        return 0;
//    }

    // フォントデータのファイルを開いて、データを読み出す
    // bmpファイルは、一種のプロポーショナルフォント
    // bmpファイルを読んで、X, Yサイズ取得後、データを取得しながら描画
    
    //フォルダ名でフォント種類を指定する
    if ((c >= 'A') && (c <= 'Z')) sprintf(st, "%c", c);
    else if (c == Fgear) sprintf(st, "Gear");   //ギヤの時だけ特別
    else if (c == Fbell) sprintf(st, "Bell");   //アラームベルの時だけ特別
    else if (c >= FSunday) strcpy(st, charname2[c - FSunday]);
    else return 1;
    
    sprintf(filename, "/%s/%s%s.bmp", CurrentFont.fontName, st, Smaller);
    res = f_open(&FileFont, filename, FA_READ);
    //3つのエラーのケースが想定される。対象の文字がない、そもそもフォントdirがない、その他のエラー
    if (res == FR_NO_FILE) {
        //フォルダはあっても対象の文字のファイルがない時は、終了
            return 0;
    } else if (res != FR_OK) {
        //Openできないケースがあった。どういう時に発生するのか不明。
        //Openできずに処理させていて、表示が乱れるので、処理を追加
        //エラーはここで発生していた事例確認
        remount();      //再マウント
        DEBUG_PRINTF("Retry Open error: %s\r\n", filename);
        do {        
            res = f_open(&FileFont, filename, FA_READ);
        } while (res != FR_OK);   //ちゃんとopenできるまでループ

//        if (f_open(&FileFont, filename, FA_READ) != FR_OK) return 10;   //マウントできなかった場合
    }
    
    res = f_read(&FileFont, SDcardBuffer, 32, &actualLength); //header=32バイト分読み込む
    if (res != FR_OK) {
        f_close(&FileFont);
        return 0; //エラーにせずリターン
    }
    //CurrentFontにも0.bmpの大きさが設定されているが、毎回チェックしてプロポーショナル対応
    fontXsize = SDcardBuffer[bcWidth]; //フォントの大きさは、縦横とも255まで
    fontYsize = SDcardBuffer[bcHeight];
    
    //シークして、データの先頭まで移動
    res = f_lseek(&FileFont, SDcardBuffer[bfOffBits]);

    if (res != FR_OK) {
        f_close(&FileFont);
        return 3;
    }

    len = ((fontXsize+31)/32)*4;        //1ドット行分のバイト数、4バイト単位
    //例：fontXsize=40ドットの時、データは5バイト、3バイトpaddingして、8バイト/行
    //    　　      41ドットの時、データは6バイト、2バイトpadding
    
    linenum = (sizeof SDcardBuffer) / len;   //バッファサイズに入るのは何ドット行分か
    //バッファは320バイトあり、2560ドット相当あるので、linenum==0にならず、最低でも8行分は一度に読み込める
    if (linenum >= fontYsize) {
        //バッファに入りきる場合は、ドット行数=Yドットサイズ
        linenum = fontYsize;
    }
    datasize = len * linenum;   //一度に読み込むデータサイズ(バイト数)
    
    //bmpのデータは、下から並んでいるで、表示させるのは下からというのに注意
    //画面をはみ出しても正常に描画できるように処理追加
    //画面内に入るように調整。描画する時に、はみ出していないかチェック要
    
    //下から描画させたい。バッファに入りきらないbmpも対応
    remainingLine = 0;   //読み込む残り行数
    for (jj = 0; jj < fontYsize; jj++) {
        yp = Cursor_y + (fontYsize - jj - 1) * CurrentFont.MagY;
        if (remainingLine == 0) {
            //読み込んだデータを使い切ったら、次のデータを読み込む
            res = f_read(&FileFont, SDcardBuffer, datasize, &actualLength);    //データを読み込む
            if (res != FR_OK) {
                f_close(&FileFont);
                return 44;
            }
            remainingLine = actualLength / len;    //読み込んだデータの行数
            pos = 0;
        }
        //addsetして、毎回x,y座標指定しない方が高速描画
        addset(Cursor_x, yp, Cursor_x + fontXsize * CurrentFont.MagX -1, yp + CurrentFont.MagY);
        for (ii = 0; ii < fontXsize; ii++) {
            if ((ii % 8) == 0) line = SDcardBuffer[pos++]; //8ドット毎にデータを取り込む
            xp = Cursor_x + ii * CurrentFont.MagX;
            if (line & 0x80) {
                // 1の描画 背景
                if (TextbgColor != TextColor) {
                    // Backカラーがfrontカラーと違う時は、その色を背景として塗る
                    if (CurrentFont.MagX * CurrentFont.MagY == 1)
                        draw_pixel(TextbgColor);
                    else
                        display_fillRect(xp, yp, CurrentFont.MagX, CurrentFont.MagY, TextbgColor);
                    //背景色を塗らない場合、アドレスが進まない問題がある
                } else {
                    //描画しないアドレスを飛ばして再設定
                    addset(xp+CurrentFont.MagX, yp, Cursor_x + fontXsize * CurrentFont.MagX -1, yp + CurrentFont.MagY);
                }
            } else {
                // 0=黒の描画　前景
                if (CurrentFont.MagX * CurrentFont.MagY == 1) //拡大していない時
                    draw_pixel(TextColor);
                else
                    display_fillRect(xp, yp, CurrentFont.MagX, CurrentFont.MagY, TextColor);
            }
            line <<= 1;
        }
        remainingLine--;
        pos = pos + 3-((pos-1) % 4); //4バイト単位なので、切り上げる
    }    
    
    f_close(&FileFont);
    
    px = fontXsize * CurrentFont.MagX + CurrentFont.XpitchAdj; //XpitchAdjをフォントのサイズに追加
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
 * 設定されているカーソル位置、色、サイズをそのまま使う時はこれを呼び出す
 * 途中でエラーあったら、中止
 */
uint8_t displayCustom_puts(char *s) {
    uint8_t res;
    char str[100];
//    uint16_t preColor = Textcolor;
    
    while (*s) {
        res = displayCustom_putc(*s++);
       
//        if (res) {
//            //時刻の更新がされない状態の時、どこでエラー発生しているか確認するため
////            DEBUG_PRINTF("err=%d\r\n", res);
//            return 1;  //ここを変更   
//        }
    }
//    Textcolor = preColor;
    return 0;
    
}


/*
 * テキスト表示する前処理
 */
void presetting(ObjectRsc *rsc) {

    SetFont(rsc->fontcode, rsc->fontName);   //フォントの設定
    display_setTextSize(rsc->fontmag);
    display_setTextPitchAdj(rsc->xpitchAdj, 0);
    display_setCursor(rsc->x, rsc->y);        //描画開始座標
    display_setColor(rsc->color);
//    CustomYsize = CurrentFont.ypitch * TextMagY + YpitchAdj;
}


// DateTimeの初期化し、前のモードの状態、特にアナログの針の表示を引きずらないように
void resetPreDateTime(void) {
    uint8_t jj;
    
    for (jj = 0; jj < 3; jj++) preTime[jj] = 0xff;
}

/*
 * リソースデータから以下を取得
 * (x,y)座標を左上の起点
 * 文字の外形より2ドット分大きめの領域を描画域と想定
 * format: HHMMssなど
 * fontmag: フォントサイズ
 *  通常の7セグを指定された時、HHMMssのssは、小さい7セグフォントを使用
 * 　　　　　デフォルトで小さい7セグ指定された時は、同じ大きさ
 * サポートするフォントは、7seg、Custom0とCustom1の3つ
 * 
 */
void dispTime(uint8_t *datetime, ObjectRsc *rsc) {
    char ss[10];
//    int16_t tempYsize;

    //ベースとなるフォントのディレクトリを指定
    //Segment7NumかSegment7sNumを想定している
    presetting(rsc);

    //書き換えが必要な桁だけ描画していたが、描画を効率化して、毎回描画でも問題なくなった
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
        //このルーチンで、Cursor自動的に進む
        display_puts(ss);
        
        //フォントサイズを小さく
        if (rsc->fontcode >= EXTFONTCODE) {
            // Userフォントで、smalllフォント定義ない時の処置が未了
            strcpy(Smaller, "s");
        } else {
            //Y方向だけ1サイズ小さくする
//            display_setTextSize(rsc->fontmag - 0x01); //MagYが0になっても、エラーにならないようになっている
            uint8_t xs = (rsc-> fontmag) >> 4;
            uint8_t ys = (rsc-> fontmag) & 0x0f;
            ys = ys / 2 + 1;
            xs = xs / 2 + 1;
            display_setTextSize((uint8_t)(xs << 4) + ys); //MagYが0になっても、エラーにならないようになっている
            
        }
        sprintf(ss, "%02x", datetime[0]);
        display_puts(ss);
        Smaller[0] = '\0';
    }
    display_setTextPitchAdj(0, 0); //presettingとペアで元に戻す
}


// θ=0~179度まで、sinθの256倍にしたsinテーブル
//　アラーム針を5分単位にした場合、2.5度単位(30度で12分割)が欲しい。1度単位で妥協
//sin(θ): θ=角度
//180度以降は、-sin(θ-180)という形で参照
//cos(θ)=sin(θ+90)で計算
//1度単位のテーブル
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

//degreeで指定された角度=thetaに対応したsinを返す(x256)
int16_t sind(int16_t theta) {
    theta = (theta + 360) % 360;    //thetaがマイナスでも対応させるため

    if (theta >= 180) {
        //-sind(θ-180)
        return -sin_table[(UINT)(theta-180)];          //度単位のテーブル用
    } else {
        return sin_table[(UINT)theta];
    }
}

//degreeで指定された角度=thetaに対応したcosを返す(x256)
int16_t cosd(int16_t theta) {
    theta = theta+90;
    return sind(theta);
}

/*
 * アナログ時計表示
 * mode: 
 * datetime: 日付時間
 * xx, yy: 中心座標
 * size: 外形円の大きさ
 * color: 針の色指定
 */
#define SizeMin  3      //分針の太さ
#define SizeHour 4      //時針の太さ
//#define ColorHour   GREY
//#define ColorMin    GREY
#define ColorSec    RED
#define ColorAlarm  YELLOW

/*
 * アナログ時計のアラーム針を描画/消去
 */
void drawAlarmNeedle(int16_t *x, int16_t *y, uint16_t color) {
    int16_t xd, yd;
    
    //アラーム針描画
    display_drawLine(x[0], y[0], x[1], y[1], color);
    
    xd = (int16_t)(x[1]-x[0]);
    yd = (int16_t)(y[1]-y[0]);
    if ( ABS(yd) > ABS(xd)) {
        //角度により、移動する方向を変え、少し太くする
        display_drawLine(x[0]+1, y[0], x[1]+1, y[1], color);
        display_drawLine(x[0]-1, y[0], x[1]-1, y[1], color);
    } else {
        display_drawLine(x[0], y[0]-1, x[1], y[1]-1, color);
        display_drawLine(x[0], y[0]+1, x[1], y[1]+1, color);
    }
}

//時針、分針の描画/消去
void drawNeedle(int16_t *x, int16_t *y, uint16_t color, uint16_t color2) {
    display_fillTriangle(x[1], y[1], x[2], y[2], x[3], y[3], color);
    display_drawTriangle(x[1], y[1], x[2], y[2], x[3], y[3], color2);
    //反対側の出っ張り
    display_fillTriangle(x[4], y[4], x[2], y[2], x[3], y[3], color);
    display_drawTriangle(x[4], y[4], x[2], y[2], x[3], y[3], color2);

}

//秒針の描画/消去
void drawSecNeedle(int16_t *x, int16_t *y, uint16_t color) {
    display_drawLine(x[1], y[1], x[2], y[2], color);
    display_fillTriangle(x[0], y[0], x[3], y[3], x[4], y[4], color);
   
}

        
//アラーム針の長さは長針の75%
//長針：分針の長さは、目盛の線より3ドット内側
//短針
//秒針
//目盛の長さ=3
//ドットの大きさ=2
void drawAnalogClock(uint8_t *datetime, ObjectRsc *rsc, uint8_t *alarmtime) {
    int16_t rc = rsc->attribute /2; //外形の半径
    int16_t xc = rsc->x + rc;    //時計の中心座標
    int16_t yc = rsc->y + rc;
    uint16_t color = rsc->color;
    uint16_t bcolor;

    int16_t rc1, rcs, rcm, rch, rca;    //時計の目盛、秒針、分針、時針、アラーム
    int16_t rc2;
    int16_t rcs2, rcm2, rcm3, rch2, rch3, rca2;    //時計のサイズ、目盛、秒針、分針、時針、アラーム
    int16_t x[5], y[5];    //座標計算した結果を格納
    int16_t angle;
    uint8_t hh, mm, ss, jj, kk;
    uint8_t almhh, almmm;
    //前回の座標　時針、分針、秒針、アラームを描画するための頂点の座標を保持
    static int16_t phx[5], phy[5], pmx[5], pmy[5], psx[5], psy[5], pax[2], pay[2];
    //今回の座標
    int16_t chx[5], chy[5], cmx[5], cmy[5], csx[5], csy[5], cax[2], cay[2];
    char str[3];
    int8_t minupdate = 0;

    rsc->xw = (int16_t)rsc->attribute; //外形サイズ更新
    rsc->yw = rsc->xw;

    //外形円の描画
    display_drawCircle(xc, yc, rc, color);  //sizeで指定された円
    rc = rc -3;
    display_drawCircle(xc, yc, rc, color);  //一回り小さめの円

    rc1 = rc - 3;        // 一番長い長針の長さ=目盛より内側に設定
    rca = rc1 *3 /4;    //アラーム用の針の長さは、長針の75%
    rch = rc1 *7/10;    //短針の長さは、70%
    rcm = rc1 -5;       //分針の長さは、目盛の線より3ドット内側
    rcs = rc1 -4;       //秒針の長さは、これ

    //アラーム針の描画準備----------------------------------------------------
    //最新のアラーム針の座標を計算
    almmm = Bcd2Hex(alarmtime[0]);
    almhh = Bcd2Hex(alarmtime[1]) % 12;
    angle = almhh * 30 + almmm/2;   //角度に変換
    cax[0] = xc;    //中心座標
    cay[0] = yc;
    cax[1] = xc + rca * sind(angle)/256;    //アラーム針の先の座標
    cay[1] = yc - rca * cosd(angle)/256;
    
    //ちらつきをなくすため、表示に変化ある針だけを消去し、それ以外は上書きする
    //前の表示を消す
    if ((cax[1] == pax[1]) && (cay[1] == pay[1])) {
        //前と座標が変わっていない時は、消さない。最初の描画も同じ座標にしているので消去しない
    } else {
        drawAlarmNeedle(pax, pay, TextbgColor);   //元の秒針を消す
        //描画用の座標を保存
        for (jj=0; jj<2; jj++) {
            pax[jj] = cax[jj];
            pay[jj] = cay[jj];
        }
    }

    //分針の描画準備--------------------------------------------------------
    //分針の位置が変わったら、再描画の前に、元の時針、分針を消す
    mm = Bcd2Hex(datetime[1]);
    if (preTime[1] != datetime[1]) {
        preTime[1] = datetime[1];
        hh = Bcd2Hex(datetime[2]);

        //分針の座標を計算
        angle = mm * 6;     //1分6度
        cmx[1] = xc + rcm * sind(angle)/256;    //針先の座標
        cmy[1] = yc - rcm * cosd(angle)/256;
    
        rcm2 = SizeMin;       //分針の幅
        angle = angle +90;  //針に対して90度の角度
        cmx[2] = xc + rcm2 * sind(angle)/256;
        cmy[2] = yc - rcm2 * cosd(angle)/256;
        cmx[3] = xc - (cmx[2]-xc);
        cmy[3] = yc - (cmy[2]-yc);

        //反対側の出っ張り
        rcm3 = 10;      //出っ張り長さ
        angle = angle +90;  //さらに90度足す
        cmx[4] = xc + rcm3 * sind(angle)/256;
        cmy[4] = yc - rcm3 * cosd(angle)/256;

        //時針の座標計算：分針が移動したら、時針も移動計算 (実際は2分毎)
        //　分のデータも取り込んで時針の角度決める
        hh = hh % 12;
        angle = hh * 30 + mm/2;   //角度に変換 8bit変数ではNG
        
        chx[1] = xc + rch * sind(angle)/256;    //針先の座標
        chy[1] = yc - rch * cosd(angle)/256;
        
        rch2 = SizeHour;    //時針の幅
        angle = angle +90;  //針に対して90度の角度
        chx[2] = xc + rch2 * sind(angle)/256;
        chy[2] = yc - rch2 * cosd(angle)/256;
        chx[3] = xc - (chx[2]-xc);
        chy[3] = yc - (chy[2]-yc);
        
        //反対側の出っ張り
        rch3 = 10;      //出っ張り長さ
        angle = angle +90;  //さらに90度足す
        chx[4] = xc + rch3 * sind(angle)/256;
        chy[4] = yc - rch3 * cosd(angle)/256;
        
        //分が変更になり時間が変わる時は、分の変更と同時に時針の消去
        drawNeedle(phx, phy, TextbgColor, TextbgColor);
        //分針の消去
        drawNeedle(pmx, pmy, TextbgColor, TextbgColor);
        minupdate = 1;  //分針updateした

        //描画座標を保存
        for (jj=1; jj<5; jj++) {
            phx[jj] = chx[jj];
            phy[jj] = chy[jj];
            pmx[jj] = cmx[jj];
            pmy[jj] = cmy[jj];
        }
    }

    //最新の秒針の座標を計算
    if (preTime[0] != datetime[0]) {
        preTime[0] = datetime[0];
        ss = Bcd2Hex(datetime[0]);  //0-59の数値
        angle = ss*6;
        csx[0] = xc;  //中心座標
        csy[0] = yc;
        csx[1] = xc + rcs * sind(angle)/256;
        csy[1] = yc - rcs * cosd(angle)/256;
        
        rcs2 = 20;   //反対側に出っ張る量
        angle = angle +180;
        csx[2] = xc + rcs2 * sind(angle)/256;
        csy[2] = yc - rcs2 * cosd(angle)/256;
        csx[3] = xc + rcs2 * sind(angle+6)/256;
        csy[3] = yc - rcs2 * cosd(angle+6)/256;
        csx[4] = xc + rcs2 * sind(angle-6)/256;
        csy[4] = yc - rcs2 * cosd(angle-6)/256;
        
        //秒針を消去
        drawSecNeedle(psx, psy, TextbgColor);

        //描画座標を保存
        for (jj=0; jj<5; jj++) {
            psx[jj] = csx[jj];
            psy[jj] = csy[jj];
        }
    }
    
    //目盛と数字の描画------------------------------------------------------------
    SetFont(FontNormal, NULL);
    display_setTextSize(rsc->fontmag);
    display_setTextPitchAdj(rsc->xpitchAdj, 0);
    //文字盤の数字の位置
    rc2 = rc1 - CurrentFont.xpitch * ((CurrentFont.MagX > CurrentFont.MagY) ? CurrentFont.MagX : CurrentFont.MagY);

    for (jj=0; jj<60; jj++) {
        angle = jj*6;
        x[1] = xc + (rc * sind(angle)+127)/256; //単純に256で割ると切捨てになるので+127として四捨五入的にしてみた
        y[1] = yc - (rc * cosd(angle)+127)/256;
        x[2] = xc + (rc1 * sind(angle)+127)/256;
        y[2] = yc - (rc1 * cosd(angle)+127)/256;
        
        if (jj % 5 == 0) {
            //5の倍数の所に文字盤の数値を表示
            display_fillCircle(x[2], y[2], 2, color);   //時の所は、ドット、半径2
            //文字盤表示
            if (!((minupdate == 0) && (jj == mm))) {
                //ただし、minupdate=0で、mmが5の倍数の時は、描画しない(そうしないとチラつく)
                //つまり、分針が数字にかかっていて、書き換えしない時は、数値は描画しないということ
//                x[3] = xc + (rc2 * sind(angle))/256 -4;
                x[3] = xc + (rc2 * sind(angle))/256 - CurrentFont.xsize / 2 * CurrentFont.MagX;
//                y[3] = yc - (rc2 * cosd(angle))/256 -3;
                y[3] = yc - (rc2 * cosd(angle))/256 - CurrentFont.ysize / 2 * CurrentFont.MagY;
                //表示する数字を設定
                if (jj==0) kk = 12;   //0時の所は、12に設定
                else kk = jj / 5;
//                if (kk>9) x[3] = x[3] - 3;  //2桁の時刻の時の位置調整
                if (kk>9) x[3] = x[3] - CurrentFont.xsize / 2 * CurrentFont.MagX;  //2桁の時刻の時の位置調整
                
                sprintf(str, "%d", kk);
                //背景色を同じにすると、背景色を塗らない=透明と同じ
                //1桁数値と2桁数値で位置を調整
                display_drawChars(x[3], y[3], str, color, color, rsc->fontmag);
            }
        } else {
            display_drawLine(x[1], y[1], x[2], y[2], color);
        }
    }
    
    // AM/PMの表示
    //ちらつきをなくすため、背景部分は変化させない描画を行うが、
    //AM/PMの切替時だけ、背景描画実施
    if (Bcd2Hex(preTime[2])/12 != Bcd2Hex(datetime[2])/12) {
        bcolor = TextbgColor;
    }
    else {
        bcolor = color;
    }
    preTime[2] = datetime[2];
    if (datetime[2] < 0x12) strcpy(str, "AM");
    else strcpy(str, "PM");
    //午前午後の表示を大きさに応じて変える　8ドットサイズをベースにしている
    //大きなフォントサイズの時も対応させるように、16ドット以上のフォント
//    int16_t size = ((rc+3) / 70) * ( (CurrentFont.xsize < 15) ? (1) : (0) ) + 1;
//    display_drawChars(xc-CurrentFont.xpitch * size, yc+rc/2, str, color, bcolor, 0x11 * size);

    display_drawChars(xc-CurrentFont.xpitch * CurrentFont.MagX, yc+rc - 3 * CurrentFont.ysize * (CurrentFont.MagY), str, color, bcolor, rsc->fontmag);
    
    //ここから、針の描画を実行
    //アラーム針描画------------------------------------------------------
    drawAlarmNeedle(pax, pay, ColorAlarm);  //アラーム針の色

    //時針の描画------------------------------------------------------
    drawNeedle(phx, phy, color, WHITE);    //時針の色は、引数からもらう

    //分針の描画------------------------------------------------------
    drawNeedle(pmx, pmy, color, WHITE);

    //秒針の描画------------------------------------------------------
    display_fillCircle(xc, yc, 3, ColorSec);    //秒針の中央円
    drawSecNeedle(psx, psy, ColorSec);
    
}

/*
 * リソースで指定された設定で時刻を表示
 */
void DrawTime(uint8_t *datetime, ObjectRsc *rsc) {

    if (rsc->disp) {
        //RscIDをそのオブジェクトの有無(=表示するかどうか)に利用
        if (rsc->format == ANALOGCLOCK) {
            //外形円の座標= (140, 130) 半径=90
            //アナログ時計表示の場合は、アラーム針も同時に表示
            drawAnalogClock(datetime, rsc, AlarmTime);
            //アナログ時計の大きさは、xwに設定されている半径の2倍
        } else {
            dispTime(datetime, rsc);
            rsc->xw = Cursor_x - rsc->x;
            rsc->yw = CurrentFont.ysize * (rsc->fontmag & 0x0f);
        }
    }
}

/*
 * 曜日のフォントがあるかちぇっくして、なければNormalフォントに切り替える
 * 切り替えた時は、1を返す
 */
int8_t CheckSunday(ObjectRsc *rsc) {
    char filename[30];
    FIL file;       //Openingやデータ書き込み用
    FRESULT res;

    if (CurrentFont.fontcode == EXTFONTCODE) {
        //Sun.bmpがある前提で、その大きさを取得する
        sprintf(filename, "/%s/Sun.bmp", CurrentFont.fontName);
        if (f_open(&file, filename, FA_READ) == FR_OK) {
            return 0;        //曜日フォントあり
        }        
    }
    //曜日フォントない時、ノーマルフォントの曜日を利用
    SetFont(FontNormal, NULL); //曜日は、内蔵フォントを使う形にしているが、カスタムで曜日のフォントも持っている場合を考慮させたい＜＜＜＜＜＜＜＜＜＜
    display_setTextSize(rsc->fontmag);    //fontsizeの上位4bit=X倍率、下位4bit=Y倍率
    return 1;
}

#define SpacebtwMonths  10  //カレンダーを横に並べたときのスペース
#define SpacebtwDays    2  //日付間の間隔　xpitchAdjは2桁の日付の文字間隔に使用

/*
 * カレンダーを横に3か月表示
 * 月間10ドットだと、300ドットに21日分表示必要。300/21=14ドット　日付間2ドットだと、1文字6ドット以下
 * 
 * 15ドットだとしても、*21=315ドットなので、間隔を2ドットにすれば入る計算
 * フォントとしては、横7ドットだと、完全にくっ付けて、次の日との間隔を1ドットにする必要があり、見た目にも厳しそう
 * リソースで指定されたフォントに応じて描画
 * 　　Small:　月と曜日はNormalフォント使用する
 * 　　Normal: 全部ノーマルで描画
 * 左上の座標: xs, ys
 * 描画する年月: year, month
 */
void basicDrawCalendar(uint8_t year, uint8_t month, int16_t xs, int16_t ys, ObjectRsc *rsc) {
    uint8_t jj, kk;
    int16_t xx, yy;
    char str[5];
    int8_t currentDay;    //最初の日曜の日付、0以下は前月
    int8_t maxdays;
    uint8_t yr, mm, dd, wd;
    uint8_t thismonth, targetDay;
    //曜日ごとの色指定
    uint16_t dayColor[7];   // = {RED, WHITE, WHITE, WHITE, WHITE, WHITE, BLUE, };
    uint8_t xpitch, ypitch; // 表示ピッチ
    uint8_t mulx, muly, mul;
    char w;
    int8_t res;

    //前月、次月の表示のため、0月、13月という設定を許すので、ここで修正
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
    wd = getWeekdays(&yr, &mm, &dd);  //月初めの曜日を取得
    //表示する最初の日付を仮想的に何日か設定
    //1日が日曜なら1のまま、月曜なら最初の日曜日は0日、火曜なら-1、水曜なら-2になる
    currentDay = (int8_t)(1- wd);
    
    //当該月の最終日を取得。その月が何日あるかを把握
    dd = 31;
    getWeekdays(&yr, &mm, &dd);  //31日の曜日を取得。その日がない場合、自動調整される
    //mmが変更になったら、31日ではなかったとわかる。
    //ddが31日が1日に変わっていたら、その月は30日だったとわかる。2月、うるう年も対応。
    if (month != mm) {
        maxdays = (int8_t)(31 - dd);    //その月の最終日
    } else {
        maxdays = (int8_t) dd;
    }   
    
    //曜日ごとの色をリソースから取得
    for (jj = 1; jj < 6; jj++) dayColor[jj] = rsc->color;
    dayColor[0] = rsc->attribute2;   //日曜日の色
    dayColor[6] = rsc->attribute;    //土曜日の色
    
    //指定された基本フォント情報を取得
    SetFont(rsc->fontcode, rsc->fontName);
    display_setTextSize(rsc->fontmag);    //上位4bit=X倍率、下位4bit=Y倍率
    display_setTextPitchAdj(rsc->xpitchAdj, 0);

    xpitch = (uint8_t)(CurrentFont.xpitch * CurrentFont.MagX + CurrentFont.XpitchAdj)*2 + SpacebtwDays;  //1日=2文字+スペース
    ypitch = (uint8_t)(CurrentFont.ypitch * CurrentFont.MagY + CurrentFont.YpitchAdj);
    
//    DEBUG_PRINTF("%d, %d  %d,%d, xp=%d\r\n", xpitch, ypitch, CurrentFont.xpitch, CurrentFont.XpitchAdj, rsc->xpitchAdj);
//    DEBUG_PRINTF("font %s: %d, %d\r\n", rsc->fontName, CurrentFont.xsize, CurrentFont.ysize);

    //対象領域をクリア: fontwで14文字分+7日のスペース分、Y方向は8行分
    //xの開始を1ドット左にして、サイズもその分大きく。枠を付けた時はみ出しを防止
    CalendarXsize = xpitch*7;  //2桁の日付がベースになっているxpitchの7日分
    CalendarYsize = ypitch * 8;     //月+曜日+最大の6週分

    //1か月分の領域をクリアする
    display_fillRect(xs, ys, CalendarXsize + SpacebtwMonths, CalendarYsize, TextbgColor);

    //月の数字を描画
    sprintf(str, "%d", month);
    //中央に表示させる　横2倍で表示させるため、式後方は、/2をしていない
    xx = xs + CalendarXsize/2 - (int16_t)strlen(str) * (int16_t)CurrentFont.xpitch;
    //月表示は横方向2倍で描画
    display_drawChars(xx, ys, str, rsc->color, TextbgColor, rsc->fontmag +0x10);

    //曜日の描画
    //曜日フォントがあるかどうか
    res = CheckSunday(rsc);
    //バランスのため、曜日の幅をpitchに収まる最大のサイズに拡大させる
    mulx = xpitch / CurrentFont.xsize;
    muly = ypitch / CurrentFont.ysize;

    yy = ys + ypitch;
    xx = xs + (xpitch - CurrentFont.xsize * mulx)/2;   //中央になるように
    mul = (uint8_t)(mulx << 4) + muly;
    str[1] = '\0';
    for (jj=0; jj<7; jj++) {
        if (rsc->format >= Month1e) w = WeekDays[jj][0];  //SMTWTFSという表記方式
        else w = FSunday + jj;    //日月火水木金土という表示
        str[0] = w;
        display_drawChars(xx, yy, str, dayColor[jj], TextbgColor, mul);
        xx += xpitch;
    }
    
    //日付の描画
    if (res) {
        //曜日でフォント変更していた時は、フォント元に戻す
        SetFont(rsc->fontcode, rsc->fontName);
        display_setTextSize(rsc->fontmag);    //上位4bit=X倍率、下位4bit=Y倍率
        display_setTextPitchAdj(rsc->xpitchAdj, 0);
    }

    yy += ypitch;
    SetFont(rsc->fontcode, rsc->fontName);  //指定のフォントに戻す
    display_setTextSize(rsc->fontmag);    //fontsizeの上位4bit=X倍率、下位4bit=Y倍率
    display_setTextPitchAdj(rsc->xpitchAdj, 0);

    //今月のカレンダーを表示しているかをチェック。今日の日付に印をつけるかどうか判断させるため
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
                
                //今日の日付に印をつける
                if (thismonth && (currentDay == targetDay)) {
                    //反転だと視認しにくいので枠で囲む
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
 * Formatに応じてカレンダーを表示する
 * 現在5種類のフォーマット
 * Month1: 今月1か月のカレンダーを表示。フォントサイズ変えることで、大きくも小さくも表示可
 * Month3: 前後の月を含めて3か月分のカレンダーを横配置で表示
 * Month3v:前後の月を含めて3か月分のカレンダーを縦配置で表示
 * 　引数の年月は、表示させる月を指す
 * 　BCDではない
 * 表示位置は、リソースで指定
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
            // 1か月のカレンダーを表示　　大きさはフォントサイズで指定
            xx = 1;
            yy = 1;
        } else if (f == Month2) {
            // 2か月分のカレンダーを表示
            xx = 2;
            yy = 1;
        } else if (f == Month2v) {
            // 2か月分のカレンダーを縦に表示
            xx = 1;
            yy = 2;
        } else if (f == Month3) {
            // 3か月分のカレンダーを表示
            xx = 3;
            yy = 1;
        } else if (f == Month3v) {
            // 3か月分のカレンダーを縦に表示
            xx = 1;
            yy = 3;
        }

        //CalendarXsize、CalendarYsizeは、最初の月を描画すると計算される
        if (xx * yy > 2) monthAdj = -1; //3か月表示の時は、前月から表示する
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
 * アラーム時刻を表示　　スイッチの状態で色を変化させる
 * sw SlideSWoff=Alarm off, SlideSWon=Alarm on
 */
void DrawAlarmTime(uint8_t *alarmtime, uint8_t sw, ObjectRsc *rsc) {
    char str[100];
    char ampm[][3] = {"AM", "PM", ""};
    uint8_t ap;
    char onOff; //[] = {'-', '+'};    //オンオフで+-切替　plus/minus
    uint16_t color;
    uint8_t hr;
    
    if (rsc->disp) {
        //アラーム時刻の表示色を、スライドSWのOn/Offで変える
        if (sw == SlideSWon) {
            //Onの時
            onOff = '+';
            color = rsc->color;
        } else {
            //Offの時
            onOff = '-';
            color = rsc->attribute;  //アラームのオフ時の色はattributeに格納
        }
        
        hr = Bcd2Hex(alarmtime[1]);
        if ((rsc->format == ALMAMPM)||(rsc->format == iALAMPM)) {
            //12時間表示
            if (hr >= 12) {
                ap= 1;   //12時以降なら午後  
                hr -= 12;   //12時間表記のため、12差し引く
            }
            else {
                ap = 0;
            }
        } else {
            //24時間表示
            ap = 2;
        }
        if ((rsc->format == ALMAMPM)||(rsc->format == ALM24)) {
            sprintf(str, "ALM%c %s%02d:%02x", onOff, ampm[ap], hr, alarmtime[0]);
        } else {
            sprintf(str, "%c%c %s%02d:%02x", Fbell, onOff, ampm[ap], hr, alarmtime[0]);
        }
        presetting(rsc);
        display_setColor(color);    //SWに対応して色を変更させるため
        display_puts(str);
        display_setTextPitchAdj(0, 0); //presettingとペアで元に戻す
        
        rsc->xw = Cursor_x - rsc->x;
        rsc->yw = CurrentFont.ysize * CurrentFont.MagY;
    }
}

/*
 * 今日の日付を表示
 * 表示方法などは、リソース内で、指定する
 * 　　format, フォントなど
 */
void DrawDate(uint8_t *datetime, ObjectRsc *rsc) {
    char str[100];
    
    if (rsc->disp) {
        switch (rsc->format) {
            case YYYYMMDDw:
                //NormalFontでは、曜日のフォントがFSunday=0x17-にある
                sprintf(str, "20%02x/%02x/%02x(%c)", datetime[6], datetime[5], datetime[4], FSunday+datetime[3]);
                break;
            case YYYYMMDDwe:  //曜日の英語表記
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
        display_setTextPitchAdj(0, 0); //presettingとペアで元に戻す
        
        rsc->xw = Cursor_x - rsc->x;
        rsc->yw = CurrentFont.ysize * CurrentFont.MagY;
    }
}


/*
 * 温度を表示
 * temp:　摂氏を10倍にして、0.1℃まで整数で対応させる
 */
void DrawTemperature(int16_t temp, ObjectRsc *rsc) {
    char str[50];
    int16_t ftemp;
    
    if (rsc->disp) {
        if (rsc->format == DEGF) {
            //tempは、10倍の摂氏　　　10F = 9/5*(10C) + 320
            ftemp = temp * 9*2 +320 *10;
            sprintf(str, "%2d%c", ftemp / 100, FdegF);  //
        } else {
            sprintf(str, "%2d.%1d%c", temp / 10, temp % 10, FdegC);  //FdegC=℃の文字コード
        }
        
        presetting(rsc);
        display_puts(str);
        display_setTextPitchAdj(0, 0); //presettingとペアで元に戻す
        
        rsc->xw = Cursor_x - rsc->x;
        rsc->yw = CurrentFont.ysize * CurrentFont.MagY;
    }
}

/*
 * 湿度を表示
 * 小数点以下は表示しない
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
 * 歯車アイコン32x32ドットを(x,y)座標に描画
 * カスタムフォントにも対応できるよう
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
