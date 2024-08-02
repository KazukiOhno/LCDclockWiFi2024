/*******************************************************************************
 *  skESP8266 - ESP-WROOM02でWiFi通信を行う為の関数ライブラリ                    *
 * =========================================================================== *
 *  VERSION DATE        BY                    CHANGE/COMMENT                   *
 * --------------------------------------------------------------------------- *
 *  1.00    2015-09-20  きむ茶工房(きむしげ)  Create                            *
 *  1.02    2015-09-30  きむ茶工房            24EP256MC202の動作確認            *
 * =========================================================================== *
 *  PIC 18F25K22 24EP256MC202                                                  *
 *  (このライブラリ自体は他のPICでもOKと思います)                                 *
 * 上記をベースに修正加えたが、ほぼ原形をとどめていないかも
 ******************************************************************************/
#include "debug.h"
#include "skESP8266.h"
#include <string.h>
#include "mcc_generated_files/uart1.h"
#include "mcc_generated_files/pin_manager.h"
#include "LCD320x240color.h"

#define CMD_AT        "AT"          // モジュールの動作状態をテストする
#define CMD_CWMODE1   "AT+CWMODE=1" // ステーションモード
#define CMD_CWMODE2   "AT+CWMODE=2" // ソフトAPモード
#define CMD_CWJAP     "AT+CWJAP=\"" SSID "\",\"" PASSWORD "\""   // APに接続する
#define CMD_CWQAP     "AT+CWQAP"                                 // APから切り離す
//#define CMD_CIPSTART  "AT+CIPSTART=\"TCP\",\"192.168.1.2\",8089" // サーバに接続する
#define CMD_CIPCLOSE  "AT+CIPCLOSE"                              // サーバから切り離す
#define CMD_CIPCLOSE1 "AT+CIPCLOSE=1"                           // サーバから切り離す
#define CMD_CIPMODE   "AT+CIPMODE=1"      // トランスペアレントモード
#define CMD_CIPMODE2  "AT+CIPMODE=0"      // 通常(ATコマンド)モードにする
//#define CMD_CIPMODE3  "AT+CIPMODE=3"      // 両モードにする
#define CMD_CIPSERVER "AT+CIPSERVER=1,80"   // サーバーモードにする
#define CMD_CIPSERVER0 "AT+CIPSERVER=0"     // サーバーモード削除
#define CMD_CIPMUX    "AT+CIPMUX=1"       // マルチ接続を有効にする
#define CMD_CIPMUX0   "AT+CIPMUX=0"       // マルチ接続を無効
#define CMD_UART      "AT+UART_CUR=115200,8,1,0,2"  // フロー制御
#define CMD_UARTHS    "AT+UART_CUR=230400,8,1,0,2"  // フロー制御
#define CMD_UARTHS2    "AT+UART_CUR=460800,8,1,0,2"  // フロー制御
#define CMD_UARTHS3    "AT+UART_CUR=921600,8,1,0,2"  // フロー制御
//#define CMD_UART0      "AT+UART_CUR=115200,8,1,0,0"  // フロー制御なし
#define CMD_UARTCUR   "AT+UART_CUR?"
#define CMD_CIPSNTPCFG "AT+CIPSNTPCFG=1,9,\"ntp.nict.jp\""  //SNTPの設定
#define CMD_CIPSNTPTIME "AT+CIPSNTPTIME?"                   //SNTPで日時取得
#define CMD_CIFSR     "AT+CIFSR"          // IPアドレスとMACアドレスを得る
#define CMD_CIPSEND   "AT+CIPSEND"        // データを送信する　<length>\r\n<data>
#define CMD_CIPSENDEX "AT+CIPSENDEX"      // データを送信する　データに\0が来ても終了
#define CMD_RST       "AT+RST"      // Restarts the module
#define CMD_RESTORE   "AT+RESTORE"      // 
#define CMD_UART_DEF "AT+UART_DEF?"

