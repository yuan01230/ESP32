/**
 * 第 2 课归档：串口双向通信（命令行控制台）
 *
 * 功能：
 *   1. 串口接收一整行指令，清理首尾空白和回车符并回显
 *   2. `help`：打印可用命令列表
 *   3. `info`：打印芯片型号、CPU 主频、运行时间、剩余 RAM 与 PSRAM
 *   4. `rgb <r> <g> <b>` 与 `off`：使用 sscanf 解析并校验范围 (0~255)，实时控制板载 RGB 灯
 *   5. 未知命令友好提示
 */
#include <Arduino.h>

// ===== 硬件引脚定义（YD-ESP32-S3）=====
constexpr uint8_t RGB_LED_PIN = 48;  // 板载 WS2812 RGB 彩灯

void printHelp() {
    Serial.println("\n---------- 可用命令列表 ----------");
    Serial.println("  help              - 显示本帮助信息");
    Serial.println("  info              - 查看芯片主频、内存与运行时间");
    Serial.println("  rgb <r> <g> <b>   - 设置 RGB 颜色 (范围 0~255，建议先用低亮度如 rgb 20 0 20)");
    Serial.println("  off               - 关闭板载 RGB 灯");
    Serial.println("----------------------------------\n");
}

void handleCommand(String cmd) {
    // 统一转为小写，大小写命令均可识别
    cmd.toLowerCase();

    if (cmd == "help") {
        printHelp();
    } else if (cmd == "info") {
        Serial.printf("[芯片型号]：%s\n", ESP.getChipModel());
        Serial.printf("[CPU主频]：%u MHz\n", ESP.getCpuFreqMHz());
        Serial.printf("[运行时间]：%lu s\n", millis() / 1000);
        Serial.printf("[剩余Ram]: %u Byte\n", ESP.getFreeHeap());
        Serial.printf("[剩余PSRAM]: %u Byte\n", ESP.getFreePsram());
    } else if (cmd == "off") {
        neopixelWrite(RGB_LED_PIN, 0, 0, 0);
        Serial.printf("[执行] RGB 灯已关闭! \n");
    } else if (cmd.startsWith("rgb ")) {
        int r = 0, g = 0, b = 0;
        int parsed = sscanf(cmd.c_str(), "rgb %d %d %d", &r, &g, &b);
        if ((parsed == 3) && (r >= 0 && r <= 255) && (g >= 0 && g <= 255) && (b >= 0 && b <= 255)) {
            neopixelWrite(RGB_LED_PIN, r, g, b);
            Serial.printf("[执行] 已设置 RGB = (%d, %d, %d)\n", r, g, b);
        } else {
            Serial.printf("[输入错误]：r:%d,g:%d,b:%d \n", r, g, b);
        }
    } else {
        Serial.printf("[未知命令] \"%s\"，输入 help 查看所有命令\n", cmd.c_str());
    }
}

void setup() {
    Serial.begin(115200);
    Serial.setTimeout(50);

    unsigned long start = millis();
    while (!Serial && (millis() - start < 3000)) {
        delay(10);
    }

    neopixelWrite(RGB_LED_PIN, 0, 0, 0);

    Serial.println("\n==========================================");
    Serial.println("  第 2 课：串口双向通信（命令行控制台）");
    Serial.println("==========================================");
    printHelp();
    Serial.print("> ");
}

void loop() {
    if (Serial.available() > 0) {
        String line = Serial.readStringUntil('\n');
        line.trim();
        if (line.length() > 0) {
            Serial.printf("[收到] %s \n", line.c_str());
            handleCommand(line);
            Serial.print("> ");
        }
    }
}
