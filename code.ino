#include "LedControl.h"
#include <stdlib.h> // 用於隨機數生成

// 定義 LED 控制
const int DIN_PIN = 0;
const int CS_PIN = 1;
const int CLK_PIN = 2;
const int BUTTON_PIN = 4;
const int columnMapping[8] = {1, 2, 3, 4, 5, 6, 7, 0};

LedControl lc = LedControl(DIN_PIN, CLK_PIN, CS_PIN, 1);
int buttonPressCount = 0; // 按鈕次數計數器

// 動畫週期控制變數，增加控制"每列點亮375毫秒，共3秒完成"的"共三秒"部分
const int RANDOM_DOTS_PERIOD =175;
const int BOTTOM_TO_TOP_PERIOD = 125;
const int DIAGONAL_LINES_PERIOD = 150;
const int EXPANDING_SQUARE_PERIOD = 125;
const int SCROLLING_TEXT_PERIOD = 500;

const byte START_ANIMATION[][8] = {// 右移一格，排列與下面相同
    // 叶 ("叶")
    {
        0b00000100,
        0b11100100,
        0b10111110,
        0b10100100,
        0b10100100,
        0b11100100,
        0b00000100,
        0b00000100
    } // 正確的圖形
};

const byte HAPPY_BIRTHDAY[][8] = { // 幫我每個字改小一格，以及下面的程式碼我要以跑馬燈將"HAPPY BIRTHDAY"由右至左跑過去
    // H
    {B01000010, B01000010, B01111110, B01000010, B01000010, B01000010, B00000000, B00000000},
    // A
    {B00111100, B01000010, B01111110, B01000010, B01000010, B01000010, B00000000, B00000000},
    // P
    {B01111100, B01000010, B01111100, B01000000, B01000000, B01000000, B00000000, B00000000},
    // P
    {B01111100, B01000010, B01111100, B01000000, B01000000, B01000000, B00000000, B00000000},
    // Y
    {B01000010, B00100100, B00011000, B00001000, B00001000, B00001000, B00000000, B00000000},
    // B
    {B01111110, B01000001, B01111110, B01000001, B01000001, B01111110, B00000000, B00000000},
    // I
    {B00111100, B00001000, B00001000, B00001000, B00001000, B00111100, B00000000, B00000000},
    // R
    {B01111100, B01000010, B01111100, B01001000, B01000100, B01000010, B00000000, B00000000},
    // T
    {B01111111, B00001000, B00001000, B00001000, B00001000, B01111111, B00000000, B00000000},
    // H
    {B01000010, B01000010, B01111110, B01000010, B01000010, B01000010, B00000000, B00000000},
    // D
    {B01111110, B01000001, B01000001, B01000001, B01000001, B01111110, B00000000, B00000000},
    // A
    {B00111100, B01000010, B01111110, B01000010, B01000010, B01000010, B00000000, B00000000},
    // Y
    {B01000010, B00100100, B00011000, B00001000, B00001000, B00001000, B00000000, B00000000}
};

volatile bool interruptFlag = false; // 中斷標誌

void handleButtonPress() {
    interruptFlag = true;
}

void displayAnimation() {
    for (int i = 0; i < 1; i++) {
        if (interruptFlag) return; // 中斷時退出
        for (int row = 0; row < 8; row++) {
            for (int col = 0; col < 8; col++) {
                lc.setLed(0, row, columnMapping[col], bitRead(START_ANIMATION[i][row], 7 - col));
            }
        }
        delay(1000); // 每個圖案顯示1000 毫秒
        lc.clearDisplay(0); // 清空顯示
    }
}

void displayRandomDots() {
    lc.clearDisplay(0);
    for (int i = 0; i < 6; i++) {
        if (interruptFlag) return; // 中斷時退出
        int randomRow = random(0, 8);
        int randomCol = random(0, 8);
        lc.setLed(0, randomRow, columnMapping[randomCol], true);
    }
    delay(RANDOM_DOTS_PERIOD);
    lc.clearDisplay(0);
}

