/* 
 * File:   AlarmSound.c
 * Author: K.Ohno
 *
 * ���d�X�s�[�J�[
 * CCP2��PWM���[�h(TMR2)�ŁA���d�X�s�[�J��炷:
 * �@250us=4kHz, duty=50%���r�[�v���Ƃ��Đݒ�
 *  
 * TMR0: ���Ă��鎞�Ԃ𐧌�@10ms���Ɋ��荞�݂��|���A�������ԁA�������Ԃ𐧌�
 * 
 */

#include "AlarmSound.h"
#include <xc.h>
#include "mcc_generated_files/tmr2.h"


/*
 * �A���[���̃p�^�[�����`
 * �f�[�^�`��:�@10ms��P�ʂɖ��Ă��鎞�ԂƏ����̎��Ԃ��y�A�Œ�`
 * �@�@�@�@�@�@�f�[�^�̏I����0�Ŏ���
 */
uint8_t SoundData[][10] = {
    {10, 20, 10, 60,  0, },             //�s�s�b�ƂȂ炷�ŏ���30�b�p
    {10, 10, 10, 10, 10, 50,  0, },     //�s�s�s�ƂȂ炷30�b-1���p
    {10, 10, 10, 10,  0, },             //�s�s�s�s�Ɩ炷
    //���ʉ�
    { 7,  1,  0, },             //�s�b��1��炷
    { 7, 15, 20,  1,  0, },     //2��s�b�s�[�Ɩ炷
};
uint8_t SoundNum;       //�A���[���̎�ނ��w��
uint8_t SoundDataPos;   //���f�[�^�z��̂ǂ̃f�[�^���g����
uint8_t SoundCount;     //���̃I��/�I�t�̒������J�E���g�_�E��
uint16_t SoundTime;     //�A���[�������Ă��鎞�Ԃ𑪒� (max 3000=30s�܂�)
uint8_t SoundFlag = 0;  //����炵�Ă��鎞�́A1

/*
 * TMR0�̊��荞�݂̃n���h�� 10ms���ɂ����ɂ���
 * 
 * SoundCount�����Z���āA0�ɂȂ�����A���̃f�[�^=���Ԃ�ݒ�
 * �������A�������ŁACCP2�̃I���I�t����
 * 
 * �܂��A���[�����葱������30�b���ƂɃA���[������ύX������
 * 
 */
void SoundHandler() {

    //�A���[���炵���ςȂ��̎��A30�b�A60�b�o�߂�����A���[������ύX����
    SoundTime++;    //�����o���Ă���̌o�ߎ��Ԃ��v���@10ms�P��
    if (SoundTime >= 24000) {
        //�A���[����240s=4���ȏ�o�߂�����A�����I�Ɏ~�߂�B�ŏ���1���͕ʘg�Ȃ̂ŁA������4���B
        CCP2EN = 0;     //��������
//        CCP2CONbits.CCP2EN = 0;     //���̕\�L�̕��@���v���O�����T�C�Y�傫���Ȃ�
        SoundFlag = 0;
        return;
    } else if (SoundTime >= 3000) {
        //�O�̏��u����30s�o�߂�����
        if (SoundNum < AlarmSound2) {
            SoundNum++; //�A���[���������̉��ɐi�߂�
            SoundTime = 0;  // �炵�Ă��鎞�Ԃ�0�Ƀ��Z�b�g
            SoundDataPos = 0;
            SoundCount = SoundData[SoundNum][SoundDataPos];
            SoundDataPos++;
            CCP2EN = 1;     //�����o��
            return;
        }
    }

    //�����ŉ������
    SoundCount--;
    if (SoundCount == 0) {
        //SoundCount��10ms���荞�ݖ��J�E���g�_�E�����āA0�ɂȂ�����A���̃f�[�^��ǂݍ���
        if (SoundData[SoundNum][SoundDataPos] == 0) {
            //�f�[�^�̏I���ɗ������̏���
            if (SoundNum <= AlarmSound2) {
                //�A���[���͉������s�[�g�����邽�߁A�ʒu��0���Z�b�g
                SoundDataPos = 0;
            } else {
                //1�񂾂��炷�ꍇ�́A�����ŏI��
                CCP2EN = 0;
                SoundFlag = 0;
                return;
            }
        }
        //���̃f�[�^�����o��
        SoundCount = SoundData[SoundNum][SoundDataPos];
        
        //SoundDataPos����̎��T�E���hOff�A�����̎���on
        if (SoundDataPos % 2) {
            CCP2EN = 0;
        } else {
            CCP2EN = 1;
        }
        SoundDataPos++;
    }
}

/*
 * �A���[���T�E���h�J�n
 * �A���[����炷���߂̐ݒ�����{
 */
void AlarmSoundOn(uint8_t sn) {
    //�T�E���h�f�[�^�Z�b�g
    SoundNum = sn;  //�T�E���h�̎�ނ�ݒ�
    SoundTime = 0;  //�A���[�������Ă��鎞��
    SoundDataPos = 0;
    SoundCount = SoundData[SoundNum][SoundDataPos++]; //�����l�ݒ�
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
