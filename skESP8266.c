/*******************************************************************************
 *  skESP8266 - ESP-WROOM02��WiFi�ʐM���s���ׂ̊֐����C�u����                    *
 * =========================================================================== *
 *  VERSION DATE        BY                    CHANGE/COMMENT                   *
 * --------------------------------------------------------------------------- *
 *  1.00    2015-09-20  ���ޒ��H�[(���ނ���)  Create                            *
 *  1.02    2015-09-30  ���ޒ��H�[            24EP256MC202�̓���m�F            *
 * =========================================================================== *
 *  PIC 18F25K22 24EP256MC202                                                  *
 *  (���̃��C�u�������̂͑���PIC�ł�OK�Ǝv���܂�)                                 *
 * ��L���x�[�X�ɏC�����������A�قڌ��`���Ƃǂ߂Ă��Ȃ�����
 ******************************************************************************/
#include "debug.h"
#include "skESP8266.h"
#include <string.h>
#include "mcc_generated_files/uart1.h"
#include "mcc_generated_files/pin_manager.h"
#include "LCD320x240color.h"

#define CMD_AT        "AT"          // ���W���[���̓����Ԃ��e�X�g����
#define CMD_CWMODE1   "AT+CWMODE=1" // �X�e�[�V�������[�h
#define CMD_CWMODE2   "AT+CWMODE=2" // �\�t�gAP���[�h
#define CMD_CWJAP     "AT+CWJAP=\"" SSID "\",\"" PASSWORD "\""   // AP�ɐڑ�����
#define CMD_CWQAP     "AT+CWQAP"                                 // AP����؂藣��
//#define CMD_CIPSTART  "AT+CIPSTART=\"TCP\",\"192.168.1.2\",8089" // �T�[�o�ɐڑ�����
#define CMD_CIPCLOSE  "AT+CIPCLOSE"                              // �T�[�o����؂藣��
#define CMD_CIPCLOSE1 "AT+CIPCLOSE=1"                           // �T�[�o����؂藣��
#define CMD_CIPMODE   "AT+CIPMODE=1"      // �g�����X�y�A�����g���[�h
#define CMD_CIPMODE2  "AT+CIPMODE=0"      // �ʏ�(AT�R�}���h)���[�h�ɂ���
//#define CMD_CIPMODE3  "AT+CIPMODE=3"      // �����[�h�ɂ���
#define CMD_CIPSERVER "AT+CIPSERVER=1,80"   // �T�[�o�[���[�h�ɂ���
#define CMD_CIPSERVER0 "AT+CIPSERVER=0"     // �T�[�o�[���[�h�폜
#define CMD_CIPMUX    "AT+CIPMUX=1"       // �}���`�ڑ���L���ɂ���
#define CMD_CIPMUX0   "AT+CIPMUX=0"       // �}���`�ڑ��𖳌�
#define CMD_UART      "AT+UART_CUR=115200,8,1,0,2"  // �t���[����
#define CMD_UARTHS    "AT+UART_CUR=230400,8,1,0,2"  // �t���[����
#define CMD_UARTHS2    "AT+UART_CUR=460800,8,1,0,2"  // �t���[����
#define CMD_UARTHS3    "AT+UART_CUR=921600,8,1,0,2"  // �t���[����
//#define CMD_UART0      "AT+UART_CUR=115200,8,1,0,0"  // �t���[����Ȃ�
#define CMD_UARTCUR   "AT+UART_CUR?"
#define CMD_CIPSNTPCFG "AT+CIPSNTPCFG=1,9,\"ntp.nict.jp\""  //SNTP�̐ݒ�
#define CMD_CIPSNTPTIME "AT+CIPSNTPTIME?"                   //SNTP�œ����擾
#define CMD_CIFSR     "AT+CIFSR"          // IP�A�h���X��MAC�A�h���X�𓾂�
#define CMD_CIPSEND   "AT+CIPSEND"        // �f�[�^�𑗐M����@<length>\r\n<data>
#define CMD_CIPSENDEX "AT+CIPSENDEX"      // �f�[�^�𑗐M����@�f�[�^��\0�����Ă��I��
#define CMD_RST       "AT+RST"      // Restarts the module
#define CMD_RESTORE   "AT+RESTORE"      // 
#define CMD_UART_DEF "AT+UART_DEF?"

