/**
  Generated Main Source File

  Company:
    Microchip Technology Inc.

  File Name:
    main.c

  Summary:
    This is the main file generated using PIC10 / PIC12 / PIC16 / PIC18 MCUs

  Description:
    This header file provides implementations for driver APIs for all modules selected in the GUI.
    Generation Information :
        Product Revision  :  PIC10 / PIC12 / PIC16 / PIC18 MCUs - 1.81.8
        Device            :  PIC18F27K42
        Driver Version    :  2.00
*/

/*
    (c) 2018 Microchip Technology Inc. and its subsidiaries. 
    
    Subject to your compliance with these terms, you may use Microchip software and any 
    derivatives exclusively with Microchip products. It is your responsibility to comply with third party 
    license terms applicable to your use of third party software (including open source software) that 
    may accompany Microchip software.
    
    THIS SOFTWARE IS SUPPLIED BY MICROCHIP "AS IS". NO WARRANTIES, WHETHER 
    EXPRESS, IMPLIED OR STATUTORY, APPLY TO THIS SOFTWARE, INCLUDING ANY 
    IMPLIED WARRANTIES OF NON-INFRINGEMENT, MERCHANTABILITY, AND FITNESS 
    FOR A PARTICULAR PURPOSE.
    
    IN NO EVENT WILL MICROCHIP BE LIABLE FOR ANY INDIRECT, SPECIAL, PUNITIVE, 
    INCIDENTAL OR CONSEQUENTIAL LOSS, DAMAGE, COST OR EXPENSE OF ANY KIND 
    WHATSOEVER RELATED TO THE SOFTWARE, HOWEVER CAUSED, EVEN IF MICROCHIP 
    HAS BEEN ADVISED OF THE POSSIBILITY OR THE DAMAGES ARE FORESEEABLE. TO 
    THE FULLEST EXTENT ALLOWED BY LAW, MICROCHIP'S TOTAL LIABILITY ON ALL 
    CLAIMS IN ANY WAY RELATED TO THIS SOFTWARE WILL NOT EXCEED THE AMOUNT 
    OF FEES, IF ANY, THAT YOU HAVE PAID DIRECTLY TO MICROCHIP FOR THIS 
    SOFTWARE.
*/

/*
 * PIC 18F27K42 + ESP-WROOM-02D
 * 
 * ESP���g���AWiFi��NTP�Ŏ������킹�����{ & �t�H���g�f�[�^�̃A�b�v���[�h�Ή�
 * SNTP�T�[�o����AESP-WROOM-02D��AT�R�}���h���g���ē����擾
 * 
 * UART1: TX1=RC6, RX1(PGD)=RB7  WiFi���j�b�g�ƒʐM
 * UART2: TX2(PGC)=RB6, RX2=RB7  �f�o�b�O�o�́@FT232��RL�����USB��PC�ɐڑ�
 * �@�@�@�@�@�@�@�@�@�@�@RX2��disable�ɂł��Ȃ��̂ŁARX1�Ɠ����s���Ɋ���U���Ă���
 * GPIO: RTS1=RB5
 * GPIO: IO15=RC7
 * 
 * TMR3: 4ms�@�@�f�o�b�O�̂��߁A1�b���Ƃ̃^�C�}�����݂Ƃ��Đݒ�
 * UART�ʐM�́A�ǂ����115200kbps�A�񓯊�8bit�AUART1�̂݊����ݎg�p
 * 
 * ��H�\��
 * ESP-WROOM-02D
 *            ________________
 *      VINT |3V3         GND | GND
 *      VINT |EN          IO16|
 *           |IO14        TOUT|
 *           |IO12        RST | VINT�Œ�
 * RB5|RTS1  |IO13|CTS    IO5 |
 * RC7|IO15  |IO15|RTS    TXD | -- UART1 RX1|RB7
 *           |IO2         RXD | -- UART1 TX1|RC6
 *           |IO0         IO4 |
 *           -----------------
 * IO15=RC7 �d��������=Low��AHi-Z�ɂ��ăt���[����
 * Flash boot: IO15/0/2=L/H/H�œd��On
 * �@�@IO0=floating, IO2������pull-up����Ă���̂ŁA�I�[�v���Ŗ��Ȃ�
 * EN, RST: ����VINT�����ɂ����B
 * 
 * PIC 18F27K42  64MHz, 128KB
 *             MCLR  |MCLR   RB7|�� PGD USRT1 RX1(RX2) from TXD of ESP
 *     LCD_CS      ��|RA0    RB6|�� PGC UART2 TX2 to USB serial (Debug)
 *     LCD_RESET   ��|RA1    RB5|�� RTS1 for CTS(IO13)@ESP
 *     LCD_DCRS    ��|RA2    RB4|�� RTC_INTB   w/WPU  IOC negative
 *     PhotoD ANA3 ��|RA3    RB3|�� CCP2 for piezo speaker
 *     T_IRQ       ��|RA4    RB2|�� I2C2 SDA2 for RTC/DHT20
 *     T_CS        ��|RA5    RB1|�� I2C2 SCL2
 *                   |VSS    RB0|�� EXT_INT INT0 RTC_INTA   w/WPU
 *     ALMSW       ��|RA7    VDD|
 *     SDCard_CS   ��|RA6    VSS|
 *     ROT_B       ��|RC0    RC7|�� IO15  Low -> Hi-Z
 *     ROT_A       ��|RC1    RC6|�� UART1 TX1 to RXD of ESP
 *     PWM6        ��|RC2    RC5|�� SPI1 SDO1 to MOSI
 *     SPI1 SCK1   ��|RC3    RC4|�� SPI1 SDI1 from MISO
 * 
 * PIC 18F27K42
 *   SPI1:  SCK1=RC3, SDI1=RC4, SDO1=RC5          ===> LCD(16M)/SDcard(8M)/Touch(2M)
 *          CS=RA6
 *   UART1: RX1=RB7, TX1=RC6                      ===> ESP-WROOM-02D
 *          RTS1=RB5, IO15=RC7
 * �@�@�@ESP�Ɣ񓯊��ʐM115Kbps�ő���M
 * �@�@�@PIC����RTS�M���𔭐M���āA�o�b�t�@�I�[�o�[�t���[�ɂȂ�Ȃ��悤�ɐ���
 *   UART2: TX2=RB6 (RX2=RB7 RX1�Ɠ����s���ɒ�`���Ă��邪���g�p�Ȃ̂�OK) ===> USB-Serial for Debug
 *   �@�@TX2���g���ăf�o�b�O�p�̃f�[�^���M(PICkit2�Ƌ��ʂł����Ȃ�)
 * �@�@�@UART2���瑗�M����PC TeraTerm��Ń`�F�b�N�����������������֗�
 * �@�@�@TeraTerm�̃X�s�[�h�ݒ�ɒ���
 *   PWM6: LCD�̃o�b�N���C�g
 * �@�@�@
 * 2024/5/24�@��v�@�\����荞�񂾁FLCDclockWiFi2024-13  size=110,669 bytes
 * �@�@LCD�f�B�X�v���C�ASD�J�[�h�AESP�ɉ�����
 * �@�@RTC�A�����x�Z���T�A�^�b�`�A���[�^���[�G���R�[�_�A�A���[��SW�A�T�E���h�A�Ɠx�Z���T
 * �@�@RTC:�@�@1�b�����ݔ����A�A���[������
 * �@�@DHT20:�@�����x����
 * �@�@Touch:�@
 * �@�@���[�^���[:�@
 * �@�@ALMSW:�@On/Off�@�`���^�����O�h�~
 * �@�@PhotoTransistor: 
 * Ver.13
 * �@�@�Ƃ肠�����œK������
 * Ver.14
 * �@�@���C�����\�[�X�̍œK���ɒ���
 * Ver.15
 * �@�@�T�[�o�֌W�̏������č\�z����
 * Ver.16
 * �@�@�ݒ�֌W�𐮗����������@�@Web�T�C�g�֘A�̍X�V
 * Ver.17
 * �@�@�f�[�^�\���A���ɃJ�X�^���t�H���g�Ή��̕��������ǂ��邽�߁A�啝�C��
 * �@�@�t�H���g�R�[�h�́A����0,1,2�Ƃ��āA�O���t�H���g�́A�R�[�h��10=EXTFONTCODE�ɂ��āA
 * �@�@�t�H���g����ʕϐ��Ɋi�[��������ɁB
 * �@�@�e�I�u�W�F�N�g�Ƀt�H���g���������|�C���^��ǉ�
 * Ver.19
 * �@�@Ver.17�͂Ȃ����ˑR�R���p�C���ł��Ȃ��Ȃ蕜���s��ԂɁB
 * �@�@�d�����Ȃ��̂ŁAVer.18�ɃR�s�[������A�_���ŁA0����\�z����Ver.19���쐬
 * �@�@XC8�̍ăC���X�g�[���AMPLABX�̍ăC���X�g�[���������B.mc3�́Ahash�ȊO�̍s���R�s�y�����
 * �@�@���p�ł��邱�Ƃ������B����Ń[���x�[�X�ł�MCC�̐ݒ���R�s�[�\�ƂȂ���
 * �@�@�\�[�X���̂́AVer.17���x�[�X�ɂ��Ă���
 * �@�@�M�A���t�H���g���ł����̂ŁA�J�X�^���t�H���g���g���ĕύX���\
 * �@�@�قڊ�����ɗ��Ă���
 * �@�@�M�A��������IP�A�h���X��\��������悤�ɂ�����A�ꉞ�̊���
 * �@�@�u���E�U����̃A�N�Z�X�̏����ɂ܂��s�����c��B�v���O���������ꂢ�ɂȂ��Ă��Ȃ�
 * �@�EESP�ւ̃f�[�^�ʐM�̃o�O���C��
 * �@�E�A���[�������͏]���X���C�hSW��On�ɂ�������EEPROM�ɏ�������ł����B���[�^���[SW��
 * �@�@�o���o���ω����邲�Ƃɏ������݂����Ȃ��������炾���ASW�𓮂������ɓd����؂��
 * �@�@���̐ݒ�͔p������Ă��܂��Ă����B�����ŁA�ύX���Ă���5����ɏ������ނ悤�ɂ��Ă݂��B
 * �@�@�����ɓd���؂�ƖY��邪�A5�������ĂΏ������܂��̂ŁA����ŏ\�����p�I���ƍl���Ă���B
 * �@�E�M�A�������ƁAIP�A�h���X��\�����ău���E�U����̃A�N�Z�X���@��������悤�ɂ����B
 * �@�E�M�A�̕\�����J�X�^���t�H���g�ŕύX�ł���悤�ɂ���
 * �@�E�A���[���̃A�C�R�������l
 */

#include "debug.h"
#include "main.h"
#include "LCD320x240color.h"
#include "RTC8025.h"
#include "skESP8266.h"
#include "Draw.h"
#include <string.h>
#include "DHT20.h"
#include "AlarmSound.h"
#include "TouchXT2046.h"
#include "event.h"

// ��Βl���擾����֐��}�N����`
#define ABS(x) ((x) < 0 ? -(x) : (x))

typedef union {
    struct {
        uint16_t d;
    };
    struct {
        uint8_t dl;
        uint8_t dh;
    };
} WordByte;


/*** ���M����HTML�̃e�L�X�g ***/
/*
 * accept=\"image/\" ��input type="file"�ɒǉ�����ƁA�A�b�v���[�h����t�@�C��������ł���
 * ���A�t�H���g�t�H���_�ɁA�R�����g���������e�L�X�g�t�@�C�������Ă���̂ŁA�S����Ώۂɂ���B
 * webkitrectory: �܂��W��������Ă��Ȃ����A�l�p�r�ŁA��p���Ă���Chrome�Ŏg����̂ƁA
 * �@�@�@�@�@�@�@�t�H���_�ۂ��ƃA�b�v���[�h�������Ƃ�������̗p�r�ɂ́A���삪�ȒP�ɂȂ�̂ŁA������g���B
 * multiple�Ƃ������������邪�A����͕����̃t�@�C����I�����Ȃ��Ƃ����Ȃ��̂ƁA
 * �@�@�i�[����t�H���_�����w�肵�Ȃ��Ƃ����Ȃ��ʓ|��������̂ŁA����͎g��Ȃ����Ƃɂ���
 */

//���̃y�[�W��\��������A0.5s��ɑO�̃y�[�W�ɖ߂�
const char comp_txt1[] = "HTTP/1.0 200 OK\r\n\r\n" \
"<html><head><title>PIC Clock</title><meta charset='Shift_JIS'></head>" \
"<body onLoad=setTimeout(\"location.href='javascript:history.back()'\",2000)>";
//�A�b�v���[�h�������̃��b�Z�[�W�������ɂ͂���
const char comp_txt2[] = \
"2�b��A�����I�ɑO�̃y�[�W�ɖ߂�܂�" \
"</form>" \
"</body>" \
"</html>";
//"<br><input type='button' onclick=\"history.back()\" value='BACK'>"

//�z�[���y�[�W�F�e�[�u���\��
const char html_txt2[] = "HTTP/1.0 200 OK\r\n\r\n" \
"<html><head><title>PIC Clock</title><meta charset='Shift_JIS'>" \
"<style>" \
" :root {" \
"	--white: #fff;" \
"	--gray: #ddd;" \
"	--darkblue: #1b4965;" \
"	--blue1: #cce8ff;" \
"	--blue2: #ddeaf6;" \
" }" \
" table {" \
"	margin: 0 0 1em 1em;" \
"	width: 90%;" \
"	border: solid 1px var(--gray);" \
" }" \
" table th {" \
"	font-size: 0.9em;" \
"	font-weight: 600;" \
"	padding: 0.5em 0.75em 0.5em 0.75em;" \
"	text-align: left;" \
"	color: var(--white);" \
"	background: var(--darkblue);" \
" }" \
" .table-clickable tbody tr:hover {" \
"	background: var(--blue2);" \
" }" \
".sorttable {" \
"  th {    " \
"    &.sort {" \
"      cursor: pointer;" \
"      &.desc::after {" \
"        content: '��';" \
"        margin-left: 10px;" \
"        font-size: 16px;" \
"      }" \
"      &.asc::after {" \
"        content: '��';" \
"        margin-left: 10px;" \
"        font-size: 16px;" \
"      }" \
"    }" \
"  }" \
"}" \
" label {" \
"    padding: 5px 20px;" \
"    color: #000;" \
"    background-color: #e5e5e5;" \
"    cursor: pointer;" \
" }" \
"   input[name='fontfile'] { display: none; }" \
"</style>" \
"</head>" \
"<body>" \
"<h2>Welcome to PIC clock</h2>" \
"<form method='post'>" \
"<table class='table-clickable sorttable' id='target-table'>" \
"<caption>SD�J�[�h�Ɋi�[����Ă���t�H���g</caption>" \
"  <thead>" \
"    <tr>" \
"      <th width='20'></th>" \
"      <th class='sort' data-sort='fname'>�t�H���g��</th>" \
"      <th class='sort' data-sort='date'>�X�V����</th>" \
"    </tr>" \
"  </thead>" \
"  <tbody class='list'>" \
;
//�����Ƀt�H���_���X�g�𐶐�����
const char html_txt2end[] = \
"  </tbody>" \
"</table>" \
"�@��L�t�H���g�ō폜���������̂�I����ARemove�{�^���������ĉ�����<br>" \
"    <button type='submit' id='RemoveButton' disabled>Remove</button>" \
"</form>" \
"<hr>�A�b�v���[�h�������t�H���g�t�H���_��I�����ĉ�����<br>" \
"<form method='post' accept-charset='Shift_JIS' enctype='multipart/form-data'>" \
" <input type='file' id='fileElem' name='fontfile' webkitdirectory onchange='if (this.files.length > 0) this.form.submit();'>" \
" <button id='fileSelect' type='button'>�t�H���g�t�H���_��I��</button><br>" \
"</form>" \
"<small>���F8�����𒴂���t�@�C�����̃t�@�C���̓A�b�v���[�h����܂���<br>" \
"�@�t�H���_���A�t�@�C������8.3�`���ŃA�b�v���[�h����܂��B�p���͂��ׂđ啶���ɂȂ�܂�</small>" \
"<hr>�A�b�v���[�h�������J�X�^���p�f�[�^�t�@�C����I�����ĉ������B�g���q��dat�ł��邱��<br>" \
"<form method='post' accept-charset='Shift_JIS' enctype='multipart/form-data'>" \
"      <input type='file' name='datafile' accept='.dat' required>" \
"      <input type='submit' value='���M'>" \
"</form>" \
"���݂̃J�X�^���f�[�^<tt><big><pre>" \
"Item : disp,  x,   y,  font#, fsize, color, format,  attr1,  attr2, xpitchAdj, FontName,<br>";
//"Item : disp,  x,  y, xw, yw,font#,fsize, color,format, attr1,  attr2, xpitchAdj, Font,<br>";
//�����ɃJ�X�^������\��
const char html_txt3end[] = \
"</pre></big></tt>" \
"<small>7�̃I�u�W�F�N�g�̏�Ԃ��J�X�^�}�C�Y�ł���B�p�����[�^�̈Ӗ��͈ȉ��̒ʂ�<br>disp: 0:��\\��, 1:�\\��, 99:�\\��(�Œ�)�A<br>" \
"x,y: �\\���I�u�W�F�N�g�̍���̍��W (0,0)-(319,239)<br>" \
"font#: �t�H���g�ԍ� (1=8x8 Normal, 2=6x8 small, 3=5x7 7�Z�O�l, 10=�O���t�H���g)�Afsize: �t�H���g�T�C�Y(16�i2����XY�̔{���@0x11���f�t�H���g�T�C�Y)�Acolor:�\\���F(16bit color=RGB565)<br>" \
"format: <br>" \
"  Date: 0=YYYY/MM/DD(w)(�j��), 1=YYYY/MM/DD(we)(�p�j��), 2=YYYY/MM/DD, 3=YY/MM/DD(w), 4=YY/MM/DD(we), 5=YY/MM/DD, 6=MM/DD(w), 7=MM/DD(we), 8=MM/DD<br>" \
"  Time: 0=HH:MM, 1=HH:MMss, 2=HH:MM:SS, 3=Analog<br>" \
"  Alarm: 0=ALMAMPM(AM/PM�\\�L), 1=ALM24(24���ԕ\\�L), 2=iALAMPM(�A���[���A�C�R��), 3=iAL24(�A���[���A�C�R��)<br>" \
"  Temp: 0=DegC, 1=DegF<br>" \
"  Humid, Gear: �t�H�[�}�b�g�w��Ȃ�<br>" \
"  Calendar: 0=1month, 1=2month(������), 2=2v month(�c����), 3=3month, 4=3v month<br>" \
"�@�@attr1: �J�����_�̓y�j�̐F<br>" \
"�@�@attr2: �J�����_�̓��j�̐F<br>" \
"xpitchAdj: -2 ? 8�@�f�t�H���g�̃t�H���g�s�b�`�������<br>" \
"Font: �J�X�^���t�H���g�̃t�H���_��<br>" \
"���A�i���O���v�\\���̎��́Axw�����a������<br>" \
"</small>" \
"<script src='https://cdnjs.cloudflare.com/ajax/libs/list.js/2.3.1/list.min.js'></script>" \
"<script>" \
"function preloader() {" \
"  let table = document.getElementById('target-table');" \
"  let rows = table.querySelectorAll('tbody tr');" \
"  let removeBtn = document.getElementById('RemoveButton');" \
"  let checkCount = 0;" \
"  rows.forEach((row) => {" \
"    row.addEventListener('click'," \
"     function () {" \
"	   if (event.target.type !== 'checkbox') {" \
"        if (row.querySelector('input').type == 'checkbox') {" \
"          if (row.querySelector(\"input[type='checkbox']\").checked == false) {" \
"            row.querySelector(\"input[type='checkbox']\").checked = true;" \
"            checkCount++;" \
"          } else {" \
"            row.querySelector(\"input[type='checkbox']\").checked = false;" \
"            checkCount--;" \
"          }" \
"        }" \
"	   }" \
"      else {" \
"        if (row.querySelector(\"input[type='checkbox']\").checked == false) {" \
"           if (checkCount > 0) {checkCount--;}" \
"        } else {" \
"           checkCount++;" \
"        }" \
"      }" \
"      if (checkCount > 0) { removeBtn.disabled = false; }" \
"      else { removeBtn.disabled = true; }" \
"     }, false" \
"    );" \
"  });" \
"  const fileSelect = document.getElementById('fileSelect');" \
"  const fileElem = document.getElementById('fileElem');" \
"  fileSelect.addEventListener('click', (e) => {" \
"    if (fileElem) {" \
"      fileElem.click();" \
"    }" \
"  }, false);" \
"}" \
"const options = {" \
"  valueNames: [" \
"    'fname'," \
"    'date'," \
"  ]," \
"};" \
"const searchList = new List('target-table', options);" \
"window.onload = preloader;" \
"window.addEventListener('pageshow',()=>{" \
"	if(window.performance.navigation.type==2) location.reload();" \
"});" \
"</script>" \
"</body>" \
"</html>";

