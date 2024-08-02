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
 * ESPを使い、WiFiでNTPで時刻合わせを実施 & フォントデータのアップロード対応
 * SNTPサーバから、ESP-WROOM-02DのATコマンドを使って日時取得
 * 
 * UART1: TX1=RC6, RX1(PGD)=RB7  WiFiユニットと通信
 * UART2: TX2(PGC)=RB6, RX2=RB7  デバッグ出力　FT232をRLを介してUSBでPCに接続
 * 　　　　　　　　　　　RX2はdisableにできないので、RX1と同じピンに割り振っている
 * GPIO: RTS1=RB5
 * GPIO: IO15=RC7
 * 
 * TMR3: 4ms　　デバッグのため、1秒ごとのタイマ割込みとして設定
 * UART通信は、どちらも115200kbps、非同期8bit、UART1のみ割込み使用
 * 
 * 回路構成
 * ESP-WROOM-02D
 *            ________________
 *      VINT |3V3         GND | GND
 *      VINT |EN          IO16|
 *           |IO14        TOUT|
 *           |IO12        RST | VINT固定
 * RB5|RTS1  |IO13|CTS    IO5 |
 * RC7|IO15  |IO15|RTS    TXD | -- UART1 RX1|RB7
 *           |IO2         RXD | -- UART1 TX1|RC6
 *           |IO0         IO4 |
 *           -----------------
 * IO15=RC7 電源投入時=Low後、Hi-Zにしてフロー制御
 * Flash boot: IO15/0/2=L/H/Hで電源On
 * 　　IO0=floating, IO2内部でpull-upされているので、オープンで問題なし
 * EN, RST: 共にVINT直結にした。
 * 
 * PIC 18F27K42  64MHz, 128KB
 *             MCLR  |MCLR   RB7|← PGD USRT1 RX1(RX2) from TXD of ESP
 *     LCD_CS      ←|RA0    RB6|→ PGC UART2 TX2 to USB serial (Debug)
 *     LCD_RESET   ←|RA1    RB5|→ RTS1 for CTS(IO13)@ESP
 *     LCD_DCRS    ←|RA2    RB4|← RTC_INTB   w/WPU  IOC negative
 *     PhotoD ANA3 →|RA3    RB3|→ CCP2 for piezo speaker
 *     T_IRQ       →|RA4    RB2|⇔ I2C2 SDA2 for RTC/DHT20
 *     T_CS        ←|RA5    RB1|⇔ I2C2 SCL2
 *                   |VSS    RB0|← EXT_INT INT0 RTC_INTA   w/WPU
 *     ALMSW       →|RA7    VDD|
 *     SDCard_CS   ←|RA6    VSS|
 *     ROT_B       →|RC0    RC7|→ IO15  Low -> Hi-Z
 *     ROT_A       →|RC1    RC6|→ UART1 TX1 to RXD of ESP
 *     PWM6        ←|RC2    RC5|→ SPI1 SDO1 to MOSI
 *     SPI1 SCK1   ←|RC3    RC4|← SPI1 SDI1 from MISO
 * 
 * PIC 18F27K42
 *   SPI1:  SCK1=RC3, SDI1=RC4, SDO1=RC5          ===> LCD(16M)/SDcard(8M)/Touch(2M)
 *          CS=RA6
 *   UART1: RX1=RB7, TX1=RC6                      ===> ESP-WROOM-02D
 *          RTS1=RB5, IO15=RC7
 * 　　　ESPと非同期通信115Kbpsで送受信
 * 　　　PICからRTS信号を発信して、バッファオーバーフローにならないように制御
 *   UART2: TX2=RB6 (RX2=RB7 RX1と同じピンに定義しているが未使用なのでOK) ===> USB-Serial for Debug
 *   　　TX2を使ってデバッグ用のデータ送信(PICkit2と共通でも問題なし)
 * 　　　UART2から送信してPC TeraTerm上でチェックした方が情報も多く便利
 * 　　　TeraTermのスピード設定に注意
 *   PWM6: LCDのバックライト
 * 　　　
 * 2024/5/24　主要機能を取り込んだ：LCDclockWiFi2024-13  size=110,669 bytes
 * 　　LCDディスプレイ、SDカード、ESPに加えて
 * 　　RTC、温湿度センサ、タッチ、ロータリーエンコーダ、アラームSW、サウンド、照度センサ
 * 　　RTC:　　1秒割込み発生、アラーム発生
 * 　　DHT20:　温湿度測定
 * 　　Touch:　
 * 　　ロータリー:　
 * 　　ALMSW:　On/Off　チャタリング防止
 * 　　PhotoTransistor: 
 * Ver.13
 * 　　とりあえず最適化完了
 * Ver.14
 * 　　メインリソースの最適化に着手
 * Ver.15
 * 　　サーバ関係の処理を再構築した
 * Ver.16
 * 　　設定関係を整理させたい　　Webサイト関連の更新
 * Ver.17
 * 　　データ構造、特にカスタムフォント対応の部分を改良するため、大幅修正
 * 　　フォントコードは、内蔵0,1,2として、外部フォントは、コードを10=EXTFONTCODEにして、
 * 　　フォント名を別変数に格納する方式に。
 * 　　各オブジェクトにフォント名をさすポインタを追加
 * Ver.19
 * 　　Ver.17はなぜか突然コンパイルできなくなり復旧不可状態に。
 * 　　仕方がないので、Ver.18にコピーするも、ダメで、0から構築したVer.19を作成
 * 　　XC8の再インストール、MPLABXの再インストールをした。.mc3は、hash以外の行をコピペすれば
 * 　　流用できることが判明。これでゼロベースでもMCCの設定をコピー可能となった
 * 　　ソース自体は、Ver.17をベースにしてある
 * 　　ギアもフォント化できたので、カスタムフォントを使って変更も可能
 * 　　ほぼ完成域に来ている
 * 　　ギアを押してIPアドレスを表示させるようにしたら、一応の完成
 * 　　ブラウザからのアクセスの処理にまだ不安が残る。プログラムもきれいになっていない
 * 　・ESPへのデータ通信のバグを修正
 * 　・アラーム時刻は従来スライドSWをOnにした時にEEPROMに書き込んでいた。ロータリーSWで
 * 　　バンバン変化するごとに書き込みたくなかったからだが、SWを動かさずに電源を切ると
 * 　　その設定は廃棄されてしまっていた。そこで、変更してから5分後に書き込むようにしてみた。
 * 　　すぐに電源切ると忘れるが、5分も立てば書き込まれるので、それで十分実用的だと考えている。
 * 　・ギアを押すと、IPアドレスを表示してブラウザからのアクセス方法が分かるようにした。
 * 　・ギアの表示もカスタムフォントで変更できるようにした
 * 　・アラームのアイコンも同様
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

// 絶対値を取得する関数マクロ定義
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


/*** 送信するHTMLのテキスト ***/
/*
 * accept=\"image/\" をinput type="file"に追加すると、アップロードするファイルを限定できる
 * が、フォントフォルダに、コメントを書いたテキストファイルを入れているので、全部を対象にする。
 * webkitrectory: まだ標準化されていないが、個人用途で、常用しているChromeで使えるのと、
 * 　　　　　　　フォルダ丸ごとアップロードしたいという今回の用途には、操作が簡単になるので、これを使う。
 * multipleという属性もあるが、これは複数のファイルを選択しないといけないのと、
 * 　　格納するフォルダ名を指定しないといけない面倒さがあるので、これは使わないことにした
 */

//このページを表示したら、0.5s後に前のページに戻す
const char comp_txt1[] = "HTTP/1.0 200 OK\r\n\r\n" \
"<html><head><title>PIC Clock</title><meta charset='Shift_JIS'></head>" \
"<body onLoad=setTimeout(\"location.href='javascript:history.back()'\",2000)>";
//アップロード完了時のメッセージをここにはさむ
const char comp_txt2[] = \
"2秒後、自動的に前のページに戻ります" \
"</form>" \
"</body>" \
"</html>";
//"<br><input type='button' onclick=\"history.back()\" value='BACK'>"

//ホームページ：テーブル表示
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
"        content: '▼';" \
"        margin-left: 10px;" \
"        font-size: 16px;" \
"      }" \
"      &.asc::after {" \
"        content: '▲';" \
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
"<caption>SDカードに格納されているフォント</caption>" \
"  <thead>" \
"    <tr>" \
"      <th width='20'></th>" \
"      <th class='sort' data-sort='fname'>フォント名</th>" \
"      <th class='sort' data-sort='date'>更新日時</th>" \
"    </tr>" \
"  </thead>" \
"  <tbody class='list'>" \
;
//ここにフォルダリストを生成する
const char html_txt2end[] = \
"  </tbody>" \
"</table>" \
"　上記フォントで削除したいものを選択後、Removeボタンを押して下さい<br>" \
"    <button type='submit' id='RemoveButton' disabled>Remove</button>" \
"</form>" \
"<hr>アップロードしたいフォントフォルダを選択して下さい<br>" \
"<form method='post' accept-charset='Shift_JIS' enctype='multipart/form-data'>" \
" <input type='file' id='fileElem' name='fontfile' webkitdirectory onchange='if (this.files.length > 0) this.form.submit();'>" \
" <button id='fileSelect' type='button'>フォントフォルダを選択</button><br>" \
"</form>" \
"<small>注：8文字を超えるファイル名のファイルはアップロードされません<br>" \
"　フォルダ名、ファイル名は8.3形式でアップロードされます。英字はすべて大文字になります</small>" \
"<hr>アップロードしたいカスタム用データファイルを選択して下さい。拡張子はdatであること<br>" \
"<form method='post' accept-charset='Shift_JIS' enctype='multipart/form-data'>" \
"      <input type='file' name='datafile' accept='.dat' required>" \
"      <input type='submit' value='送信'>" \
"</form>" \
"現在のカスタムデータ<tt><big><pre>" \
"Item : disp,  x,   y,  font#, fsize, color, format,  attr1,  attr2, xpitchAdj, FontName,<br>";
//"Item : disp,  x,  y, xw, yw,font#,fsize, color,format, attr1,  attr2, xpitchAdj, Font,<br>";
//ここにカスタム情報を表示
const char html_txt3end[] = \
"</pre></big></tt>" \
"<small>7つのオブジェクトの状態をカスタマイズできる。パラメータの意味は以下の通り<br>disp: 0:非表\示, 1:表\示, 99:表\示(固定)、<br>" \
"x,y: 表\示オブジェクトの左上の座標 (0,0)-(319,239)<br>" \
"font#: フォント番号 (1=8x8 Normal, 2=6x8 small, 3=5x7 7セグ様, 10=外部フォント)、fsize: フォントサイズ(16進2桁でXYの倍率　0x11がデフォルトサイズ)、color:表\示色(16bit color=RGB565)<br>" \
"format: <br>" \
"  Date: 0=YYYY/MM/DD(w)(曜日), 1=YYYY/MM/DD(we)(英曜日), 2=YYYY/MM/DD, 3=YY/MM/DD(w), 4=YY/MM/DD(we), 5=YY/MM/DD, 6=MM/DD(w), 7=MM/DD(we), 8=MM/DD<br>" \
"  Time: 0=HH:MM, 1=HH:MMss, 2=HH:MM:SS, 3=Analog<br>" \
"  Alarm: 0=ALMAMPM(AM/PM表\記), 1=ALM24(24時間表\記), 2=iALAMPM(アラームアイコン), 3=iAL24(アラームアイコン)<br>" \
"  Temp: 0=DegC, 1=DegF<br>" \
"  Humid, Gear: フォーマット指定なし<br>" \
"  Calendar: 0=1month, 1=2month(横並び), 2=2v month(縦並び), 3=3month, 4=3v month<br>" \
"　　attr1: カレンダの土曜の色<br>" \
"　　attr2: カレンダの日曜の色<br>" \
"xpitchAdj: -2 ? 8　デフォルトのフォントピッチを微調整<br>" \
"Font: カスタムフォントのフォルダ名<br>" \
"※アナログ時計表\示の時は、xwが半径を示す<br>" \
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

