/*
 * イベント処理プログラム
 * 割込み中にPushEvent()は、実施しないこと。reentrantというワーニングが出るだけだが、
 * 実際に発生すると厄介なバグになる可能性あり
 * 
 * 2024/5/24 独立させた
 * 
 */

#include "event.h"
#include "mcc_generated_files/mcc.h"

//Eventを配列にして、キューを持たせている。PushEventとPopEventで操作
uint8_t Event[16];  //イベント発生時にここにpushする
uint8_t PushPoint;
uint8_t PopPoint;
uint8_t ContNum;    //キューに入っているベント数


/*
 * イベントハンドラの設定
 * 呼び出すのは、常にEventHandler
*/
void SetEventHandler(void (* handler)(void)){
    PreviousEventHandler = NULL;
    EventHandler = handler;
}

/*
 * 今走らせているイベントハンドラを保存して、新たなイベントハンドラを走らせる
 */
void PopupEventHandler(void (* handler)(void)){
    PreviousEventHandler = EventHandler;    //現在のハンドラを保存
    EventHandler = handler;
}

/*
 * 保持していたイベントハンドラに戻す
 */
void ReturnEventHandler(void){
    if (PreviousEventHandler) EventHandler = PreviousEventHandler;
}

/*
 * イベントバッファがフルだったら、1を返す
 */
uint8_t PushEvent(uint8_t ev) {
    if (ContNum < sizeof(Event)) {
        Event[PushPoint] = ev;
        PushPoint = (PushPoint +1) % sizeof(Event);
        ContNum++;  //バッファはサイクリックに使うので、いくつ有効かを示す
        return 0;
    } else {
        return 1;   //バッファフルエラー
    }
}

/*
 * イベントバッファに何も入っていない時は、0を返す
 */
uint8_t PopEvent(void) {
    uint8_t ev;

    if (ContNum > 0) {
        ev = Event[PopPoint];
        PopPoint = (PopPoint + 1) % sizeof(Event);
        ContNum--;
    } else {
        ev = 0; //バッファが空の時
    }
    return ev;
}

/*
 * バッファのクリア
 */
void ClearEventBuffer(void) {
    PushPoint = 0;
    PopPoint = 0;
    ContNum = 0;    //キューに入っている数
}