//�J�X�^����ʐݒ�p�f�[�^
const char html_custom1[] = \
"<head><title>Customizing</title><meta charset='Shift_JIS'></head>" \
"<style>" \
"  .area {" \
"      position: relative;" \
"      width: 320px;height: 240px;" \
"      border: 1px solid #F00;" \
"      background-color: #000;}" \
"  .dragd{cursor:move;" \
"		 position:absolute;" \
"		 z-index:1000;" \
"        border:none}" \
"  .drag{z-index:1001}" \
"  .XYclass{" \
"    width: 80px;" \
"    border:none;" \
"    outline:none;" \
"    text-align:center;" \
"  }" \
"  #fsize, #attr1, #attr2{width: 60px;text-align:right}" \
"  #xpAdj{width:60px;text-align:right;}" \
"  #fName{width:90px}" \
"  table tr td:nth-of-type(2){" \
"      text-align: center;}" \
;
const char html_custom2[] = \
"</style>" \
"<section>" \
"  <h2>��ʂ̃J�X�^���ݒ�</h2>" \
"  <p>disp�Ń`�F�b�N�������i�����L�g���ňړ����Ĉʒu�����߂ĉ�����</p>" \
"  <div class='area'>" \
"    <div class='dragd' id='box0'>Date</div>" \
"    <div class='dragd' id='box1'>Time</div>" \
"    <div class='dragd' id='box2'>Alarm</div>" \
"    <div class='dragd' id='box3'>Temp</div>" \
"    <div class='dragd' id='box4'>Humid</div>" \
"    <div class='dragd' id='box5'>Calendar</div>" \
"    <div class='dragd' id='box6'>Gear</div>" \
"  </div>" \
"  <button type='button' onclick='window.location.reload();' style='margin-left:350px;'>���Z�b�g</button>" \
"</section>" \
"<br><form method='post'>" \
"  <table border='1' style='border-collapse: collapse'>" \
"    <caption>���i���</caption>" \
"    <thead>" \
"<tr>" \
"	<th>Item</th><th>disp</th><th>x, y</th><th>font#</th><th>fsize</th><th>color</th><th>format</th><th>attr1</th><th>attr2</th><th>xpAdj</th><th> Font</th></tr>" \
"    </thead>" \
"    <tbody>" \
"<tr><td>Date</td>" \
"	<input type='hidden' name='Date'>" \
"	<td><input type='checkbox' name='disp' style='margin:auto;margin-left:auto; margin-right:auto'></td>" \
"	<td><input type='text' class='XYclass' name='XY'></td>" \
"	<td><select name='font'>" \
"	    <option value='1'>Normal</option>" \
"	    <option value='2'>Small</option>" \
"	    <option value='3'>7seglike</option>" \
"	    <option value='10'>Custom</option>" \
"	</select></td>" \
"	<td><input type='text' id='fsize' name='fsize' placeholder='XY�{��'></td>" \
"	<td><input type='color' name='color'></td>" \
"	<td><select name='format'>" \
"	    <option value='0'>YYYY/MM/DD(w)</option>" \
"	    <option value='1'>YYYY/MM/DD(we)</option>" \
"	    <option value='2'>YYYY/MM/DD</option>" \
"	    <option value='3'>YY/MM/DD(w)</option>" \
"	    <option value='4'>YY/MM/DD(we)</option>" \
"	    <option value='5'>YY/MM/DD</option>" \
"	    <option value='6'>MM/DD(w)</option>" \
"	    <option value='7'>MM/DD(we)</option>" \
"	    <option value='8'>MM/DD</option>" \
"	</select></td>" \
"	<td><input type='text' id='attr1' name='attr1' disabled></td>" \
"	<td><input type='text' id='attr2' name='attr2' disabled></td>" \
"	<td><input type='text' id='xpAdj' name='xpAdj'></td>" \
"	<td><input type='text' id='fName' name='fName'></td>" \
"</tr>" \
"<tr><td>Time</td>" \
"	<input type='hidden' name='Time'>" \
"	<td><input type='checkbox' name='disp'></td>" \
"	<td><input type='text' class='XYclass' name='XY'></td>" \
"	<td><select name='font'>" \
"	    <option value='1'>Normal</option>" \
"	    <option value='2'>Small</option>" \
"	    <option value='3'>7seglike</option>" \
"	    <option value='10'>Custom</option>" \
"	</select></td>" \
"	<td><input type='text' id='fsize' name='fsize' placeholder='XY�{��'></td>" \
"	<td><input type='color' name='color'></td>" \
"	<td><select name='format'>" \
"	    <option value='0'>HH:MM</option>" \
"	    <option value='1'>HH:MMss</option>" \
"	    <option value='2'>HH:MM:SS</option>" \
"	    <option value='3'>Analog</option>" \
"	</select></td>" \
"	<td><input type='text' id='attr1' name='attr1' placeholder='�A�i���a'></td>" \
"	<td><input type='text' id='attr2' name='attr2'></td>" \
"	<td><input type='text' id='xpAdj' name='xpAdj'></td>" \
"	<td><input type='text' id='fName' name='fName'></td>" \
"</tr>" \
"<tr><td>Alarm</td>" \
"	<input type='hidden' name='Alarm'>" \
"	<td><input type='checkbox' name='disp'></td>" \
"	<td><input type='text' class='XYclass' name='XY'></td>" \
"	<td><select name='font'>" \
"	    <option value='1'>Normal</option>" \
"	    <option value='2'>Small</option>" \
"	    <option value='3'>7seglike</option>" \
"	    <option value='10'>Custom</option>" \
"	</select></td>" \
"	<td><input type='text' id='fsize' name='fsize' placeholder='XY�{��'></td>" \
"	<td><input type='color' name='color'></td>" \
"	<td><select name='format'>" \
"	    <option value='0'>ALM AMPM</option>" \
"	    <option value='1'>ALM 24H</option>" \
"	    <option value='2'>i AMPM</option>" \
"	    <option value='3'>i 24H</option>" \
"	</select></td>" \
"	<td><input type='color' name='attr1'></td>" \
"	<td><input type='text' id='attr2' name='attr2' disabled></td>" \
"	<td><input type='text' id='xpAdj' name='xpAdj'></td>" \
"	<td><input type='text' id='fName' name='fName'></td>" \
"</tr>" \
"<tr><td>Temp </td>" \
"	<input type='hidden' name='Temp'>" \
"	<td><input type='checkbox' name='disp'></td>" \
"	<td><input type='text' class='XYclass' name='XY'></td>" \
"	<td><select name='font'>" \
"	    <option value='1'>Normal</option>" \
"	    <option value='2'>Small</option>" \
"	    <option value='3'>7seglike</option>" \
"	    <option value='10'>Custom</option>" \
"	</select></td>" \
"	<td><input type='text' id='fsize' name='fsize' placeholder='XY�{��'></td>" \
"	<td><input type='color' name='color'></td>" \
"	<td><select name='format'>" \
"	    <option value='0'>DegC</option>" \
"	    <option value='1'>DegF</option>" \
"	</select></td>" \
"	<td><input type='text' id='attr1' name='attr1' disabled></td>" \
"	<td><input type='text' id='attr2' name='attr2' disabled></td>" \
"	<td><input type='text' id='xpAdj' name='xpAdj'></td>" \
"	<td><input type='text' id='fName' name='fName'></td>" \
"</tr>" \
"<tr><td>Humid</td>" \
"	<input type='hidden' name='Humid'>" \
"	<td><input type='checkbox' name='disp'></td>" \
"	<td><input type='text' class='XYclass' name='XY'></td>" \
"	<td><select name='font'>" \
"	    <option value='1'>Normal</option>" \
"	    <option value='2'>Small</option>" \
"	    <option value='3'>7seglike</option>" \
"	    <option value='10'>Custom</option>" \
"	</select></td>" \
"	<td><input type='text' id='fsize' name='fsize' placeholder='XY�{��'></td>" \
"	<td><input type='color' name='color'></td>" \
"	<td><select name='format' disabled></select></td>" \
"	<td><input type='text' id='attr1' name='attr1' disabled></td>" \
"	<td><input type='text' id='attr2' name='attr2' disabled></td>" \
"	<td><input type='text' id='xpAdj' name='xpAdj'></td>" \
"	<td><input type='text' id='fName' name='fName'></td>" \
"</tr>" \
"<tr><td>Calen</td>" \
"	<input type='hidden' name='Calen'>" \
"	<td><input type='checkbox' name='disp'></td>" \
"	<td><input type='text' class='XYclass' name='XY'></td>" \
"	<td><select name='font'>" \
"	    <option value='1'>Normal</option>" \
"	    <option value='2'>Small</option>" \
"	    <option value='3'>7seglike</option>" \
"	    <option value='10'>Custom</option>" \
"	</select></td>" \
"	<td><input type='text' id='fsize' name='fsize' placeholder='XY�{��'></td>" \
"	<td><input type='color' name='color'></td>" \
"	<td><select name='format'>" \
"	    <option value='0'>Month1</option>" \
"	    <option value='1'>Month2</option>" \
"	    <option value='2'>Month2v</option>" \
"	    <option value='3'>Month3</option>" \
"	    <option value='4'>Month3v</option>" \
"	    <option value='10'>Month1e</option>" \
"	    <option value='11'>Month2e</option>" \
"	    <option value='12'>Month2ve</option>" \
"	    <option value='13'>Month3e</option>" \
"	    <option value='14'>Month3ve</option>" \
"	</select></td>" \
"	<td><input type='color' name='attr1'></td>" \
"	<td><input type='color' name='attr2'></td>" \
"	<td><input type='text' id='xpAdj' name='xpAdj'></td>" \
"	<td><input type='text' id='fName' name='fName'></td>" \
"</tr>" \
"<tr><td>Gear</td>" \
"	<input type='hidden' name='Gear'>" \
"	<td><input type='checkbox' name='disp'></td>" \
"	<td><input type='text' class='XYclass' name='XY'></td>" \
"	<td><select name='font'>" \
"	    <option value='1'>Normal</option>" \
"	    <option value='2'>Small</option>" \
"	    <option value='3'>7seglike</option>" \
"	    <option value='10'>Custom</option>" \
"	</select></td>" \
"	<td><input type='text' id='fsize' name='fsize' value='-'></td>" \
"	<td><input type='color' name='color'></td>" \
"	<td>-</td>" \
"	<td>-</td>" \
"	<td>-</td>" \
"	<td>-</td>" \
"	<td><input type='text' id='fName' name='fName'></td>" \
"</tr>" \
"    </tbody>" \
"  </table><br>" \
"  <button type='submit'>Upload</button>" \
"</form>" \
"<script>" \
" var XY = document.getElementsByClassName('XYclass');" \
" var elements = document.getElementsByClassName('dragd');" \
"  (function(){" \
"      var x, y, index, drag;" \
"      for(var i = 0; i < elements.length; i++) {" \
"	  elements[i].addEventListener('mousedown', mdown, false);" \
"      }" \
"   function mdown(e) {" \
"	  this.classList.add('drag');" \
"	  x = e.pageX - this.offsetLeft;" \
"	  y = e.pageY - this.offsetTop;" \
"	  document.body.addEventListener('mousemove', mmove, false);" \
"	  for(var i = 0; i < elements.length; i++) {" \
"	      if (elements[i].classList.contains('drag')) index = i;" \
"	  }" \
"	  drag = document.getElementsByClassName('drag')[0];	  " \
"   }" \
"   function mmove(e) {" \
"	  e.preventDefault();" \
"	  drag.style.top = e.pageY - y + 'px';" \
"	  drag.style.left = e.pageX - x + 'px';" \
"	  XY[index].value =" \
"              (e.pageX - x).toString().padStart(3, ' ') + ', ' + (e.pageY - y).toString().padStart(3, ' ');" \
"	  " \
"	  drag.addEventListener('mouseup', mup, false);" \
"	  document.body.addEventListener('mouseleave', mup, false);" \
"   }" \
"   function mup(e) {" \
"	  var drag = document.getElementsByClassName('drag')[0];" \
"	  document.body.removeEventListener('mousemove', mmove, false);" \
"	  drag.removeEventListener('mouseup', mup, false);" \
"	  drag.classList.remove('drag');" \
"   }" \
"   this.ondragstart = function () {" \
"	  return false;" \
"   };" \
"  })();" \
"  window.onload = function () {" \
"      for(var i = 0; i < elements.length; i++) {" \
"	  XY[i].value = " \
"              elements[i].offsetLeft.toString().padStart(3, ' ') + ', ' + elements[i].offsetTop.toString().padStart(3, ' ');" \
"      }" \
;

const char html_custom5[] = "  };</script>";


// ��M�f�[�^�̏����X�e�[�^�X
#define SNONE       0
#define SGET        12
#define SPOST       13

#define PROCESSBUF_SIZE 256

//���E��؎q�́A70�����ȉ��炵��
/*
 * ��ԍŏ��̖{�̕����̑O�ɂ́A --boundary �� CRLF ����Ȃ鋫�E����������܂�
 * �{�̕����Ɩ{�̕����̊Ԃɂ́A CRLF �� --boundary �� CRLF ����Ȃ鋫�E����������܂�
 * ��ԍŌ�̖{�̕����̌�ɂ́A CRLF �� --boundary-- �� CRLF ����Ȃ鋫�E����������܂�
 * �܂����E������́A���̒��g�ƈ�v���邱�Ƃ̂Ȃ��悤�ɂƂ�����������Ă���̂ŁA������
 * ���E������ƈ�v������������΂悢����
 */
#define BOUNDARYSTRLENGTH   80
char BoundaryStr[BOUNDARYSTRLENGTH] = "\0";    // �O�̉��s��--�A�Ō��--��ǉ����āA����/�ŏI���p
uint16_t BoundaryStrLen;   //���E������̒��� BoundaryStr�̒���


#define TXT 0
#define BIN 1

uint8_t BinOrText;  //BIN=1, TXT=0
//uint16_t PacketLength = 0;    //���݂̃p�P�b�g�̓ǂݏo���ׂ��f�[�^���̎c��
uint16_t ContentLength;     // Content-Length�Ɏ�����Ă��钷��
uint8_t StartContent;   //Content�����J�n������1

uint8_t InBoundary;    //���E������͈̔͂����Ă���(1)���ǂ���
FIL BinFile;    //

uint8_t FileCount;
char Dirname[10];   //�t�H���g�f�B���N�g���̖��O
char Filename[50];  //�o�C�i���t�@�C���̖��O
#define MAXLISTITEM     30      //�ݒ��ʂł͍ő�28�܂łɐ������Ă���
char FontNameList[MAXLISTITEM][9];   //�t�H���g���̃��X�g
WORD FontListDate[MAXLISTITEM];			/* Modified date */
WORD FontListTime[MAXLISTITEM];			/* Modified time */

uint8_t FontNum;

uint8_t RepeatKey = 0;  //���s�[�g�����邩�ǂ���

//�\�����[�h
uint8_t DisplayMode;    // �ǂ̉�ʕ\�����[�h��

// ���т́ARTC�ƍ��킹�āA�b�A���A���A�j���A���A���A�N�@�@BCD�t�H�[�}�b�g
uint8_t DateTime[7] = {0x00, 0x00, 0x00, 0x00, 0x01, 0x01, 0x22};
int8_t TZ = +9; //����^�C���]�[���́AJST�ɌŒ�

//�A���[�������Amm,hh, �A���[�����L���ȗj���@RTC�Ɠ�������
uint8_t AlarmTime[3] = {0, 0, 0x7f};    //mm, hh, wday
uint8_t TmpTime[7]; //�ݒ�r���̎�����ێ�

// 0: �A���[��off�A1: �A���[�������ݔ����A2: �A���[������
uint8_t AlarmStatus = 0;
uint8_t SnoozeCount = 0;    //�X�k�[�Y����ڂ�
int16_t WriteALM2EEPROM;   //�A���[��������EERPOM�ɏ������ނ܂ł̎���(��)�@���[�^���[���邮��񂵂Ă��鎞�͏������܂Ȃ��悤��

//�����x�Z���T�̒l��ێ��@���ۂ̒l��10�{�ɂ��Đ�����
int16_t Temp, Humidity;

//�^�b�`�����p
uint16_t TouchRawX, TouchRawY;    //�^�b�`��������X,Y�̐����W
int16_t TouchX, TouchY;    //�f�B�v���C���W�Ɋ��Z�����^�b�`���W
//���W�␳�p��(20,20)��(300,220)�̃^�b�`��̍��W��ێ�
uint16_t TouchXY[4];    //�␳�p�@��舵���₷���悤�ɔz��ɕύX�B������x1,y1, x2,y2
uint8_t TouchCount;     //����������p�Ɏg�p�B�^�b�`�����0�ɂȂ�
uint16_t PixelData[9];  //3x3�̃s�N�Z���f�[�^��ۑ�����
int16_t PixelXY[2];     //��荞�񂾃f�[�^�̍���̍��W
uint8_t TouchStart;     //�^�b�`�J�n�t���O    bit3:Up, 2:repeat, 1:long, 0:down

