// 第 0 课归档：ESP32-S3-N16R8 硬件自检与板载 RGB 循环测试
#include <Arduino.h>

#ifndef RGB_LED_PIN
#define RGB_LED_PIN 48
#endif

static uint32_t counter = 0;

template <typename... Args>
void dualPrintf(const char *fmt, Args... args) {
    Serial.printf(fmt, args...);
    Serial0.printf(fmt, args...);
}

void dualPrintln(const char *msg) {
    Serial.println(msg);
    Serial0.println(msg);
}

void setup() {
    Serial.begin(115200);
    Serial0.begin(115200);

    unsigned long start = millis();
    while (!Serial && (millis() - start < 3000)) {
        delay(10);
    }

    dualPrintln("\n========================================");
    dualPrintln("  ESP32-S3-N16R8 入门测试程序启动成功！");
    dualPrintln("========================================");
    dualPrintf("芯片型号: %s (Rev %d, %d 核)\n",
               ESP.getChipModel(), ESP.getChipRevision(), ESP.getChipCores());
    dualPrintf("CPU 主频: %u MHz\n", ESP.getCpuFreqMHz());
    dualPrintf("Flash 大小: %.2f MB\n", ESP.getFlashChipSize() / (1024.0 * 1024.0));

    if (psramFound()) {
        dualPrintf("PSRAM 状态: 已启用 (总计 %.2f MB, 可用 %.2f MB)\n",
                   ESP.getPsramSize() / (1024.0 * 1024.0),
                   ESP.getFreePsram() / (1024.0 * 1024.0));
    } else {
        dualPrintln("PSRAM 状态: 未检测到 (请检查 platformio.ini 的 qio_opi 配置)");
    }
    dualPrintf("可用内部 RAM (Heap): %u KB\n", ESP.getFreeHeap() / 1024);
    dualPrintln("----------------------------------------");
}

void loop() {
    const uint8_t colors[4][3] = {
        {16, 0, 0},
        {0, 16, 0},
        {0, 0, 16},
        {0, 0, 0}
    };
    const char *colorNames[4] = {"红 (Red)", "绿 (Green)", "蓝 (Blue)", "灭 (Off)"};

    uint8_t idx = counter % 4;
    neopixelWrite(RGB_LED_PIN, colors[idx][0], colors[idx][1], colors[idx][2]);

    dualPrintf("[%lu] 运行正常 | 耗时: %lu s | RGB(GPIO%d): %s | 空闲内存: %u B\n",
               (unsigned long)counter,
               (unsigned long)(millis() / 1000),
               RGB_LED_PIN,
               colorNames[idx],
               ESP.getFreeHeap());

    counter++;
    delay(1000);
}