/*
 * バッファにデータがあれば、１バイト分のデータを返す
 * ただし、バイナリも対応させるため、0がエラーを示す方式をとれないので、返り値で示す
 * 
 * 返り値: 
 * 　　エラーの有無は、*errで判断　　1:エラー、0:正常
 * 　　正常な時、１バイトデータを返す
 */
uint8_t ReadFromBuf(uint8_t *err) {
    uint8_t data;
    
    if (UART1_is_rx_ready()) {
        // 受信データがある時は、読み出す
        *err = 0;
        data = UART1_Read();
    } else {
        *err = 1;
        data = 0;
    }
    return data;
}

/*
 * パケットヘッダの処理は、ここで行う
 * 
 * 受信して改行コードを区切りとしてbufに取り出す
 * テキスト、バイナリでも関係なし
 * 
 * len: 格納できるバッファのサイズをもらい、実際に格納したバイト数を返す
 * -1: データなし
 * 0: 正常終了　\r\nで終了
 * バッファフルか、データは取得したがタイムアウトのケースは、\0の終端なしで、以下の2ケース
 * 1: \rでデータ終了
 * 2: 上記以外で終了した時
 * 
 */
int8_t ESP_GetDataP(char *buf, uint16_t *len, uint8_t skipCR) {
    uint16_t maxlen;
    uint8_t dt, err, preCR;
    uint8_t cnt;
    int8_t ans;
    
    maxlen = *len;  //格納可能文字数を保存
    *len = 0;
    ans = -1;
    cnt = 0;
    preCR = 0;
    
    while (1) {
        dt = ReadFromBuf(&err); //1バイト読み出す
        if (err) {
            //バッファにデータがない時は、改行来るまで少し待つ。
            //スタックする可能性もあるが、改行来ないと処理できないので、必ず来るものと想定
            __delay_ms(250);
            DEBUG_PRINTF(".");
//            if ((cnt % 4)==0) display_puts(".");  //1sに1回だけ出力
            cnt++;
            if (cnt > 10) {    // 2s待ってデータ来ない時は終了
                if (ans == -1) return ans;
                else return (int8_t)(2 - preCR);
            }
        } else {
            cnt = 0;    //データ受信できたら、リセット
            buf[*len] = dt;
            *len = *len + 1;
            ans = 1;    //データを取得できたら1

            if (preCR || skipCR) {
                if (dt == '\n') {
                    // \nまで来たら読出し終了  \rのチェックも後で追加した方が良いはず
                    buf[*len] = '\0';    //文字列終端を付加して終了
                    return 0;    //改行コードを受け取ったら0
                }
                else preCR = 0;
            }
            if (dt == '\r') preCR = 1;
            if (*len >= maxlen) {
                //文字列終端は格納できず、pred=1なら1、それ以外は2を返す
                return (int8_t)(2 - preCR);
            }
        }
    }
}

int8_t ESP_GetData(char *buf, uint16_t *len) {
    return ESP_GetDataP(buf, len, 0);
}


/*
 * Messages of ESP8266 AT
 ready              The AT firmware is ready.
 ERROR              AT command error, or error occurred during execution.
 WIFI CONNECTED     ESP8266 station connected to an AP.
 WIFI GOT IP        ESP8266 station got IP address.
 WIFI DISCONNECT    ESP8266 station disconnected from an AP.
 busy s...          Busy sending. The system is sending data now, cannot accept the newly input.
 busy p...          Busy processing. The system is in process of handling the previous command, cannot accept the newly input.
 <conn_id>,CONNECT  A network connection of which ID is <conn_id> is established.
 <conn_id>,CLOSED   A network connection of which ID is <conn_id> ends.
 +IPD               Received network data.
 +STA_CONNECTED:<sta_mac>   A station connects to the ESP8266 softAP.
 +DIST_STA_IP:<sta_mac>,<sta_ip>    ESP8266 softAP distributes an IP address to the station connected.
 +STA_DISCONNECTED:<sta_mac>        A station disconnects from the ESP8266 softAP.
 */