//�X���C�hSW
uint8_t SlideSWStatus = 0x0f;   //�����l�͂ǂ̏�Ԃł��Ȃ��l��ݒ肵�Ă���

//���[�^���[SW�����p
int8_t RotCount = 0; //�񂵂���(�N���b�N����̃��[�^���[���ƁA����N���b�N��������)
int8_t Accel = 1;   //������]���������ɁA1�N���b�N��Accel�{�ɉ���������

uint8_t NeedUpdateTime =  0; //�����ݒ���Event�����ł��Ȃ��̂ŁA������t���O�Ƃ��Ďg�p

//�J�X�^���f�[�^�t�@�C�����i�[����t�H���_
char *DataFolder = "Data";
uint8_t WHICHJOB;     //�ǂ�ȃW���u�����s���Ă��邩
#define SETCUSTOM       1   //�J�X�^���f�[�^�̎�
#define REMOVEFONT      2   //�t�H���g�폜
#define UPLOADFONT      3   //�t�H���g�̃A�b�v���[�h
#define UPLOADCUSTOM    4   //�J�X�^���f�[�^�̃A�b�v���[�h

  /*
 * SD�J�[�h���g�����̗���
 * 
 * FATFS Drive;
 * FIL FileObject;
 * f_mount(&Drive, "", 0);
 * f_open(&FileObject, "filename", FA_READ);    FA_WRITE
 * f_read(&FileObject, Buffer, 32, &actualLength);  //32�o�C�g�ǂݍ���
 * f_write(&FileObject, str, strlen(str), &actualLength);
 * f_close(&FileObject);
 * 
 */
//SD�J�[�h�֌W
FIL FileObject;       //Opening��f�[�^�������ݗp
FATFS Drive;
////SD�J�[�h����f�[�^��ǂݏo�����Ɏg���o�b�t�@
uint8_t SDcardBuffer[DATASIZE];    //���[�J���ł͓��肫��Ȃ��Ă��A�����ł͂܂�������

// mcc_generated_files/fatfs/ff_time.c���ȉ��̂悤�ɏ��������ă^�C���X�^���v�𐳊m��
/*
DWORD get_fattime (void)
{
    uint8_t dateTime[7];    //���т́A�b�A���A���A�j���A���A���A�N

    RTC_read(dateTime);
    return decimalToFatTime(2000+Bcd2Hex(dateTime[6]), Bcd2Hex(dateTime[5]), Bcd2Hex(dateTime[4]), Bcd2Hex(dateTime[2]), Bcd2Hex(dateTime[1]), Bcd2Hex(dateTime[0]));

}
 */

/* 
 * �O���t�H���g�̖��O=�t�H���_���������Ɋi�[
 * �e�I�u�W�F�N�g�Ŏg�p����t�H���g�����i�[����
 * SD�J�[�h�Ɋi�[����Ă���J�X�^���t�H���g�̃t�H���_��(�ő�8����)�Ɠ���
 */
#define NumberOfUserFont   7
char CustomFontDir[NumberOfUserFont][9] = {
    "7segsl", "FUNWA28", "", "", "", "", "",
};

#define NOFORMAT    9999

//��ʏ�ɔz�u���镔�i�̒�`
ObjectRsc MainRsc1[] = {
    //ID,  x,  y,    xw,  yw, fontcode,  fontMag,color, format,   attribute, attribute2, xpitchAdj
    { 10,  30,  10,  80,  18, FontNormal, 0x22, WHITE, YYYYMMDDw, 0, 0, },
    { 20,  20,  35, 176,  58, Font7like,  0x77, LIME,  HHMMss,    0, 0, 0, },    //�Ԋu 2dot�L����
    { 50,  50, 120,   0,   0, FontNormal, 0x22, WHITE, ALM24,     GREY, },       //�A���[��off���̐F=GREY
    { 30, 230, 100,   0,   0, FontNormal, 0x22, WHITE, DEGC,      0, },
    { 40, 240, 120,   0,   0, FontNormal, 0x22, WHITE, NOFORMAT,  0, },
    { 60,   0, 155, 100,  80, FontSmall,  0x11, WHITE, Month3,    BLUE, RED, 0, },    //�y���̐F�A�Ԋu1dot�L����
    { 70, 280,   0,  32,  32, FontNormal,    0, LIGHTGREY, NOFORMAT},    //����
};

ObjectRsc MainRsc2[] = {
    { 10,   0,   0,  80,  18, FontNormal, 0x22, WHITE, YYYYMMDDw, 0, 0, },
    { 20,  10,  18, 176,  58, FontNormal, 0x23, LIME,  HHMMSS,    0, 0, 2, },    //�Ԋu 2dot�L����
    { 50, 200,  40,   0,   0, FontNormal, 0x12, WHITE, ALM24,     GREY, },       //�A���[��off���̐F=GREY
    { 30, 230,   0,   0,   0, FontNormal, 0x22, WHITE, DEGC,      0, },
    { 40, 240,  17,   0,   0, FontNormal, 0x22, WHITE, NOFORMAT,  0, },
    { 60,  25,  65, 320, 200, FontNormal, 0x22, WHITE, Month1,    BLUE, RED, 3, },    //�y���̐F�A�Ԋu3dot�L����
    { 70, 280, 200,  32,  32, FontNormal,    0,  GREY,  NOFORMAT},    //����
};

ObjectRsc MainRsc3[] = {
    { 10,   0,   0,  80,  18, FontNormal, 0x22, WHITE, YYYYMMDDw,   0, 0, },
    { 20,  10,  25, 180, 180, 0,          0,    GREY,  ANALOGCLOCK, Large, }, //�傫�Ȏ��v
    { 50,  40, 218,   0,   0, FontNormal, 0x13, WHITE, ALMAMPM,     GREY, },  //�A���[��off���̐F=GREY
    { 30, 230,   0,   0,   0, FontNormal, 0x22, WHITE, DEGC,        0, },
    { 40, 240,  20,   0,   0, FontNormal, 0x21, WHITE, NOFORMAT,    0, },
    { 60, 220,  40, 100,  80, FontSmall,  0x11, WHITE, Month3v,     BLUE, RED, 1, },
    { 70, 183, 200,  32,  32, FontNormal,    0,  GREY,  NOFORMAT},
};

//���[�U���\�����@���J�X�^�}�C�Y�ł������
ObjectRsc CustomRsc[] = {
    { 1,  30,  10,  80,  18, FontNormal, 0x22, WHITE,     YYYYMMDDw, 0, 0, 0, CustomFontDir[0], },
    { 1,  30,  35, 176,  58, Font7like,  0x56, ROYALBLUE, HHMMss,    0, 0, 2, CustomFontDir[1], },
//    { 1,  30,  35, 176,  58, FontNormal, 0x11, ROYALBLUE, HHMMss,    0, 0, 2, CustomFontDir[1], },
    { 1,  50, 120,  50,  20, FontNormal, 0x22, WHITE,     ALM24,     GREY, 0, 0, CustomFontDir[2], },  //�A���[����On/Off�����ݒ�H
    { 1, 230, 100,  50,  20, FontNormal, 0x22, WHITE,     DEGC,      0, 0, 0, CustomFontDir[3], },
    { 1, 240, 120,  50,  20, FontNormal, 0x22, WHITE,     NOFORMAT,  0, 0, 0, CustomFontDir[4], },
    { 1,   3, 155, 100,  80, FontSmall,  0x11, WHITE,     Month3,    BLUE, RED, 0, CustomFontDir[5], },
    { 1, 280,   0,  32,  32, FontNormal,    0, LIGHTGREY, NOFORMAT,  0, 0, 0, CustomFontDir[6], },    //�F��ύX
};

ObjectRsc *CurrentRsc;    //���ݎQ�Ƃ��ׂ����\�[�X

//DisplayMode�̔ԍ��Ɖ��L�̗v�f�͈�v�����Ă���
//DisplayMode=DisplayMod1�́AMainRsc1�̃��\�[�X
ObjectRsc *RscData[] = {
  MainRsc1,
  MainRsc2,
  MainRsc3,
  CustomRsc,  
};

const char CustomSettingText[][6] = { "Date", "Time", "Alarm", "Temp", "Humid", "Calen", "Gear", };

const char TouchMsg[] = "Touch anywhere to close this screen";

// Prototype�錾

/*
 * TX2����f�o�b�O�p�̕�������o��
 * printf�ŏo�͂����邽�߂�redirect�p���[�`��
 * UART2��TX�ɏo�͂����̂ŁA������V���A���A�_�v�^�����PC�Ń��j�^�ł���
 * Windows��ł́ATeraTerm��COM3�Őڑ�
 * �@�ݒ� > �V���A���|�[�g���J���A�X�s�[�h��115200�ɕύX���čĐݒ肷��Ηǂ�
 */
#ifdef DEBUG
void putch(char c) {
    while (!UART2_is_tx_ready()) ;
    UART2_Write(c);

    while (!UART2_is_tx_done()) ;
}
#endif


/*
 * 10ms���Ɋ��荞��
 * Touch: �`���^�����O�`�F�b�N�@�@TouchStart���t���O�Ɏg��
 * �X���C�hSW: ��ԃ`�F�b�N
 */
void Timer5Handler() {
    //�`���^�����O�����p    
    static uint8_t touch_status = 0xff;     //����6bit�̂ݎg�p
    static uint8_t slideSW_status = 0x55;   //����6bit�̂ݎg�p

    //�^�b�`�����A��߂����ɃC�x���g�����B�������ɂȂ��������C�x���g����
    //�^�b�`�ŁA0
    touch_status = ((uint8_t)(touch_status << 1) | T_IRQ_GetValue()) & 0x3f;
    if (touch_status == 0x38) {
        //3��A���^�b�`(T_IRQ=0)�Ɗ��m������
        TouchStart |= 0x01;     // TouchDown Flag
        TouchCount = 0; //�J�E���^���Z�b�g
    } else if ((touch_status & 0x0f) == 0) {
        //�A���^�b�`(0)���p�����Ă�����A10ms���ɃJ�E���g�A�b�v
        if (TouchCount < 200) {
            TouchCount++;
            if (TouchCount == 200) {
                //200x10ms=2s�������̏���
                //TouchCount��200�ɂȂ����������ALong����
                TouchStart |= 0x02;     //TouchLong Flag
            }
        }
        TouchStart |= 0x04; //�������֌W�Ȃ��A�^�b�`���p�����Ă��鎞  
        if (RepeatKey) RepeatKey++;
    } else if (touch_status == 0x07) {
        //�^�b�`���Ȃ���T_IRQ=1�A3��A���Ń^�b�`��߂��Ɣ���
        TouchStart = 0x08;  //TouchUp Flag
    }
    
    //�A���[���p�X���C�hSW�̏�ԃ`�F�b�N���A�ω�����������SlideSWStatus���X�V
    slideSW_status = ((uint8_t)((slideSW_status << 1) | ALMSW_GetValue())) & 0x0f;
    if (slideSW_status == 0x08) {
        SlideSWStatus = SlideSWon2off;        //�I�t�ɕω�
    } else if (slideSW_status == 0x07) {
        SlideSWStatus = SlideSWoff2on;        //�I���ɕω�
    }
}


/*
 * �^�b�`�������Wx,y(�O���t�B�b�N���W�Ɋ��Z��������)���A�w�肳�ꂽ�I�u�W�F�N�g��
 * ��`�͈͂ɓ����Ă��邩�`�F�b�N
 * �w�肵���I�u�W�F�N�g���������Ɣ��肳����1��Ԃ��B����ȊO�́A0
 * obj: RscObject�Œ�`���Ă������
 */
int8_t ButtonPush(int16_t x, int16_t y, ObjectRsc *objrsc) {
    int16_t xx, yy;
    
    xx = x - objrsc->x;
    yy = y - objrsc->y;

    if ((xx >= 0) && (yy >= 0)) {
        if ((xx < objrsc->xw) && (yy < objrsc->yw)) {
            return 1;   //�{�^���̋�`�̈�ɓ����Ă����ꍇ
        }
    }
    return 0;   //�̈�O�̎�
}

/* 
 * �^�b�`�����Ƃ���̎��Ӄf�[�^���擾 (3x3�̗̈�)
 * �f�[�^�́APixelData[]�ɁA����̍��W��PixelXY[]�Ɋi�[
 */
void GetPixelData(int16_t x, int16_t y) {
    uint8_t jj, kk;
    uint8_t rgb[27];    //3x3=9�h�b�g���̃f�[�^�i�[�̈� x RBG�e1�o�C�g

    PixelXY[0] = x-1;
    PixelXY[1] = y-1;
    addset(PixelXY[0], PixelXY[1], x+1, y+1);
    read_pixel(rgb, 9);
    for (jj = 0; jj < 9; jj++) {
        kk = jj * 3;
        PixelData[jj] = Getcolor565(rgb[kk], rgb[kk+1], rgb[kk+2]);
    }
}

/* 
 * �ێ����Ă������f�[�^�ŏ����߂�
 * PixelXY[0]��9999�ȊO��������Ƃ���������t���A�f�[�^�̗L�������m��
 */
void SetPixelData() {
    if (PixelXY[0]!=9999) glcd_array(PixelXY[0], PixelXY[1], 3, 3, (uint8_t *)PixelData);
}

char *aboutText0 = "How to customize.\r\n";
char *aboutText1 = "Please access \r\n";
char *aboutText2 = "/index.html\r\n  to upload/remove custom font and custom data.\r\n\n";
char *aboutText3 = "/index2.html\r\n  to modify parts and upload.";

//�^�b�`���ꂽ���̏���
void TouchProcedure() {
    uint8_t mm, hh;
    uint8_t snoozeAlarmTime[3] = {0, 0, 0x7f};
    char str[100], ip[30];

    //�^�b�`�������Ɉ������
    SetPixelData();     //���̃f�[�^��߂�
    GetPixelData(TouchX, TouchY);   //�����ޏꏊ�̃f�[�^��ǂݍ���
    display_fillRect(TouchX-1, TouchY-1, 3, 3, RED);    // 3x3�̐Ԃ��l�p������
    
#ifdef DEBUG2
    sprintf(str, "%d,%d -> %3d,%3d ", TouchRawX, TouchRawY, TouchX, TouchY);
    display_drawChars(145, 30, str, WHITE, TextbgColor, 0x11);
    sprintf(str, "Adj=%d,%d, %d,%d ", TouchXY[0], TouchXY[1], TouchXY[2], TouchXY[3]);
    display_drawChars(145, 40, str, WHITE, TextbgColor, 0x11);
#endif
    //�A���[��������(AlarmStatus>0)�Ƀ^�b�`������X�k�[�Y
    if (AlarmStatus) {
        AlarmSoundOff();
        RTC_resetAlarm();
        AlarmStatus = 0;
        if (SnoozeCount < 12) {
            //5����ɃA���[���Đݒ�@12��܂ŁA1���Ԍ�܂�
            mm = Bcd2Hex(AlarmTime[0]);
            hh = Bcd2Hex(AlarmTime[1]);
            SnoozeCount++;
            IncDecTime(SnoozeCount * 5, &hh, &mm);    //5��*Smooth�񐔂̎����ɐݒ�
//            IncDecTime(SnoozeCount *2, &hh, &mm);    //�f�o�b�O�p�ɒZ��
            snoozeAlarmTime[0] = Hex2Bcd(mm);
            snoozeAlarmTime[1] = Hex2Bcd(hh);
            //�����ŁARTC�ɃA���[�������Đݒ�
            RTC_setAlarmTime(snoozeAlarmTime);
            DEBUG_PRINTF("Snooze: %02d:%02d\r\n", hh, mm);            
        } else {
            //�X�k�[�Y�I��
            //�A���[��SW��On�̂܂܂ł������Ή��̂��߁A�I���W�i���̃A���[���������Z�b�g������
            RTC_setAlarmTime(AlarmTime);
        }
        return; //�A���[���Ȃ��Ă��鎞�̃^�b�`�̏����́A�����ŏI��
    }
    
    if (ButtonPush(TouchX, TouchY, &CurrentRsc[GearObj])) {
        //���ԃA�C�R�����N���b�N������A�ݒ��ʂ�
        lcd_fill(BLACK);
        ESP_GetIPaddress(ip);   //IP�A�h���X���擾

        SetFont(FontNormal, NULL);
        display_drawChars(0, 0, aboutText0, WHITE, BLACK, 0x22);
        
        display_setTextSize(0x12);
        display_setTextPitchAdj(1, 0);
        display_setCursor(0, 30);        //�`��J�n���W
        display_setColor(WHITE);
        display_setWrap(true);
        
        display_puts(aboutText1);
        display_puts(ip);
        display_puts(aboutText2);
        display_puts(ip);
        display_puts(aboutText3);
        display_setWrap(false);
        
        display_setTextPitchAdj(0, 0);
        display_drawChars(10, 210, (char*)TouchMsg, GREY, BLACK, 0x12);

        while ((TouchStart & 0x01) == 0);
        AlarmSoundOn(AlarmSingle);

        ClearEventBuffer();
        PushEvent(EventFormInit);
        TouchStart = 0;
    }
    else if (TouchX < 160 && TouchY > 120) {
        //��ʂ̍�����1/4�G���A���^�b�`���ĕ\�����[�h�ύX
        DisplayMode = (DisplayMode +1) % (sizeof(RscData)/sizeof(RscData[0])); //���̏��A�\�����[�h��4��
        DATAEE_WriteByte(AddressDisplayMode, DisplayMode);  //�ύX������EEPROM�ɏ�����
        PushEvent(EventFormInit);
    }
    
}

/*
 * DATAEE_WriteByte�̃��[�h��
 */
void DATAEE_WriteWord(uint16_t bAdd, uint16_t bData) {
    DATAEE_WriteByte(bAdd, lo(bData));
    DATAEE_WriteByte(bAdd+1, hi(bData));
}

/*
 * DATAEE_ReadByte�̃��[�h��
 * union�𗘗p����18�o�C�g�v���O�����T�C�Y�팸
 */
uint16_t DATAEE_ReadWord(uint16_t bAdd) {
    WordByte data;
    
    data.dl = DATAEE_ReadByte(bAdd++);
    data.dh = DATAEE_ReadByte(bAdd);
    return data.d;
}


/*
 * �J�X�^���f�[�^��EEPROM�ɕۑ�
 */