/*
 * �o�b�t�@�Ƀf�[�^������΁A�P�o�C�g���̃f�[�^��Ԃ�
 * �������A�o�C�i�����Ή������邽�߁A0���G���[�������������Ƃ�Ȃ��̂ŁA�Ԃ�l�Ŏ���
 * 
 * �Ԃ�l: 
 * �@�@�G���[�̗L���́A*err�Ŕ��f�@�@1:�G���[�A0:����
 * �@�@����Ȏ��A�P�o�C�g�f�[�^��Ԃ�
 */
uint8_t ReadFromBuf(uint8_t *err) {
    uint8_t data;
    
    if (UART1_is_rx_ready()) {
        // ��M�f�[�^�����鎞�́A�ǂݏo��
        *err = 0;
        data = UART1_Read();
    } else {
        *err = 1;
        data = 0;
    }
    return data;
}

/*
 * �p�P�b�g�w�b�_�̏����́A�����ōs��
 * 
 * ��M���ĉ��s�R�[�h����؂�Ƃ���buf�Ɏ��o��
 * �e�L�X�g�A�o�C�i���ł��֌W�Ȃ�
 * 
 * len: �i�[�ł���o�b�t�@�̃T�C�Y�����炢�A���ۂɊi�[�����o�C�g����Ԃ�
 * -1: �f�[�^�Ȃ�
 * 0: ����I���@\r\n�ŏI��
 * �o�b�t�@�t�����A�f�[�^�͎擾�������^�C���A�E�g�̃P�[�X�́A\0�̏I�[�Ȃ��ŁA�ȉ���2�P�[�X
 * 1: \r�Ńf�[�^�I��
 * 2: ��L�ȊO�ŏI��������
 * 
 */