/*
 * コマンドに対する返信データをチェック
 * 　現状OKの結果しか使っていない
 */
#define ANS_OK        "OK"          // コマンドの返答"OK"
#define ANS_FAIL      "FAIL"        // コマンドの返答"FAIL"
#define ANS_ERROR     "ERROR"       // コマンドの返答"ERROR"

int8_t CheckResponseData(char *data) {
    
    // 受信データの解析を行う
    //SEND OKという時もあるので、strcmpではダメ
    if (strstr(data, ANS_OK) != NULL) {
        return 0;      // OKの返答有り
    }
    if (strstr(data, ANS_FAIL) != NULL) {
        return 31;     // FAILの返答有(APに接続時のエラー)
    }
    if (strncmp(data, ANS_ERROR, 5) == 0) {
        return 32;     // ERRORの返答有
    }
    return -1;
}

/*
 * 返り値
 * OK: 0
 * 
 */
int8_t ESP_Receive()
{
    uint8_t cnt;
    int8_t ans;
    char str[80];
    uint16_t len;

    cnt = 0;
    ans = -1;
    // OK返答を全部受信するまで待つ
    while (1) {
        len = sizeof(str);
        ans = ESP_GetData(str, &len);
        //受信データが正常ならデータのチェックを実施する
        if (ans == 0) {
            str[strlen(str) -2] = '\0';   //改行コードがある時は削除
            DEBUG_PRINTF("c[%s]\r\n", str);    //改行コードも返信に入っているはず
            ans = CheckResponseData(str);
            if ((ans == 0)||(ans == 32)) break;    //返答がOKなら終了   ERRORでも終わりにする
            
            cnt = 0;
        }
        cnt++;
        if (cnt > 4) return ans;   // 5回トライしてダメなら終了させ、無限ループを回避
    }
    return ans;
}


/******************************************************************************
 *    指定した個数のデータを送信
 *    dat : 送信するデータを格納した配列
 *    num : 送信するデータのバイト数を指定
 ******************************************************************************/
void UART_Send(const char *dat, uint16_t num)
{
     while (num > 0) {
         UART1_Write(*dat++);        // 送信する
         num--;
     }     
}

/*
 * コマンドを改行コードを付加して送信
 * コマンドは、文字列で、\0で終端されている必要あり
 */
void Send_cmd(const char *cmd)
{
    while (*cmd != 0) {
        UART1_Write(*cmd++);
    }
    UART_Send("\r\n", 2);
    __delay_ms(200);    //オリジナルは1000ms
}

/******************************************************************************
 *    ESP-WROOM02にATコマンドを送信する処理
 *    コマンド文字列の最後は\0で終端、[CR][LF]は関数で付加
 *    この関数はコマンドに対する返答を待ちます、約5sでタイムアウト
 *
 *    cmd : 送信するコマンドを格納した配列へのアドレスを指定
 *    ans : 0=正常  31=APに接続時のエラー  32=ERRORで返答した  -1=相手が返答無
 ******************************************************************************/
int8_t ESP_CommandSend(const char *cmd)
{
    // ATコマンドを送信する
    Send_cmd(cmd);
    
    return ESP_Receive();
}


/*
 * ESP-WROOM-02Dから日時を取得
 */