void StoreCustomData() {
    uint8_t jj;
    uint16_t addr;
    char *p;

    //�J�X�^���ݒ����������
    for (jj=0; jj < OBJECTNUM; jj++) {
        addr = AddressCustom + (uint16_t)jj * 32;
        DATAEE_WriteByte(addr++, CustomRsc[jj].disp);
        DATAEE_WriteWord(addr, (uint16_t)CustomRsc[jj].x);
        addr += 2;
        DATAEE_WriteWord(addr, (uint16_t)CustomRsc[jj].y);
        addr += 2;
        DATAEE_WriteWord(addr, (uint16_t)CustomRsc[jj].xw);
        addr += 2;
        DATAEE_WriteWord(addr, (uint16_t)CustomRsc[jj].yw);
        addr += 2;
        DATAEE_WriteByte(addr++, CustomRsc[jj].fontcode);
        DATAEE_WriteByte(addr++, CustomRsc[jj].fontmag);
        DATAEE_WriteWord(addr, CustomRsc[jj].color);
        addr += 2;
        DATAEE_WriteWord(addr, CustomRsc[jj].format);
        addr += 2;
        DATAEE_WriteWord(addr, CustomRsc[jj].attribute);
        addr += 2;
        DATAEE_WriteWord(addr, CustomRsc[jj].attribute2);
        addr += 2;
        DATAEE_WriteByte(addr++, (uint8_t)CustomRsc[jj].xpitchAdj);
        }

    for (jj = 0; jj < NumberOfUserFont; jj++) {
        p = CustomFontDir[jj];
        addr = AddressFontName + (uint16_t)jj * 9;
        while (1) {
            DATAEE_WriteByte(addr++, *p);
            if (*p++ == '\0') break;
        };
    }
    
}


/*
 * �^�b�`�̍��W���O���t�B�b�N�̍��W�ɕϊ�
 * ����͊��ɂ���ĕω����邽�߁ATouchXT2046.c���炱���ֈړ�����
 */
void TransCoordination(uint16_t x, uint16_t y, int16_t *xg, int16_t *yg) {
    int32_t xx, yy;

#ifdef DEBUG2
    char str[100];
    sprintf(str, "P1=(%d, %d) P2=(%d, %d)", TouchXY[0], TouchXY[1], TouchXY[2], TouchXY[3]);
    display_drawChars(0, 140, str, WHITE, TextbgColor, 0x11);
#endif
    
    if ((TouchXY[2] == TouchXY[0]) || (TouchXY[3] == TouchXY[1])) {
        *xg = (int16_t)x;
        *yg = (int16_t)y;
        return;
    }

    //�ȉ��̌v�Z���x�ɒ���
    // (20,20)  (300,220)�̃^�b�`��̍��W��TouchXY[0], TouchXY[1], TouchXY[2], TouchXY[3]�ɕێ�
    xx =  (int32_t)x - (int32_t)TouchXY[0];
    xx = xx * 280 / ((int32_t)TouchXY[2] - (int32_t)TouchXY[0]) + 20;
    if (xx < 0) *xg = 0;
    else if (xx >= LCD_WIDTH) *xg = LCD_WIDTH -1;
    else *xg = (int16_t) xx;
    
    yy = (int32_t)y - (int32_t)TouchXY[1];
    yy = yy * 200 / ((int32_t)TouchXY[3] - (int32_t)TouchXY[1]) + 20;
    if (yy < 0) *yg = 0;
    else if (yy >= LCD_HEIGHT) *yg = LCD_HEIGHT -1;
    else *yg = (int16_t) yy;

}


/*
 * �^�b�`���W�����p���b�Z�[�W
 * �Ώۂ̃��b�Z�[�W�𔒂ŁA����ȊO���O���[��
 * num=0/1  ����ȊO�̃G���[�`�F�b�N���Ă��Ȃ�
 */
void TouchAdjMsg(uint8_t num) {
    uint16_t color[] = { WHITE, GREY };
    char str[100];

    //����̏\����`��@���S���W(20,20)
    display_drawLine(16, 20, 24, 20, color[num]);
    display_drawLine(20, 16, 20, 24, color[num]);

    strcpy(str, "1. Touch + at Up Left");
    display_drawChars(50, 100, str, color[num], TextbgColor, 0x12);
    strcpy(str, "2. Touch + at Bottom Right");
    display_drawChars(50, 120, str, color[1-num], TextbgColor, 0x12);

    //�E���̏\���@�@���S���W(300, 220))
    display_drawLine(296, 220, 304, 220, color[1-num]);
    display_drawLine(300, 216, 300, 224, color[1-num]);
    
}


/*
 * (20,20)�A(300,220)��2�_���^�b�`�������̍��W��(TouchXY[0], TouchXY[1])�A(TouchXY[2], TouchXY[3])��
 * ��荞�ނ��߂̃��[�`��
 */
void TouchAdjust(void) {
    char str[100];
    int16_t dx, dy;
    uint8_t jj;

    lcd_fill(BLACK); //��ʂ��N���A
    display_setTextColor(WHITE, BLACK); //Textcolr��TextbgColor��ݒ�
    SetFont(FontNormal, NULL);
    display_setTextSize(0x12);
            
    //�ŏ��ɍ�����^�b�`���āA���ɉE�����^�b�`���Ă��炤�B�Ó��łȂ��ꍇ�́A��蒼��
    while (1) {
        TouchAdjMsg(0);
        // 1��ڂ̃^�b�`�̈ʒu
        while ((TouchStart & 0x01) == 0);
        while (GetTouchLocation(&TouchXY[0], &TouchXY[1]) == -1);
        //�^�b�`������s�b�Ƃ��������o��
        AlarmSoundOn(AlarmSingle);
        //2��ڂ̎�������̂ŁA��U������Try Again�̃��b�Z�[�W������
        display_setCursor(100, 180);
        display_puts("               ");    // Try Again�̕���������
        sprintf(str, "X1=%4d, Y1=%4d", TouchXY[0], TouchXY[1]);
        display_setCursor(50, 20);
        display_puts(str);            
        
        TouchAdjMsg(1);
        // 2��ڂ̃^�b�`�܂ŏ����x���B�����ǂ����āA1��ڂƓ����ɂȂ�������
        __delay_ms(500);
        while ((TouchStart & 0x01) == 0);   //�����݂Ń^�b�`�ƔF�������܂Ń��[�v
        while (GetTouchLocation(&TouchXY[2], &TouchXY[3]) == -1);
        //�^�b�`������s�b�Ƃ��������o��
        AlarmSoundOn(AlarmSingle);
        sprintf(str, "X2=%4d, Y2=%4d", TouchXY[2], TouchXY[3]);
        display_setCursor(100, 200);
        display_puts(str);
        
        DEBUG_PRINTF("P1=(%d, %d) P2=(%d, %d)\r\n", TouchXY[0], TouchXY[1], TouchXY[2], TouchXY[3]);
        
        //2��ڂ̃^�b�`��1��ڂƋ߂����́A��蒼������
        //2�_�̋�����X,Y�e1500�ʂ���@(1500/16)^2=8800�@�ȉ��̌v�Z���ƁA18000�ɂȂ�
        dx = ABS((int16_t)(TouchXY[0] - TouchXY[2]))/16;  //2�悵����int16�𒴂��Ȃ��悤
        dy = ABS((int16_t)(TouchXY[1] - TouchXY[3]))/16;
#ifdef DEBUG
        DEBUG_PRINTF("dx=%d, dy=%d\r\n", dx, dy);
#endif
        if (dx*dx + dy*dy < 12000) {
            //�������Z�����́A������x
            display_setCursor(100, 180);
            display_puts("- Try Again -  ");
            __delay_ms(500);
            AlarmSoundOn(AlarmTwice);
            continue;
        }
        break;
    }
    AlarmSoundOn(AlarmTwice);
    AlarmSoundOn(AlarmSingle);
    //���������^�b�`���W����������
    for (jj = 0; jj< 4; jj++) DATAEE_WriteWord(AddressTouch + jj*2, TouchXY[jj]);
}


/*
 * TMR1��500us���̊��荞�݂ŁA���[�^���[�G���R�[�_�̏�Ԃ��`�F�b�N
 * 24�N���b�N/1��]
 * 1�b��1��]������A24��C�x���g��������B
 * �L�[�̃`���^�����O�h�~�Ɠ��l�̃v���O����
 * 3��=1.5ms�ԁA������ԂȂ�A���̃��x���ƔF��������
 * �@�����̊��荞�݂ŁA1�b�Ԃ�2000��`�F�b�N�ł���B1�b��2��]�������Ƃ��āA
 * �@�@�@24�N���b�N��x2��̏�ԕω��������B1�T�C�N����40��قǃT���v�����O�ł���v�Z�Ȃ̂ŁA
 * �@�@�@�M���ω��͏\����荞�߂�
 * �@��3��̃T���v�����O���ł����]���x�́A1����=(3+3)*500us=3ms�A24�N���b�N*3ms=72ms�Ȃ̂ŁA
 * �@�@1/72ms=13��]�ƁA����Ȃ�1�b�Ԃŉ�]�ł��Ȃ����x���Ȃ̂ŁA�\��
 * �N���b�N�̈���_�ł́AA�[�q�͏��OFF=1�̏��
 * 
 * RotCount���X�V���Ă���
 * �C�x���g�������x��Ă��ARotCount�̒l���g���ď�������̂ŁA�S���̃C�x���g�������K�v�Ƃ���
 * �킯�ł͂Ȃ��A�����������āARotCount=0�Ȃ�c�����C�x���g�̓X�L�b�v�����
 * 
 */
void RotaryHandler() {
    static uint8_t StartRot = 0;    //
    static int8_t Bdata = 0; // ���v���=1�A�����v���=0
    static uint8_t RotA_status = 0xff;  //ROT_A�����ω��_���`�F�b�N
    uint8_t newBdata;

    //A�[�q���̃��x�����`�F�b�N�B��~��=1�A��������0�ƂȂ�A�N���b�N�|�C���g��1�ɖ߂�
    RotA_status = (uint8_t)(RotA_status << 1) | ROT_A_GetValue();

    if ((StartRot == 0) && (RotA_status & 0x07) == 0) {
        //A�[�q��0�ɂȂ�����A��]�J�n�Ɣ��f
        StartRot = 1;
        //��]�J�n�Ɣ��f��������B�[�q�̃��x���ŉ�]�����𔻒�
        Bdata = ROT_B_GetValue();
    } else if ((StartRot == 1) && ((RotA_status & 0x07) == 0x07)) {
        //��]���삵�Ă��āAA�[�q��1�ɂȂ������̏���
        newBdata = ROT_B_GetValue();
        // B�[�q�����A1��0�̎�CW(�C���N��)�A0��1�̎�RCW(�f�N��)
        if (Bdata < newBdata) {
            //A�[�q��H�ɂȂ������AB�[�q��H�Ȃ甽���v�����ɉ�]
            RotCount--;
            //���[�^���C�x���g�������Ŕ��s���Ȃ��B��ʂɔ��s�����̂͌�����������
            //���C�����[�v�ŁARotCount��0�łȂ���΃C�x���g���s�����A��x�ɏ���������
        } else if (Bdata > newBdata) {
            //A�[�q��H�ɂȂ������AB�[�q��L�Ȃ玞�v�����ɉ�]
            RotCount++;
        }
        //�����Ɖ�]�������ɖ߂����ꍇ�A�J�E���g�������A�I������P�[�X����B
        StartRot = 0;
    }    
}


/*
 * INTA��INT0���荞�݂�
 * 1�b���ƂɊ��荞�݂�����̂ŁA�����̍X�V�C�x���g����
 */
void RTC_handler() {
    EXT_INT0_InterruptFlagClear();
    NeedUpdateTime = 1;
}

/*
 * �A���[��W(RTC_INTB=RB4��IOC)�ɂ�銄�荞��
 * 18F27K42�ł́ANegative�̎w�肪�ł���̂ŁA������ɕύX���M�����x���̔��f�s�v��
 */
void AlarmWHandler() {
    AlarmStatus = 1;
}

/*
 * ���t���ς�������`�F�b�N���āA�ύX����Γ��t�\���X�V
 * Year/Month/Day/Weekday��EEPROM�Ɋi�[���Ă����A�d�r����ւ���̕ύX�̎�Ԃ��Ȃ�
 */
uint8_t WriteYMD() {
    uint8_t jj, tmp;
    uint8_t changeDate = 0;
    uint16_t addr;

    //�����EEPROM���������������Ȃ��悤�ɁA�ω��������������ɂ���
    addr = AddressYMD;
    for (jj = 3; jj < 7; jj++) {
        tmp = DATAEE_ReadByte(addr);
        //�j���A���A���A�N���`�F�b�N
        if (tmp != DateTime[jj]) {
            DATAEE_WriteByte(addr, DateTime[jj]);
            changeDate = 1;
        }
        addr++;
    }
    return changeDate;
}


/*
 * SD�J�[�h���}�E���g����
 * ����mount���Ă�����A��Uunmount
 *f_mount�́A���[�N�G���A�̐ݒ�ł���ASD�J�[�h�̗L���Ɋ֌W�Ȃ��A��ɐ�������
 */
void remount(void) {
//    FRESULT res;
    f_mount(0, "", 0);  //unmount
//    DEBUG_PRINTF("res0=%d\r\n", res);
    f_mount(&Drive, "", 0);   //��ɐ���
//    DEBUG_PRINTF("res1=%d\r\n", res);
}


/*
 * NTP�T�[�o����������擾���ARTC�ɐݒ�
 */
void GetTimeFromNTP(uint8_t *datetime) {
    uint16_t yy;
    uint8_t mm, dd, h, m, s;

    Get_time(&yy, &mm, &dd, &h, &m, &s);

    datetime[0] = Hex2Bcd(s);
    datetime[1] = Hex2Bcd(m);
    datetime[2] = Hex2Bcd(h);
    RTC_setTime(datetime);  //������RTC�ɐݒ�
        
    datetime[4] = Hex2Bcd(dd);
    datetime[5] = Hex2Bcd(mm);
    datetime[6] = Hex2Bcd(yy % 100);
    
    //���t���`�F�b�N���āA�j����ݒ�
    DateErrorCheck(datetime);
    RTC_setDate(datetime);  //���t��RTC�ɐݒ�
    
    DEBUG_PRINTF("%d/%d/%d ", yy, mm, dd);
    DEBUG_PRINTF("%2x:%02x:%02x\r\n", datetime[2], datetime[1], datetime[0]);
}


/*
 * SD�J�[�h���̃t�H���g�̃��X�g���擾
 * 
 * SD�J�[�h����t�H���_��ǂݏo�� (�t�H���g�t�H���_�ȊO���Ώۂɓ����Ă��܂����C�ɂ��Ȃ�)
 * �t�H���g�f�B���N�g�����X�g��\������
 * 1�t�H���g=FolderW x FolderH�̗̈�ɋL���@�@��4�A�c7��28�܂łɐ�������
 * �擾�����t�H���_��=�t�H���g���́AFontNameList�֊i�[
 */
FRESULT GetFontFolder(char* path) {
    FRESULT res;
    FILINFO fno;
    FFDIR dir;

    DEBUG_PRINTF("%s:\r\n", path);
    remount();
    FontNum = 0;
    res = f_opendir(&dir, path);                       /* �f�B���N�g�����J�� */
    if (res == FR_OK) {
        while (1) {
            res = f_readdir(&dir, &fno);                   /* �f�B���N�g�����ڂ�1�ǂݏo�� */
            if (res != FR_OK || fno.fname[0] == 0) break;  /* �G���[�܂��͍��ږ����̂Ƃ��͔����� */
            if (fno.fname[0] == '.') continue;             /* �h�b�g�G���g���͖��� */
            if (fno.fattrib & AM_HID) {
                // Hidden�t�@�C���͕\�����Ȃ�
            } else if (fno.fattrib & AM_DIR) {          /* �f�B���N�g�� */
                DEBUG_PRINTF("dir: %s\r\n", fno.fname);
                strcpy(FontNameList[FontNum], fno.fname);
                FontListDate[FontNum] = fno.fdate;  //WORD�f�[�^
                FontListTime[FontNum] = fno.ftime;  //WORD
                FontNum++;
                if (FontNum >= MAXLISTITEM) {
                    //����t�H���g�t�@�C���̍ő�𐧌��B��ʕ\��������Ȃ�����
                    DEBUG_PRINTF("Reach max of buffer\r\n");
                    break;
                }
            } else {
                DEBUG_PRINTF("file: %s\r\n", fno.fname);
            }
        }
        DEBUG_PRINTF("Total FontNum=%d\r\n\r\n", FontNum);
    }
    f_closedir(&dir);
    return res;
}

/*
 * SD�J�[�h�̎w�肳�ꂽ�f�B���N�g�����폜
 * �@�f�B���N�g�����̃t�@�C����S���폜��A�t�H���_���폜�Ƃ����菇
 */
void RemoveFolder(char *dirname) {
    FRESULT res;     /* API�߂�l */
    FILINFO fno;    /* �t�@�C����� */
    FFDIR dir;
    char filename[64];
    
//    f_unlink("*.*");    //���C���h�J�[�h�͎g���Ȃ�
    DEBUG_PRINTF("Remove Folder\r\n");
    remount();
    f_chdir("/");
    res = f_opendir(&dir, dirname);    /* �f�B���N�g�����J�� */
    if (res == FR_OK) {
        while (1) {
            res = f_readdir(&dir, &fno);                   /* �f�B���N�g�����ڂ�1�ǂݏo�� */
            if (fno.fname[0] == 0xff) {
                //res=0�ɂȂ���A�t�@�C�������������Ȏ�������Bdir���J�������Čp��
                //���ꂪ��������̂́A0�T�C�Y�̃t�@�C�����쐬�������̃t�@�C��
                res = f_closedir(&dir);
                res = f_opendir(&dir, dirname);    /* �f�B���N�g�����J�� */
                continue;
            }
            DEBUG_PRINTF("file: %s, %d\r\n", fno.fname, res);
            if (res != FR_OK || fno.fname[0] == '\0') break;  /* �G���[�܂��͍��ږ����̂Ƃ��͔����� */
            if (fno.fname[0] == '.') continue;             /* �h�b�g�G���g���͖��� */
            if (fno.fattrib & AM_DIR) {          /* �f�B���N�g���͖��� */
            } else {
                // �f�B���N�g���͈ړ����Ă��Ȃ��̂ŁApath��ǉ�����K�v����
                sprintf(filename, "/%s/%s", dirname, fno.fname);
                do {
                    res = f_unlink(filename);
                    DEBUG_PRINTF("del file: %s, res=%d\r\n", filename, res);
                } while (res != FR_OK);
                
            }
        }
    }
    res = f_closedir(&dir);
//    DEBUG_PRINTF("closedir res=%d\r\n", res);
    res = f_rmdir(dirname); //�f�B���N�g�����폜
    DEBUG_PRINTF("rd: '%s, res=%d'\r\n", dirname, res);
                
}


/*
 * SD�J�[�h�ɐݒ���ۑ��p��Data�t�H���_���Ȃ���΍쐬
 * 
 */
