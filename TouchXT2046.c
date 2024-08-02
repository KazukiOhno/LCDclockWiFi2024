/*
 * File:   TouchXT2046.c
 * Author: K.Ohno
 *
 *Color LCD 320x240�ɓ��ڂ̃^�b�`�C���^�t�F�[�X�p�ėp���[�`��
 * 
 * LCD�{�[�h���5�s���C���^�t�F�[�X
 * T_IRQ: �^�b�`�������ALow�ɂȂ�
 * T_DO: PIC��SDI�ɐڑ�
 * T_DIN: PIC��SDO�ɐڑ�
 * T_CS: �^�b�`�̃`�b�v�Z���N�g�@Low��Active
 * T_CLK: SPI�C���^�t�F�[�X�p�N���b�N
 * 
 * SPI�C���^�t�F�[�X��LCD�Ƌ��L���ACS�ŒʐM����Ώۂ�I��
 * �ő�Ή����x=2MHz
 * 
 */

#include "debug.h"
#include "TouchXT2046.h" 

/*
 * �^�b�`���W�̐��̒l���擾����
 * 12�r�b�g�̍��W���擾
 */
void GetTouchRawXY(uint16_t *tx, uint16_t *ty) {
    uint8_t data[2];

    //S A2 A1 A0 MODE SER/DFR PD1 PD0
    //1 1  0  1
    //  YP=+IN X-Position �𑪒�(�������ɂ��Ă���̂ŁAY������)
    SPI1_ExchangeByte(0b11010011);
    SPI1_ReadBlock(data, 2);    //12�r�b�g�A2�o�C�g������ǂݍ���
    *ty = ((uint16_t)data[0] << 4) | (data[1] >> 4);
    
    // Y-Position (���̎g�����ł�X���W)���擾
    SPI1_ExchangeByte(0b10010011);
    SPI1_ReadBlock(data, 2);    //12�r�b�g�A2�o�C�g������ǂݍ���
    *tx = ((uint16_t)data[0] << 4) | (data[1] >> 4);
}


/*
 * �^�b�`����X,Y���W��Ԃ�
 * �G���[�̎��A-1��Ԃ�
 * oversampling��15����8�ɂ��Ă݂����A���܂茋�ʂ͗ǂ��Ȃ�����
 * 
 * �^�b�`�������W���͂��̃��[�`���ł̂ݎ擾
 */
int8_t GetTouchLocation(uint16_t *tx, uint16_t *ty) {
    int8_t jj;
    uint8_t oversampling = 15;   //�^�b�`���W��15���荞�ݕ��ω�����
    uint16_t sample_sumX;   //12bit�f�[�^�Ȃ̂ŁA16��(=4bit)�܂ŉ��Z�ł���
    uint16_t sample_sumY;
    uint16_t samplesX[16], samplesY[16];    //oversampling�̐��ɍ��킹�邱��
    uint8_t num;
    uint16_t avg, diff, overrange;

    if (T_IRQ_GetValue() != 0) return -1;

    if (SPI1_Open(Touch2M_CONFIG) == false) return -1;
    //2MHz�@�f�t�H���g�ݒ�8MHz�ɂ��Ă���̂ŁA�^�b�`�p�ɒᑬ�ɂ���
    T_CS_SetLow(); //�^�b�`CS��Low��
    
    // oversampling�񑪒肵�A���ώ��
    sample_sumX = 0;
    sample_sumY = 0;
    for (jj = 0; jj < oversampling; jj++) {
        GetTouchRawXY(&samplesX[jj], &samplesY[jj]);
        sample_sumX += samplesX[jj];
        sample_sumY += samplesY[jj];
    }
    SPI1_ExchangeByte(0b10000000);  // PowerDown�ɂ��āAIRQ��Enabled�ɂ���
    
    T_CS_SetHigh(); //CS��High��
    SPI1_Close();
    
    avg = sample_sumX / oversampling;
    num = oversampling;
    overrange = avg / 10;    //���ϒl��10%
    for (jj = 0; jj < oversampling; jj++) {
        if (avg > samplesX[jj]) diff = avg - samplesX[jj];
        else diff = samplesX[jj] - avg;
        if (diff > overrange) {
            //���ς��O��Ă���f�[�^���폜
            sample_sumX -= samplesX[jj];
            num--;
        }
    }
    if (num > 0) *tx = sample_sumX / num;
    else return -1;  //�L���f�[�^�Ȃ��̂ŁA�G���[
    
    avg = sample_sumY / oversampling;
    num = oversampling;
    overrange = avg / 10;    //���ϒl��10%�@�@����80�ɂ��Ă���
    for (jj = 0; jj < oversampling; jj++) {
        if (avg > samplesY[jj]) diff = avg - samplesY[jj];
        else diff = samplesY[jj] - avg;
        if (diff > overrange) {
            //���ς��傫���O��Ă���f�[�^���폜
            sample_sumY -= samplesY[jj];
            num--;
        }
    }
    if (num > 0) *ty = sample_sumY / num;
    else return -1;  //�L���f�[�^�Ȃ��̂ŁA�G���[

    DEBUG_PRINTF("T1=(%d, %d)\r\n", *tx, *ty);

    return 0;    
}