int8_t ESP_GetDataP(char *buf, uint16_t *len, uint8_t skipCR) {
    uint16_t maxlen;
    uint8_t dt, err, preCR;
    uint8_t cnt;
    int8_t ans;
    
    maxlen = *len;  //�i�[�\��������ۑ�
    *len = 0;
    ans = -1;
    cnt = 0;
    preCR = 0;
    
    while (1) {
        dt = ReadFromBuf(&err); //1�o�C�g�ǂݏo��
        if (err) {
            //�o�b�t�@�Ƀf�[�^���Ȃ����́A���s����܂ŏ����҂B
            //�X�^�b�N����\�������邪�A���s���Ȃ��Ə����ł��Ȃ��̂ŁA�K��������̂Ƒz��
            __delay_ms(250);
            DEBUG_PRINTF(".");
//            if ((cnt % 4)==0) display_puts(".");  //1s��1�񂾂��o��
            cnt++;
            if (cnt > 10) {    // 2s�҂��ăf�[�^���Ȃ����͏I��
                if (ans == -1) return ans;
                else return (int8_t)(2 - preCR);
            }
        } else {
            cnt = 0;    //�f�[�^��M�ł�����A���Z�b�g
            buf[*len] = dt;
            *len = *len + 1;
            ans = 1;    //�f�[�^���擾�ł�����1

            if (preCR || skipCR) {
                if (dt == '\n') {
                    // \n�܂ŗ�����Ǐo���I��  \r�̃`�F�b�N����Œǉ����������ǂ��͂�
                    buf[*len] = '\0';    //������I�[��t�����ďI��
                    return 0;    //���s�R�[�h���󂯎������0
                }
                else preCR = 0;
            }
            if (dt == '\r') preCR = 1;
            if (*len >= maxlen) {
                //������I�[�͊i�[�ł����Apred=1�Ȃ�1�A����ȊO��2��Ԃ�
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
 * �R�}���h�ɑ΂���ԐM�f�[�^���`�F�b�N
 * �@����OK�̌��ʂ����g���Ă��Ȃ�
 */
#define ANS_OK        "OK"          // �R�}���h�̕ԓ�"OK"
#define ANS_FAIL      "FAIL"        // �R�}���h�̕ԓ�"FAIL"
#define ANS_ERROR     "ERROR"       // �R�}���h�̕ԓ�"ERROR"

int8_t CheckResponseData(char *data) {
    
    // ��M�f�[�^�̉�͂��s��
    //SEND OK�Ƃ�����������̂ŁAstrcmp�ł̓_��
    if (strstr(data, ANS_OK) != NULL) {
        return 0;      // OK�̕ԓ��L��
    }
    if (strstr(data, ANS_FAIL) != NULL) {
        return 31;     // FAIL�̕ԓ��L(AP�ɐڑ����̃G���[)
    }
    if (strncmp(data, ANS_ERROR, 5) == 0) {
        return 32;     // ERROR�̕ԓ��L
    }
    return -1;
}

/*
 * �Ԃ�l
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
    // OK�ԓ���S����M����܂ő҂�
    while (1) {
        len = sizeof(str);
        ans = ESP_GetData(str, &len);
        //��M�f�[�^������Ȃ�f�[�^�̃`�F�b�N�����{����
        if (ans == 0) {
            str[strlen(str) -2] = '\0';   //���s�R�[�h�����鎞�͍폜
            DEBUG_PRINTF("c[%s]\r\n", str);    //���s�R�[�h���ԐM�ɓ����Ă���͂�
            ans = CheckResponseData(str);
            if ((ans == 0)||(ans == 32)) break;    //�ԓ���OK�Ȃ�I��   ERROR�ł��I���ɂ���
            
            cnt = 0;
        }
        cnt++;
        if (cnt > 4) return ans;   // 5��g���C���ă_���Ȃ�I�������A�������[�v�����
    }
    return ans;
}


/******************************************************************************
 *    �w�肵�����̃f�[�^�𑗐M
 *    dat : ���M����f�[�^���i�[�����z��
 *    num : ���M����f�[�^�̃o�C�g�����w��
 ******************************************************************************/
void UART_Send(const char *dat, uint16_t num)
{
     while (num > 0) {
         UART1_Write(*dat++);        // ���M����
         num--;
     }     
}

/*
 * �R�}���h�����s�R�[�h��t�����đ��M
 * �R�}���h�́A������ŁA\0�ŏI�[����Ă���K�v����
 */
void Send_cmd(const char *cmd)
{
    while (*cmd != 0) {
        UART1_Write(*cmd++);
    }
    UART_Send("\r\n", 2);
    __delay_ms(200);    //�I���W�i����1000ms
}

/******************************************************************************
 *    ESP-WROOM02��AT�R�}���h�𑗐M���鏈��
 *    �R�}���h������̍Ō��\0�ŏI�[�A[CR][LF]�͊֐��ŕt��
 *    ���̊֐��̓R�}���h�ɑ΂���ԓ���҂��܂��A��5s�Ń^�C���A�E�g
 *
 *    cmd : ���M����R�}���h���i�[�����z��ւ̃A�h���X���w��
 *    ans : 0=����  31=AP�ɐڑ����̃G���[  32=ERROR�ŕԓ�����  -1=���肪�ԓ���
 ******************************************************************************/
int8_t ESP_CommandSend(const char *cmd)
{
    // AT�R�}���h�𑗐M����
    Send_cmd(cmd);
    
    return ESP_Receive();
}


/*
 * ESP-WROOM-02D����������擾
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
    //�������ς݂Ȃ̂ŁA�ȉ��̐ݒ肾�����{
    ESP_CommandSend(CMD_CIPSNTPCFG);
    
    Send_cmd(CMD_CIPSNTPTIME);
    ans = -1;
    // �ԓ�OK������܂Ń��[�v
    while(1) {
        len = sizeof(st);
        ans = ESP_GetData(st, &len);
        //��M�f�[�^������Ȃ�f�[�^�̃`�F�b�N�����{����
        if (ans == 0) {
            DEBUG_PRINTF("--%s", st);    //���s�R�[�h���ԐM�ɓ����Ă���͂�
            if (strncmp(st, "+CIP", 4) == 0) {
                //+CIPSNTPTIME:Mon May 20 20:32:34 2024  �Ƃ����t�H�[�}�b�g
                sscanf(st+13, "%s %s %hhd %hhd:%hhd:%hhd %hd", wday, stmon, dd, h, m, s, &year);
                *yy = year % 100;
                jj = 12;
                while ((strcmp(stmon, month[jj]) != 0) && (jj > 0)) jj--;
                *mm = jj;
            } else {
                if (CheckResponseData(st) == 0) break;    //OK����M������I��
            }
        }
        else {
            //�G���[�̎��́A�������Ȃ�
            DEBUG_PRINTF("data error ");
            break;
        }
    }
    DEBUG_PRINTF("complete get time>>>>>\r\n");
}

/*
 * ESP-WROOM02���W���[���̏�����
 * 0: ����I��
 * ����ȊO�͉��炩�̃G���[�����̂ŁA�v�f�o�b�O
 */
int8_t ESP_Init(void) {
    int8_t ans;

    DEBUG_PRINTF("uart1RxCount=%d\r\n", uart1RxCount);
    DEBUG_PRINTF("Initialize ESP\r\n");
    UART1_Initialize(); //�ŏ��Ɏ��s����Ă��邪�A�o�b�t�@���I�[�o�[�t���[���Ă���ꍇ������̂ŁA�����ōēx���Z�b�g
    ans = ESP_CommandSend(CMD_AT);
    display_puts(".");
    if (ans != 0) {
        // �g�����X�y�A�����g���[�h�����H�@�����Ȃ��x���[�h����
        UART_Send("+++", 3);
        __delay_ms(1500);    // �Œ�1s���҂��Ă���A����AT�R�}���h�𑗐M����K�v����
        ans = ESP_CommandSend(CMD_AT);
        if (ans != 0) return 1;
    }
    
    ans = ESP_CommandSend(CMD_UARTCUR);    //������Ԃ��m�F
    ans = ESP_CommandSend(CMD_UART);    //115200bps,8bit,stop=1bit,�p���e�B�Ȃ�,enable CTS
    display_puts(".");
    if (ans != 0) return 11;
    
    //�{�[���[�g��115kbps���獂����
    //115kbps�̏�ԂŁA�ݒ��ύX���āA���̌�APIC����U1BRGL��ύX����
    ans = ESP_CommandSend(CMD_UARTHS3);    //921600 bps,8bit,stop=1bit,�p���e�B�Ȃ�,enable CTS
    display_puts(".");
    DEBUG_PRINTF("ans=%d\r\n", ans);
    if (ans != 0) return 11;
    U1BRGL = 0x10;  //921600 bps���삷��悤�ɐݒ�
    
    ans = ESP_CommandSend(CMD_UARTCUR);    //������Ԃ��m�F

    //�t���[����p�Ƀs���̐ݒ��ύX
    RTS1_SetLow();  //�t���[����ŁA��M�J�nOK

    // AP�ɐڑ�����@�@����͑��̃R�}���h��莞�Ԃ�������
    ans = ESP_CommandSend(CMD_CWJAP);
    display_puts(".");
    if (ans != 0) return 6;
    //�X�e�[�V�������[�h
    ans = ESP_CommandSend(CMD_CWMODE1);
    display_puts(".");
    if (ans != 0) return 8;
    // �}���`�ڑ���L���ɂ���
    ans = ESP_CommandSend(CMD_CIPMUX);
    display_puts(".");
    if (ans != 0) return 7;
    
    return 0;
}

/*****************************************************************************
 *    ESP-WROOM02���T�[�o�[���[�h�ŏ�����
 *    (�X�e�[�V�������[�h/�}���`�ڑ�/�T�[�o�[���N��)
 *    ans : 0=����  ���s�����ꍇ�͉����Ŏ��s�������i���ԍ���Ԃ��܂�
 ******************************************************************************/
int8_t ESP_ServerInit(void)
{
    int8_t ans;
    
    DEBUG_PRINTF("Initialize Server\r\n");
    // �T�[�o�[���N������
    ans = ESP_CommandSend(CMD_CIPSERVER);
    if (ans != 0) return 8;
    return 0;
}

/******************************************************************************
 *    �N���C�A���g��HTML�e�L�X�g�𑗐M���鏈��
 *
 *    id  �F���M���鑊��̃N���C�A���gID���w��
 *    *txt�F���M����HTML�e�L�X�g�@\0�I�[
 * ESP-WROOM-02D�ł�20848�o�C�g����x�ɑ��M�ł���Max�ŁA����ȏ㑗�M���悤�Ƃ����ERROR�ƂȂ�
 * �ʐM������Z���Ԃɂ��邽�߁A�p�P�b�g��傫��������
 * �������M���������ɂ�����Ăяo���A����ł�OK�BCIPSENDEX�͎����I�ɔ��s�B
 * ��{�I��2048�o�C�g���Ƃɑ��M����B
 * ���f�����������́Atxt��NULL�ŌĂяo��
 * 
 * �G���[�̎��A1��Ԃ�
 */
int8_t ESP_HtmlSnd2(uint8_t id, const char *txt)
{
    int8_t ans;
    char st[80];
    int timeout = 0;
    static uint16_t remainLen = 0;

    if (txt == NULL) {
        if (remainLen == 0) return 0;
        //�󕶎��̎��͏������I��������
        if (remainLen > 2) {
            UART1_Write(0x5c);  // CPISENDEX��"\\0"�ŏI���ł���
            UART1_Write('0');
            remainLen = 0;  //���̑��M�̂��߂Ƀ��Z�b�g���Ă����K�v����
        } else {
            //�c�著�M�ʂ�2�o�C�g�ȉ��̎��́A�󔒂𑗐M
            while (remainLen-- > 0) UART2_Write(' ');
        }
        ans = ESP_Receive();
        DEBUG_PRINTF("�I�� ans=%d\r\n", ans);
        return ans;
    }

    do {
        if (remainLen == 0) {
            // ����f�[�^�̃T�C�Y��2048�Œ�
            remainLen = 2048;
            sprintf(st, "%s=%d,%d", CMD_CIPSENDEX, id, remainLen);
            do {
                ans = ESP_CommandSend(st);
                if (ans == 0) break;
            } while (timeout++ < 5);    //�Ō�A6�Ŕ�����
            if (timeout > 5) return ans;    //ans��0�ȊO
        }
        
        // HTML�e�L�X�g��2048�o�C�g�ɂȂ�܂ő��M
        while ((*txt != '\0') && (remainLen > 0)) {
            UART1_Write(*txt++);
            remainLen--;
        }
        if (remainLen == 0) {
            ans = ESP_Receive();
            DEBUG_PRINTF("RCV:%d\r\n", ans);
        }
    } while (*txt != '\0');   //���M�f�[�^���c���Ă�����A���̃p�P�b�g�𑗐M������        

    return 0;
}

void ESP_HtmlSndEnd(uint8_t id)
{
    char st[80];
    
    // �����TCP�|�[�g���N���[�Y���ڑ���ؒf����       // �N���C�A���gID
    sprintf(st, "%s=%d", CMD_CIPCLOSE, id);
    ESP_CommandSend(st);
}


/******************************************************************************
 *    IP�A�h���X���擾
 *    �G���[���ɂ��A�h���X�������Ȃ������ꍇ�A�������Ȃ�
 *
 *    *ip  : IP�A�h���X��Ԃ�("xxx.xxx.xxx.xxx"�ŕԂ��AMAX16����)
 ******************************************************************************/
void ESP_GetIPaddress(char *ip)
{
    int8_t ans;
    char st[80];
    uint16_t len;
    
    Send_cmd(CMD_CIFSR);

    ans = -1;
    // �ԓ���S����M����܂ő҂�
    while (1) {
        len = sizeof(st);
        ans = ESP_GetData(st, &len);
        DEBUG_PRINTF("--%s", st);    //���s�R�[�h���ԐM�ɓ����Ă���
        //��M�f�[�^������Ȃ�f�[�^�̃`�F�b�N�����{����
        if (ans == 0) {
            if (strncmp(st+10, "IP,", 3)==0) {
                sscanf(st+14, "%[^\"]", ip);
            }
            else {
                ans = CheckResponseData(st);
                if (ans == 0) break;    //OK�������犮��
            }
        }
        else {
            break;      //�G���[�̎�
        }
    }
}
