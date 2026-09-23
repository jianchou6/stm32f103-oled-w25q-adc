# 硬件接线说明 STM32F103C8T6
|外设|STM32引脚|
|---|---|
|OLED‑SCL|PB6|
|OLED‑SDA|PB7|
|W25Q‑CS|PA4|
|SPI1‑SCK|PA5|
|SPI1‑MISO|PA6|
|SPI1‑MOSI|PA7|
|ADC光照采集|PA1|
|ADC电压采集|PA0|
|按键输入|PB0|
|告警LED|PB1|
|USART1 TX(串口上报)|PA9|

> 注意：所有模块必须共GND，否则出现I2C/SPI串口乱码通信异常。
