/*
 * �C�x���g�����v���O����
 * �����ݒ���PushEvent()�́A���{���Ȃ����ƁBreentrant�Ƃ������[�j���O���o�邾�������A
 * ���ۂɔ�������Ɩ��ȃo�O�ɂȂ�\������
 * 
 * 2024/5/24 �Ɨ�������
 * 
 */

#include "event.h"
#include "mcc_generated_files/mcc.h"

//Event��z��ɂ��āA�L���[���������Ă���BPushEvent��PopEvent�ő���
uint8_t Event[16];  //�C�x���g�������ɂ�����push����
uint8_t PushPoint;
uint8_t PopPoint;
uint8_t ContNum;    //�L���[�ɓ����Ă���x���g��


/*
 * �C�x���g�n���h���̐ݒ�
 * �Ăяo���̂́A���EventHandler
*/
void SetEventHandler(void (* handler)(void)){
    PreviousEventHandler = NULL;
    EventHandler = handler;
}

/*
 * �����点�Ă���C�x���g�n���h����ۑ����āA�V���ȃC�x���g�n���h���𑖂点��
 */
void PopupEventHandler(void (* handler)(void)){
    PreviousEventHandler = EventHandler;    //���݂̃n���h����ۑ�
    EventHandler = handler;
}

/*
 * �ێ����Ă����C�x���g�n���h���ɖ߂�
 */
void ReturnEventHandler(void){
    if (PreviousEventHandler) EventHandler = PreviousEventHandler;
}

/*
 * �C�x���g�o�b�t�@���t����������A1��Ԃ�
 */
uint8_t PushEvent(uint8_t ev) {
    if (ContNum < sizeof(Event)) {
        Event[PushPoint] = ev;
        PushPoint = (PushPoint +1) % sizeof(Event);
        ContNum++;  //�o�b�t�@�̓T�C�N���b�N�Ɏg���̂ŁA�����L����������
        return 0;
    } else {
        return 1;   //�o�b�t�@�t���G���[
    }
}

/*
 * �C�x���g�o�b�t�@�ɉ��������Ă��Ȃ����́A0��Ԃ�
 */
uint8_t PopEvent(void) {
    uint8_t ev;

    if (ContNum > 0) {
        ev = Event[PopPoint];
        PopPoint = (PopPoint + 1) % sizeof(Event);
        ContNum--;
    } else {
        ev = 0; //�o�b�t�@����̎�
    }
    return ev;
}

/*
 * �o�b�t�@�̃N���A
 */
void ClearEventBuffer(void) {
    PushPoint = 0;
    PopPoint = 0;
    ContNum = 0;    //�L���[�ɓ����Ă��鐔
}
