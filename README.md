# STM32F103C8T6 环境监测终端
基于HAL库实现的嵌入式采集终端，支持电压采集、光敏检测、OLED自动对比度调节、W25Q数据掉电保存、按键记录、LED告警。

## ✨ 功能清单
1. **ADC双通道采集**
   - 通道0：实时电压采集 `adc_volt_now`（由WTH118电位器调节）
   - 通道1：光敏电阻采集（光照越强，输出电压越低，ADC值越小）
2. **SSD1306 0.96寸 I2C OLED（4引脚：SDA/SCL/VCC/GND，无BL背光引脚）**
   - 软件I2C驱动，地址0x78
   - 8×16 ASCII字模显示
   - 根据光敏采样值**自动调节SSD1306对比度(0x81命令)**，实现屏幕自适应亮度
   - 显示：实时电压 NOW、光敏电压值 LUV
3. **W25Qxx SPI Flash**
   - SPI1驱动，读取/写入上次保存电压，掉电不丢失
4. **按键功能**
   - 按下按键：将W25Q内保存的电压LAST覆盖NOW显示在OLED上
5. **LED告警提示 PB1**
   - 光敏低于阈值，LED非阻塞闪烁告警；光照正常LED熄灭
6. **非阻塞代码设计**
   - 全部基于`HAL_GetTick()`时间戳，无长`HAL_Delay`，多任务并行，不阻塞ADC、OLED、Flash读写

## 📌 硬件接线
> STM32F103C8T6

### OLED（软件I2C）
| STM32 | OLED |
| ---- | ---- |
| PB6 | SCL |
| PB7 | SDA |
| 3.3V | VCC |
| GND | GND |
> ⚠️ 模块为4Pin版本，无BL背光引脚，**无法硬件PWM调节背光**，只能使用SSD1306内部对比度命令调节画面灰度。

### W25Q Flash（SPI1）
| STM32 | W25Q |
| ---- | ---- |
| PA5 | SCK |
| PA6 | MISO |
| PA7 | MOSI |
| PA4 | CS |
| 3.3V | VCC |
| GND | GND |

### ADC采集
| STM32 | 外设 |
| ---- | ---- |
| PA0 | 待测电压采集 |
| PA1 | 光敏电阻 |

### 按键 & LED
| STM32 | 外设 |
| ---- | ---- |
| PA2 | 保存按键（下拉输入） |
| PB1 | 告警LED |

## 📦 工程结构
STM32F103_Monitor/
├── Core/
│   ├── Inc/
│   │   ├── main.h
│   │   ├── oled.h        // OLED 软件 I2C 驱动、字模
│   │   ├── w25q.h        // W25Q SPI 驱动
│   │   └── stm32f1xx_hal_conf.h
│   └── Src/
│       ├── main.c        // 主逻辑：ADC、按键、LED、OLED 刷新、自动对比度
│       ├── oled.c
│       ├── w25q.c
│       └── stm32f1xx_it.c
├── Drivers/              // ST 官方 HAL 库，CubeMX 自动生成
├── .gitignore
└── README.md

## 🧠 核心原理说明
1. **OLED自适应亮度**
光敏特性：光照越强 → 光敏输出电压越低 → ADC采样值越小。
公式：uint8_t contrast  = (uint8_t)(0xFF * (1.0f - (adc_buf[1] / 4095.0f)));
限制最小对比度 `0x15`，防止黑暗环境屏幕完全黑屏；使用`HAL_GetTick()`做 200ms 节流，避免频繁发送 I2C 命令占用总线。

  因为 OLED 无 BL 引脚，不能硬件 PWM 背光调光，使用 SSD1306 寄存器命令 `0x81` 修改画面对比度。
2. **W25Q 存储逻辑**

- 上电读取 Flash 中保存的电压值，作为`last`初始值
- 按键按下：读取当前`adc_volt_now`，写入 W25Q，更新`last`
 ⚠️ W25Q 写入前必须擦除扇区，不可直接覆盖字节。

3. **非阻塞 LED 闪烁告警**
使用`HAL_GetTick()`时间戳，不使用`HAL_Delay`，不影响其他任务。光敏低于阈值，PB1 闪烁；光照达标，LED 强制熄灭。
4. **ADC 双通道**
ADC1，PA0、PA1，单次采样，读取原始 ADC 值用于亮度计算；浮点换算成电压用于 OLED 屏幕显示。

## ⚙️ CubeMX 配置要点

1. RCC：外部高速晶振 HSE，72MHz 系统时钟
2. GPIO
   - PB6、PB7：推挽输出（软件 I2C）
   - PB1：推挽输出（LED）
   - PA2：下拉输入（按键）
3. SPI1：全双工主机，波特率分频 8
4. ADC1：通道 PA0、PA1，12bit，软件触发
5. SYS：Debug Serial Wire

## 📝 编译 & 烧录

1. 打开 STM32CubeIDE，导入工程
2. 编译，无报错后使用 ST-Link 下载
3. 上电：OLED 初始化，读取 W25Q 历史电压，开始实时采集
4. 按下按键：保存当前电压到 Flash，屏幕更新 last 值

## ❗ 常见踩坑记录

1. **OLED 黑屏 / 乱码**
   - 检查 I2C 地址，本项目 OLED 地址 `0x78`
   - 软件 I2C 时序，增加 I2C_Delay_us；优先直连芯片，减少杜邦线长线干扰
   - 字模错误：使用 8*16 标准字模，字模位序匹配 SSD1306
2. **W25Q 读出来一直是 0**
   - SPI 引脚方向，CS 引脚电平；读 ID 命令 0x90，先验证 W25Q 通信是否正常
   - 写操作前必须扇区擦除
3. **自动亮度逻辑反了**
   - 本项目光敏特性：光照越强 ADC 值越小；如果更换光敏，需要修改对比度计算公式
4. **浮点运算卡顿**
STM32F1 无硬件 FPU；对性能敏感场景，可将对比度计算替换为纯整数运算版本。