FRESULT MakeDirecotory(char *dir) {
    FRESULT res;
    
    res = f_stat(dir, NULL);   //���Ƀf�B���N�g�������邩�H
    if (res == FR_NO_FILE) {
        //�f�B���N�g���Ȃ���΍쐬
        res = f_mkdir(dir);
        DEBUG_PRINTF("mkdir\r\n");
        return res;
    } else {
        DEBUG_PRINTF("Already %s exists\r\n", dir);
    }
    return res;
}


/*
 * �z�[���y�[�W�̕\��
 */
void SendHomepage(uint8_t clientID) {
    uint8_t jj;
    char msgstr[PROCESSBUF_SIZE];
    uint16_t year;
    uint8_t month, day, hour, minute, second;

    GetFontFolder("/");    //���[�g�̑S�t�H���_�����o��
    ESP_HtmlSnd2(clientID, html_txt2);

    for (jj=0; jj<FontNum; jj++) {
        year = (FontListDate[jj] >> 9) + 1980;
        month = (FontListDate[jj] >> 5) & 0x0f;
        day = (FontListDate[jj] & 0x1f);
        hour = (FontListTime[jj] >> 11);
        minute = (FontListTime[jj] >> 5) & 0x3f;
        second = (FontListTime[jj] & 0x1f);

        sprintf(msgstr, "<tr><td><input type='checkbox' name='removedir' value='%s'></td>" \
                "<td class='fname'>%s</td><td class='date'>%d/%02d/%02d %02d:%02d:%02d</td></tr>", 
                FontNameList[jj], FontNameList[jj], year, month, day, hour, minute, second);
        if (ESP_HtmlSnd2(clientID, msgstr)) return;
    }
    
    ESP_HtmlSnd2(clientID, html_txt2end);
    
    //�J�X�^���ݒ�̃f�[�^��html�Ƃ��ďo��
//    ShowCustomData(clientID);
    //�ȉ��ŁA�S���̃I�u�W�F�N�g�����������߂�
    //�����ŃJ���}��؂�Ŋe���ڂ��ƂɃf�[�^���o  �e�L�X�g�t�@�C���̕����ǂ��Ƃ̔��f
    for (jj = 0; jj < OBJECTNUM; jj++) {
//        sprintf(msgstr, "%-5s: %4u,%3d,%3d,%3d,%3d,  %2d,  0x%02x, 0x%04x, %4d, 0x%04x, 0x%04x,      %2d, %s,<br>", 
        sprintf(msgstr, "%-5s: %4u, %3d, %3d,    %2d,  0x%02x, 0x%04x,  %4d, 0x%04x, 0x%04x,      %2d,   %s,<br>", 
                CustomSettingText[jj],
                CustomRsc[jj].disp, CustomRsc[jj].x, CustomRsc[jj].y,   //CustomRsc[jj].xw, CustomRsc[jj].yw, 
                CustomRsc[jj].fontcode, CustomRsc[jj].fontmag, CustomRsc[jj].color, 
                CustomRsc[jj].format, CustomRsc[jj].attribute, CustomRsc[jj].attribute2, CustomRsc[jj].xpitchAdj,
                CustomFontDir[jj]);
        if (ESP_HtmlSnd2(clientID, msgstr)) return;
    }
    
    if (ESP_HtmlSnd2(clientID, html_txt3end)) return;
    if (ESP_HtmlSnd2(clientID, NULL)) return;   //�G���[�����f
    
    ESP_HtmlSndEnd(clientID);
}


/*
 * 16bit�J���[��24bit�J���[�ɕϊ�
 */
uint32_t ColorFrom565(uint16_t color) {
    uint8_t r, g, b;
    uint32_t col;
    
    b = (color << 3 ) & 0xff;   //�ŉ��ʂ�5bit��8�{���āA8bit��
    color = color >> 5;
    g = (uint8_t)((color & 0x3f) << 2);    //
    r = (color >> 3) & 0xf8;    //�ŏ�ʂ�5bit
    col = (((uint32_t)r << 16) | ((uint32_t)g << 8) | (b));
    return col;
}

/*
 * �J�X�^�}�C�Y�p�̉��(index2.html)���\��
 */
void SendCustomPage(uint8_t clientID) {
    uint8_t jj, fs, len;
    char msgstr[PROCESSBUF_SIZE];   //256�o�C�g
    uint32_t color;
    const char ItemText[][10] = { "disp", "font", "fsize", "color", "format", "attr1", "attr2", "xpAdj", "fName", };
    uint16_t r, g, b;

    ESP_HtmlSnd2(clientID, html_custom1);

    //�����ŃJ���}��؂�Ŋe���ڂ��ƂɃf�[�^���o  �e�L�X�g�t�@�C���̕����ǂ��Ƃ̔��f
    /*
        #box0{top:10px;left:30px;width:80px;height:18px;
	    background-color:#f48fb1}
     */
    //�e�I�u�W�F�N�g��style�ݒ�
    //���̐F��ݒ肵���F�ɂ����B�����F
    for (jj = 0; jj < OBJECTNUM; jj++) {
        fs = (uint8_t)(CustomRsc[jj].yw *4/5);     //���̍����ɍ��킹�ĕ����T�C�Y��ݒ�
        len = (uint8_t)strlen(CustomSettingText[jj]);
        //��L�t�H���g�T�C�Y�ŕ\���������ɉ����𒴂��鎞
        if (fs * len > CustomRsc[jj].xw) fs = (uint8_t)(CustomRsc[jj].xw /len);  //���̍����ɍ��킹�ĕ����T�C�Y��ݒ�
        
        //�w�i�F�����̐F���x�[�X�ɖ��x��2�{��1/2�{�ɂ���
        color = ColorFrom565(CustomRsc[jj].color);
        DEBUG_PRINTF("fc=%06lX ", color);
        r = color >> 16;
        g = (color >> 8) & 0x00ff;
        b = color & 0x00ff;

        r = r << 1;
        g = g << 1;
        b = b << 1;
        if ((r > 127)||(g > 127)||(b > 127)) {
            r = r >> 2;
            g = g >> 2;
            b = b >> 2;
        }
        color = ((uint32_t)r << 16) | ((uint32_t)g << 8) | (b);
        DEBUG_PRINTF("after=%06lX\r\n", color);
        
        sprintf(msgstr, "#box%d{top:%dpx;left:%dpx;width:%dpx;height:%dpx;background-color:#%06lX;font-size:%dpx;color:#%06lX;display:flex;justify-content:center;align-items:center;}", 
                jj, CustomRsc[jj].y, CustomRsc[jj].x, CustomRsc[jj].xw, CustomRsc[jj].yw, color, fs, ColorFrom565(CustomRsc[jj].color));
        if (ESP_HtmlSnd2(clientID, msgstr)) return;
    }

    ESP_HtmlSnd2(clientID, html_custom2);

    for (jj = 0; jj < 9; jj++) {
        sprintf(msgstr, "const %sItems = document.getElementsByName('%s');", ItemText[jj], ItemText[jj]);
        ESP_HtmlSnd2(clientID, msgstr);
    }
    
    for (jj = 0; jj < OBJECTNUM; jj++) {
        if (CustomRsc[jj].disp == 1) {
            //disp�ݒ肳��Ă�����̂�true�ɂ���
            sprintf(msgstr, "dispItems[%d].checked=true;", jj);
            ESP_HtmlSnd2(clientID, msgstr);
        }

        //�t�H���g
        if (CustomRsc[jj].fontcode != 0) {
            sprintf(msgstr, "fontItems[%d].value='%d';", jj, CustomRsc[jj].fontcode);
            ESP_HtmlSnd2(clientID, msgstr);
        }

        //format
        if (CustomRsc[jj].format != NOFORMAT) {
            sprintf(msgstr, "formatItems[%d].value='%d';", jj, CustomRsc[jj].format);
            ESP_HtmlSnd2(clientID, msgstr);
        }

        //fsize�́A�e�L�X�g�{�b�N�X�ŁA�����l�́Avalue�ɓ����
        //color�́A���l��value��16�i6���œ����B�擪��#
        sprintf(msgstr, "fsizeItems[%d].value='0x%02X';colorItems[%d].value = '#%06lX';", 
                jj, CustomRsc[jj].fontmag, jj, ColorFrom565(CustomRsc[jj].color));
        ESP_HtmlSnd2(clientID, msgstr);
        
        //attr1, attr2, xpAdj, fName�@�@�܂Ƃ߂邱�ƂŁA�u���E�U�̍X�V��33s��24s�ɏk�܂���
        if (jj != GearObj) {
            if ((jj == CalendarObj)||(jj == AlarmObj)) {
                //�F�ݒ�Ɏg���Ă���@�A���[���A�J�����_�[
                sprintf(msgstr, "attr1Items[%d].value='#%06lX';", jj, ColorFrom565(CustomRsc[jj].attribute));
            } else {
                sprintf(msgstr, "attr1Items[%d].value='%d';", jj, CustomRsc[jj].attribute);
                
            }
            ESP_HtmlSnd2(clientID, msgstr);

            if (jj == CalendarObj) {
                //�J�����_�̎�
                sprintf(msgstr, "attr2Items[%d].value='#%06lX';", jj, ColorFrom565(CustomRsc[jj].attribute2));
            } else {
                sprintf(msgstr, "attr2Items[%d].value = '%04X';", jj, CustomRsc[jj].attribute2);
            }
            ESP_HtmlSnd2(clientID, msgstr);
        }
        sprintf(msgstr, "xpAdjItems[%d].value = '%d';fNameItems[%d].value = '%s';", 
                jj, CustomRsc[jj].xpitchAdj, jj, CustomRsc[jj].fontName);
        if (ESP_HtmlSnd2(clientID, msgstr)) return;
    }

    if (ESP_HtmlSnd2(clientID, html_custom5)) return;   //�G���[�̎�
    ESP_HtmlSnd2(clientID, NULL);

    ESP_HtmlSndEnd(clientID);
}


/*
 * �J�X�^���ݒ�̃f�[�^��SD�J�[�h����ǂݏo��
 */
FRESULT LoadCustomData(char *filename) {
    FRESULT res;
    uint8_t kk, len;
    char str[150];
    char font[20] = "";
    char *p;
    FIL file;

    res = f_open(&file, filename, FA_READ);
    if (res != FR_OK) return res;   //������open�ł��Ȃ�������I��
    //disp��S��0�Ƀ��Z�b�g
    for (kk = 0; kk < OBJECTNUM; kk++) CustomRsc[kk].disp = 0;
    
    //1�s�ڂ̃e�L�X�g�f�[�^��ǂݏo�����A�f�[�^�͕s�v
    f_gets(str, sizeof str, &file);     //\n�܂œǂݏo��
    
    //���s���ɂ�����ɂ́A�ŏ��̍��ږ������āA�i�[�ԍ����m�肷��K�v����
    //:�܂ł̕������L�[���[�h�ɂȂ��Ă��āA�w�胏�[�h�ȊO�͖���
    while (f_gets(str, sizeof str, &file) != NULL) {     //\n�܂œǂݏo��
        //�擪����:�܂ł����ږ�
        p = strtok(str, ":");
        len = (uint8_t)strlen(p)+1;
        //�ǂݍ��񂾃f�[�^���ǂ̍��ڂ��A�T�[�`����kk�ɐݒ�
        //��v�������̂����������ꍇ�A�Ȃ��ꍇ�̓X�L�b�v
        kk = 0;
        while (strncmp(CustomSettingText[kk], p, 4) && kk < OBJECTNUM) kk++;
        
        if (kk < OBJECTNUM) {
            DEBUG_PRINTF("fn=%p, %s.%p\r\n", CustomRsc[1].fontName, CustomFontDir[kk], CustomFontDir[kk]);
            //:�̎��̏���
            font[0] = '\0';
            //                 disp,x,y,xw,yw, fontc, fsize, col, fmt, atr, atr2, xp, font
//            sscanf(str + len, "%hhd,%d,%d,%d,%d, %hhd, %hhx, %hx, %d, %hx, %hx, %hhd, %[^,],", 
                 //&CustomRsc[kk].xw, &CustomRsc[kk].yw, 
            sscanf(str + len, "%hhd, %hd, %hd, %hhd, %hhx, %hx, %hd, %hx, %hx, %hhd, %[^,],", 
                    &CustomRsc[kk].disp, &CustomRsc[kk].x, &CustomRsc[kk].y,
                    &CustomRsc[kk].fontcode, &CustomRsc[kk].fontmag, &CustomRsc[kk].color, 
                    &CustomRsc[kk].format, &CustomRsc[kk].attribute, &CustomRsc[kk].attribute2, &CustomRsc[kk].xpitchAdj,
                    CustomFontDir[kk]);

//            DEBUG_PRINTF("%-5s: %4u,%3d,%3d,%3d,%3d, %2d, 0x%02x, 0x%04x, %4d, 0x%04x, 0x%04x, %2d, %s,\r\n", 
            DEBUG_PRINTF("%-5s: %4u, %3d,%3d, %2d, 0x%02x, 0x%04x, %4d, 0x%04x, 0x%04x, %2d, %s,\r\n", 
                CustomSettingText[kk],
                CustomRsc[kk].disp, CustomRsc[kk].x, CustomRsc[kk].y,   //CustomRsc[kk].xw, CustomRsc[kk].yw, 
                CustomRsc[kk].fontcode, CustomRsc[kk].fontmag, CustomRsc[kk].color, 
                CustomRsc[kk].format, CustomRsc[kk].attribute, CustomRsc[kk].attribute2, CustomRsc[kk].xpitchAdj,
                CustomRsc[kk].fontName);
        }
    }
    f_close(&file);

    return res;
}


/*
 * WiFi��M�����f�[�^�̏������s��
 * 
 * ��{�I�ɁA���s�R�[�h(CR/LF)����؂薈�ɏ���
 * �@�o�b�t�@�t���̎��Ȃǂ́A���s�R�[�h���t���Ȃ��ꍇ����
 * 
 * boundary�����񂪗������ƁA��s����M��o�C�i���f�[�^�Ƃ��Ď�舵��
 * �@���E������������A�I���
 * 
 * ��M����f�[�^�́A�ȉ��̂悤�Ɏn�܂�p�P�b�g�P��
 * +IPD,x,ddd:***    �����̃o�C�g��������M����K�v����
 * 
 * �}���`�p�[�g�Ńf�[�^������\����z�肵�āABoundary������ǂ��`�F�b�N
 * Content-Disposition
 * Content-Type
 * �̏����悭���āA���������߂�
 * bmp�t�@�C���̏ꍇ: �ȉ��̂悤�Ȋ����ɂȂ��Ă���
 * ------webKitFormBoundaryKjqavoSoqHf8J7Yg
 * Content-Disposition: form-data; name="fontfile"; filename="ForDebug/1.bmp"
 * Content-Type: image/bmp
 * (CR/LF)
 * 42 B 4D M AE . 00 . 00�̂悤�Ƀo�C�i���f�[�^������
 * (CR/LF)
 * ------webKitFormBoundaryKjqavoSoqHf8J7Yg
 * bmp�f�[�^�ɓ��肷��Ȃ�ABM�Ƃ��������ɑ���3�o�C�g���t�@�C���T�C�Y�Ȃ̂ŁA
 * ����𗘗p���������邪�Atxt�t�@�C�����ƁA���̎���g���Ȃ��B
 * ��͂�boundary��������g���̂��������Ǝv����B
 * boundary=""�Ŏ�����镶����ɁA(CR/LF)--��擪�ɒǉ������̂��A�}���`�p�[�g�̎n�܂�ŁA
 * ���ꂪ�����̃p�[�g�ɂȂ�ꍇ������A�I�����́A���̕�����ɂ����--�����������񂪏I���ƂȂ�B
 * �Ȃ��Abaoundary�ł������Ă��镔�����ʂ̃p�P�b�g(�w�b�_��+IPD,0,nnn:���t��)�ɂȂ��Ă���ꍇ�����邵�A
 * ���̂��̂Ɋ܂܂�Ă���ꍇ������̂ŁA���ӁB���Ԃ�p�P�b�g�̃T�C�Y���傫������Ƃ����Ȃ�l�q�B
 * 
 * 
 */


/*
 * �f�o�b�O�p�Ƀo�C�i���f�[�^�����₷���t�H�[�}�b�g�ŕ\���@1�o�C�g����
 * ���̃f�[�^���I�[�v���ς݂̃t�@�C���ɏ�������
 * 
 * 16�o�C�g���̃f�[�^���܂Ƃ܂�����ASCII�ŕ\��
 */
void DisplayBinary(uint16_t *ad, uint8_t data)
{
    uint16_t addr = *ad;
    static char log[18];

    //�o�C�i������\��
    if (addr % 16 ==0) {
        DEBUG_PRINTF("%06X: ", addr);
    }
    DEBUG_PRINTF("%02X ", data);
    //�o�C�i����1�o�C�g���������ށ@�@�t�@�C���̓I�[�v���ς� <<<<<<<<<<<<<<<<<<<<<

    // ASCII�\���p�̃f�[�^����
    if ((data < 0x20)||(data >= 0x7f)) log[addr % 16] = '.';
    else log[addr % 16] = data;
    addr++;
    //16�o�C�g���܂Ƃ܂�����\��
    if (addr % 16 == 0) {
        log[16] = '\0';
        DEBUG_PRINTF("%s\r\n", log);
    }
    *ad = addr;
}

void DisplayBinaryBuf(uint16_t *ad, uint8_t *data, uint16_t len) {
    uint16_t jj;
    
    for (jj = 0; jj < len; jj++) {
        DisplayBinary(ad, *data);
        data++;
    }
    
}


