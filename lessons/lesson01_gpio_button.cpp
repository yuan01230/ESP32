/**
 * 第 1 课归档：GPIO 与按键交互（YD-ESP32-S3）
 *
 * 功能：
 *   1. 使用板载 BOOT/USR 按键（GPIO0，内部上拉输入，按下为 LOW，松开为 HIGH）
 *   2. 软件消抖（30ms），避免机械弹片抖动导致连击
 *   3. 短按：循环切换 6 种 RGB 颜色（红 -> 绿 -> 蓝 -> 紫 -> 青 -> 白）
 *   4. 长按（>= 1000ms）：开启 / 关闭 RGB 灯光
 */
#include <Arduino.h>

// ===== 硬件引脚定义（YD-ESP32-S3）=====
constexpr uint8_t BOOT_BTN_PIN = 0;   // 板载 BOOT/USR 按键
constexpr uint8_t RGB_LED_PIN  = 48;  // 板载 WS2812 RGB 彩灯

// ===== 时间参数（毫秒）=====
constexpr unsigned long DEBOUNCE_MS   = 30;    // 消抖时间：稳定 30ms 才算真实变化
constexpr unsigned long LONG_PRESS_MS = 1000;  // 长按阈值：按住 >= 1000ms 算长按

// ===== 6 种预设颜色（低亮度保护眼睛，最大 255）=====
struct ColorPreset {
    const char *name;
    uint8_t r;
    uint8_t g;
    uint8_t b;
};

constexpr ColorPreset COLORS[] = {
    {"红 (Red)",    20,  0,  0},
    {"绿 (Green)",   0, 20,  0},
    {"蓝 (Blue)",    0,  0, 20},
    {"紫 (Purple)", 16,  0, 16},
    {"青 (Cyan)",    0, 16, 16},
    {"白 (White)",  14, 14, 14},
};
constexpr size_t COLOR_COUNT = sizeof(COLORS) / sizeof(COLORS[0]);

// ===== 灯光状态 =====
bool isLedOn = true;
size_t colorIndex = 0;

// ===== 按键消抖 & 状态机变量 =====
int lastRawState = HIGH;             // 上一次读到的原始电平
int stableState  = HIGH;             // 消抖后确认的稳定电平
unsigned long lastDebounceTime = 0;  // 最近一次电平发生跳变的时刻
unsigned long pressStartTime   = 0;  // 确认「按下」的起始时刻
bool longPressHandled = false;       // 本次按下是否已经触发过长按

/**
 * 根据 isLedOn 和 colorIndex 刷新板载 RGB 灯光
 */
void updateLed() {
    if (isLedOn != true) {
        neopixelWrite(RGB_LED_PIN, 0, 0, 0);
    } else {
        neopixelWrite(RGB_LED_PIN, COLORS[colorIndex].r, COLORS[colorIndex].g, COLORS[colorIndex].b);
    }
}

void setup() {
    Serial.begin(115200);
    unsigned long start = millis();
    while (!Serial && (millis() - start < 3000)) {
        delay(10);
    }

    // BOOT_BTN_PIN 接 GND，需要内部上拉电阻
    pinMode(BOOT_BTN_PIN, INPUT_PULLUP);
    updateLed();

    Serial.println("\n=== 第 1 课：GPIO 与按键交互 启动 ===");
    Serial.printf("初始状态: 开灯 | 颜色: %s\n", COLORS[colorIndex].name);
    Serial.println("操作说明: 短按 BOOT 键切换颜色，长按 1 秒开关灯");
}

void loop() {
    unsigned long now = millis();

    // 1. 读取当前按键的原始电平（HIGH = 未按，LOW = 按下）
    int rawState = digitalRead(BOOT_BTN_PIN);

    // 2. 消抖：只要原始电平发生跳变，就刷新防抖计时
    if (rawState != lastRawState) {
        lastDebounceTime = now;
        lastRawState = rawState;
    }

    // 3. 新电平稳定超过 30ms 且与上次稳定状态不同，确认发生真实按键变化
    if ((now - lastDebounceTime >= DEBOUNCE_MS) && rawState != stableState) {
        stableState = rawState;

        if (stableState == LOW) {
            pressStartTime = now;
            longPressHandled = false;
            Serial.println("[按键]按下");
        } else {
            Serial.println("[按键]松开");
            if (longPressHandled == false) {
                colorIndex = (colorIndex + 1) % COLOR_COUNT;
                isLedOn = true;
                updateLed();
                Serial.printf("[短按] 切换颜色 -> %s\n", COLORS[colorIndex].name);
            }
        }
    }

    // 4. 长按检测：按住满 1 秒立刻开关灯，无需等松手
    if ((stableState == LOW) && (longPressHandled == false) && (now - pressStartTime >= LONG_PRESS_MS)) {
        longPressHandled = true;
        isLedOn = !isLedOn;
        updateLed();
        Serial.printf("[长按] 灯光已%s\n", isLedOn ? "开启" : "关闭");
    }
}
