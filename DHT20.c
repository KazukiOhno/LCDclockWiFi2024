/*
 * File:   DHT20.c
 * Author: K.Ohno
 *
 * 温湿度センサ：ASAIR DHT20, ±0.01℃, ±0.024%RH
 * 
 * I2C通信　　　400kHz動作も可能
 * I2C2インタフェースをRTCと共用。バスのpullupは、RTCモジュールに任せる
 *          _____________
 * VDD  1--|  □ □ □   |
 * SDA  2--|  □ □ □ □|
 * GND  3--|  □ □ □ □|
 * SCL  4--|  □ □ □   |
 *          -------------
 * ピンコネは、AM2320と同一なので、置き換え可
 * 
 */

#include "DHT20.h"
#include "mcc_generated_files/examples/i2c2_master_example.h"

#define DHT20  0x38    //DHT20のI2Cアドレス
//7bit長のLSBにあるR/Wビットを除いたアドレス(7bit長)
//実際に使用する時は、LSBにR/Wが付け加わるので、Write=0x70、Read=0x71


/*
 * CRCの計算
 * CRC[7:0] = 1 + X^4 + X^5 + X^8 = 0x131
 */
uint8_t Calc_CRC8(uint8_t *message, uint8_t num) {
    uint8_t ii;
    uint8_t byte;
    uint8_t crc = 0xFF;
    
    for (byte = 0; byte < num; byte++) {
        crc ^= message[byte];
        for (ii = 8; ii> 0; --ii) {
            if (crc & 0x80) crc = (uint8_t)(crc<<1) ^ 0x31;
            else crc = (uint8_t)(crc<<1);
        }
    }
    return crc;
}

typedef union {
    struct {
        uint32_t d32;
    };
    struct {
        uint8_t d1;     //LSB
        uint8_t d2;
        uint8_t d3;
        uint8_t d4;     //MSB
    };
} uint32Byte;

uint8_t AHT20_Read_CTdata(uint32_t *ct) {
    uint8_t data[7];
    uint32Byte data32;
	uint16_t cnt = 0;
	
    I2C2_Write2ByteRegister(DHT20, 0xAC, 0x3300);   //Send measurement command
    __delay_ms(80);

    cnt = 0;
	while (((I2C2_Read1ByteRegister(DHT20, 0x71) & 0x80) == 0x80)) {
        //wait for idle.  Bit7=1: Equipment is busy
        __delay_ms(2);  //		SensorDelay_us(1508);
		if (cnt++ >= 100) {
            break;
        }
	}

    I2C2_ReadNBytes(DHT20, data, 7);    //7バイト読み出す
    //1st byte: state
    //2nd byte: Humidity (MSB)
    //3rd byte: Humidity
    //4th byte: Humidity data (4bit), Temperature data (4bit) MSB
    //5th byte: Temperature data
    //6th byte: Temperature data (LSB)
    //7th byte: CRC data

    //crcチェックをする
    if (Calc_CRC8(data, 6) == data[6]) {
        //unionを利用したら、200バイト小さくなった
        data32.d4 = 0;
        data32.d3 = data[1];
        data32.d2 = data[2];
        data32.d1 = data[3];
        ct[0] = data32.d32 >> 4;        //Humidity 20bit data
        
        data32.d3 = data[3];
        data32.d2 = data[4];
        data32.d1 = data[5];
        ct[1] = data32.d32 & 0x0fffff;  //Temperature 20bit data
        return 0;
    } else {
        //CRCエラーの時
        return 1;
    }
}


void JH_Reset_REG(uint8_t addr) {
    uint8_t data[3];
    
    I2C2_Write2ByteRegister(DHT20, addr, 0x0000);

	__delay_ms(5);
    I2C2_ReadNBytes(DHT20, data, 3);
    
    __delay_ms(10);
    I2C2_Write2ByteRegister(DHT20, 0xB0 | addr, data[1]);

}

/*
 * サンプルプログラムから以下の設定が必要とのこと
 */
void AHT20_Start_Init(void) {
	JH_Reset_REG(0x1b);
	JH_Reset_REG(0x1c);
	JH_Reset_REG(0x1e);
}


/*****************************
 * 温湿度情報を取得
 * 結果は、各10倍にした整数で返す
 * 例：25.6C=256
 *****************************/
void Get_TempHumidity(int16_t *temp, int16_t *humidity) {
    uint32_t CT_data[2];

    //Initialization
    if ((I2C2_Read1ByteRegister(DHT20, 0x71) & 0x18) != 0x18) {
        AHT20_Start_Init();
        __delay_ms(10);
    }

    if (AHT20_Read_CTdata(CT_data)) return;     //エラー時はそのままリターン
        
    // エラーでなければ、10倍した値に変換
    *humidity = CT_data[0]*100*10/1024/1024;    //実際の湿度は、1/10
    *temp = CT_data[1]*200*10/1024/1024 - 500;  //実際の温度は、1/10

}
