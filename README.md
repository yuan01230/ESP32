# YD-ESP32-S3 (N16R8) 零基础入门工程

本工程基于 **PlatformIO + Arduino 框架** 配置，已针对 **源地 YD-ESP32-S3 (N16R8)**（16MB QIO Flash + 8MB Octal PSRAM + 双 Type-C + GPIO48 RGB）完成硬件参数适配。

## 目录结构

- `LEARNING_TASKS.md`：**ESP32-S3-N16R8 零基础任务制学习课程与进度表**（推荐从这里开始打卡学习）
- `platformio.ini`：工程配置（板型、16MB Flash、8MB OPI PSRAM、串口参数）
- `src/main.cpp`：当前运行代码（第 0 课入门测试程序：串口打印硬件信息 + 板载 RGB 灯循环闪烁）

## 常用命令

```powershell
# 1. 编译工程
pio run

# 2. 编译并烧录到开发板（自动识别串口）
pio run -t upload

# 3. 打开串口监视器查看输出（按 Ctrl+C 退出）
pio device monitor

# 4. 烧录并直接打开串口监视器
pio run -t upload -t monitor

# 5. 查看当前识别到的串口设备
pio device list
```

## Type-C 接口小贴士

1. **必须使用带数据传输功能的 Type-C 线**（纯充电线只有供电，电脑设备管理器不会出现串口）。
2. 如果你的板子有**两个 Type-C 口**：
   - 标有 **COM / UART / TTL** 的口连接板载 USB 转串口芯片（如 CH343 / CH340 / CP2102），插拔稳定，最推荐日常烧录和看日志。
   - 标有 **USB / OTG** 的口直连 ESP32-S3 芯片原生 USB（GPIO19/20），免驱，支持 USB 设备模拟。
3. **手动进入下载模式**（如遇到烧录连不上时）：
   - 按住板子上的 `BOOT`（GPIO0）键不放 → 短按一下 `RST`（EN）键 → 松开 `BOOT` 键，再重新烧录。
