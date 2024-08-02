/*
 * File:   DHT20.c
 * Author: K.Ohno
 *
 * �����x�Z���T�FASAIR DHT20, �}0.01��, �}0.024%RH
 * 
 * I2C�ʐM�@�@�@400kHz������\
 * I2C2�C���^�t�F�[�X��RTC�Ƌ��p�B�o�X��pullup�́ARTC���W���[���ɔC����
 *          _____________
 * VDD  1--|  �� �� ��   |
 * SDA  2--|  �� �� �� ��|
 * GND  3--|  �� �� �� ��|
 * SCL  4--|  �� �� ��   |
 *          -------------
 * �s���R�l�́AAM2320�Ɠ���Ȃ̂ŁA�u��������
 * 
 */

#include "DHT20.h"
#include "mcc_generated_files/examples/i2c2_master_example.h"

#define DHT20  0x38    //DHT20��I2C�A�h���X
//7bit����LSB�ɂ���R/W�r�b�g���������A�h���X(7bit��)
//���ۂɎg�p���鎞�́ALSB��R/W���t�������̂ŁAWrite=0x70�ARead=0x71


/*
 * CRC�̌v�Z
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

    I2C2_ReadNBytes(DHT20, data, 7);    //7�o�C�g�ǂݏo��
    //1st byte: state
    //2nd byte: Humidity (MSB)
    //3rd byte: Humidity
    //4th byte: Humidity data (4bit), Temperature data (4bit) MSB
    //5th byte: Temperature data
    //6th byte: Temperature data (LSB)
    //7th byte: CRC data

    //crc�`�F�b�N������
    if (Calc_CRC8(data, 6) == data[6]) {
        //union�𗘗p������A200�o�C�g�������Ȃ���
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
        //CRC�G���[�̎�
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
 * �T���v���v���O��������ȉ��̐ݒ肪�K�v�Ƃ̂���
 */
void AHT20_Start_Init(void) {
	JH_Reset_REG(0x1b);
	JH_Reset_REG(0x1c);
	JH_Reset_REG(0x1e);
}


/*****************************
 * �����x�����擾
 * ���ʂ́A�e10�{�ɂ��������ŕԂ�
 * ��F25.6C=256
 *****************************/
void Get_TempHumidity(int16_t *temp, int16_t *humidity) {
    uint32_t CT_data[2];

    //Initialization
    if ((I2C2_Read1ByteRegister(DHT20, 0x71) & 0x18) != 0x18) {
        AHT20_Start_Init();
        __delay_ms(10);
    }

    if (AHT20_Read_CTdata(CT_data)) return;     //�G���[���͂��̂܂܃��^�[��
        
    // �G���[�łȂ���΁A10�{�����l�ɕϊ�
    *humidity = CT_data[0]*100*10/1024/1024;    //���ۂ̎��x�́A1/10
    *temp = CT_data[1]*200*10/1024/1024 - 500;  //���ۂ̉��x�́A1/10

}
