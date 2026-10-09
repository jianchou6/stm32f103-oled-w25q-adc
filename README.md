# STM32F103 环境监测终端（FreeRTOS多任务版本）
> 仓库分支说明
> - master：最新版本 | FreeRTOS多任务环境监测终端
> - baremetal：历史版本 | 裸机单核MCU实现，无操作系统

## 📌 项目简介
基于STM32F103C8T6，使用FreeRTOS实现多任务实时环境监测终端。
实现多任务解耦：ADC电压采集任务、OLED显示任务、按键检测任务、串口打印任务。
采用队列进行任务间数据通信，软件I2C驱动OLED，DMA采集ADC电压。

## 🛠️ 硬件资源
- MCU：STM32F103C8T6
- 外设：0.96寸I2C OLED、ADC采集模块、独立按键、LED指示灯
- 供电：3.3V

## 📋 FreeRTOS任务设计
1. Task_ADC：ADC+DMA采样电压，将采样数据通过队列发送给显示任务
2. Task_Display：接收队列数据，刷新OLED屏幕，显示电压与报警状态
3. Task_Key：按键扫描，触发数据保存/读取功能
4. Task_Serial：串口XCOM打印实时监测数据

## 📑 关键技术点
- FreeRTOS 任务创建、优先级分配
- 队列实现任务间安全数据传输，任务解耦
- 软件I2C时序驱动OLED
- DMA直接内存访问，ADC采样不占用CPU
- 非阻塞式软件定时器实现LED报警闪烁

## 📎 文档资源
- [硬件接线说明](./docs/hardware_wiring.md)
- 运行演示视频：【这里填写视频链接】

## ✨ 版本日志
- V2.0（master分支）：引入FreeRTOS，重构为多任务实时系统，任务解耦
- V1.0（baremetal分支）：裸机单核实现，基础ADC采集+OLED显示