//カスタム画面設定用データ
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
"  <h2>画面のカスタム設定</h2>" \
"  <p>dispでチェックした部品を下記枠内で移動して位置を決めて下さい</p>" \
"  <div class='area'>" \
"    <div class='dragd' id='box0'>Date</div>" \
"    <div class='dragd' id='box1'>Time</div>" \
"    <div class='dragd' id='box2'>Alarm</div>" \
"    <div class='dragd' id='box3'>Temp</div>" \
"    <div class='dragd' id='box4'>Humid</div>" \
"    <div class='dragd' id='box5'>Calendar</div>" \
"    <div class='dragd' id='box6'>Gear</div>" \
"  </div>" \
"  <button type='button' onclick='window.location.reload();' style='margin-left:350px;'>リセット</button>" \
"</section>" \
"<br><form method='post'>" \
"  <table border='1' style='border-collapse: collapse'>" \
"    <caption>部品情報</caption>" \
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
"	<td><input type='text' id='fsize' name='fsize' placeholder='XY倍率'></td>" \
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
"	<td><input type='text' id='fsize' name='fsize' placeholder='XY倍率'></td>" \
"	<td><input type='color' name='color'></td>" \
"	<td><select name='format'>" \
"	    <option value='0'>HH:MM</option>" \
"	    <option value='1'>HH:MMss</option>" \
"	    <option value='2'>HH:MM:SS</option>" \
"	    <option value='3'>Analog</option>" \
"	</select></td>" \
"	<td><input type='text' id='attr1' name='attr1' placeholder='アナ直径'></td>" \
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
"	<td><input type='text' id='fsize' name='fsize' placeholder='XY倍率'></td>" \
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
"	<td><input type='text' id='fsize' name='fsize' placeholder='XY倍率'></td>" \
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
"	<td><input type='text' id='fsize' name='fsize' placeholder='XY倍率'></td>" \
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
"	<td><input type='text' id='fsize' name='fsize' placeholder='XY倍率'></td>" \
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


// 受信データの処理ステータス
#define SNONE       0
#define SGET        12
#define SPOST       13

#define PROCESSBUF_SIZE 256

//境界区切子は、70文字以下らしい
/*
 * 一番最初の本体部分の前には、 --boundary と CRLF からなる境界文字列を入れます
 * 本体部分と本体部分の間には、 CRLF と --boundary と CRLF からなる境界文字列を入れます
 * 一番最後の本体部分の後には、 CRLF と --boundary-- と CRLF からなる境界文字列を入れます
 * また境界文字列は、その中身と一致することのないようにという制約もついているので、純粋に
 * 境界文字列と一致したかを見ればよいだけ
 */
#define BOUNDARYSTRLENGTH   80
char BoundaryStr[BOUNDARYSTRLENGTH] = "\0";    // 前の改行と--、最後に--を追加して、中間/最終共用
uint16_t BoundaryStrLen;   //境界文字列の長さ BoundaryStrの長さ


#define TXT 0
#define BIN 1

uint8_t BinOrText;  //BIN=1, TXT=0
//uint16_t PacketLength = 0;    //現在のパケットの読み出すべきデータ数の残り
uint16_t ContentLength;     // Content-Lengthに示されている長さ
uint8_t StartContent;   //Content部が開始したら1

uint8_t InBoundary;    //境界文字列の範囲を見ている(1)かどうか
FIL BinFile;    //

uint8_t FileCount;
char Dirname[10];   //フォントディレクトリの名前
char Filename[50];  //バイナリファイルの名前
#define MAXLISTITEM     30      //設定画面では最大28個までに制限していた
char FontNameList[MAXLISTITEM][9];   //フォント名のリスト
WORD FontListDate[MAXLISTITEM];			/* Modified date */
WORD FontListTime[MAXLISTITEM];			/* Modified time */

uint8_t FontNum;

uint8_t RepeatKey = 0;  //リピートさせるかどうか

//表示モード
uint8_t DisplayMode;    // どの画面表示モードか

// 並びは、RTCと合わせて、秒、分、時、曜日、日、月、年　　BCDフォーマット
uint8_t DateTime[7] = {0x00, 0x00, 0x00, 0x00, 0x01, 0x01, 0x22};
int8_t TZ = +9; //現状タイムゾーンは、JSTに固定

//アラーム時刻、mm,hh, アラームが有効な曜日　RTCと同じ順序
uint8_t AlarmTime[3] = {0, 0, 0x7f};    //mm, hh, wday
uint8_t TmpTime[7]; //設定途中の時刻を保持

// 0: アラームoff、1: アラーム割込み発生、2: アラーム鳴動中
uint8_t AlarmStatus = 0;
uint8_t SnoozeCount = 0;    //スヌーズ何回目か
int16_t WriteALM2EEPROM;   //アラーム時刻をEERPOMに書き込むまでの時間(分)　ロータリーぐるぐる回している時は書き込まないように

//温湿度センサの値を保持　実際の値の10倍にして整数化
int16_t Temp, Humidity;

//タッチ処理用
uint16_t TouchRawX, TouchRawY;    //タッチした時のX,Yの生座標
int16_t TouchX, TouchY;    //ディプレイ座標に換算したタッチ座標
//座標補正用に(20,20)と(300,220)のタッチ上の座標を保持
uint16_t TouchXY[4];    //補正用　取り扱いやすいように配列に変更。順序はx1,y1, x2,y2
uint8_t TouchCount;     //長押し判定用に使用。タッチ直後に0になる
uint16_t PixelData[9];  //3x3のピクセルデータを保存する
int16_t PixelXY[2];     //取り込んだデータの左上の座標
uint8_t TouchStart;     //タッチ開始フラグ    bit3:Up, 2:repeat, 1:long, 0:down

//スライドSW
uint8_t SlideSWStatus = 0x0f;   //初期値はどの状態でもない値を設定しておく

//ロータリーSW処理用
int8_t RotCount = 0; //回した回数(クリックありのロータリーだと、何回クリックさせたか)
int8_t Accel = 1;   //高速回転させた時に、1クリックをAccel倍に加速させる

uint8_t NeedUpdateTime =  0; //割込み中にEvent発生できないので、これをフラグとして使用

//カスタムデータファイルを格納するフォルダ
char *DataFolder = "Data";
uint8_t WHICHJOB;     //どんなジョブを実行しているか
#define SETCUSTOM       1   //カスタムデータの時
#define REMOVEFONT      2   //フォント削除
#define UPLOADFONT      3   //フォントのアップロード
#define UPLOADCUSTOM    4   //カスタムデータのアップロード

  /*
 * SDカードを使う時の流れ
 * 
 * FATFS Drive;
 * FIL FileObject;
 * f_mount(&Drive, "", 0);
 * f_open(&FileObject, "filename", FA_READ);    FA_WRITE
 * f_read(&FileObject, Buffer, 32, &actualLength);  //32バイト読み込む
 * f_write(&FileObject, str, strlen(str), &actualLength);
 * f_close(&FileObject);
 * 
 */
//SDカード関係
FIL FileObject;       //Openingやデータ書き込み用
FATFS Drive;
////SDカードからデータを読み出す時に使うバッファ
uint8_t SDcardBuffer[DATASIZE];    //ローカルでは入りきらなくても、ここではまだ入った

// mcc_generated_files/fatfs/ff_time.cを以下のように書き換えてタイムスタンプを正確に
/*
DWORD get_fattime (void)
{
    uint8_t dateTime[7];    //並びは、秒、分、時、曜日、日、月、年

    RTC_read(dateTime);
    return decimalToFatTime(2000+Bcd2Hex(dateTime[6]), Bcd2Hex(dateTime[5]), Bcd2Hex(dateTime[4]), Bcd2Hex(dateTime[2]), Bcd2Hex(dateTime[1]), Bcd2Hex(dateTime[0]));

}
 */

/* 
 * 外部フォントの名前=フォルダ名をここに格納
 * 各オブジェクトで使用するフォント名を格納する
 * SDカードに格納されているカスタムフォントのフォルダ名(最大8文字)と同一
 */
#define NumberOfUserFont   7
char CustomFontDir[NumberOfUserFont][9] = {
    "7segsl", "FUNWA28", "", "", "", "", "",
};

#define NOFORMAT    9999

//画面上に配置する部品の定義
ObjectRsc MainRsc1[] = {
    //ID,  x,  y,    xw,  yw, fontcode,  fontMag,color, format,   attribute, attribute2, xpitchAdj
    { 10,  30,  10,  80,  18, FontNormal, 0x22, WHITE, YYYYMMDDw, 0, 0, },
    { 20,  20,  35, 176,  58, Font7like,  0x77, LIME,  HHMMss,    0, 0, 0, },    //間隔 2dot広げた
    { 50,  50, 120,   0,   0, FontNormal, 0x22, WHITE, ALM24,     GREY, },       //アラームoff時の色=GREY
    { 30, 230, 100,   0,   0, FontNormal, 0x22, WHITE, DEGC,      0, },
    { 40, 240, 120,   0,   0, FontNormal, 0x22, WHITE, NOFORMAT,  0, },
    { 60,   0, 155, 100,  80, FontSmall,  0x11, WHITE, Month3,    BLUE, RED, 0, },    //土日の色、間隔1dot広げた
    { 70, 280,   0,  32,  32, FontNormal,    0, LIGHTGREY, NOFORMAT},    //歯車
};

ObjectRsc MainRsc2[] = {
    { 10,   0,   0,  80,  18, FontNormal, 0x22, WHITE, YYYYMMDDw, 0, 0, },
    { 20,  10,  18, 176,  58, FontNormal, 0x23, LIME,  HHMMSS,    0, 0, 2, },    //間隔 2dot広げた
    { 50, 200,  40,   0,   0, FontNormal, 0x12, WHITE, ALM24,     GREY, },       //アラームoff時の色=GREY
    { 30, 230,   0,   0,   0, FontNormal, 0x22, WHITE, DEGC,      0, },
    { 40, 240,  17,   0,   0, FontNormal, 0x22, WHITE, NOFORMAT,  0, },
    { 60,  25,  65, 320, 200, FontNormal, 0x22, WHITE, Month1,    BLUE, RED, 3, },    //土日の色、間隔3dot広げた
    { 70, 280, 200,  32,  32, FontNormal,    0,  GREY,  NOFORMAT},    //歯車
};

ObjectRsc MainRsc3[] = {
    { 10,   0,   0,  80,  18, FontNormal, 0x22, WHITE, YYYYMMDDw,   0, 0, },
    { 20,  10,  25, 180, 180, 0,          0,    GREY,  ANALOGCLOCK, Large, }, //大きな時計
    { 50,  40, 218,   0,   0, FontNormal, 0x13, WHITE, ALMAMPM,     GREY, },  //アラームoff時の色=GREY
    { 30, 230,   0,   0,   0, FontNormal, 0x22, WHITE, DEGC,        0, },
    { 40, 240,  20,   0,   0, FontNormal, 0x21, WHITE, NOFORMAT,    0, },
    { 60, 220,  40, 100,  80, FontSmall,  0x11, WHITE, Month3v,     BLUE, RED, 1, },
    { 70, 183, 200,  32,  32, FontNormal,    0,  GREY,  NOFORMAT},
};

//ユーザが表示方法をカスタマイズできるもの
ObjectRsc CustomRsc[] = {
    { 1,  30,  10,  80,  18, FontNormal, 0x22, WHITE,     YYYYMMDDw, 0, 0, 0, CustomFontDir[0], },
    { 1,  30,  35, 176,  58, Font7like,  0x56, ROYALBLUE, HHMMss,    0, 0, 2, CustomFontDir[1], },
//    { 1,  30,  35, 176,  58, FontNormal, 0x11, ROYALBLUE, HHMMss,    0, 0, 2, CustomFontDir[1], },
    { 1,  50, 120,  50,  20, FontNormal, 0x22, WHITE,     ALM24,     GREY, 0, 0, CustomFontDir[2], },  //アラームのOn/Off両方設定？
    { 1, 230, 100,  50,  20, FontNormal, 0x22, WHITE,     DEGC,      0, 0, 0, CustomFontDir[3], },
    { 1, 240, 120,  50,  20, FontNormal, 0x22, WHITE,     NOFORMAT,  0, 0, 0, CustomFontDir[4], },
    { 1,   3, 155, 100,  80, FontSmall,  0x11, WHITE,     Month3,    BLUE, RED, 0, CustomFontDir[5], },
    { 1, 280,   0,  32,  32, FontNormal,    0, LIGHTGREY, NOFORMAT,  0, 0, 0, CustomFontDir[6], },    //色を変更
};

