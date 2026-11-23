/**
 * 第 1 课：GPIO 与按键交互（YD-ESP32-S3）
 *
 * 目标：
 *   1. 使用板载 BOOT/USR 按键（GPIO0，按下为 LOW，松开为 HIGH）
 *   2. 实现软件消抖（30ms），避免按一次触发多次
 *   3. 短按：循环切换 6 种 RGB 颜色（红 -> 绿 -> 蓝 -> 紫 -> 青 -> 白）
 *   4. 长按（>= 1000ms）：开启 / 关闭 RGB 灯光
 *
 * 请依次完成下面的 TODO 1 ~ TODO 4！完成后告诉我，我帮你审查、烧录并验收。
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
    // ============================================================
    // TODO 1：根据 isLedOn 控制 RGB 灯
    // 提示：
    //   - 如果 isLedOn 为 false：调用 neopixelWrite(RGB_LED_PIN, 0, 0, 0);
    //   - 如果 isLedOn 为 true ：取出 COLORS[colorIndex]，
    //     调用 neopixelWrite(RGB_LED_PIN, c.r, c.g, c.b);
    // ============================================================

}

void setup() {
    Serial.begin(115200);
    unsigned long start = millis();
    while (!Serial && (millis() - start < 3000)) {
        delay(10);
    }

    // ============================================================
    // TODO 2：配置按键引脚模式并初始化灯光
    // 提示：
    //   1. 调用 pinMode(BOOT_BTN_PIN, INPUT_PULLUP);
    //      （思考：为什么要用 INPUT_PULLUP 而不是普通 INPUT？）
    //   2. 调用 updateLed(); 让开机默认亮起第一种颜色
    // ============================================================

    Serial.println("\n=== 第 1 课：GPIO 与按键交互 启动 ===");
    Serial.printf("初始状态: 开灯 | 颜色: %s\n", COLORS[colorIndex].name);
    Serial.println("操作说明: 短按 BOOT 键切换颜色，长按 1 秒开关灯");
}

void loop() {
    unsigned long now = millis();

    // 1. 读取当前按键的原始电平（HIGH = 未按，LOW = 按下）
    int rawState = digitalRead(BOOT_BTN_PIN);

    // ============================================================
    // TODO 3：实现按键消抖 + 按下/松开事件检测
    // 思路提示：
    //   (a) 如果 rawState != lastRawState：
    //       说明电平刚刚发生跳变（可能是真实操作，也可能是机械抖动）
    //       -> 更新 lastDebounceTime = now;
    //       -> 更新 lastRawState = rawState;
    //
    //   (b) 如果 (now - lastDebounceTime) >= DEBOUNCE_MS 并且 rawState != stableState：
    //       说明新电平已经稳定持续了 30ms，是一次真实的状态变化！
    //       -> 更新 stableState = rawState;
    //       -> 分两种情况处理：
    //          情况 1：stableState == LOW（刚刚按下）
    //            - 记录 pressStartTime = now;
    //            - 重置 longPressHandled = false;
    //            - Serial.println("[按键] 按下");
    //          情况 2：stableState == HIGH（刚刚松开）
    //            - Serial.println("[按键] 松开");
    //            - 如果 !longPressHandled（没触发过长按，说明是短按）：
    //              * colorIndex = (colorIndex + 1) % COLOR_COUNT;
    //              * isLedOn = true;
    //              * updateLed();
    //              * Serial.printf("[短按] 切换颜色 -> %s\n", COLORS[colorIndex].name);
    // ============================================================


    // ============================================================
    // TODO 4：实现长按检测（按住满 1 秒立刻响应，不用等松手）
    // 思路提示：
    //   当同时满足 3 个条件时：
    //     1. stableState == LOW（按键正处于稳定按下状态）
    //     2. !longPressHandled（这次按下还没触发过长按）
    //     3. (now - pressStartTime) >= LONG_PRESS_MS（按住时间已满 1 秒）
    //   执行：
    //     - longPressHandled = true;
    //     - isLedOn = !isLedOn;
    //     - updateLed();
    //     - Serial.printf("[长按] 灯光已%s\n", isLedOn ? "开启" : "关闭");
    // ============================================================

}