void Get_time(uint16_t *yy, uint8_t *mm, uint8_t *dd, uint8_t *h, uint8_t *m, uint8_t *s) {
    uint8_t jj;
    char st[80];
    int8_t ans;
    char stmon[10], wday[10];
    const char *month[] = { "", 
        "Jan", "Feb", "Mar", "Apr", "May", "Jun", 
        "Jul", "Aug", "Sep", "Oct", "Nov", "Dec",
    };
    uint16_t year;
    uint16_t len;

    DEBUG_PRINTF("<<<<< start get_time\r\n");
    //初期化済みなので、以下の設定だけ実施
    ESP_CommandSend(CMD_CIPSNTPCFG);
    
    Send_cmd(CMD_CIPSNTPTIME);
    ans = -1;
    // 返答OKが来るまでループ
    while(1) {
        len = sizeof(st);
        ans = ESP_GetData(st, &len);
        //受信データが正常ならデータのチェックを実施する
        if (ans == 0) {
            DEBUG_PRINTF("--%s", st);    //改行コードも返信に入っているはず
            if (strncmp(st, "+CIP", 4) == 0) {
                //+CIPSNTPTIME:Mon May 20 20:32:34 2024  というフォーマット
                sscanf(st+13, "%s %s %hhd %hhd:%hhd:%hhd %hd", wday, stmon, dd, h, m, s, &year);
                *yy = year % 100;
                jj = 12;
                while ((strcmp(stmon, month[jj]) != 0) && (jj > 0)) jj--;
                *mm = jj;
            } else {
                if (CheckResponseData(st) == 0) break;    //OKを受信したら終了
            }
        }
        else {
            //エラーの時は、何もしない
            DEBUG_PRINTF("data error ");
            break;
        }
    }
    DEBUG_PRINTF("complete get time>>>>>\r\n");
}

/*
 * ESP-WROOM02モジュールの初期化
 * 0: 正常終了
 * それ以外は何らかのエラー発生ので、要デバッグ
 */
int8_t ESP_Init(void) {
    int8_t ans;

    DEBUG_PRINTF("uart1RxCount=%d\r\n", uart1RxCount);
    DEBUG_PRINTF("Initialize ESP\r\n");
    UART1_Initialize(); //最初に実行されているが、バッファがオーバーフローしている場合があるので、ここで再度リセット
    ans = ESP_CommandSend(CMD_AT);
    display_puts(".");
    if (ans != 0) {
        // トランスペアレントモードかも？　そうなら一度モード解除
        UART_Send("+++", 3);
        __delay_ms(1500);    // 最低1s程待ってから、次のATコマンドを送信する必要あり
        ans = ESP_CommandSend(CMD_AT);
        if (ans != 0) return 1;
    }
    
    ans = ESP_CommandSend(CMD_UARTCUR);    //初期状態を確認
    ans = ESP_CommandSend(CMD_UART);    //115200bps,8bit,stop=1bit,パリティなし,enable CTS
    display_puts(".");
    if (ans != 0) return 11;
    
    //ボーレートを115kbpsから高速化
    //115kbpsの状態で、設定を変更して、その後、PIC側のU1BRGLを変更する
    ans = ESP_CommandSend(CMD_UARTHS3);    //921600 bps,8bit,stop=1bit,パリティなし,enable CTS
    display_puts(".");
    DEBUG_PRINTF("ans=%d\r\n", ans);
    if (ans != 0) return 11;
    U1BRGL = 0x10;  //921600 bps動作するように設定
    
    ans = ESP_CommandSend(CMD_UARTCUR);    //初期状態を確認

    //フロー制御用にピンの設定を変更
    RTS1_SetLow();  //フロー制御で、受信開始OK

    // APに接続する　　これは他のコマンドより時間がかかる
    ans = ESP_CommandSend(CMD_CWJAP);
    display_puts(".");
    if (ans != 0) return 6;
    //ステーションモード
    ans = ESP_CommandSend(CMD_CWMODE1);
    display_puts(".");
    if (ans != 0) return 8;
    // マルチ接続を有効にする
    ans = ESP_CommandSend(CMD_CIPMUX);
    display_puts(".");
    if (ans != 0) return 7;
    
    return 0;
}

/*****************************************************************************
 *    ESP-WROOM02をサーバーモードで初期化
 *    (ステーションモード/マルチ接続/サーバーを起動)
 *    ans : 0=正常  失敗した場合は何処で失敗したか進捗番号を返します
 ******************************************************************************/
