/*******************************************************************************
 *  skESP8266.h - ESP-WROOM02でWiFi通信を行う関数のインクルードファイル         *
 * ============================================================================ *
 *  VERSION DATE        BY                    CHANGE/COMMENT                    *
 * ---------------------------------------------------------------------------- *
 *  1.00    2015-09-20  きむ茶工房            Create                            *
 *  1.02    2015-09-30  きむ茶工房            24EP256MC202の動作確認            *
 * ===========================================================================  *
 *  PIC 18F25K22 24EP256MC202                                              　   *
 *  (このライブラリ自体は他のPICでもOKと思います)                               *
 * これをベースに修正しましたが、原形がとどまっているのかどうか。。。
 *******************************************************************************/
#ifndef _SKESP8266_H_
#define _SKESP8266_H_

#include "mcc_generated_files/mcc.h"

#define SSID        ""  //Set your WiFi server SSID
#define PASSWORD    ""  //Set your WiFi server password

// 関数のプロトタイプ宣言
int8_t ESP_GetDataP(char *buf, uint16_t *len, uint8_t skipCR);
int8_t ESP_GetData(char *buf, uint16_t *len);

void Get_time(uint16_t *yy, uint8_t *mm, uint8_t *dd, uint8_t *h, uint8_t *m, uint8_t *s);

int8_t ESP_Init(void);
int8_t ESP_ServerInit(void);

int8_t ESP_HtmlSnd2(uint8_t id, const char *txt);
void ESP_HtmlSndEnd(uint8_t id);

void ESP_GetIPaddress(char *ip);

#endif