void displayBottomToTopCycle() {
    for (int i = 7; i >= 0; i--) {
        if (interruptFlag) return; // 中斷時退出
        lc.setRow(0, i, B11111111); // 點亮整橫列
        delay(BOTTOM_TO_TOP_PERIOD);                 // 每列點亮 375 毫秒，共 3 秒完成
    }
    for (int i = 0; i < 8; i++) {
        if (interruptFlag) return; // 中斷時退出
        lc.setRow(0, i, 0x00);      // 熄滅整橫列
        delay(BOTTOM_TO_TOP_PERIOD);
    }
}

void displayDiagonalLines() {
    for (int i = 0; i < 8; i++) {
        if (interruptFlag) return; // 中斷時退出
        lc.clearDisplay(0);
        for (int j = 0; j <= i; j++) {
            lc.setLed(0, j, columnMapping[j], true); // 點亮左上到右下的對角線，再從右下到左上暗回去
        }
        delay(DIAGONAL_LINES_PERIOD);
        // 點亮右上到左下的對角線，再從左下到右上暗回去
        for (int j = 0; j <= i; j++) {
            lc.setLed(0, j, columnMapping[7 - j], true);
        }
        delay(DIAGONAL_LINES_PERIOD);
    }
}

void displayButtomtoTop() { 
    lc.clearDisplay(0);
    for (int i = 0; i < 4; i++) {
        if (interruptFlag) return; // 中斷時退出
        lc.setColumn(0, columnMapping[i], B11111111);     // 左側亮
        lc.setColumn(0, columnMapping[7 - i], B11111111); // 右側亮
        delay(EXPANDING_SQUARE_PERIOD);
    }
    for (int i = 3; i >= 0; i--) {
        if (interruptFlag) return; // 中斷時退出
        lc.setColumn(0, columnMapping[i], 0x00);          // 左側熄滅
        lc.setColumn(0, columnMapping[7 - i], 0x00);      // 右側熄滅
        delay(EXPANDING_SQUARE_PERIOD);
    }
}




void displayScrollingText(const byte text[][8], int length) {
    for (int offset = 0; offset < length * 8 + 8; offset++) { // 正常方向的跑馬燈，從右至左滾動
        if (interruptFlag) return; // 中斷時退出
        lc.clearDisplay(0);
        for (int row = 0; row < 8; row++) {
            for (int col = 0; col < 8; col++) {
                int charIndex = (offset + col) / 8; // 計算當前顯示字元索引
                if (charIndex >= 0 && charIndex < length) {
                    bool bit = bitRead(text[charIndex][row], 7 - (offset + col) % 8);
                    lc.setLed(0, row, columnMapping[col], bit);
                }
            }
        }
        delay(SCROLLING_TEXT_PERIOD);
    }
    lc.clearDisplay(0);
}

void setup() {
    // 增加上面的電源穩定等待時間
    delay(100); // 電源穩定延遲
    pinMode(BUTTON_PIN, INPUT);

    // 替代 digitalPinToInterrupt 使用 ATTiny85 的 PCINT 系統
    GIMSK |= (1 << PCIE);  // 啟用引腳變更中斷
    PCMSK |= (1 << PCINT4); // 啟用 PB4 對應的中斷
    sei(); // 啟用全局中斷

    lc.shutdown(0, false);          // 喚醒 MAX7219
    lc.setIntensity(0, 8);          // 設定亮度
    lc.clearDisplay(0);             // 清空顯示
    randomSeed(analogRead(0));      // 初始化隨機數種子

    // 顯示開頭動畫
    displayAnimation();
}

ISR(PCINT0_vect) {
    if (digitalRead(BUTTON_PIN) == HIGH) { // 檢查按鈕是否被按下
        interruptFlag = true;
    }
}

void loop() {
    if (interruptFlag) {
        interruptFlag = false;     // 重置中斷標誌
        buttonPressCount++;       // 增加按鈕次數
    }

    // 根據按鈕次數變換樣式
    switch (buttonPressCount % 5) {
        case 0:
            displayRandomDots(); // 隨機顯示 6 個點
            break;
        case 1:
            displayBottomToTopCycle(); // LED 從頂部到底部的動畫
            break;
        case 2:
            displayDiagonalLines(); // 顯示對角線動畫
            break;
        case 3:
            displayButtomtoTop(); // 顯示擴展方形動畫
            break;
        case 4:
            displayScrollingText(HAPPY_BIRTHDAY, 13); // 跑馬燈顯示 "HAPPY BIRTHDAY"
            break;
    }
}