int8_t ESP_ServerInit(void)
{
    int8_t ans;
    
    DEBUG_PRINTF("Initialize Server\r\n");
    // サーバーを起動する
    ans = ESP_CommandSend(CMD_CIPSERVER);
    if (ans != 0) return 8;
    return 0;
}

/******************************************************************************
 *    クライアントにHTMLテキストを送信する処理
 *
 *    id  ：送信する相手のクライアントIDを指定
 *    *txt：送信するHTMLテキスト　\0終端
 * ESP-WROOM-02Dでは20848バイトが一度に送信できるMaxで、それ以上送信しようとするとERRORとなる
 * 通信処理を短時間にするため、パケットを大きくさせる
 * 何か送信したい時にこれを呼び出す、何回でもOK。CIPSENDEXは自動的に発行。
 * 基本的に2048バイトごとに送信する。
 * 中断させたい時は、txtをNULLで呼び出す
 * 
 * エラーの時、1を返す
 */
int8_t ESP_HtmlSnd2(uint8_t id, const char *txt)
{
    int8_t ans;
    char st[80];
    int timeout = 0;
    static uint16_t remainLen = 0;

    if (txt == NULL) {
        if (remainLen == 0) return 0;
        //空文字の時は処理を終了させる
        if (remainLen > 2) {
            UART1_Write(0x5c);  // CPISENDEXは"\\0"で終了できる
            UART1_Write('0');
            remainLen = 0;  //次の送信のためにリセットしておく必要あり
        } else {
            //残り送信量が2バイト以下の時は、空白を送信
            while (remainLen-- > 0) UART2_Write(' ');
        }
        ans = ESP_Receive();
        DEBUG_PRINTF("終了 ans=%d\r\n", ans);
        return ans;
    }

    do {
        if (remainLen == 0) {
            // 送るデータのサイズは2048固定
            remainLen = 2048;
            sprintf(st, "%s=%d,%d", CMD_CIPSENDEX, id, remainLen);
            do {
                ans = ESP_CommandSend(st);
                if (ans == 0) break;
            } while (timeout++ < 5);    //最後、6で抜ける
            if (timeout > 5) return ans;    //ansは0以外
        }
        
        // HTMLテキストを2048バイトになるまで送信
        while ((*txt != '\0') && (remainLen > 0)) {
            UART1_Write(*txt++);
            remainLen--;
        }
        if (remainLen == 0) {
            ans = ESP_Receive();
            DEBUG_PRINTF("RCV:%d\r\n", ans);
        }
    } while (*txt != '\0');   //送信データが残っていたら、次のパケットを送信させる        

    return 0;
}

void ESP_HtmlSndEnd(uint8_t id)
{
    char st[80];
    
    // 相手のTCPポートをクローズし接続を切断する       // クライアントID
    sprintf(st, "%s=%d", CMD_CIPCLOSE, id);
    ESP_CommandSend(st);
}


/******************************************************************************
 *    IPアドレスを取得
 *    エラー等によりアドレスが得られなかった場合、何もしない
 *
 *    *ip  : IPアドレスを返す("xxx.xxx.xxx.xxx"で返す、MAX16文字)
 ******************************************************************************/
void ESP_GetIPaddress(char *ip)
{
    int8_t ans;
    char st[80];
    uint16_t len;
    
    Send_cmd(CMD_CIFSR);

    ans = -1;
    // 返答を全部受信するまで待つ
    while (1) {
        len = sizeof(st);
        ans = ESP_GetData(st, &len);
        DEBUG_PRINTF("--%s", st);    //改行コードも返信に入っている
        //受信データが正常ならデータのチェックを実施する
        if (ans == 0) {
            if (strncmp(st+10, "IP,", 3)==0) {
                sscanf(st+14, "%[^\"]", ip);
            }
            else {
                ans = CheckResponseData(st);
                if (ans == 0) break;    //OKだったら完了
            }
        }
        else {
            break;      //エラーの時
        }
    }
}