ObjectRsc *CurrentRsc;    //現在参照すべきリソース

//DisplayModeの番号と下記の要素は一致させてある
//DisplayMode=DisplayMod1は、MainRsc1のリソース
ObjectRsc *RscData[] = {
  MainRsc1,
  MainRsc2,
  MainRsc3,
  CustomRsc,  
};

const char CustomSettingText[][6] = { "Date", "Time", "Alarm", "Temp", "Humid", "Calen", "Gear", };

const char TouchMsg[] = "Touch anywhere to close this screen";

// Prototype宣言

/*
 * TX2からデバッグ用の文字列を出力
 * printfで出力させるためのredirect用ルーチン
 * UART2のTXに出力されるので、それをシリアルアダプタを介してPCでモニタできる
 * Windows上では、TeraTermのCOM3で接続
 * 　設定 > シリアルポートを開き、スピードを115200に変更して再設定すれば良い
 */
#ifdef DEBUG
void putch(char c) {
    while (!UART2_is_tx_ready()) ;
    UART2_Write(c);

    while (!UART2_is_tx_done()) ;
}
#endif


/*
 * 10ms毎に割り込み
 * Touch: チャタリングチェック　　TouchStartをフラグに使う
 * スライドSW: 状態チェック
 */
void Timer5Handler() {
    //チャタリング処理用    
    static uint8_t touch_status = 0xff;     //下位6bitのみ使用
    static uint8_t slideSW_status = 0x55;   //下位6bitのみ使用

    //タッチした、やめた時にイベント発生。長押しになった時もイベント発生
    //タッチで、0
    touch_status = ((uint8_t)(touch_status << 1) | T_IRQ_GetValue()) & 0x3f;
    if (touch_status == 0x38) {
        //3回連続タッチ(T_IRQ=0)と感知したら
        TouchStart |= 0x01;     // TouchDown Flag
        TouchCount = 0; //カウンタリセット
    } else if ((touch_status & 0x0f) == 0) {
        //連続タッチ(0)が継続していたら、10ms毎にカウントアップ
        if (TouchCount < 200) {
            TouchCount++;
            if (TouchCount == 200) {
                //200x10ms=2s長押しの処理
                //TouchCountが200になった時だけ、Long判定
                TouchStart |= 0x02;     //TouchLong Flag
            }
        }
        TouchStart |= 0x04; //長押し関係なく、タッチが継続している時  
        if (RepeatKey) RepeatKey++;
    } else if (touch_status == 0x07) {
        //タッチしないとT_IRQ=1、3回連続でタッチやめたと判定
        TouchStart = 0x08;  //TouchUp Flag
    }
    
    //アラーム用スライドSWの状態チェックし、変化した時だけSlideSWStatusを更新
    slideSW_status = ((uint8_t)((slideSW_status << 1) | ALMSW_GetValue())) & 0x0f;
    if (slideSW_status == 0x08) {
        SlideSWStatus = SlideSWon2off;        //オフに変化
    } else if (slideSW_status == 0x07) {
        SlideSWStatus = SlideSWoff2on;        //オンに変化
    }
}


/*
 * タッチした座標x,y(グラフィック座標に換算したもの)が、指定されたオブジェクトの
 * 矩形範囲に入っているかチェック
 * 指定したオブジェクトを押したと判定されると1を返す。それ以外は、0
 * obj: RscObjectで定義しているもの
 */
int8_t ButtonPush(int16_t x, int16_t y, ObjectRsc *objrsc) {
    int16_t xx, yy;
    
    xx = x - objrsc->x;
    yy = y - objrsc->y;

    if ((xx >= 0) && (yy >= 0)) {
        if ((xx < objrsc->xw) && (yy < objrsc->yw)) {
            return 1;   //ボタンの矩形領域に入っていた場合
        }
    }
    return 0;   //領域外の時
}

/* 
 * タッチしたところの周辺データを取得 (3x3の領域)
 * データは、PixelData[]に、左上の座標はPixelXY[]に格納
 */