void SetData(char *item, char *data) {
    uint8_t kk;
    static uint8_t TargetObj;
    uint8_t r, g, b;
    char *p;
    
    //�Ώۖ������̃f�[�^���ŏ��ɗ���ŁA������L�^
    kk = 0;
    while (strncmp(CustomSettingText[kk], item, 4) && kk < OBJECTNUM) kk++;
    if (kk < OBJECTNUM) {
        TargetObj = kk;
        return;
    }
    
    //��L�ȊO�̎��́A�f�[�^�Ƃ��āATargetObj�̃I�u�W�F�N�g��Ώۂɂ���
    //checkbox�́A�`�F�b�N���ꂽ���ɂ���POST����Ȃ��B�܂�disp��������on�����Ȃ�
    //�I�t�ɂȂ��Ă��鍀�ڂ𔻒f�ł��Ȃ��̂ŁA�������ɑS���N���A���Ă����K�v����
    if (strncmp(item, "disp", 4) == 0) {
        if (strncmp(data, "on", 2) == 0) CustomRsc[TargetObj].disp = 1;
        DEBUG_PRINTF("   disp = %d\r\n", CustomRsc[TargetObj].disp);
    }
    else if (strncmp(item, "XY", 2) == 0) {
        //�X�y�[�X��+�ɂȂ��Ă���̂ŁA�󔒂ɕϊ��@�@++1�Ƃ��ƂȂ��Ă���Ɛ������ϊ��ł��Ȃ�����
        do {
            p = strchr(data, '+');
            if (p != NULL) *p = ' ';
            else break;
        } while (1);
        sscanf(data, "%hd%%2C%hd", &CustomRsc[TargetObj].x, &CustomRsc[TargetObj].y);
        DEBUG_PRINTF("   XY=%d, %d\r\n", CustomRsc[TargetObj].x, CustomRsc[TargetObj].y);
    }
    else if (strncmp(item, "font", 4) == 0) {
        CustomRsc[TargetObj].fontcode = (uint8_t)atoi(data);
    }    
    else if (strncmp(item, "fsize", 5) == 0) {
        sscanf(data, "%hhx", &CustomRsc[TargetObj].fontmag);
        DEBUG_PRINTF("   fontmag=%x\r\n", CustomRsc[TargetObj].fontmag);
    }
    else if (strncmp(item, "color", 5) == 0) {
        sscanf(data, "%%23%2hhX%2hhX%2hhX", &r, &g, &b);
        CustomRsc[TargetObj].color = Getcolor565_8(r, g, b);
        DEBUG_PRINTF("   => %x %x %x -> %x\r\n", r, g, b, CustomRsc[TargetObj].color);
    }
    else if (strncmp(item, "format", 5) == 0) {
        CustomRsc[TargetObj].format = (uint16_t)atoi(data);
    }
    else if (strncmp(item, "attr1", 5) == 0) {
        if ((TargetObj == 5)||(TargetObj == 2)) {   //�J�����_���A���[���̎�
            sscanf(data, "%%23%2hhX%2hhX%2hhX", &r, &g, &b);
            CustomRsc[TargetObj].attribute = Getcolor565_8(r, g, b);
        } else {
            sscanf(data, "%hd", &CustomRsc[TargetObj].attribute);
        }        
    }
    else if (strncmp(item, "attr2", 5) == 0) {
        if (TargetObj == 5) {
            sscanf(data, "%%23%2hhX%2hhX%2hhX", &r, &g, &b);
            CustomRsc[TargetObj].attribute2 = Getcolor565_8(r, g, b);
        } else {
            sscanf(data, "%hd", &CustomRsc[TargetObj].attribute2);
        }
    }
    else if (strncmp(item, "xpAdj", 5) == 0) {
        CustomRsc[TargetObj].xpitchAdj = (int8_t)atoi(data);
    }
    else if (strncmp(item, "fName", 5) == 0) {
        strcpy(CustomFontDir[TargetObj], data);
    }
    
}

/*
 * POST���ꂽ�f�[�^����������
 * �J�X�^���ݒ�p�f�[�^�̏���
 * �Ō�̃f�[�^�̊J�n�|�C���g��Ԃ�
 * �����r���Ő؂�Ă��鎞�̏����Ɏg�p
 * 
 */
void POSTdata2(char *st) {
    char * nxtp;
    char * stp;
    char item[20], data[20], target[80];
    static char holdstr[80] = "";

    nxtp = strchr(st, '&');
    if (nxtp == NULL) return;
    *nxtp = '\0';
    sprintf(target, "%s%s", holdstr, st);    //�ŏ������A�O��̎c�蕶���ƘA��
    holdstr[0] = '\0';
    stp = target;

    while (1) {
        item[0] = '\0';
        data[0] = '\0';
        sscanf(stp, "%[^=]=%s", item, data);
        DEBUG_PRINTF("%s = %s\r\n", item, data);
        SetData(item, data);

        if (nxtp == NULL) break;
        
        stp = nxtp + 1;
        nxtp = strchr(stp, '&');
        if (nxtp != NULL) {
            *nxtp = '\0';
        }
        else {
            //&���Ȃ�������A�ێ�������Ƃ��Ċi�[���ďI��
            //������last='&'�̎��́A���ʂɏ������āAholdstr�Ȃ��ŏI��������
            if (ContentLength > 0) {
                sprintf(holdstr, "%s", stp);
                DEBUG_PRINTF("�����ۗ� %s\r\n", holdstr);
                break;
            } else {
                //�Ō�܂Ŏ�荞�񂾂͂��Ȃ̂ŁA���̂܂܏���
                DEBUG_PRINTF("Content�I��\r\n");                
            }
        }
    }
}

char *CRLF = "\r\n";

/*
 * ���̋��E������܂Ńo�C�i���Ƃ��ď���
 * ���s�R�[�h�������̓o�b�t�@�����ς��܂Ńf�[�^���擾���ď������Ă���
 * ���E������́A���s�Ɉ͂܂�Ă��邪�A�p�P�b�g�ɂ܂������Ă���P�[�X�����邪�|�C���g
 * 
 */
/*
 * 
 * �o�C�i�����ȊO�ł��A�ȉ��̂悤�Ƀp�P�b�g���؂�Ă��܂��ꍇ������
 * Content-Dis �Ő؂�āA�����́Aposition�ƂȂ����Ă���
(76>212)Content-Disposition: form-data; name="fontfile"; filename="Funwa28/4s.bmp"
 'Funwa28/4s.bmp' is filename.
open dir=Funwa28, fn=4s.bmp
(25>187)Content-Type: image/bmp
(2>185)
 Start Binary area
Only CRLF
000000: 42 4D 82 00 00 00 00 00 00 00 3E 00 00 00 28 00 BM........>...(.
000010: 00 00 0C 00 00 00 11 00 00 00 01 00 01 00 00 00 ................
000020: 00 00 44 00 00 00 13 0B 00 00 13 0B 00 00 00 00 ..D.............
000030: 00 00 00 00 00 00 00 00 00 00 FF FF FF 00 FF F0 ................
000040: 00 00 FF 30 00 00 FE 10 00 00 C0 10 00 00 80 00 ...0............
000050: 00 00 80 00 00 00 80 00 00 00 86 10 00 00 C2 10 ................
000060: 00 00 C0 10 00 00 E0 10 00 00 F0 10 00 00 F0 10 ................
000070: 00 00 F8 10 00 00 FC 30 00 00 FE 70 00 00 FF F0 .......0...p....
000080: 00 00
------WebKitFormBoundaryxjooIvhCYhUBVFyL
  boundary start2
file close1
(13>-2)Content-Dis
>>>Packet END<<<
---Found +IPD,0,1460:position: form-data; name="fontfile"; filename="Funwa28/5.bmp" (len=76)---
  ID=0,   PacketLength=1460
  This line's data=64
Continue Multipart
(64>1396)position: form-data; name="fontfile"; filename="Funwa28/5.bmp"
(25>1371)Content-Type: image/bmp
(2>1369)

 */

/*
 * POST�p�P�b�g���̊e�s�̓��e�ɉ����ď�������
 * �e�L�X�g���́A���s�R�[�h�ł�����ƏI����Ă��邱�Ƃ�z��
 * 
 * st�̐擪�ɂ͉��s�R�[�h�͂Ȃ����A�Ō�ɂ͉��s�R�[�h����
 */
void POSTdata(char *st) {
    FRESULT res;
    
    //�}���`�p�[�g���L���Ȏ��A���E�����񂪂��łɐݒ肳��Ă���΁A���E�����񂩂ǂ������ׂ�
    if (BoundaryStr[0] != '\0') {
        //���E���������������ɂ�2�s���̃f�[�^���K�v�B�����ɗ������ɉ��s�܂ł�
        //�f�[�^�͂��邱�Ƃ͂킩���Ă��邪�A���̎��̍s�����邩�͊m�F���Ă��Ȃ��B
        //�ʃp�P�b�g�ɂ܂������Ă���ꍇ������A���̎蓖�Ă��K�v�B
        //�f�[�^�͂��łɎ�M���Ă���P�[�X�������Ǝv���̂ŁA�]�v�ȕ���(+IPD,)�̍폜
        
        if (strncmp(st, BoundaryStr, BoundaryStrLen) == 0) {
            //BoundaryStr: "--%s--" �̌`
            //�����ŁA���E�̈�I��
            DEBUG_PRINTF("**************  boundary End *************\r\n");
            BinOrText = TXT;
            InBoundary = 0; //���E��I��
            BoundaryStr[0] = '\0';  //�}���`�p�[�g���I������̂ŕ�����N���A
            return;
        }

        if (strncmp(st, BoundaryStr, BoundaryStrLen -2) == 0) {
            //BoundaryStr: "--%s" �̌`
            //��������boundary�̈悪�n�܂�
            //Content-Length�́A�ŏ��̋��E������̌ォ��J�E���g�n�߂�
            DEBUG_PRINTF("boundary (re)start =========\r\n");
            InBoundary = 1; //���E��J�n
            BinOrText = TXT;    //��U�e�L�X�g��
            return;
        }
    }
    
    /*
     * �p�P�b�g���Ƃ͕ʂɁA���e�̒������w�肳���
     * ���e�Ƃ́A���E�������܂ރf�[�^�����������Ă���
     */
    if (strncmp(st, "Content-Length:", 15) == 0) {
        //�����𐔒l�ɕϊ����邽�߁A���s�R�[�h�̏��𕶎��I�[�ɂ���
        ContentLength = (uint16_t)atoi(st + 15);       // ������̒����𐔒l�ɕϊ�����
        DEBUG_PRINTF("Content-Length=%d\r\n", ContentLength);
        return;
    }
    /*
     * multipart�́A�ȉ���2�s���܂ށB
     * Content-Disposition: 
     * Content-Type: 
     * 
     * (data)
     */
    //Content-Type: application/octet-stream�@�Ƃ������̂����邪�A���͑ΏۊO
    if (strncmp(st, "Content-Type: multipart/form-data; boundary=", 44) == 0) {
        //�w�肳�ꂽ���E������́A���̌�ɏo�ė��鋫�E�̍ŏ��A�r���A�Ō�ł��ꂼ��Ⴄ�`�Ŋ��p�����
        //�ŏ�:�@�擪��-��2�t��
        //�r��:�@�擪��-��2�t���ƂƂ��ɁA���ɂ�--���t��
        //�Ō�:�@�r���̂��̂ɂ���Ɍ���--���ǉ������
        //�����āA�K���O��͉��s�R�[�h�ň͂܂��

        //���E������擾�@���ۂɎg���鎞�ɍ��킹�āA�擪��--�t��
        //��:Content-Type: multipart/form-data; boundary=----WebKitFormBoundaryIEYlyg6d8k8gvZ50
        //���E������  -(�n�C�t��)��4��WebKit�Ŏn�܂�v38�����@���s�R�[�h�폜���Ă���
        BoundaryStrLen = strlen(st + 44);
        if (BoundaryStrLen > BOUNDARYSTRLENGTH - 7) {
            //���E������ϐ��Ɋi�[�ł���
            DEBUG_PRINTF("\r\n!! ERROR BoundaryStr overflow !!\r\n");
            BoundaryStrLen = 0; //���E�������ݒ�
            return;
        }
        
        //���E������̐ݒ�@�@�I�[�p�����p�ӂ��A��r��������ς��āA���Ԃ͔��f������
        sprintf(BoundaryStr, "--%s--", st + 44);
        BoundaryStrLen = BoundaryStrLen + 4;    //BoundaryStr�̒����B�ŏ���--
        DEBUG_PRINTF("Set boundary='%s'(%d)\r\n", BoundaryStr, BoundaryStrLen);
        
        InBoundary = 0; //�}���`�p�[�g����
        return;
    }

    //�t�H���g�t�@�C�����A�b�v���[�h�����N�_
    if (strncmp(st, "Content-Disposition: form-data; name=\"fontfile\"; filename=", 58) == 0) {
        WHICHJOB = UPLOADFONT;
        sscanf(st+59, "%[^/]/%[^\"]\"", Dirname, Filename);
        DEBUG_PRINTF("open dir=%s, fn=%s \r\n", Dirname, Filename);
        if (Filename[0] == '\0') return;
        
        //�t�@�C�������������́A���~
        if (strlen(Filename) > 12) {
            DEBUG_PRINTF("Skip due to long filename\r\n");
            return;
        }
        
        f_chdir("/");
        MakeDirecotory(Dirname);
        f_chdir(Dirname);

        do {        
            res = f_open(&BinFile, Filename, FA_CREATE_ALWAYS | FA_WRITE);
            DEBUG_PRINTF("res=%d\r\n", res);
        } while (res != FR_OK);   //������open�ł���܂Ń��[�v
        //���̂悤�ɂ��Ȃ��ƁA�����Ə������߂��A�������ȃt�@�C����ԂɂȂ��Ă��܂����t�@�C�����������ꂽ
        
        FileCount++;
        return;
    }

    //�J�X�^���p�f�[�^�t�@�C���̃A�b�v���[�h
    //���X�V�X�e���ŊǗ����Ă������̃t�@�C�����́AData?.dat�@?�́A1-8
    //����́APIC����1�t�@�C���Ɍ���B�����t�@�C�����̂�PC��ŁA����䂦�A�t�@�C�����͌Œ艻����
    if (strncmp(st, "Content-Disposition: form-data; name=\"datafile\"; filename=", 58) == 0) {
        WHICHJOB = UPLOADCUSTOM;
        sscanf(st+59, "%[^\"]\"", Filename);
        DEBUG_PRINTF("Upload filename=%s\r\n", Filename);
        if (Filename[0] == '\0') return;
        f_chdir("/");
        MakeDirecotory(DataFolder); //�܂��쐬����Ă��Ȃ��P�[�X��z��B���ɂ����Ă����Ȃ�
        f_chdir(DataFolder);
        //�A�b�v���[�h����f�[�^�t�@�C�������[�g�f�B���N�g���ɏ�������
        do {
            res = f_open(&BinFile, "Data0.dat", FA_CREATE_ALWAYS | FA_WRITE);
        } while (res != FR_OK);   //������open�ł���܂Ń��[�v
        FileCount++;
        return;
    }
    
    //remove
    if (strncmp(st, "removedir=", 10) == 0) {
        char *p, *p2;

        WHICHJOB = REMOVEFONT;
        DEBUG_PRINTF("Remove Dir\r\n");
        //�����I�����Ă����ꍇ&�ŘA������Ă��邱�Ƃ��l��
        p2 = st;
        do {
            p = strchr(p2, '&');
            *p = '\0';
            RemoveFolder(p2 + 10);    //removedir=�@������
//            DEBUG_PRINTF("rd=%s\r\n", p2+10);
            p2 = p + 1;
        } while (p != NULL);
        
        return;
    }
}


/*
 * ���N�G�X�g���C���̏��u
 * +IPD�̌���ID�ƃp�P�b�g�����擾
 * �G���[�̎���-1��Ԃ��B��{�I��ESP_GetDataP�̕Ԃ�l�Ɠ������̂�Ԃ�
 * 
 */
int8_t GetRequestLine(char *st, uint16_t *len, uint8_t *clientID, uint16_t *PacketLen, uint8_t skip) {
    int8_t ans;
    char *p;
    uint16_t len2;

    ans = ESP_GetDataP(st, len, skip);
    DEBUG_PRINTF("original:ans=%d,len=%d:%s\r\n", ans, *len, st);
    
    if (ans == -1) return ans;
    //\r\n�����ȂǁA+IPD,�Ŏn�܂��Ă��Ȃ�������A�����I��
    if (strncmp(st, "+IPD,", 5) != 0) return -1;

    //�p�P�b�g�w�b�_����:�܂łɂ���AclientID�Ǝ�M���ׂ����������擾
    sscanf(st + 5, "%hhd,%hd:", clientID, PacketLen);
    DEBUG_PRINTF("ID=%d, PacketLen=%d\r\n", *clientID, *PacketLen);
    p = strchr(st, ':');
    len2 = (uint16_t)((p - st) +1);   //:�܂ł̒���
    *len = *len - len2;
    //:�܂ł��폜���āA�c��������n��
    memmove(st, st + len2, *len +1); //\0�܂Ŋ܂߂邽��
    *PacketLen = *PacketLen - *len;
    DEBUG_PRINTF("t(-%d=%d)cnt=%d,%s", *len, *PacketLen, ContentLength, st);
    return ans;
}


/*
 * ��M�����f�[�^�̏����F�p�P�b�g�̏���
 * ��{�I�ɉ��s�R�[�h�܂ł�1�s���ɏ������Ă���
 * �����p�P�b�g�ɕ�����Ă��鎞�́A�O�̃p�P�b�g�ŏ����ł��Ȃ�����������ɑ����Ď�荞��
 * 
 * �ŏ��̍s(+IPD,����n�܂�s)���`�F�b�N���A�N���C�A���gID�ƕ��������擾
 * ��: +IPD,0,473:GET /index.html HTTP/1.1
 *         A A   A 
 * �@�@�@�@�ŏ��̃J���}�@�@id:�N���C�A���gID
 *�@:�ȍ~�����N�G�X�g���C��
 */
