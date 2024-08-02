/*******************************************************************************
 *  skESP8266.h - ESP-WROOM02��WiFi�ʐM���s���֐��̃C���N���[�h�t�@�C��         *
 * ============================================================================ *
 *  VERSION DATE        BY                    CHANGE/COMMENT                    *
 * ---------------------------------------------------------------------------- *
 *  1.00    2015-09-20  ���ޒ��H�[            Create                            *
 *  1.02    2015-09-30  ���ޒ��H�[            24EP256MC202�̓���m�F            *
 * ===========================================================================  *
 *  PIC 18F25K22 24EP256MC202                                              �@   *
 *  (���̃��C�u�������̂͑���PIC�ł�OK�Ǝv���܂�)                               *
 * ������x�[�X�ɏC�����܂������A���`���Ƃǂ܂��Ă���̂��ǂ����B�B�B
 *******************************************************************************/
#ifndef _SKESP8266_H_
#define _SKESP8266_H_

#include "mcc_generated_files/mcc.h"

#define SSID        ""  //Set your WiFi server SSID
#define PASSWORD    ""  //Set your WiFi server password

// �֐��̃v���g�^�C�v�錾
int8_t ESP_GetDataP(char *buf, uint16_t *len, uint8_t skipCR);
int8_t ESP_GetData(char *buf, uint16_t *len);

void Get_time(uint16_t *yy, uint8_t *mm, uint8_t *dd, uint8_t *h, uint8_t *m, uint8_t *s);

int8_t ESP_Init(void);
int8_t ESP_ServerInit(void);

int8_t ESP_HtmlSnd2(uint8_t id, const char *txt);
void ESP_HtmlSndEnd(uint8_t id);

void ESP_GetIPaddress(char *ip);

#endif