void GetPixelData(int16_t x, int16_t y) {
    uint8_t jj, kk;
    uint8_t rgb[27];    //3x3=9ドット分のデータ格納領域 x RBG各1バイト

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
 * 保持しておいたデータで書き戻す
 * PixelXY[0]が9999以外だったらという制限を付け、データの有効性を確保
 */
void SetPixelData() {
    if (PixelXY[0]!=9999) glcd_array(PixelXY[0], PixelXY[1], 3, 3, (uint8_t *)PixelData);
}

char *aboutText0 = "How to customize.\r\n";
char *aboutText1 = "Please access \r\n";
char *aboutText2 = "/index.html\r\n  to upload/remove custom font and custom data.\r\n\n";
char *aboutText3 = "/index2.html\r\n  to modify parts and upload.";

//タッチされた時の処理
void TouchProcedure() {
    uint8_t mm, hh;
    uint8_t snoozeAlarmTime[3] = {0, 0, 0x7f};
    char str[100], ip[30];

    //タッチした所に印をつける
    SetPixelData();     //元のデータを戻す
    GetPixelData(TouchX, TouchY);   //書込む場所のデータを読み込む
    display_fillRect(TouchX-1, TouchY-1, 3, 3, RED);    // 3x3の赤い四角を書く
    
#ifdef DEBUG2
    sprintf(str, "%d,%d -> %3d,%3d ", TouchRawX, TouchRawY, TouchX, TouchY);
    display_drawChars(145, 30, str, WHITE, TextbgColor, 0x11);
    sprintf(str, "Adj=%d,%d, %d,%d ", TouchXY[0], TouchXY[1], TouchXY[2], TouchXY[3]);
    display_drawChars(145, 40, str, WHITE, TextbgColor, 0x11);
#endif
    //アラームが鳴動中(AlarmStatus>0)にタッチしたらスヌーズ
    if (AlarmStatus) {
        AlarmSoundOff();
        RTC_resetAlarm();
        AlarmStatus = 0;
        if (SnoozeCount < 12) {
            //5分後にアラーム再設定　12回まで、1時間後まで
            mm = Bcd2Hex(AlarmTime[0]);
            hh = Bcd2Hex(AlarmTime[1]);
            SnoozeCount++;
            IncDecTime(SnoozeCount * 5, &hh, &mm);    //5分*Smooth回数の時刻に設定
//            IncDecTime(SnoozeCount *2, &hh, &mm);    //デバッグ用に短く
            snoozeAlarmTime[0] = Hex2Bcd(mm);
            snoozeAlarmTime[1] = Hex2Bcd(hh);
            //ここで、RTCにアラーム時刻再設定
            RTC_setAlarmTime(snoozeAlarmTime);
            DEBUG_PRINTF("Snooze: %02d:%02d\r\n", hh, mm);            
        } else {
            //スヌーズ終了
            //アラームSWはOnのままでも翌日対応のため、オリジナルのアラーム時刻をセットし直す
            RTC_setAlarmTime(AlarmTime);
        }
        return; //アラームなっている時のタッチの処理は、ここで終了
    }
    
    if (ButtonPush(TouchX, TouchY, &CurrentRsc[GearObj])) {
        //歯車アイコンをクリックしたら、設定画面へ
        lcd_fill(BLACK);
        ESP_GetIPaddress(ip);   //IPアドレスを取得

        SetFont(FontNormal, NULL);
        display_drawChars(0, 0, aboutText0, WHITE, BLACK, 0x22);
        
        display_setTextSize(0x12);
        display_setTextPitchAdj(1, 0);
        display_setCursor(0, 30);        //描画開始座標
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
        //画面の左下の1/4エリアをタッチして表示モード変更
        DisplayMode = (DisplayMode +1) % (sizeof(RscData)/sizeof(RscData[0])); //今の所、表示モードは4つ
        DATAEE_WriteByte(AddressDisplayMode, DisplayMode);  //変更したらEEPROMに書込む
        PushEvent(EventFormInit);
    }
    
}

/*
 * DATAEE_WriteByteのワード版
 */
void DATAEE_WriteWord(uint16_t bAdd, uint16_t bData) {
    DATAEE_WriteByte(bAdd, lo(bData));
    DATAEE_WriteByte(bAdd+1, hi(bData));
}

/*
 * DATAEE_ReadByteのワード版
 * unionを利用して18バイトプログラムサイズ削減
 */
uint16_t DATAEE_ReadWord(uint16_t bAdd) {
    WordByte data;
    
    data.dl = DATAEE_ReadByte(bAdd++);
    data.dh = DATAEE_ReadByte(bAdd);
    return data.d;
}


/*
 * カスタムデータをEEPROMに保存
 */
void StoreCustomData() {
    uint8_t jj;
    uint16_t addr;
    char *p;

    //カスタム設定を書き込む
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
 * タッチの座標をグラフィックの座標に変換
 * これは環境によって変化あるため、TouchXT2046.cからここへ移動した
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

    //以下の計算精度に注意
    // (20,20)  (300,220)のタッチ上の座標をTouchXY[0], TouchXY[1], TouchXY[2], TouchXY[3]に保持
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
 * タッチ座標調整用メッセージ
 * 対象のメッセージを白で、それ以外をグレーで
 * num=0/1  それ以外のエラーチェックしていない
 */
void TouchAdjMsg(uint8_t num) {
    uint16_t color[] = { WHITE, GREY };
    char str[100];

    //左上の十字を描画　中心座標(20,20)
    display_drawLine(16, 20, 24, 20, color[num]);
    display_drawLine(20, 16, 20, 24, color[num]);

    strcpy(str, "1. Touch + at Up Left");
    display_drawChars(50, 100, str, color[num], TextbgColor, 0x12);
    strcpy(str, "2. Touch + at Bottom Right");
    display_drawChars(50, 120, str, color[1-num], TextbgColor, 0x12);

    //右下の十字　　中心座標(300, 220))
    display_drawLine(296, 220, 304, 220, color[1-num]);
    display_drawLine(300, 216, 300, 224, color[1-num]);
    
}


/*
 * (20,20)、(300,220)の2点をタッチした時の座標を(TouchXY[0], TouchXY[1])、(TouchXY[2], TouchXY[3])に
 * 取り込むためのルーチン
 */
void TouchAdjust(void) {
    char str[100];
    int16_t dx, dy;
    uint8_t jj;

    lcd_fill(BLACK); //画面をクリア
    display_setTextColor(WHITE, BLACK); //TextcolrとTextbgColorを設定
    SetFont(FontNormal, NULL);
    display_setTextSize(0x12);
            
    //最初に左上をタッチして、次に右下をタッチしてもらう。妥当でない場合は、やり直し
    while (1) {
        TouchAdjMsg(0);
        // 1回目のタッチの位置
        while ((TouchStart & 0x01) == 0);
        while (GetTouchLocation(&TouchXY[0], &TouchXY[1]) == -1);
        //タッチしたらピッという音を出す
        AlarmSoundOn(AlarmSingle);
        //2回目の時もあるので、一旦ここでTry Againのメッセージを消す
        display_setCursor(100, 180);
        display_puts("               ");    // Try Againの文字を消す
        sprintf(str, "X1=%4d, Y1=%4d", TouchXY[0], TouchXY[1]);
        display_setCursor(50, 20);
        display_puts(str);            
        
        TouchAdjMsg(1);
        // 2回目のタッチまで少し遅延。反応良すぎて、1回目と同じになったため
        __delay_ms(500);
        while ((TouchStart & 0x01) == 0);   //割込みでタッチと認識されるまでループ
        while (GetTouchLocation(&TouchXY[2], &TouchXY[3]) == -1);
        //タッチしたらピッという音を出す
        AlarmSoundOn(AlarmSingle);
        sprintf(str, "X2=%4d, Y2=%4d", TouchXY[2], TouchXY[3]);
        display_setCursor(100, 200);
        display_puts(str);
        
        DEBUG_PRINTF("P1=(%d, %d) P2=(%d, %d)\r\n", TouchXY[0], TouchXY[1], TouchXY[2], TouchXY[3]);
        
        //2回目のタッチが1回目と近い時は、やり直させる
        //2点の距離がX,Y各1500位ある　(1500/16)^2=8800　以下の計算だと、18000になる
        dx = ABS((int16_t)(TouchXY[0] - TouchXY[2]))/16;  //2乗した時int16を超えないよう
        dy = ABS((int16_t)(TouchXY[1] - TouchXY[3]))/16;
#ifdef DEBUG
        DEBUG_PRINTF("dx=%d, dy=%d\r\n", dx, dy);
#endif
        if (dx*dx + dy*dy < 12000) {
            //距離が短い時は、もう一度
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
    //調整したタッチ座標を書き込む
    for (jj = 0; jj< 4; jj++) DATAEE_WriteWord(AddressTouch + jj*2, TouchXY[jj]);
}


/*
 * TMR1の500us毎の割り込みで、ロータリーエンコーダの状態をチェック
 * 24クリック/1回転
 * 1秒で1回転したら、24回イベント発生する。
 * キーのチャタリング防止と同様のプログラム
 * 3回=1.5ms間、同じ状態なら、そのレベルと認識させる
 * 　→この割り込みで、1秒間に2000回チェックできる。1秒に2回転させたとして、
 * 　　　24クリック分x2回の状態変化が発生。1サイクルで40回ほどサンプリングできる計算なので、
 * 　　　信号変化は十分取り込める
 * 　→3回のサンプリングができる回転速度は、1周期=(3+3)*500us=3ms、24クリック*3ms=72msなので、
 * 　　1/72ms=13回転と、そんなに1秒間で回転できないレベルなので、十分
 * クリックの安定点では、A端子は常にOFF=1の状態
 * 
 * RotCountを更新していく
 * イベント処理が遅れても、RotCountの値を使って処理するので、全部のイベントを処理必要という
 * わけではなく、処理完了して、RotCount=0なら残ったイベントはスキップされる
 * 
 */
void RotaryHandler() {
    static uint8_t StartRot = 0;    //
    static int8_t Bdata = 0; // 時計回り=1、反時計回り=0
    static uint8_t RotA_status = 0xff;  //ROT_Aだけ変化点をチェック
    uint8_t newBdata;

    //A端子側のレベルをチェック。停止時=1、動かすと0となり、クリックポイントで1に戻る
    RotA_status = (uint8_t)(RotA_status << 1) | ROT_A_GetValue();

    if ((StartRot == 0) && (RotA_status & 0x07) == 0) {
        //A端子が0になったら、回転開始と判断
        StartRot = 1;
        //回転開始と判断した時のB端子のレベルで回転方向を判定
        Bdata = ROT_B_GetValue();
    } else if ((StartRot == 1) && ((RotA_status & 0x07) == 0x07)) {
        //回転動作していて、A端子が1になった時の処理
        newBdata = ROT_B_GetValue();
        // B端子側が、1→0の時CW(インクリ)、0→1の時RCW(デクリ)
        if (Bdata < newBdata) {
            //A端子がHになった時、B端子がHなら反時計方向に回転
            RotCount--;
            //ロータリイベントをここで発行しない。大量に発行されるのは効率悪いから
            //メインループで、RotCountが0でなければイベント発行させ、一度に処理させる
        } else if (Bdata > newBdata) {
            //A端子がHになった時、B端子がLなら時計方向に回転
            RotCount++;
        }
        //ちゃんと回転せず元に戻った場合、カウントさせず、終了するケースあり。
        StartRot = 0;
    }    
}


/*
 * INTAをINT0割り込みで
 * 1秒ごとに割り込みが入るので、時刻の更新イベント発生
 */
void RTC_handler() {
    EXT_INT0_InterruptFlagClear();
    NeedUpdateTime = 1;
}

/*
 * アラームW(RTC_INTB=RB4のIOC)による割り込み
 * 18F27K42では、Negativeの指定ができるので、そちらに変更し信号レベルの判断不要に
 */
void AlarmWHandler() {
    AlarmStatus = 1;
}

/*
 * 日付が変わったかチェックして、変更あれば日付表示更新
 * Year/Month/Day/WeekdayをEEPROMに格納しておき、電池入れ替え後の変更の手間を省く
 */
uint8_t WriteYMD() {
    uint8_t jj, tmp;
    uint8_t changeDate = 0;
    uint16_t addr;

    //何回もEEPROMを書き換えをしないように、変化あった時だけにする
    addr = AddressYMD;
    for (jj = 3; jj < 7; jj++) {
        tmp = DATAEE_ReadByte(addr);
        //曜日、日、月、年をチェック
        if (tmp != DateTime[jj]) {
            DATAEE_WriteByte(addr, DateTime[jj]);
            changeDate = 1;
        }
        addr++;
    }
    return changeDate;
}


/*
 * SDカードをマウントする
 * 既にmountしていたら、一旦unmount
 *f_mountは、ワークエリアの設定であり、SDカードの有無に関係なく、常に成功する
 */
void remount(void) {
//    FRESULT res;
    f_mount(0, "", 0);  //unmount
//    DEBUG_PRINTF("res0=%d\r\n", res);
    f_mount(&Drive, "", 0);   //常に成功
//    DEBUG_PRINTF("res1=%d\r\n", res);
}


/*
 * NTPサーバから日時を取得し、RTCに設定
 */
void GetTimeFromNTP(uint8_t *datetime) {
    uint16_t yy;
    uint8_t mm, dd, h, m, s;

    Get_time(&yy, &mm, &dd, &h, &m, &s);

    datetime[0] = Hex2Bcd(s);
    datetime[1] = Hex2Bcd(m);
    datetime[2] = Hex2Bcd(h);
    RTC_setTime(datetime);  //時刻をRTCに設定
        
    datetime[4] = Hex2Bcd(dd);
    datetime[5] = Hex2Bcd(mm);
    datetime[6] = Hex2Bcd(yy % 100);
    
    //日付をチェックして、曜日を設定
    DateErrorCheck(datetime);
    RTC_setDate(datetime);  //日付をRTCに設定
    
    DEBUG_PRINTF("%d/%d/%d ", yy, mm, dd);
    DEBUG_PRINTF("%2x:%02x:%02x\r\n", datetime[2], datetime[1], datetime[0]);
}


/*
 * SDカード内のフォントのリストを取得
 * 
 * SDカードからフォルダを読み出す (フォントフォルダ以外も対象に入ってしまうが気にしない)
 * フォントディレクトリリストを表示する
 * 1フォント=FolderW x FolderHの領域に記入　　横4個、縦7個の28個までに制限する
 * 取得したフォルダ名=フォント名は、FontNameListへ格納
 */
FRESULT GetFontFolder(char* path) {
    FRESULT res;
    FILINFO fno;
    FFDIR dir;

    DEBUG_PRINTF("%s:\r\n", path);
    remount();
    FontNum = 0;
    res = f_opendir(&dir, path);                       /* ディレクトリを開く */
    if (res == FR_OK) {
        while (1) {
            res = f_readdir(&dir, &fno);                   /* ディレクトリ項目を1個読み出す */
            if (res != FR_OK || fno.fname[0] == 0) break;  /* エラーまたは項目無しのときは抜ける */
            if (fno.fname[0] == '.') continue;             /* ドットエントリは無視 */
            if (fno.fattrib & AM_HID) {
                // Hiddenファイルは表示しない
            } else if (fno.fattrib & AM_DIR) {          /* ディレクトリ */
                DEBUG_PRINTF("dir: %s\r\n", fno.fname);
                strcpy(FontNameList[FontNum], fno.fname);
                FontListDate[FontNum] = fno.fdate;  //WORDデータ
                FontListTime[FontNum] = fno.ftime;  //WORD
                FontNum++;
                if (FontNum >= MAXLISTITEM) {
                    //現状フォントファイルの最大を制限。画面表示しきれないため
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
 * SDカードの指定されたディレクトリを削除
 * 　ディレクトリ内のファイルを全部削除後、フォルダを削除という手順
 */
void RemoveFolder(char *dirname) {
    FRESULT res;     /* API戻り値 */
    FILINFO fno;    /* ファイル情報 */
    FFDIR dir;
    char filename[64];
    
//    f_unlink("*.*");    //ワイルドカードは使えない
    DEBUG_PRINTF("Remove Folder\r\n");
    remount();
    f_chdir("/");
    res = f_opendir(&dir, dirname);    /* ディレクトリを開く */
    if (res == FR_OK) {
        while (1) {
            res = f_readdir(&dir, &fno);                   /* ディレクトリ項目を1個読み出す */
            if (fno.fname[0] == 0xff) {
                //res=0になるも、ファイル名がおかしな時がある。dirを開き直して継続
                //これが発生するのは、0サイズのファイルを作成した次のファイル
                res = f_closedir(&dir);
                res = f_opendir(&dir, dirname);    /* ディレクトリを開く */
                continue;
            }
            DEBUG_PRINTF("file: %s, %d\r\n", fno.fname, res);
            if (res != FR_OK || fno.fname[0] == '\0') break;  /* エラーまたは項目無しのときは抜ける */
            if (fno.fname[0] == '.') continue;             /* ドットエントリは無視 */
            if (fno.fattrib & AM_DIR) {          /* ディレクトリは無視 */
            } else {
                // ディレクトリは移動していないので、pathを追加する必要あり
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
    res = f_rmdir(dirname); //ディレクトリを削除
    DEBUG_PRINTF("rd: '%s, res=%d'\r\n", dirname, res);
                
}


/*
 * SDカードに設定情報保存用のDataフォルダがなければ作成
 * 
 */
FRESULT MakeDirecotory(char *dir) {
    FRESULT res;
    
    res = f_stat(dir, NULL);   //既にディレクトリがあるか？
    if (res == FR_NO_FILE) {
        //ディレクトリなければ作成
        res = f_mkdir(dir);
        DEBUG_PRINTF("mkdir\r\n");
        return res;
    } else {
        DEBUG_PRINTF("Already %s exists\r\n", dir);
    }
    return res;
}


/*
 * ホームページの表示
 */
void SendHomepage(uint8_t clientID) {
    uint8_t jj;
    char msgstr[PROCESSBUF_SIZE];
    uint16_t year;
    uint8_t month, day, hour, minute, second;

    GetFontFolder("/");    //ルートの全フォルダを取り出す
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
    
    //カスタム設定のデータをhtmlとして出力
//    ShowCustomData(clientID);
    //以下で、全部のオブジェクト情報を書き込める
    //ここでカンマ区切りで各項目ごとにデータ書出  テキストファイルの方が良いとの判断
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
    if (ESP_HtmlSnd2(clientID, NULL)) return;   //エラー時中断
    
    ESP_HtmlSndEnd(clientID);
}


/*
 * 16bitカラーを24bitカラーに変換
 */
uint32_t ColorFrom565(uint16_t color) {
    uint8_t r, g, b;
    uint32_t col;
    
    b = (color << 3 ) & 0xff;   //最下位の5bitを8倍して、8bit化
    color = color >> 5;
    g = (uint8_t)((color & 0x3f) << 2);    //
    r = (color >> 3) & 0xf8;    //最上位の5bit
    col = (((uint32_t)r << 16) | ((uint32_t)g << 8) | (b));
    return col;
}

/*
 * カスタマイズ用の画面(index2.html)を構成
 */
void SendCustomPage(uint8_t clientID) {
    uint8_t jj, fs, len;
    char msgstr[PROCESSBUF_SIZE];   //256バイト
    uint32_t color;
    const char ItemText[][10] = { "disp", "font", "fsize", "color", "format", "attr1", "attr2", "xpAdj", "fName", };
    uint16_t r, g, b;

    ESP_HtmlSnd2(clientID, html_custom1);

    //ここでカンマ区切りで各項目ごとにデータ書出  テキストファイルの方が良いとの判断
    /*
        #box0{top:10px;left:30px;width:80px;height:18px;
	    background-color:#f48fb1}
     */
    //各オブジェクトのstyle設定
    //箱の色を設定した色にした。文字色
    for (jj = 0; jj < OBJECTNUM; jj++) {
        fs = (uint8_t)(CustomRsc[jj].yw *4/5);     //箱の高さに合わせて文字サイズを設定
        len = (uint8_t)strlen(CustomSettingText[jj]);
        //上記フォントサイズで表示した時に横幅を超える時
        if (fs * len > CustomRsc[jj].xw) fs = (uint8_t)(CustomRsc[jj].xw /len);  //箱の高さに合わせて文字サイズを設定
        
        //背景色を元の色をベースに明度を2倍か1/2倍にする
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
            //disp設定されているものをtrueにする
            sprintf(msgstr, "dispItems[%d].checked=true;", jj);
            ESP_HtmlSnd2(clientID, msgstr);
        }

        //フォント
        if (CustomRsc[jj].fontcode != 0) {
            sprintf(msgstr, "fontItems[%d].value='%d';", jj, CustomRsc[jj].fontcode);
            ESP_HtmlSnd2(clientID, msgstr);
        }

        //format
        if (CustomRsc[jj].format != NOFORMAT) {
            sprintf(msgstr, "formatItems[%d].value='%d';", jj, CustomRsc[jj].format);
            ESP_HtmlSnd2(clientID, msgstr);
        }

        //fsizeは、テキストボックスで、初期値は、valueに入れる
        //colorは、期値をvalueに16進6桁で入れる。先頭に#
        sprintf(msgstr, "fsizeItems[%d].value='0x%02X';colorItems[%d].value = '#%06lX';", 
                jj, CustomRsc[jj].fontmag, jj, ColorFrom565(CustomRsc[jj].color));
        ESP_HtmlSnd2(clientID, msgstr);
        
        //attr1, attr2, xpAdj, fName　　まとめることで、ブラウザの更新が33s→24sに縮まった
        if (jj != GearObj) {
            if ((jj == CalendarObj)||(jj == AlarmObj)) {
                //色設定に使っている　アラーム、カレンダー
                sprintf(msgstr, "attr1Items[%d].value='#%06lX';", jj, ColorFrom565(CustomRsc[jj].attribute));
            } else {
                sprintf(msgstr, "attr1Items[%d].value='%d';", jj, CustomRsc[jj].attribute);
                
            }
            ESP_HtmlSnd2(clientID, msgstr);

            if (jj == CalendarObj) {
                //カレンダの時
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

    if (ESP_HtmlSnd2(clientID, html_custom5)) return;   //エラーの時
    ESP_HtmlSnd2(clientID, NULL);

    ESP_HtmlSndEnd(clientID);
}


/*
 * カスタム設定のデータをSDカードから読み出す
 */
FRESULT LoadCustomData(char *filename) {
    FRESULT res;
    uint8_t kk, len;
    char str[150];
    char font[20] = "";
    char *p;
    FIL file;

    res = f_open(&file, filename, FA_READ);
    if (res != FR_OK) return res;   //ちゃんとopenできなかったら終了
    //dispを全部0にリセット
    for (kk = 0; kk < OBJECTNUM; kk++) CustomRsc[kk].disp = 0;
    
    //1行目のテキストデータを読み出すが、データは不要
    f_gets(str, sizeof str, &file);     //\nまで読み出す
    
    //順不同にさせるには、最初の項目名を見て、格納番号を確定する必要あり
    //:までの文字がキーワードになっていて、指定ワード以外は無視
    while (f_gets(str, sizeof str, &file) != NULL) {     //\nまで読み出す
        //先頭から:までが項目名
        p = strtok(str, ":");
        len = (uint8_t)strlen(p)+1;
        //読み込んだデータがどの項目か、サーチしてkkに設定
        //一致したものが見つかった場合、ない場合はスキップ
        kk = 0;
        while (strncmp(CustomSettingText[kk], p, 4) && kk < OBJECTNUM) kk++;
        
        if (kk < OBJECTNUM) {
            DEBUG_PRINTF("fn=%p, %s.%p\r\n", CustomRsc[1].fontName, CustomFontDir[kk], CustomFontDir[kk]);
            //:の次の所へ
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
 * WiFi受信したデータの処理を行う
 * 
 * 基本的に、改行コード(CR/LF)を区切り毎に処理
 * 　バッファフルの時などは、改行コードが付かない場合あり
 * 
 * boundary文字列が来たあと、空行を受信後バイナリデータとして取り扱う
 * 　境界文字が来たら、終わり
 * 
 * 受信するデータは、以下のように始まるパケット単位
 * +IPD,x,ddd:***    ここのバイト数分を受信する必要あり
 * 
 * マルチパートでデータが来る可能性を想定して、Boundary文字を良くチェック
 * Content-Disposition
 * Content-Type
 * の情報をよく見て、処理を決める
 * bmpファイルの場合: 以下のような感じになっている
 * ------webKitFormBoundaryKjqavoSoqHf8J7Yg
 * Content-Disposition: form-data; name="fontfile"; filename="ForDebug/1.bmp"
 * Content-Type: image/bmp
 * (CR/LF)
 * 42 B 4D M AE . 00 . 00のようにバイナリデータが続く
 * (CR/LF)
 * ------webKitFormBoundaryKjqavoSoqHf8J7Yg
 * bmpデータに特定するなら、BMという文字に続く3バイトがファイルサイズなので、
 * それを利用する手もあるが、txtファイルだと、その手も使えない。
 * やはりboundary文字列を使うのが正統かと思われる。
 * boundary=""で示される文字列に、(CR/LF)--を先頭に追加したのが、マルチパートの始まりで、
 * それが複数のパートになる場合もあり、終了部は、その文字列にさらに--がついた文字列が終了となる。
 * なお、baoundaryでくくられている部分が別のパケット(ヘッダに+IPD,0,nnn:が付く)になっている場合もあるし、
 * 元のものに含まれている場合もあるので、注意。たぶんパケットのサイズが大きすぎるとそうなる様子。
 * 
 * 
 */


/*
 * デバッグ用にバイナリデータを見やすいフォーマットで表示　1バイトだけ
 * そのデータをオープン済みのファイルに書き込む
 * 
 * 16バイト分のデータがまとまったらASCIIで表示
 */
void DisplayBinary(uint16_t *ad, uint8_t data)
{
    uint16_t addr = *ad;
    static char log[18];

    //バイナリ部を表示
    if (addr % 16 ==0) {
        DEBUG_PRINTF("%06X: ", addr);
    }
    DEBUG_PRINTF("%02X ", data);
    //バイナリの1バイトを書き込む　　ファイルはオープン済み <<<<<<<<<<<<<<<<<<<<<

    // ASCII表示用のデータ準備
    if ((data < 0x20)||(data >= 0x7f)) log[addr % 16] = '.';
    else log[addr % 16] = data;
    addr++;
    //16バイト分まとまったら表示
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
    
    //対象名だけのデータが最初に来るで、それを記録
    kk = 0;
    while (strncmp(CustomSettingText[kk], item, 4) && kk < OBJECTNUM) kk++;
    if (kk < OBJECTNUM) {
        TargetObj = kk;
        return;
    }
    
    //上記以外の時は、データとして、TargetObjのオブジェクトを対象にする
    //checkboxは、チェックされた時にしかPOSTされない。つまりdispが来たらonしかない
    //オフになっている項目を判断できないので、初期時に全部クリアしておく必要あり
    if (strncmp(item, "disp", 4) == 0) {
        if (strncmp(data, "on", 2) == 0) CustomRsc[TargetObj].disp = 1;
        DEBUG_PRINTF("   disp = %d\r\n", CustomRsc[TargetObj].disp);
    }
    else if (strncmp(item, "XY", 2) == 0) {
        //スペースが+になっているので、空白に変換　　++1とかとなっていると正しく変換できないため
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
        if ((TargetObj == 5)||(TargetObj == 2)) {   //カレンダかアラームの時
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
 * POSTされたデータを処理する
 * カスタム設定用データの処理
 * 最後のデータの開始ポイントを返す
 * もし途中で切れている時の処理に使用
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
    sprintf(target, "%s%s", holdstr, st);    //最初だけ、前回の残り文字と連結
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
            //&がなかったら、保持文字列として格納して終了
            //ただしlast='&'の時は、普通に処理して、holdstrなしで終了させる
            if (ContentLength > 0) {
                sprintf(holdstr, "%s", stp);
                DEBUG_PRINTF("処理保留 %s\r\n", holdstr);
                break;
            } else {
                //最後まで取り込んだはずなので、そのまま処理
                DEBUG_PRINTF("Content終了\r\n");                
            }
        }
    }
}

char *CRLF = "\r\n";

/*
 * 次の境界が来るまでバイナリとして処理
 * 改行コードもしくはバッファいっぱいまでデータを取得して処理していく
 * 境界文字列は、改行に囲まれているが、パケットにまたがっているケースもあるがポイント
 * 
 */
/*
 * 
 * バイナリ部以外でも、以下のようにパケットが切れてしまう場合がある
 * Content-Dis で切れて、続きは、positionとつながっている
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
 * POSTパケット内の各行の内容に応じて処理する
 * テキスト部は、改行コードできちんと終わっていることを想定
 * 
 * stの先頭には改行コードはないが、最後には改行コードあり
 */
void POSTdata(char *st) {
    FRESULT res;
    
    //マルチパートが有効な時、境界文字列がすでに設定されていれば、境界文字列かどうか調べる
    if (BoundaryStr[0] != '\0') {
        //境界文字列を検索するには2行分のデータが必要。ここに来た時に改行までの
        //データはあることはわかっているが、その次の行があるかは確認していない。
        //別パケットにまたがっている場合もあり、その手当てが必要。
        //データはすでに受信しているケースが多いと思うので、余計な部分(+IPD,)の削除
        
        if (strncmp(st, BoundaryStr, BoundaryStrLen) == 0) {
            //BoundaryStr: "--%s--" の形
            //ここで、境界領域終了
            DEBUG_PRINTF("**************  boundary End *************\r\n");
            BinOrText = TXT;
            InBoundary = 0; //境界域終了
            BoundaryStr[0] = '\0';  //マルチパートが終わったので文字列クリア
            return;
        }

        if (strncmp(st, BoundaryStr, BoundaryStrLen -2) == 0) {
            //BoundaryStr: "--%s" の形
            //ここからboundary領域が始まる
            //Content-Lengthは、最初の境界文字列の後からカウント始める
            DEBUG_PRINTF("boundary (re)start =========\r\n");
            InBoundary = 1; //境界域開始
            BinOrText = TXT;    //一旦テキストに
            return;
        }
    }
    
    /*
     * パケット長とは別に、内容の長さが指定される
     * 内容とは、境界文字を含むデータ部分を示している
     */
    if (strncmp(st, "Content-Length:", 15) == 0) {
        //長さを数値に変換するため、改行コードの所を文字終端にする
        ContentLength = (uint16_t)atoi(st + 15);       // 文字列の長さを数値に変換する
        DEBUG_PRINTF("Content-Length=%d\r\n", ContentLength);
        return;
    }
    /*
     * multipartは、以下の2行を含む。
     * Content-Disposition: 
     * Content-Type: 
     * 
     * (data)
     */
    //Content-Type: application/octet-stream　とかいうのもあるが、今は対象外
    if (strncmp(st, "Content-Type: multipart/form-data; boundary=", 44) == 0) {
        //指定された境界文字列は、その後に出て来る境界の最初、途中、最後でそれぞれ違う形で活用される
        //最初:　先頭に-が2つ付く
        //途中:　先頭に-が2つ付くとともに、後ろにも--が付く
        //最後:　途中のものにさらに後ろに--が追加される
        //そして、必ず前後は改行コードで囲まれる

        //境界文字列取得　実際に使われる時に合わせて、先頭に--付加
        //例:Content-Type: multipart/form-data; boundary=----WebKitFormBoundaryIEYlyg6d8k8gvZ50
        //境界文字列  -(ハイフン)が4つにWebKitで始まる計38文字　改行コード削除している
        BoundaryStrLen = strlen(st + 44);
        if (BoundaryStrLen > BOUNDARYSTRLENGTH - 7) {
            //境界文字列変数に格納できず
            DEBUG_PRINTF("\r\n!! ERROR BoundaryStr overflow !!\r\n");
            BoundaryStrLen = 0; //境界文字未設定
            return;
        }
        
        //境界文字列の設定　　終端用だけ用意し、比較文字数を変えて、中間は判断させる
        sprintf(BoundaryStr, "--%s--", st + 44);
        BoundaryStrLen = BoundaryStrLen + 4;    //BoundaryStrの長さ。最初に--
        DEBUG_PRINTF("Set boundary='%s'(%d)\r\n", BoundaryStr, BoundaryStrLen);
        
        InBoundary = 0; //マルチパートあり
        return;
    }

    //フォントファイルがアップロードされる起点
    if (strncmp(st, "Content-Disposition: form-data; name=\"fontfile\"; filename=", 58) == 0) {
        WHICHJOB = UPLOADFONT;
        sscanf(st+59, "%[^/]/%[^\"]\"", Dirname, Filename);
        DEBUG_PRINTF("open dir=%s, fn=%s \r\n", Dirname, Filename);
        if (Filename[0] == '\0') return;
        
        //ファイル名が長い時は、中止
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
        } while (res != FR_OK);   //ちゃんとopenできるまでループ
        //このようにしないと、ちゃんと書き込めず、おかしなファイル状態になってしまったファイルが生成された
        
        FileCount++;
        return;
    }

    //カスタム用データファイルのアップロード
    //元々システムで管理していた時のファイル名は、Data?.dat　?は、1-8
    //今回は、PIC側は1ファイルに限定。複数ファイル持つのはPC上で、それゆえ、ファイル名は固定化する
    if (strncmp(st, "Content-Disposition: form-data; name=\"datafile\"; filename=", 58) == 0) {
        WHICHJOB = UPLOADCUSTOM;
        sscanf(st+59, "%[^\"]\"", Filename);
        DEBUG_PRINTF("Upload filename=%s\r\n", Filename);
        if (Filename[0] == '\0') return;
        f_chdir("/");
        MakeDirecotory(DataFolder); //まだ作成されていないケースを想定。既にあっても問題なし
        f_chdir(DataFolder);
        //アップロードするデータファイルをルートディレクトリに書き込む
        do {
            res = f_open(&BinFile, "Data0.dat", FA_CREATE_ALWAYS | FA_WRITE);
        } while (res != FR_OK);   //ちゃんとopenできるまでループ
        FileCount++;
        return;
    }
    
    //remove
    if (strncmp(st, "removedir=", 10) == 0) {
        char *p, *p2;

        WHICHJOB = REMOVEFONT;
        DEBUG_PRINTF("Remove Dir\r\n");
        //複数選択していた場合&で連結されていることを考慮
        p2 = st;
        do {
            p = strchr(p2, '&');
            *p = '\0';
            RemoveFolder(p2 + 10);    //removedir=　を除く
//            DEBUG_PRINTF("rd=%s\r\n", p2+10);
            p2 = p + 1;
        } while (p != NULL);
        
        return;
    }
}


/*
 * リクエストラインの処置
 * +IPDの後ろのIDとパケット長を取得
 * エラーの時は-1を返す。基本的にESP_GetDataPの返り値と同じものを返す
 * 
 */
int8_t GetRequestLine(char *st, uint16_t *len, uint8_t *clientID, uint16_t *PacketLen, uint8_t skip) {
    int8_t ans;
    char *p;
    uint16_t len2;

    ans = ESP_GetDataP(st, len, skip);
    DEBUG_PRINTF("original:ans=%d,len=%d:%s\r\n", ans, *len, st);
    
    if (ans == -1) return ans;
    //\r\nだけなど、+IPD,で始まっていなかったら、処理終了
    if (strncmp(st, "+IPD,", 5) != 0) return -1;

    //パケットヘッダから:までにある、clientIDと受信すべき文字数を取得
    sscanf(st + 5, "%hhd,%hd:", clientID, PacketLen);
    DEBUG_PRINTF("ID=%d, PacketLen=%d\r\n", *clientID, *PacketLen);
    p = strchr(st, ':');
    len2 = (uint16_t)((p - st) +1);   //:までの長さ
    *len = *len - len2;
    //:までを削除して、残りを引き渡す
    memmove(st, st + len2, *len +1); //\0まで含めるため
    *PacketLen = *PacketLen - *len;
    DEBUG_PRINTF("t(-%d=%d)cnt=%d,%s", *len, *PacketLen, ContentLength, st);
    return ans;
}


/*
 * 受信したデータの処理：パケットの処理
 * 基本的に改行コードまでの1行毎に処理していく
 * 複数パケットに分かれている時は、前のパケットで処理できなかった文字列に続けて取り込む
 * 
 * 最初の行(+IPD,から始まる行)かチェックし、クライアントIDと文字数を取得
 * 例: +IPD,0,473:GET /index.html HTTP/1.1
 *         A A   A 
 * 　　　　最初のカンマ　　id:クライアントID
 *　:以降がリクエストライン
 */
void PacketProcess() {
    uint16_t len, remlen;
    static char st[PROCESSBUF_SIZE] = "";   //現状256バイト
    char *st2;
    int8_t ans;
    uint8_t FlagLF, preFlagLF;
    uint8_t clientID, id;
    static uint16_t PacketLen = 0;  //このパケットの長さ
    uint8_t skipCR, jj;
    static uint16_t addr = 0;
    FRESULT res;
    UINT actualLength;
    uint8_t send;

    //データがない時は処理終了
    if (!UART1_is_rx_ready()) return;
    WHICHJOB = 0;
    len = sizeof(st);
    ans = GetRequestLine(st, &len, &clientID, &PacketLen, 0);
    if (ans != 0) return;   //ans=0以外は想定していないので排除
    
    //コマンドの判断
    if (strncmp(st, "GET ", 4) == 0) {
        //GETの処理
        //ページにアクセスしてきた時のみ、クライアントにHTMLテキストを送信する
        if (strstr(st, "index.html") != NULL) send = 1;
        else if (strstr(st, "index2.html") != NULL) {
            send = 2;
            WHICHJOB = SETCUSTOM;
        }
        else send = 0;
        // GET /favicon.ico HTTP/1.1　が送信されて来ていたので、今まで2回送信していた

        DEBUG_PRINTF("-- GET start --\r\n");
        while (PacketLen > 0) {
            //データ読み飛ばすだけで何も処理しない。ここで複数行処理する
            len = sizeof(st);
            ans = ESP_GetData(st, &len);
            PacketLen = PacketLen - len;
            DEBUG_PRINTF("G(-%d=%d)%s", len, PacketLen, st);
        }
        //タイムアウト、データに不備があった時の処理はしていない
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
            //checkboxがオフの時は何もPOSTされないので、ここで一旦リセット
            for (jj = 0; jj< OBJECTNUM; jj++) CustomRsc[jj].disp = 0;
        }

        //ブラウザから送信されてきた時。単純なフォームのデータ送信もこれ
        BoundaryStr[0] = '\0'; //境界文字列をリセット
        DEBUG_PRINTF("-- POST start --\r\n");
        FileCount = 0;  // アップロードされたファイル数
        StartContent = 0;
        ContentLength = 0;
        InBoundary = 0;
        BinOrText = TXT;
        
        while (1) {
            DEBUG_PRINTF("%c:", 'a'+BinOrText); //a(ASCII=TXT)または、b(BIN)
            if (PacketLen == 0) {
                reGetNextPacket0:
                do { 
                    len = sizeof(st) - 1;
                    ans = GetRequestLine(st, &len, &id, &PacketLen, skipCR);
                } while (ans == -1);    //バイナリモードの時は、ansが0以外の時もあるので、修正
                //別のパケットが割り込んでくる場合あり　　その時はIDが異なる
                if (clientID != id) {
                    //clientIDが異なる場合は、そのパケットを受信して破棄
                    while (PacketLen > 0) {
                        //データ読み飛ばすだけで何も処理しない。ここで複数行処理する
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
                if (len > PacketLen) len = PacketLen;   //残りパケットが少ない時は、それに合わせる
                ans = ESP_GetData(st, &len);
                if ((ans != 0) && (len < sizeof(st))) st[len] = '\0';   //終端入れられれば入れる
                PacketLen = PacketLen - len;
                if (StartContent) ContentLength = ContentLength - len;
                DEBUG_PRINTF("P ans=%d(%d=%d)cnt=%d, %-256s|\r\n", ans, -len, PacketLen, ContentLength, st);
            }
            if (ans == -1) return;
            if (ans == 1) skipCR = 1;    //\rで終わっていたら、\rの判定をさせないように準備
            else skipCR = 0;

            //パケットの最後で、境界文字が分割されてしまう場合あり
            //もし、ContentLengthがあり、lenが境界文字長さより短い時は追加取得させる
            if ((PacketLen == 0) && (ans !=0) && (ContentLength > 0)) {
                remlen = len;   //処置保留のバイト数
                st2 = st + remlen;
                //続きを次のパケットから読み出す
                reGetNextPacket:
                do {
                    len = sizeof(st) - remlen - 1;
                    if (len == 0) break;
                    ans = GetRequestLine(st2, &len, &id, &PacketLen, skipCR);
                } while (ans == -1);    //バイナリモードの時は、ansが0以外の時もあるので、修正
                if (clientID != id) {
                    //clientIDが異なる場合は、そのパケットを受信して破棄
                    while (PacketLen > 0) {
                        //データ読み飛ばすだけで何も処理しない。ここで複数行処理する
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
                DEBUG_PRINTF("+(-%d=%d)ans=%d,連結%s", len, PacketLen, ans, st);
            }
            
            if (BinOrText == BIN) {
                preFlagLF = FlagLF;
                //今回読み込んだ行の最後が\r\nだったら、それは処理せず、改行ありフラグセット
                //ans=0：最後の改行コードを除き、バイナリとして処理させる
                //ans=1：\rを除き、バイナリとして処理
                //ans=2：全データをバイナリとして処理
                if (ans == 0) FlagLF = 2;
                else if (ans == 1) FlagLF = 1;
                else FlagLF = 0;
                DEBUG_PRINTF("FlagLF=%d\r\n", FlagLF);
                
                if (preFlagLF == 2) {
                    if (ans == 0) {
                        //境界文字列かどうかチェック
                        POSTdata(st);
                    }
                    //そうでない場合は、処理保留にした改行コードと今回のデータをバイナリとして処理
                    //処理保留にした改行コードと今回のデータをバイナリとして処理
                }
                else if (preFlagLF == 1) {
                    if ((ans == 0) && (st[0] == '\n')) {
                        //境界文字列かどうかチェック　ただし、lenが1文字だけだったら、
                        if (len > 1) POSTdata(st + 1);
                    }
                }
                //境界文字列だったら、テキストモードに戻っている
                if (BinOrText == TXT) {
                    DEBUG_PRINTF("%d:Return to TXT\r\n:", preFlagLF);
                    while (addr % 16 > 0) DisplayBinary(&addr, 0);
                    
                    res = f_close(&BinFile);    // 境界文字が来たので、ファイル閉じる <<<<<<<<<<<<<<<<<<

                    addr = 0;
                } else {
                    //上記以外のケースは、バイナリとして処理する
                    //処理するのは、preFlagLF分を加え、FlagLF分は次回へ
                    DEBUG_PRINTF("    bin len=%d +%d-%d\r\n", len, preFlagLF, FlagLF);
                    //POSTdatabin(preFlagLF);   //Binデータの処理実施
                    DisplayBinaryBuf(&addr, (uint8_t *)CRLF, preFlagLF);   //前の行の改行コードを必要に応じて
                    //バイナリの1バイトを書き込む　　ファイルはオープン済み <<<<<<<<<<<<<<<<<<<<<
                    res = f_write(&BinFile, CRLF, preFlagLF, &actualLength);
//                    DEBUG_PRINTF("res=%d (0:OK)\r\n", res);
                    DisplayBinaryBuf(&addr, (uint8_t *)st, len - FlagLF);
                    res = f_write(&BinFile, st, len - FlagLF, &actualLength);
//                    DEBUG_PRINTF("res=%d (0:OK)\r\n", res);
                }

                if (ContentLength == 0) break;  //終了
                
            }
            else {  //TXT
                if (ans == 0) {
                    if (len == 2) {
                        //CRLFのみの行
                        DEBUG_PRINTF("OnlyCRLF\r\n");
                        if (StartContent == 0) {
                            DEBUG_PRINTF("---- Start Content ----\r\n");
                            StartContent = 1;   //ヘッダ終わりの空行が来たら、以降メッセージボディ
                        }
                        if (InBoundary == 1) {
                            //boundary内の空行以降はバイナリデータ
                            BinOrText = BIN;    //バイナリ部に移行
                            FlagLF = 0;
                            DEBUG_PRINTF("bbbbbbbbb Start Binary area bbbbbbbbb\r\n");
                        }
                    } else {
                        //ヘッダの処理
                        st[len -2] = '\0';  //処理の簡素化のため、改行コード部を除く
                        POSTdata(st);     //1行の処理
                    }
                }
                else {
                    //続きを読み込む必要があるかを判断
                    if (StartContent) {
                        //index2で送信された時の処理
                        if (WHICHJOB == SETCUSTOM) {
                            DEBUG_PRINTF("index2処理%d\r\n", len);
                            //データが多いので、256バイトいっぱいになっている場合あり。ContentLength=0まで処理を継続する
                            //改行ではなく&で区切られているので、それ毎にパラメータとデータを処理していく
                            st[len] = '\0'; //終端を付加
                            
                            POSTdata2(st);  //POSTされたデータの処理
                            if (ContentLength == 0) {
                                PushEvent(EventFormInit);
                                StoreCustomData();  //EEPROMに書き込む
                                break;
                            }
                        }
                        else
                            if (ContentLength == 0) {
                            //改行も終端もなしで終了するケースあり removeの時など
                            if (len < sizeof(st)) st[len] = '\0';
                            DEBUG_PRINTF("改行なしでContent終\r\n");
                            POSTdata(st);
                            break;  //ここでPOST処理終了
                        }
                    }
                    else {
                        DEBUG_PRINTF("******* ERROR ********\r\n");
                        return; //ここには来ないはず
                    }
                }
            }
        }   //POSTのリクエストを処理するループ
        if (ContentLength == 0) {
            //マルチパートの処理が終わっていない時は、次のパケットに備える
            DEBUG_PRINTF("\r\n--- FINISH POST ---\r\n");
            
            if (WHICHJOB == UPLOADCUSTOM) {
                //カスタムデータをアップロードした時、そのデータをカスタム変数に反映させる
                f_chdir(DataFolder);
                LoadCustomData("Data0.dat");
                StoreCustomData();
                PushEvent(EventFormInit);
            }
            // クライアントにHTMLテキストを送信する
            //完了メッセージを表示
            ESP_HtmlSnd2(clientID, comp_txt1);
            sprintf(st, " ");
            if (WHICHJOB == UPLOADFONT) {
                sprintf(st, "<h1>Completed upload file</h1>");
                ESP_HtmlSnd2(clientID, st);
                sprintf(st, "ファイル:%s(%d個)をアップロードしました<br>", Dirname, FileCount);
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
            //ここでCLOSEして切断すると、ブラウザから送信できなくなる
            //ブラウザ側からCLOSED信号を受け付けた時は、ブラウザで操作すると、CONNECTメッセージ
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
 * waitTouch=0: 表示終了後、自動的に処理終わらせる場合
 *           1: タッチしたら次の処理に移行させる
 */
void OpeningScreen(uint8_t waitTouch) {
    int8_t jj, res;
    int16_t x, y, w, h;
    //カラーバーと同じく、75%の白、黄、シアン、緑、マゼンタ、赤、青
    uint16_t color[] = {
//        WHITE, YELLOW, CYAN, GREEN, MAGENTA, RED, BLUE
        0xBDF7, //白
        0xBDE0, //黄
        0x05F7, //シアン
        0x05E0, //緑
        0xB817, //マゼンタ
        0xB800, //赤
        0x0017, //青
    };

    lcd_fill(BLACK); //画面をクリア
//    SetFont(FontNormal, NULL);
    display_setTextColor(WHITE, BLACK);     //TextcolrとTextbgColorを設定
    display_setTextSize(0x12);
    
    display_setCursor(0, 0);
    display_puts(TextOpenTitle);
    
    display_setTextPitchAdj(-1, 2);    //ピッチをデフォルトから変更

    display_setCursor(0, 30);
    for (jj = 0; jj < 5; jj++) {
        display_puts(TextOpenMsg[jj]);
    }
    
    display_setTextPitchAdj(0, 0); //元に戻す

//    return; //デバッグ中は、以降スキップ
    
    //SDカードがあれば、bmpを表示する　エラーの時は、リトライさせない
    res = ReadBmp16("Opening.bmp");
    DEBUG_PRINTF("res(opening file)=%d\r\n", res);

    //カラーバー表示
    x = 0;
    y = 220;
    w = 40;
    h = 20;
    for (jj=0; jj<7; jj++) {
        display_fillRect(x, y, w, h, color[jj]);
        x += w;
    }

    if (waitTouch) {
        //タッチしたら、終了するというメッセージ出す
        display_drawChars(20, 201, (char*)TouchMsg, WHITE, WHITE, 0x12);
    }
    else {
        __delay_ms(1000);
    }
}


/*
 * 照度センサで、明るさを取得し、それに応じてバックライトを設定する
 */
void AdjustBacklight() {
    static uint16_t Brightness = 1023;
    uint16_t BackLight;
    uint16_t data;

    //ADCの結果は、12bitの0-4095。明るいと大きな数値
    //ただし、暗いとADCの結果がかなりばらつく
    //昼間なら3.07V@3.29Vで、954-958、夜の蛍光灯下だと、50-150位だったか
    //改めて見てみると、5秒ごとの測定、蛍光灯下で、1000-1500位が普通で、たまに800くらいになったりする
    //ばらつきを低減するため、8回の移動平均を取る
    //ADC_GetConversionでチャネル設定、ADC開始-取得まで実行
    data = ADCC_GetSingleConversion(PhotoTr);
    Brightness = Brightness/8*7 + data/4;
    
    // Dutyを変更してバックライトの明るさを変更
    // Brightnessが一定数以上の時は、バックライトはほぼ常時点灯。
    BackLight = Brightness/8*3 + 10;
    // BackLightは、最低10-max999   0だと全く画面見えなくなるので、+10としている
    if (BackLight > 999) BackLight = 999;
    //PWMは、10ビットの解像度 (TMR6の周期=PR6=0xffに設定した時)
//    DEBUG_PRINTF("Bright=%4d, BackL=%d\r\n", Brightness, BackLight);  //輝度のレベルを表示
    
    PWM6_LoadDutyValue(BackLight);
}

/*
 * 1秒ごとに更新される時刻を利用して実施する処理
 * 現状3つ
 * 　・秒の一位の数字が9の時:　温湿度センサ
 * 　・秒の一位の数字が3か8の時：　照度チェック
 * 　・毎時:　日付の更新チェック
 */
void TimeEvent() {

    if ((DateTime[0] & 0x0f) == 1) {
        //温湿度は、秒の下1桁が1の時に更新
        Get_TempHumidity(&Temp, &Humidity);
        PushEvent(EventTemperatureUpdate);
        PushEvent(EventHumidityUpdate);
    }   

    if (((DateTime[0] & 0x0f) % 5) == 3) {
        //秒の下1桁が3,8の時に照度チェック
        //照度センサで、明るさを5秒ごとに取得する
        AdjustBacklight();
    }
    
    //日付が変わったかは、特にトリガないので、ここでチェック。毎時00分の時だけ実施
    if (DateTime[1] == 0x00) {
        if (WriteYMD()) { //更新あれば、日付も更新する
            PushEvent(EventDateUpdate);
            //日付が変わったら、SNTP受信させる
            PushEvent(EventGetTime);
        }
    }
}

/*
 * スライドSWオフの処理ルーチン
 */
void SlideSWoffProc() {
   
    SlideSWStatus = SlideSWoff;
    AlarmSoundOff();
    RTC_resetAlarm();
    AlarmStatus = 0;    //スライドSWでOffした時は、snooze終了
    SnoozeCount = 0;
    DrawAlarmTime(AlarmTime, SlideSWStatus, &CurrentRsc[AlarmObj]);    //アラーム表示の更新
}

void WriteAlarm() {
    uint8_t jj, tmp;
    uint16_t addr;

    //EEPROMにアラーム時刻を書き込み、電源切れても再投入時、使えるように
    addr = AddressAlarm;
    for (jj = 0; jj < 3; jj++) {
        tmp = DATAEE_ReadByte(addr);
        if (tmp != AlarmTime[jj]) {
            //変化があるデータのみ書き換え
            DATAEE_WriteByte(addr, AlarmTime[jj]);
        }
        addr++;
    }
}

/*
 * スライドSWオンの処理ルーチン
 * アラームセットして、その時刻をEEPROMに書き込む
 */
void SlideSWonProc() {

    RTC_setAlarmTime(AlarmTime);
    WriteAlarm();
    SlideSWStatus = SlideSWon;
    DrawAlarmTime(AlarmTime, SlideSWStatus, &CurrentRsc[AlarmObj]);    //アラーム表示の更新
}

/*
 * RotCountが0でない時は、処理する
 * ロータリーは、連続的にイベント発生するが、イベント発生毎に処理できなくてもよく、
 * RotCount=0でない時だけ処理すれば良い
 */
void RotaryProcedure() {
    int16_t delta;  //8bitだと不足することを想定
    uint8_t mm, hh;

    //ノーマル時のロータリーSWはアラーム時刻を変更
    if (RotCount == 0) return;

    delta = RotCount;
    RotCount = 0;
    
    mm = Bcd2Hex(AlarmTime[0]);
    if (CurrentRsc[TimeObj].format != ANALOGCLOCK) {
        //デジタル時計の時は1分単位
        //早く回転させたら、倍速、4倍速で　　ただし、deltaは、int8ということに注意
        if ((delta > 2) || (delta < -2)) {
            //8倍加速まで
            if (Accel < 8) Accel = Accel * 2;
            delta = delta * Accel;
        }
        else Accel = 1;
    } else {
        //アナログ時計の場合
        delta = delta * 5;  //5分単位に
        mm = mm - (mm % 5);    //5分単位に切り下げ
    }
    
    hh = Bcd2Hex(AlarmTime[1]);
    IncDecTime(delta, &hh, &mm);
    AlarmTime[0] = Hex2Bcd(mm);
    AlarmTime[1] = Hex2Bcd(hh);
    
    //アラームSWがonだったら、RTCのアラーム時刻を変更
    if (SlideSWStatus == SlideSWon) RTC_setAlarmTime(AlarmTime);
    WriteALM2EEPROM = 300;  //300s後にEEPROMに書き込む
    
    //EEPROMに書き込むタイミングが問題。ロータリーをぐるぐる回している時は、
    //何度もEEPROMにアラーム時刻を書き込みたくない
    //アラームSWをOnにした時だけに限定する
}


/*
 * メイン画面の表示を更新する
 * CurrentRscの情報を元に初めて描画する時に呼び出される
 * 設定ボタン等は、ここで描画
 */
void DrawForm() {
    
    //時刻表示は変化があった所だけ表示更新するので、BCDではありえない数値を設定しておく
//    resetPreDateTime();
    lcd_fill(TextbgColor);

    //設定ボタン　　Webアクセスでカスタマイズできるので、設定ボタンは必須ではなくなったので
    DrawGearIcon(&CurrentRsc[GearObj]);
    DEBUG_PRINTF("Gear w=%d,%d\r\n", CurrentRsc[GearObj].xw, CurrentRsc[GearObj].yw);

    //時刻表示更新
    resetPreDateTime();
    DrawTime(DateTime, &CurrentRsc[TimeObj]);
    DEBUG_PRINTF("Time w=%d,%d\r\n", CurrentRsc[TimeObj].xw, CurrentRsc[TimeObj].yw);

    //日付の更新
    DrawDate(DateTime, &CurrentRsc[DateObj]);    //日付を描画
    DEBUG_PRINTF("Date w=%d,%d\r\n", CurrentRsc[DateObj].xw, CurrentRsc[DateObj].yw);
    DrawCalendar(&CurrentRsc[CalendarObj]);  //日付更新したらカレンダーも更新
    DEBUG_PRINTF("Calendar w=%d,%d\r\n", CurrentRsc[CalendarObj].xw, CurrentRsc[CalendarObj].yw);

    //温湿度が更新されたら
    DrawTemperature(Temp, &CurrentRsc[TempObj]);
    DEBUG_PRINTF("Temp w=%d,%d\r\n", CurrentRsc[TempObj].xw, CurrentRsc[TempObj].yw);
    DrawHumidity(Humidity, &CurrentRsc[HumidityObj]);
    DEBUG_PRINTF("Humid w=%d,%d\r\n", CurrentRsc[HumidityObj].xw, CurrentRsc[HumidityObj].yw);

    //アラーム表示の更新　　スライドSWの状態で色を変える
    DrawAlarmTime(AlarmTime, SlideSWStatus, &CurrentRsc[AlarmObj]);
    DEBUG_PRINTF("Alm w=%d,%d\r\n", CurrentRsc[AlarmObj].xw, CurrentRsc[AlarmObj].yw);

}


/*
 * メインのイベントハンドラ
 * 
 */
void MainEventHandler() {
    uint8_t evt;
    char str[100];

    evt = PopEvent();
    if (evt == EventNone) {
        __delay_ms(10); //イベントない時は、無駄に空回りさせないようwait
        return;
    }
#ifdef DEBUG
//    DEBUG_PRINTF("e=%03d\r\n", evt);
#endif
    
    switch (evt) {
        case EventTimeUpdate:
            RTC_read(DateTime);
            resetCTFG();    //RTCの1秒ごとの割込み後、解除必要
            DrawTime(DateTime, &CurrentRsc[TimeObj]);    //時刻表示更新
            TimeEvent();
            break;

        case EventDateUpdate:
            DrawDate(DateTime, &CurrentRsc[DateObj]);    //日付を描画
            DrawCalendar(&CurrentRsc[CalendarObj]);  //日付更新したらカレンダーも更新
            break;

        case EventFormInit:
            resetCTFG();    //RTCの割込み解除
            //リソースを初期化して、画面描画する
            CurrentRsc = RscData[DisplayMode]; //モードに対応したリソースを設定
            DEBUG_PRINTF("DisplayMode=%d\r\n", DisplayMode);
            DrawForm();
            PixelXY[0] = 9999;  //データがないことを示す
            break;

        case EventSlideSWoff:       //スライドSWのOff処理
            SlideSWoffProc();
            break;
        case EventSlideSWon:        //スライドSWのOn処理
            SlideSWonProc();
            break;
        case EventRotary:
            RotaryProcedure();
            DrawAlarmTime(AlarmTime, SlideSWStatus, &CurrentRsc[AlarmObj]);    //アラー時刻表示の更新
            //アナログ時計の場合は、DrawTimeも呼び出さないとアラーム針の更新がされないが、1秒毎に更新されるので、特に問題はない
            //アナログ時計全体を再表示するので、ここで更新してもそれほどレスポンスよくみえない
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
            GetTimeFromNTP(DateTime);       //NTPサーバから現時刻を取得
            break;
    }    
}


/*
 * 初期データを設定する
 * 初めて起動した時は、全初期化、2回目以降はEEPROMからデータ取得
 * 
 */
void SetInitialData()
{
    uint8_t jj, kk, dd;
    uint16_t addr;
    char *p;

    //初めて起動した時は、全初期化、2回目以降はEEPROMからデータ取得
    if (DATAEE_ReadByte(AddressInit) == 0xff) {
        TouchAdjust();    //タッチの調整を実施し、そのデータをEEPROMに保持
      
        lcd_fill(BLUE); //ここで画面をクリアしないと、タッチが成功したのかわからない時がある

        DATAEE_WriteByte(AddressInit, 0x55);
        //アラーム時刻を初期化
        addr = AddressAlarm;
        for (jj = 0; jj < 3; jj++) {
            DATAEE_WriteByte(addr, AlarmTime[jj]);
            addr++;
        }
        DisplayMode = DisplayMode0; //最初の表示モードは、外部データ不要な0に
        DATAEE_WriteByte(AddressDisplayMode, DisplayMode);
        //カスタムデータの初期化
        StoreCustomData();  //EEPROMにも書き込み2回目以降の起動時に読み出すデータがあるようにする

    } else {
        //2回目以降の電源投入なら、タッチ座標補正のデータを読み出す
        for (jj = 0; jj < 4; jj++) TouchXY[jj] = DATAEE_ReadWord(AddressTouch + jj*2);
        
        //日付、アラーム設定を読み出す
        addr = AddressYMD;
        for (jj=0; jj<4; jj++) {
            dd = DATAEE_ReadByte(addr++);
            if (dd != 0xff) DateTime[jj+3] = dd;    //適切な書き込みがされていなかった場合を回避
        }
        addr = AddressAlarm;
        for (jj=0; jj<3; jj++) AlarmTime[jj] = DATAEE_ReadByte(addr++);
        //前回終了時の表示モードにする
        DisplayMode = DATAEE_ReadByte(AddressDisplayMode);
        
        //カスタム設定を読み出す
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

    //Touch/SlideSWは、TMR5の10ms割り込みで状態チェック
    TMR5_SetInterruptHandler(Timer5Handler);

    // BackLightを照度センサの結果を元に点灯
    AdjustBacklight();
    
    //LCDの初期化
    lcd_init();
    lcd_fill(BLUE); //画面をクリア

    SetFont(FontNormal, NULL);  //初期フォント設定

    // RTCの初期化
    init_RTC(DateTime);
    //アラームの初期化
    TMR0_SetInterruptHandler(SoundHandler); //アラーム音のTMR0の初期設定    
    //起動音
    AlarmSoundOn(AlarmTwice);

    //初めて起動した時は、タッチ調整、全初期化、2回目以降はEEPROMからデータ取得
    SetInitialData();

    display_setTextSize(0x12);
    display_setCursor(10, 50);
    display_setTextColor(WHITE, WHITE); //TextcolrとTextbgColorを設定
    display_puts("Initializing");   //Touch調整でInitializingの文字が消えてしまう

    //ESPの初期化
    IO15_TRIS = 1;  //電源投入時Lowが印加されたら、その後すみやかにHi-Zにする

    display_setWrap(true);
    ans = ESP_Init();
    DEBUG_PRINTF("ESP Init ans = %d\r\n", ans);
    display_setWrap(false);
    
    //オープニング画面を表示
    OpeningScreen(0);

    GetTimeFromNTP(DateTime);       //NTPサーバから現時刻を取得
    INT0_SetInterruptHandler(RTC_handler);  //RTC_INTA (1s毎)の割り込み処理
    IOCBF4_SetInterruptHandler(AlarmWHandler); //アラーム Alarm Wの割り込み
    resetCTFG();
    
    //サーバモードに移行
    DEBUG_PRINTF("Server Startup\r\n");
    // WiFiモジュールのESP-WROOM02をAPとしサーバーモードで起動させる
    ans = ESP_ServerInit();
    if (ans == 0) {
        DEBUG_PRINTF("Start-up Done! \r\n");
    } else {
        DEBUG_PRINTF("RUN is suspended. Error:ans=%d\r\n", ans);
        while(1) ;       // 処理中断　　再度トライさせてみても良いかも
    }

    // ESP-WROOM02自身のIPアドレスを表示する　このIPにPC等のブラウザからアクセスする
    ESP_GetIPaddress(st);
    DEBUG_PRINTF("ESP IP=%s\r\n", st);

    // ロータリー用TMR1の割り込み設定
    TMR1_SetInterruptHandler(RotaryHandler);

    DEBUG_PRINTF("Finish initialization\r\n");

    //状態を初期化する

    DEBUG_PRINTF("Initial folder status\r\n");
    f_chdir("/");
    GetFontFolder("/");    //ルートの全フォルダを取り出す
    
    DEBUG_PRINTF("OK to access from browser\r\n");

    // 電源投入直後の最初のイベントは、フォームの初期化
    ClearEventBuffer();     //イベントバッファの初期化
    SetEventHandler(MainEventHandler);
    PushEvent(EventFormInit);
    
    while (1)
    {
        // Add your application code
        PacketProcess();
        
        EventHandler();
        //割込みのイベント処理
        if (TouchStart & 0x01) {
            if (GetTouchLocation(&TouchRawX, &TouchRawY) == 0) {
                //ここでタッチ座標を取得して、イベント発生
                TransCoordination(TouchRawX, TouchRawY, &TouchX, &TouchY);
                DEBUG_PRINTF("%d, %d\r\n", TouchX, TouchY);
                PushEvent(EventTouchDown);
                //タッチしたらピッという音を出す
                AlarmSoundOn(AlarmSingle);
            }
            TouchStart &= 0xfe;
        } else if (TouchStart & 0x02) {
            //長押し
            if (GetTouchLocation(&TouchRawX, &TouchRawY) == 0) { //ここでも座標取得
                TransCoordination(TouchRawX, TouchRawY, &TouchX, &TouchY);
                PushEvent(EventTouchLong);
            }
            TouchStart &= 0xfd;
        } else if (TouchStart & 0x04) {
            //リピートが有効時、300ms毎にタッチダウンイベントを発生させる
            if (RepeatKey > 30) {
                if (GetTouchLocation(&TouchRawX, &TouchRawY) == 0) { //ここでも座標取得
                    TransCoordination(TouchRawX, TouchRawY, &TouchX, &TouchY);
                    //リピートが有効時、300ms毎にリピートイベントを発生させる
                    PushEvent(EventTouchRepeat);
                    RepeatKey = 1;  //0ではリピートdisableになるので、1にリセット
                }
            }
            TouchStart &= 0xfb;
        } else if (TouchStart & 0x08) {
            //タッチアップ　　座標はタッチダウン中の処理で取得
            PushEvent(EventTouchUp);
            RepeatKey = 0;  //リピート解除
            TouchStart = 0;
        }
 
        //ロータリーのイベントは相当数になるので、RotCountに値が残っていたらイベント発生
        if (RotCount != 0) {
            PushEvent(EventRotary);
        }

        //スライドSW
        if (SlideSWStatus == SlideSWon2off) {
            PushEvent(EventSlideSWoff);
        } else if (SlideSWStatus == SlideSWoff2on) {
            PushEvent(EventSlideSWon);
        }

        //時刻の更新
        if (NeedUpdateTime) {
            PushEvent(EventTimeUpdate);
            NeedUpdateTime = 0;      
            if (WriteALM2EEPROM > 0) {
                WriteALM2EEPROM--;
                if (WriteALM2EEPROM == 0) WriteAlarm();
            }
        }
        
        if (AlarmStatus == 1) {
            AlarmSoundOn(AlarmSound0);    //アラームの最初の音を鳴らす
            AlarmStatus++;
            DEBUG_PRINTF("Alarm on\r\n");
        }

    }
}
/**
 End of File
*/