void PacketProcess() {
    uint16_t len, remlen;
    static char st[PROCESSBUF_SIZE] = "";   //����256�o�C�g
    char *st2;
    int8_t ans;
    uint8_t FlagLF, preFlagLF;
    uint8_t clientID, id;
    static uint16_t PacketLen = 0;  //���̃p�P�b�g�̒���
    uint8_t skipCR, jj;
    static uint16_t addr = 0;
    FRESULT res;
    UINT actualLength;
    uint8_t send;

    //�f�[�^���Ȃ����͏����I��
    if (!UART1_is_rx_ready()) return;
    WHICHJOB = 0;
    len = sizeof(st);
    ans = GetRequestLine(st, &len, &clientID, &PacketLen, 0);
    if (ans != 0) return;   //ans=0�ȊO�͑z�肵�Ă��Ȃ��̂Ŕr��
    
    //�R�}���h�̔��f
    if (strncmp(st, "GET ", 4) == 0) {
        //GET�̏���
        //�y�[�W�ɃA�N�Z�X���Ă������̂݁A�N���C�A���g��HTML�e�L�X�g�𑗐M����
        if (strstr(st, "index.html") != NULL) send = 1;
        else if (strstr(st, "index2.html") != NULL) {
            send = 2;
            WHICHJOB = SETCUSTOM;
        }
        else send = 0;
        // GET /favicon.ico HTTP/1.1�@�����M����ė��Ă����̂ŁA���܂�2�񑗐M���Ă���

        DEBUG_PRINTF("-- GET start --\r\n");
        while (PacketLen > 0) {
            //�f�[�^�ǂݔ�΂������ŉ����������Ȃ��B�����ŕ����s��������
            len = sizeof(st);
            ans = ESP_GetData(st, &len);
            PacketLen = PacketLen - len;
            DEBUG_PRINTF("G(-%d=%d)%s", len, PacketLen, st);
        }
        //�^�C���A�E�g�A�f�[�^�ɕs�������������̏����͂��Ă��Ȃ�
        DEBUG_PRINTF("--- FINISH GET ---\r\n");

        if (send==1) SendHomepage(clientID);
        else if (send==2) SendCustomPage(clientID);

        return;
    }
    else if (strncmp(st, "POST", 4) == 0) {
        if (strstr(st, "index.html") != NULL) send = 1;
        else if (strstr(st, "index2.html") != NULL) {
            send = 2;
            WHICHJOB = SETCUSTOM;
            //checkbox���I�t�̎��͉���POST����Ȃ��̂ŁA�����ň�U���Z�b�g
            for (jj = 0; jj< OBJECTNUM; jj++) CustomRsc[jj].disp = 0;
        }

        //�u���E�U���瑗�M����Ă������B�P���ȃt�H�[���̃f�[�^���M������
        BoundaryStr[0] = '\0'; //���E����������Z�b�g
        DEBUG_PRINTF("-- POST start --\r\n");
        FileCount = 0;  // �A�b�v���[�h���ꂽ�t�@�C����
        StartContent = 0;
        ContentLength = 0;
        InBoundary = 0;
        BinOrText = TXT;
        
        while (1) {
            DEBUG_PRINTF("%c:", 'a'+BinOrText); //a(ASCII=TXT)�܂��́Ab(BIN)
            if (PacketLen == 0) {
                reGetNextPacket0:
                do { 
                    len = sizeof(st) - 1;
                    ans = GetRequestLine(st, &len, &id, &PacketLen, skipCR);
                } while (ans == -1);    //�o�C�i�����[�h�̎��́Aans��0�ȊO�̎�������̂ŁA�C��
                //�ʂ̃p�P�b�g�����荞��ł���ꍇ����@�@���̎���ID���قȂ�
                if (clientID != id) {
                    //clientID���قȂ�ꍇ�́A���̃p�P�b�g����M���Ĕj��
                    while (PacketLen > 0) {
                        //�f�[�^�ǂݔ�΂������ŉ����������Ȃ��B�����ŕ����s��������
                        len = sizeof(st) - 1;
                        ans = ESP_GetData(st, &len);
                        PacketLen = PacketLen - len;
                        DEBUG_PRINTF("intG0(-%d=%d)%s", len, PacketLen, st);
                    }
                    goto reGetNextPacket0;
                }                
                if (StartContent) ContentLength = ContentLength - len;    
                DEBUG_PRINTF("t(-%d=%d)cnt=%d,%s", len, PacketLen, ContentLength, st);
            } else {
                len = sizeof(st) - 1;
                if (len > PacketLen) len = PacketLen;   //�c��p�P�b�g�����Ȃ����́A����ɍ��킹��
                ans = ESP_GetData(st, &len);
                if ((ans != 0) && (len < sizeof(st))) st[len] = '\0';   //�I�[�������Γ����
                PacketLen = PacketLen - len;
                if (StartContent) ContentLength = ContentLength - len;
                DEBUG_PRINTF("P ans=%d(%d=%d)cnt=%d, %-256s|\r\n", ans, -len, PacketLen, ContentLength, st);
            }
            if (ans == -1) return;
            if (ans == 1) skipCR = 1;    //\r�ŏI����Ă�����A\r�̔���������Ȃ��悤�ɏ���
            else skipCR = 0;

            //�p�P�b�g�̍Ō�ŁA���E��������������Ă��܂��ꍇ����
            //�����AContentLength������Alen�����E�����������Z�����͒ǉ��擾������
            if ((PacketLen == 0) && (ans !=0) && (ContentLength > 0)) {
                remlen = len;   //���u�ۗ��̃o�C�g��
                st2 = st + remlen;
                //���������̃p�P�b�g����ǂݏo��
                reGetNextPacket:
                do {
                    len = sizeof(st) - remlen - 1;
                    if (len == 0) break;
                    ans = GetRequestLine(st2, &len, &id, &PacketLen, skipCR);
                } while (ans == -1);    //�o�C�i�����[�h�̎��́Aans��0�ȊO�̎�������̂ŁA�C��
                if (clientID != id) {
                    //clientID���قȂ�ꍇ�́A���̃p�P�b�g����M���Ĕj��
                    while (PacketLen > 0) {
                        //�f�[�^�ǂݔ�΂������ŉ����������Ȃ��B�����ŕ����s��������
                        len = sizeof(st) - remlen - 1;
                        ans = ESP_GetData(st2, &len);
                        PacketLen = PacketLen - len;
                        DEBUG_PRINTF("intG1(-%d=%d)%s", len, PacketLen, st2);
                    }
                    goto reGetNextPacket;
                }
                if (StartContent) ContentLength = ContentLength - len;   
                DEBUG_PRINTF("%c:(-%d)cnt=%d,%s", 'a'+BinOrText, len, ContentLength, st2);
                len = len + remlen;
                DEBUG_PRINTF("+(-%d=%d)ans=%d,�A��%s", len, PacketLen, ans, st);
            }
            
            if (BinOrText == BIN) {
                preFlagLF = FlagLF;
                //����ǂݍ��񂾍s�̍Ōオ\r\n��������A����͏��������A���s����t���O�Z�b�g
                //ans=0�F�Ō�̉��s�R�[�h�������A�o�C�i���Ƃ��ď���������
                //ans=1�F\r�������A�o�C�i���Ƃ��ď���
                //ans=2�F�S�f�[�^���o�C�i���Ƃ��ď���
                if (ans == 0) FlagLF = 2;
                else if (ans == 1) FlagLF = 1;
                else FlagLF = 0;
                DEBUG_PRINTF("FlagLF=%d\r\n", FlagLF);
                
                if (preFlagLF == 2) {
                    if (ans == 0) {
                        //���E�����񂩂ǂ����`�F�b�N
                        POSTdata(st);
                    }
                    //�����łȂ��ꍇ�́A�����ۗ��ɂ������s�R�[�h�ƍ���̃f�[�^���o�C�i���Ƃ��ď���
                    //�����ۗ��ɂ������s�R�[�h�ƍ���̃f�[�^���o�C�i���Ƃ��ď���
                }
                else if (preFlagLF == 1) {
                    if ((ans == 0) && (st[0] == '\n')) {
                        //���E�����񂩂ǂ����`�F�b�N�@�������Alen��1����������������A
                        if (len > 1) POSTdata(st + 1);
                    }
                }
                //���E�����񂾂�����A�e�L�X�g���[�h�ɖ߂��Ă���
                if (BinOrText == TXT) {
                    DEBUG_PRINTF("%d:Return to TXT\r\n:", preFlagLF);
                    while (addr % 16 > 0) DisplayBinary(&addr, 0);
                    
                    res = f_close(&BinFile);    // ���E�����������̂ŁA�t�@�C������ <<<<<<<<<<<<<<<<<<

                    addr = 0;
                } else {
                    //��L�ȊO�̃P�[�X�́A�o�C�i���Ƃ��ď�������
                    //��������̂́ApreFlagLF���������AFlagLF���͎����
                    DEBUG_PRINTF("    bin len=%d +%d-%d\r\n", len, preFlagLF, FlagLF);
                    //POSTdatabin(preFlagLF);   //Bin�f�[�^�̏������{
                    DisplayBinaryBuf(&addr, (uint8_t *)CRLF, preFlagLF);   //�O�̍s�̉��s�R�[�h��K�v�ɉ�����
                    //�o�C�i����1�o�C�g���������ށ@�@�t�@�C���̓I�[�v���ς� <<<<<<<<<<<<<<<<<<<<<
                    res = f_write(&BinFile, CRLF, preFlagLF, &actualLength);
//                    DEBUG_PRINTF("res=%d (0:OK)\r\n", res);
                    DisplayBinaryBuf(&addr, (uint8_t *)st, len - FlagLF);
                    res = f_write(&BinFile, st, len - FlagLF, &actualLength);
//                    DEBUG_PRINTF("res=%d (0:OK)\r\n", res);
                }

                if (ContentLength == 0) break;  //�I��
                
            }
            else {  //TXT
                if (ans == 0) {
                    if (len == 2) {
                        //CRLF�݂̂̍s
                        DEBUG_PRINTF("OnlyCRLF\r\n");
                        if (StartContent == 0) {
                            DEBUG_PRINTF("---- Start Content ----\r\n");
                            StartContent = 1;   //�w�b�_�I���̋�s��������A�ȍ~���b�Z�[�W�{�f�B
                        }
                        if (InBoundary == 1) {
                            //boundary���̋�s�ȍ~�̓o�C�i���f�[�^
                            BinOrText = BIN;    //�o�C�i�����Ɉڍs
                            FlagLF = 0;
                            DEBUG_PRINTF("bbbbbbbbb Start Binary area bbbbbbbbb\r\n");
                        }
                    } else {
                        //�w�b�_�̏���
                        st[len -2] = '\0';  //�����̊ȑf���̂��߁A���s�R�[�h��������
                        POSTdata(st);     //1�s�̏���
                    }
                }
                else {
                    //������ǂݍ��ޕK�v�����邩�𔻒f
                    if (StartContent) {
                        //index2�ő��M���ꂽ���̏���
                        if (WHICHJOB == SETCUSTOM) {
                            DEBUG_PRINTF("index2����%d\r\n", len);
                            //�f�[�^�������̂ŁA256�o�C�g�����ς��ɂȂ��Ă���ꍇ����BContentLength=0�܂ŏ������p������
                            //���s�ł͂Ȃ�&�ŋ�؂��Ă���̂ŁA���ꖈ�Ƀp�����[�^�ƃf�[�^���������Ă���
                            st[len] = '\0'; //�I�[��t��
                            
                            POSTdata2(st);  //POST���ꂽ�f�[�^�̏���
                            if (ContentLength == 0) {
                                PushEvent(EventFormInit);
                                StoreCustomData();  //EEPROM�ɏ�������
                                break;
                            }
                        }
                        else
                            if (ContentLength == 0) {
                            //���s���I�[���Ȃ��ŏI������P�[�X���� remove�̎��Ȃ�
                            if (len < sizeof(st)) st[len] = '\0';
                            DEBUG_PRINTF("���s�Ȃ���Content�I\r\n");
                            POSTdata(st);
                            break;  //������POST�����I��
                        }
                    }
                    else {
                        DEBUG_PRINTF("******* ERROR ********\r\n");
                        return; //�����ɂ͗��Ȃ��͂�
                    }
                }
            }
        }   //POST�̃��N�G�X�g���������郋�[�v
        if (ContentLength == 0) {
            //�}���`�p�[�g�̏������I����Ă��Ȃ����́A���̃p�P�b�g�ɔ�����
            DEBUG_PRINTF("\r\n--- FINISH POST ---\r\n");
            
            if (WHICHJOB == UPLOADCUSTOM) {
                //�J�X�^���f�[�^���A�b�v���[�h�������A���̃f�[�^���J�X�^���ϐ��ɔ��f������
                f_chdir(DataFolder);
                LoadCustomData("Data0.dat");
                StoreCustomData();
                PushEvent(EventFormInit);
            }
            // �N���C�A���g��HTML�e�L�X�g�𑗐M����
            //�������b�Z�[�W��\��
            ESP_HtmlSnd2(clientID, comp_txt1);
            sprintf(st, " ");
            if (WHICHJOB == UPLOADFONT) {
                sprintf(st, "<h1>Completed upload file</h1>");
                ESP_HtmlSnd2(clientID, st);
                sprintf(st, "�t�@�C��:%s(%d��)���A�b�v���[�h���܂���<br>", Dirname, FileCount);
            }
            else if (WHICHJOB == SETCUSTOM) {
                sprintf(st, "<h1>Completed to set New data</h1>");
            } else if (WHICHJOB == REMOVEFONT) {
                sprintf(st, "<h1>Removed Font(s)</h1>");
            } else if (WHICHJOB == UPLOADCUSTOM) {
                sprintf(st, "<h1>Uploaded Custom Data</h1>");
            }
            
            ESP_HtmlSnd2(clientID, st);
            if (ESP_HtmlSnd2(clientID, comp_txt2)) return;
            ESP_HtmlSnd2(clientID, NULL);
            
            ESP_HtmlSndEnd(clientID);
            //������CLOSE���Đؒf����ƁA�u���E�U���瑗�M�ł��Ȃ��Ȃ�
            //�u���E�U������CLOSED�M�����󂯕t�������́A�u���E�U�ő��삷��ƁACONNECT���b�Z�[�W
            //
        }
    }
}


char TextOpenTitle[] = "LCD Clock 2024 v1.3";
char *TextOpenMsg[] = {
    "PIC: 18F27K42 128KB\r\n",
    "LCD: 2.8inch 320x240dot, 16bit color+Touch\r\n",
    "RTC: RX-8025NB\r\n",
    "Temp and Humidity Sensor: DHT20\r\n",
    "WiFi: ESP WROOM-02D\r\n",
};

/*
 * waitTouch=0: �\���I����A�����I�ɏ����I��点��ꍇ
 *           1: �^�b�`�����玟�̏����Ɉڍs������
 */
void OpeningScreen(uint8_t waitTouch) {
    int8_t jj, res;
    int16_t x, y, w, h;
    //�J���[�o�[�Ɠ������A75%�̔��A���A�V�A���A�΁A�}�[���^�A�ԁA��
    uint16_t color[] = {
//        WHITE, YELLOW, CYAN, GREEN, MAGENTA, RED, BLUE
        0xBDF7, //��
        0xBDE0, //��
        0x05F7, //�V�A��
        0x05E0, //��
        0xB817, //�}�[���^
        0xB800, //��
        0x0017, //��
    };

    lcd_fill(BLACK); //��ʂ��N���A
//    SetFont(FontNormal, NULL);
    display_setTextColor(WHITE, BLACK);     //Textcolr��TextbgColor��ݒ�
    display_setTextSize(0x12);
    
    display_setCursor(0, 0);
    display_puts(TextOpenTitle);
    
    display_setTextPitchAdj(-1, 2);    //�s�b�`���f�t�H���g����ύX

    display_setCursor(0, 30);
    for (jj = 0; jj < 5; jj++) {
        display_puts(TextOpenMsg[jj]);
    }
    
    display_setTextPitchAdj(0, 0); //���ɖ߂�

//    return; //�f�o�b�O���́A�ȍ~�X�L�b�v
    
    //SD�J�[�h������΁Abmp��\������@�G���[�̎��́A���g���C�����Ȃ�
    res = ReadBmp16("Opening.bmp");
    DEBUG_PRINTF("res(opening file)=%d\r\n", res);

    //�J���[�o�[�\��
    x = 0;
    y = 220;
    w = 40;
    h = 20;
    for (jj=0; jj<7; jj++) {
        display_fillRect(x, y, w, h, color[jj]);
        x += w;
    }

    if (waitTouch) {
        //�^�b�`������A�I������Ƃ������b�Z�[�W�o��
        display_drawChars(20, 201, (char*)TouchMsg, WHITE, WHITE, 0x12);
    }
    else {
        __delay_ms(1000);
    }
}


/*
 * �Ɠx�Z���T�ŁA���邳���擾���A����ɉ����ăo�b�N���C�g��ݒ肷��
 */
void AdjustBacklight() {
    static uint16_t Brightness = 1023;
    uint16_t BackLight;
    uint16_t data;

    //ADC�̌��ʂ́A12bit��0-4095�B���邢�Ƒ傫�Ȑ��l
    //�������A�Â���ADC�̌��ʂ����Ȃ�΂��
    //���ԂȂ�3.07V@3.29V�ŁA954-958�A��̌u���������ƁA50-150�ʂ�������
    //���߂Č��Ă݂�ƁA5�b���Ƃ̑���A�u�������ŁA1000-1500�ʂ����ʂŁA���܂�800���炢�ɂȂ����肷��
    //�΂����ጸ���邽�߁A8��̈ړ����ς����
    //ADC_GetConversion�Ń`���l���ݒ�AADC�J�n-�擾�܂Ŏ��s
    data = ADCC_GetSingleConversion(PhotoTr);
    Brightness = Brightness/8*7 + data/4;
    
    // Duty��ύX���ăo�b�N���C�g�̖��邳��ύX
    // Brightness����萔�ȏ�̎��́A�o�b�N���C�g�͂قڏ펞�_���B
    BackLight = Brightness/8*3 + 10;
    // BackLight�́A�Œ�10-max999   0���ƑS����ʌ����Ȃ��Ȃ�̂ŁA+10�Ƃ��Ă���
    if (BackLight > 999) BackLight = 999;
    //PWM�́A10�r�b�g�̉𑜓x (TMR6�̎���=PR6=0xff�ɐݒ肵����)
//    DEBUG_PRINTF("Bright=%4d, BackL=%d\r\n", Brightness, BackLight);  //�P�x�̃��x����\��
    
    PWM6_LoadDutyValue(BackLight);
}

/*
 * 1�b���ƂɍX�V����鎞���𗘗p���Ď��{���鏈��
 * ����3��
 * �@�E�b�̈�ʂ̐�����9�̎�:�@�����x�Z���T
 * �@�E�b�̈�ʂ̐�����3��8�̎��F�@�Ɠx�`�F�b�N
 * �@�E����:�@���t�̍X�V�`�F�b�N
 */
void TimeEvent() {

    if ((DateTime[0] & 0x0f) == 1) {
        //�����x�́A�b�̉�1����1�̎��ɍX�V
        Get_TempHumidity(&Temp, &Humidity);
        PushEvent(EventTemperatureUpdate);
        PushEvent(EventHumidityUpdate);
    }   

    if (((DateTime[0] & 0x0f) % 5) == 3) {
        //�b�̉�1����3,8�̎��ɏƓx�`�F�b�N
        //�Ɠx�Z���T�ŁA���邳��5�b���ƂɎ擾����
        AdjustBacklight();
    }
    
    //���t���ς�������́A���Ƀg���K�Ȃ��̂ŁA�����Ń`�F�b�N�B����00���̎��������{
    if (DateTime[1] == 0x00) {
        if (WriteYMD()) { //�X�V����΁A���t���X�V����
            PushEvent(EventDateUpdate);
            //���t���ς������ASNTP��M������
            PushEvent(EventGetTime);
        }
    }
}

/*
 * �X���C�hSW�I�t�̏������[�`��
 */
void SlideSWoffProc() {
   
    SlideSWStatus = SlideSWoff;
    AlarmSoundOff();
    RTC_resetAlarm();
    AlarmStatus = 0;    //�X���C�hSW��Off�������́Asnooze�I��
    SnoozeCount = 0;
    DrawAlarmTime(AlarmTime, SlideSWStatus, &CurrentRsc[AlarmObj]);    //�A���[���\���̍X�V
}

void WriteAlarm() {
    uint8_t jj, tmp;
    uint16_t addr;

    //EEPROM�ɃA���[���������������݁A�d���؂�Ă��ē������A�g����悤��
    addr = AddressAlarm;
    for (jj = 0; jj < 3; jj++) {
        tmp = DATAEE_ReadByte(addr);
        if (tmp != AlarmTime[jj]) {
            //�ω�������f�[�^�̂ݏ�������
            DATAEE_WriteByte(addr, AlarmTime[jj]);
        }
        addr++;
    }
}

