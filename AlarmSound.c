/* 
 * File:   AlarmSound.c
 * Author: K.Ohno
 *
 * 圧電スピーカー
 * CCP2のPWMモード(TMR2)で、圧電スピーカを鳴らす:
 * 　250us=4kHz, duty=50%をビープ音として設定
 *  
 * TMR0: 鳴っている時間を制御　10ms毎に割り込みを掛け、発音時間、消音時間を制御
 * 
 */

#include "AlarmSound.h"
#include <xc.h>
#include "mcc_generated_files/tmr2.h"


/*
 * アラームのパターンを定義
 * データ形式:　10msを単位に鳴っている時間と消音の時間をペアで定義
 * 　　　　　　データの終わりは0で示す
 */
uint8_t SoundData[][10] = {
    {10, 20, 10, 60,  0, },             //ピピッとならす最初の30秒用
    {10, 10, 10, 10, 10, 50,  0, },     //ピピピとならす30秒-1分用
    {10, 10, 10, 10,  0, },             //ピピピピと鳴らす
    //効果音
    { 7,  1,  0, },             //ピッと1回鳴らす
    { 7, 15, 20,  1,  0, },     //2回ピッピーと鳴らす
};
uint8_t SoundNum;       //アラームの種類を指定
uint8_t SoundDataPos;   //音データ配列のどのデータを使うか
uint8_t SoundCount;     //音のオン/オフの長さをカウントダウン
uint16_t SoundTime;     //アラームが鳴っている時間を測定 (max 3000=30sまで)
uint8_t SoundFlag = 0;  //音を鳴らしている時は、1

/*
 * TMR0の割り込みのハンドラ 10ms毎にここにくる
 * 
 * SoundCountを減算して、0になったら、次のデータ=時間を設定
 * 発音か、消音かで、CCP2のオンオフする
 * 
 * またアラームが鳴り続けたら30秒ごとにアラーム音を変更させる
 * 
 */
void SoundHandler() {

    //アラーム鳴らしっぱなしの時、30秒、60秒経過したらアラーム音を変更する
    SoundTime++;    //音を出してからの経過時間を計測　10ms単位
    if (SoundTime >= 24000) {
        //アラームが240s=4分以上経過したら、強制的に止める。最初の1分は別枠なので、ここは4分。
        CCP2EN = 0;     //音を消す
//        CCP2CONbits.CCP2EN = 0;     //この表記の方法がプログラムサイズ大きくなる
        SoundFlag = 0;
        return;
    } else if (SoundTime >= 3000) {
        //前の処置から30s経過したら
        if (SoundNum < AlarmSound2) {
            SoundNum++; //アラーム音を次の音に進める
            SoundTime = 0;  // 鳴らしている時間を0にリセット
            SoundDataPos = 0;
            SoundCount = SoundData[SoundNum][SoundDataPos];
            SoundDataPos++;
            CCP2EN = 1;     //音を出す
            return;
        }
    }

    //ここで音を作る
    SoundCount--;
    if (SoundCount == 0) {
        //SoundCountを10ms割り込み毎カウントダウンして、0になったら、次のデータを読み込む
        if (SoundData[SoundNum][SoundDataPos] == 0) {
            //データの終了に来た時の処理
            if (SoundNum <= AlarmSound2) {
                //アラームは音をリピートさせるため、位置を0リセット
                SoundDataPos = 0;
            } else {
                //1回だけ鳴らす場合は、ここで終了
                CCP2EN = 0;
                SoundFlag = 0;
                return;
            }
        }
        //次のデータを取り出す
        SoundCount = SoundData[SoundNum][SoundDataPos];
        
        //SoundDataPosが奇数の時サウンドOff、偶数の時はon
        if (SoundDataPos % 2) {
            CCP2EN = 0;
        } else {
            CCP2EN = 1;
        }
        SoundDataPos++;
    }
}

/*
 * アラームサウンド開始
 * アラームを鳴らすための設定を実施
 */
void AlarmSoundOn(uint8_t sn) {
    //サウンドデータセット
    SoundNum = sn;  //サウンドの種類を設定
    SoundTime = 0;  //アラームが鳴っている時間
    SoundDataPos = 0;
    SoundCount = SoundData[SoundNum][SoundDataPos++]; //初期値設定
    TMR0_StartTimer();
    TMR2_Start();
    CCP2EN = 1;
    SoundFlag = 1;        
}

void AlarmSoundOff(void) {
    TMR0_StopTimer();
    TMR2_Stop();
    CCP2EN = 0;
    SoundFlag = 0;
}