/*
 * �X���C�hSW�I���̏������[�`��
 * �A���[���Z�b�g���āA���̎�����EEPROM�ɏ�������
 */
void SlideSWonProc() {

    RTC_setAlarmTime(AlarmTime);
    WriteAlarm();
    SlideSWStatus = SlideSWon;
    DrawAlarmTime(AlarmTime, SlideSWStatus, &CurrentRsc[AlarmObj]);    //�A���[���\���̍X�V
}

/*
 * RotCount��0�łȂ����́A��������
 * ���[�^���[�́A�A���I�ɃC�x���g�������邪�A�C�x���g�������ɏ����ł��Ȃ��Ă��悭�A
 * RotCount=0�łȂ���������������Ηǂ�
 */
void RotaryProcedure() {
    int16_t delta;  //8bit���ƕs�����邱�Ƃ�z��
    uint8_t mm, hh;

    //�m�[�}�����̃��[�^���[SW�̓A���[��������ύX
    if (RotCount == 0) return;

    delta = RotCount;
    RotCount = 0;
    
    mm = Bcd2Hex(AlarmTime[0]);
    if (CurrentRsc[TimeObj].format != ANALOGCLOCK) {
        //�f�W�^�����v�̎���1���P��
        //������]��������A�{���A4�{���Ł@�@�������Adelta�́Aint8�Ƃ������Ƃɒ���
        if ((delta > 2) || (delta < -2)) {
            //8�{�����܂�
            if (Accel < 8) Accel = Accel * 2;
            delta = delta * Accel;
        }
        else Accel = 1;
    } else {
        //�A�i���O���v�̏ꍇ
        delta = delta * 5;  //5���P�ʂ�
        mm = mm - (mm % 5);    //5���P�ʂɐ؂艺��
    }
    
    hh = Bcd2Hex(AlarmTime[1]);
    IncDecTime(delta, &hh, &mm);
    AlarmTime[0] = Hex2Bcd(mm);
    AlarmTime[1] = Hex2Bcd(hh);
    
    //�A���[��SW��on��������ARTC�̃A���[��������ύX
    if (SlideSWStatus == SlideSWon) RTC_setAlarmTime(AlarmTime);
    WriteALM2EEPROM = 300;  //300s���EEPROM�ɏ�������
    
    //EEPROM�ɏ������ރ^�C�~���O�����B���[�^���[�����邮��񂵂Ă��鎞�́A
    //���x��EEPROM�ɃA���[���������������݂����Ȃ�
    //�A���[��SW��On�ɂ����������Ɍ��肷��
}


/*
 * ���C����ʂ̕\�����X�V����
 * CurrentRsc�̏������ɏ��߂ĕ`�悷�鎞�ɌĂяo�����
 * �ݒ�{�^�����́A�����ŕ`��
 */
void DrawForm() {
    
    //�����\���͕ω����������������\���X�V����̂ŁABCD�ł͂��肦�Ȃ����l��ݒ肵�Ă���
//    resetPreDateTime();
    lcd_fill(TextbgColor);

    //�ݒ�{�^���@�@Web�A�N�Z�X�ŃJ�X�^�}�C�Y�ł���̂ŁA�ݒ�{�^���͕K�{�ł͂Ȃ��Ȃ����̂�
    DrawGearIcon(&CurrentRsc[GearObj]);
    DEBUG_PRINTF("Gear w=%d,%d\r\n", CurrentRsc[GearObj].xw, CurrentRsc[GearObj].yw);

    //�����\���X�V
    resetPreDateTime();
    DrawTime(DateTime, &CurrentRsc[TimeObj]);
    DEBUG_PRINTF("Time w=%d,%d\r\n", CurrentRsc[TimeObj].xw, CurrentRsc[TimeObj].yw);

    //���t�̍X�V
    DrawDate(DateTime, &CurrentRsc[DateObj]);    //���t��`��
    DEBUG_PRINTF("Date w=%d,%d\r\n", CurrentRsc[DateObj].xw, CurrentRsc[DateObj].yw);
    DrawCalendar(&CurrentRsc[CalendarObj]);  //���t�X�V������J�����_�[���X�V
    DEBUG_PRINTF("Calendar w=%d,%d\r\n", CurrentRsc[CalendarObj].xw, CurrentRsc[CalendarObj].yw);

    //�����x���X�V���ꂽ��
    DrawTemperature(Temp, &CurrentRsc[TempObj]);
    DEBUG_PRINTF("Temp w=%d,%d\r\n", CurrentRsc[TempObj].xw, CurrentRsc[TempObj].yw);
    DrawHumidity(Humidity, &CurrentRsc[HumidityObj]);
    DEBUG_PRINTF("Humid w=%d,%d\r\n", CurrentRsc[HumidityObj].xw, CurrentRsc[HumidityObj].yw);

    //�A���[���\���̍X�V�@�@�X���C�hSW�̏�ԂŐF��ς���
    DrawAlarmTime(AlarmTime, SlideSWStatus, &CurrentRsc[AlarmObj]);
    DEBUG_PRINTF("Alm w=%d,%d\r\n", CurrentRsc[AlarmObj].xw, CurrentRsc[AlarmObj].yw);

}


/*
 * ���C���̃C�x���g�n���h��
 * 
 */
void MainEventHandler() {
    uint8_t evt;
    char str[100];

    evt = PopEvent();
    if (evt == EventNone) {
        __delay_ms(10); //�C�x���g�Ȃ����́A���ʂɋ��肳���Ȃ��悤wait
        return;
    }
#ifdef DEBUG
//    DEBUG_PRINTF("e=%03d\r\n", evt);
#endif
    
    switch (evt) {
        case EventTimeUpdate:
            RTC_read(DateTime);
            resetCTFG();    //RTC��1�b���Ƃ̊����݌�A�����K�v
            DrawTime(DateTime, &CurrentRsc[TimeObj]);    //�����\���X�V
            TimeEvent();
            break;

        case EventDateUpdate:
            DrawDate(DateTime, &CurrentRsc[DateObj]);    //���t��`��
            DrawCalendar(&CurrentRsc[CalendarObj]);  //���t�X�V������J�����_�[���X�V
            break;

        case EventFormInit:
            resetCTFG();    //RTC�̊����݉���
            //���\�[�X�����������āA��ʕ`�悷��
            CurrentRsc = RscData[DisplayMode]; //���[�h�ɑΉ��������\�[�X��ݒ�
            DEBUG_PRINTF("DisplayMode=%d\r\n", DisplayMode);
            DrawForm();
            PixelXY[0] = 9999;  //�f�[�^���Ȃ����Ƃ�����
            break;

        case EventSlideSWoff:       //�X���C�hSW��Off����
            SlideSWoffProc();
            break;
        case EventSlideSWon:        //�X���C�hSW��On����
            SlideSWonProc();
            break;
        case EventRotary:
            RotaryProcedure();
            DrawAlarmTime(AlarmTime, SlideSWStatus, &CurrentRsc[AlarmObj]);    //�A���[�����\���̍X�V
            //�A�i���O���v�̏ꍇ�́ADrawTime���Ăяo���Ȃ��ƃA���[���j�̍X�V������Ȃ����A1�b���ɍX�V�����̂ŁA���ɖ��͂Ȃ�
            //�A�i���O���v�S�̂��ĕ\������̂ŁA�����ōX�V���Ă�����قǃ��X�|���X�悭�݂��Ȃ�
            break;
            
        case EventTouchDown:
            TouchProcedure();
            break;
//        case EventTouchLong:
//            TouchLongProcedure();
//            break;                    
//        case EventTouchUp:
//            break;
            
        case EventTemperatureUpdate:
            DrawTemperature(Temp, &CurrentRsc[TempObj]);
            break;
        case EventHumidityUpdate:
            DrawHumidity(Humidity, &CurrentRsc[HumidityObj]);
            break;
        case EventGetTime:
            GetTimeFromNTP(DateTime);       //NTP�T�[�o���猻�������擾
            break;
    }    
}


/*
 * �����f�[�^��ݒ肷��
 * ���߂ċN���������́A�S�������A2��ڈȍ~��EEPROM����f�[�^�擾
 * 
 */
void SetInitialData()
{
    uint8_t jj, kk, dd;
    uint16_t addr;
    char *p;

    //���߂ċN���������́A�S�������A2��ڈȍ~��EEPROM����f�[�^�擾
    if (DATAEE_ReadByte(AddressInit) == 0xff) {
        TouchAdjust();    //�^�b�`�̒��������{���A���̃f�[�^��EEPROM�ɕێ�
      
        lcd_fill(BLUE); //�����ŉ�ʂ��N���A���Ȃ��ƁA�^�b�`�����������̂��킩��Ȃ���������

        DATAEE_WriteByte(AddressInit, 0x55);
        //�A���[��������������
        addr = AddressAlarm;
        for (jj = 0; jj < 3; jj++) {
            DATAEE_WriteByte(addr, AlarmTime[jj]);
            addr++;
        }
        DisplayMode = DisplayMode0; //�ŏ��̕\�����[�h�́A�O���f�[�^�s�v��0��
        DATAEE_WriteByte(AddressDisplayMode, DisplayMode);
        //�J�X�^���f�[�^�̏�����
        StoreCustomData();  //EEPROM�ɂ���������2��ڈȍ~�̋N�����ɓǂݏo���f�[�^������悤�ɂ���

    } else {
        //2��ڈȍ~�̓d�������Ȃ�A�^�b�`���W�␳�̃f�[�^��ǂݏo��
        for (jj = 0; jj < 4; jj++) TouchXY[jj] = DATAEE_ReadWord(AddressTouch + jj*2);
        
        //���t�A�A���[���ݒ��ǂݏo��
        addr = AddressYMD;
        for (jj=0; jj<4; jj++) {
            dd = DATAEE_ReadByte(addr++);
            if (dd != 0xff) DateTime[jj+3] = dd;    //�K�؂ȏ������݂�����Ă��Ȃ������ꍇ�����
        }
        addr = AddressAlarm;
        for (jj=0; jj<3; jj++) AlarmTime[jj] = DATAEE_ReadByte(addr++);
        //�O��I�����̕\�����[�h�ɂ���
        DisplayMode = DATAEE_ReadByte(AddressDisplayMode);
        
        //�J�X�^���ݒ��ǂݏo��
        for (jj=0; jj < OBJECTNUM; jj++) {
            addr = AddressCustom + (uint16_t)jj*32;
            CustomRsc[jj].disp = DATAEE_ReadByte(addr++);
            CustomRsc[jj].x = (int16_t)DATAEE_ReadWord(addr);
            addr += 2;
            CustomRsc[jj].y = (int16_t)DATAEE_ReadWord(addr);
            addr += 2;
            CustomRsc[jj].xw = (int16_t)DATAEE_ReadWord(addr);
            addr += 2;
            CustomRsc[jj].yw = (int16_t)DATAEE_ReadWord(addr);
            addr += 2;

            CustomRsc[jj].fontcode = DATAEE_ReadByte(addr++);
            CustomRsc[jj].fontmag = DATAEE_ReadByte(addr++);

            CustomRsc[jj].color = DATAEE_ReadWord(addr);
            addr += 2;
            CustomRsc[jj].format = DATAEE_ReadWord(addr);
            addr += 2;
            CustomRsc[jj].attribute = DATAEE_ReadWord(addr);
            addr += 2;
            CustomRsc[jj].attribute2 = DATAEE_ReadWord(addr);
            addr += 2;

            CustomRsc[jj].xpitchAdj = (int8_t)DATAEE_ReadByte(addr++);
        }
        
        for (jj = 0; jj < NumberOfUserFont; jj++) {
            p = CustomFontDir[jj];
            addr = AddressFontName + (uint16_t)jj * 9;
            while (1) {
                *p = DATAEE_ReadByte(addr++);
                DEBUG_PRINTF("%c", *p);
                if (*p++ == '\0') break;
            };
            DEBUG_PRINTF("  %d\r\n", jj);
        }
    }
}

/*
                         Main application
 */
void main(void)
{
    int8_t ans;
    char st[30];

    // Initialize the device
    SYSTEM_Initialize();

    // If using interrupts in PIC18 High/Low Priority Mode you need to enable the Global High and Low Interrupts
    // If using interrupts in PIC Mid-Range Compatibility Mode you need to enable the Global Interrupts
    // Use the following macros to:

    // Enable the Global Interrupts
    INTERRUPT_GlobalInterruptEnable();

    // Disable the Global Interrupts
    //INTERRUPT_GlobalInterruptDisable();

    DEBUG_PRINTF("\r\n[[[[Program Start]]]]\r\n");

    //Touch/SlideSW�́ATMR5��10ms���荞�݂ŏ�ԃ`�F�b�N
    TMR5_SetInterruptHandler(Timer5Handler);

    // BackLight���Ɠx�Z���T�̌��ʂ����ɓ_��
    AdjustBacklight();
    
    //LCD�̏�����
    lcd_init();
    lcd_fill(BLUE); //��ʂ��N���A

    SetFont(FontNormal, NULL);  //�����t�H���g�ݒ�

    // RTC�̏�����
    init_RTC(DateTime);
    //�A���[���̏�����
    TMR0_SetInterruptHandler(SoundHandler); //�A���[������TMR0�̏����ݒ�    
    //�N����
    AlarmSoundOn(AlarmTwice);

    //���߂ċN���������́A�^�b�`�����A�S�������A2��ڈȍ~��EEPROM����f�[�^�擾
    SetInitialData();

    display_setTextSize(0x12);
    display_setCursor(10, 50);
    display_setTextColor(WHITE, WHITE); //Textcolr��TextbgColor��ݒ�
    display_puts("Initializing");   //Touch������Initializing�̕����������Ă��܂�

    //ESP�̏�����
    IO15_TRIS = 1;  //�d��������Low��������ꂽ��A���̌シ�݂₩��Hi-Z�ɂ���

    display_setWrap(true);
    ans = ESP_Init();
    DEBUG_PRINTF("ESP Init ans = %d\r\n", ans);
    display_setWrap(false);
    
    //�I�[�v�j���O��ʂ�\��
    OpeningScreen(0);

    GetTimeFromNTP(DateTime);       //NTP�T�[�o���猻�������擾
    INT0_SetInterruptHandler(RTC_handler);  //RTC_INTA (1s��)�̊��荞�ݏ���
    IOCBF4_SetInterruptHandler(AlarmWHandler); //�A���[�� Alarm W�̊��荞��
    resetCTFG();
    
    //�T�[�o���[�h�Ɉڍs
    DEBUG_PRINTF("Server Startup\r\n");
    // WiFi���W���[����ESP-WROOM02��AP�Ƃ��T�[�o�[���[�h�ŋN��������
    ans = ESP_ServerInit();
    if (ans == 0) {
        DEBUG_PRINTF("Start-up Done! \r\n");
    } else {
        DEBUG_PRINTF("RUN is suspended. Error:ans=%d\r\n", ans);
        while(1) ;       // �������f�@�@�ēx�g���C�����Ă݂Ă��ǂ�����
    }

    // ESP-WROOM02���g��IP�A�h���X��\������@����IP��PC���̃u���E�U����A�N�Z�X����
    ESP_GetIPaddress(st);
    DEBUG_PRINTF("ESP IP=%s\r\n", st);

    // ���[�^���[�pTMR1�̊��荞�ݐݒ�
    TMR1_SetInterruptHandler(RotaryHandler);

    DEBUG_PRINTF("Finish initialization\r\n");

    //��Ԃ�����������

    DEBUG_PRINTF("Initial folder status\r\n");
    f_chdir("/");
    GetFontFolder("/");    //���[�g�̑S�t�H���_�����o��
    
    DEBUG_PRINTF("OK to access from browser\r\n");

    // �d����������̍ŏ��̃C�x���g�́A�t�H�[���̏�����
    ClearEventBuffer();     //�C�x���g�o�b�t�@�̏�����
    SetEventHandler(MainEventHandler);
    PushEvent(EventFormInit);
    
    while (1)
    {
        // Add your application code
        PacketProcess();
        
        EventHandler();
        //�����݂̃C�x���g����
        if (TouchStart & 0x01) {
            if (GetTouchLocation(&TouchRawX, &TouchRawY) == 0) {
                //�����Ń^�b�`���W���擾���āA�C�x���g����
                TransCoordination(TouchRawX, TouchRawY, &TouchX, &TouchY);
                DEBUG_PRINTF("%d, %d\r\n", TouchX, TouchY);
                PushEvent(EventTouchDown);
                //�^�b�`������s�b�Ƃ��������o��
                AlarmSoundOn(AlarmSingle);
            }
            TouchStart &= 0xfe;
        } else if (TouchStart & 0x02) {
            //������
            if (GetTouchLocation(&TouchRawX, &TouchRawY) == 0) { //�����ł����W�擾
                TransCoordination(TouchRawX, TouchRawY, &TouchX, &TouchY);
                PushEvent(EventTouchLong);
            }
            TouchStart &= 0xfd;
        } else if (TouchStart & 0x04) {
            //���s�[�g���L�����A300ms���Ƀ^�b�`�_�E���C�x���g�𔭐�������
            if (RepeatKey > 30) {
                if (GetTouchLocation(&TouchRawX, &TouchRawY) == 0) { //�����ł����W�擾
                    TransCoordination(TouchRawX, TouchRawY, &TouchX, &TouchY);
                    //���s�[�g���L�����A300ms���Ƀ��s�[�g�C�x���g�𔭐�������
                    PushEvent(EventTouchRepeat);
                    RepeatKey = 1;  //0�ł̓��s�[�gdisable�ɂȂ�̂ŁA1�Ƀ��Z�b�g
                }
            }
            TouchStart &= 0xfb;
        } else if (TouchStart & 0x08) {
            //�^�b�`�A�b�v�@�@���W�̓^�b�`�_�E�����̏����Ŏ擾
            PushEvent(EventTouchUp);
            RepeatKey = 0;  //���s�[�g����
            TouchStart = 0;
        }
 
        //���[�^���[�̃C�x���g�͑������ɂȂ�̂ŁARotCount�ɒl���c���Ă�����C�x���g����
        if (RotCount != 0) {
            PushEvent(EventRotary);
        }

        //�X���C�hSW
        if (SlideSWStatus == SlideSWon2off) {
            PushEvent(EventSlideSWoff);
        } else if (SlideSWStatus == SlideSWoff2on) {
            PushEvent(EventSlideSWon);
        }

        //�����̍X�V
        if (NeedUpdateTime) {
            PushEvent(EventTimeUpdate);
            NeedUpdateTime = 0;      
            if (WriteALM2EEPROM > 0) {
                WriteALM2EEPROM--;
                if (WriteALM2EEPROM == 0) WriteAlarm();
            }
        }
        
        if (AlarmStatus == 1) {
            AlarmSoundOn(AlarmSound0);    //�A���[���̍ŏ��̉���炷
            AlarmStatus++;
            DEBUG_PRINTF("Alarm on\r\n");
        }

    }
}
/**
 End of File
*/