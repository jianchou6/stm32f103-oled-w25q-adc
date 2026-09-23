#ifndef __W25Q_H
#define __W25Q_H
#include "stm32f1xx_hal.h"
#include "main.h"

#define W25Q_CS_GPIO_Port GPIOA
#define W25Q_CS_Pin GPIO_PIN_4
#define FLASH_SAVE_ADDR  0x080000U
#define VOL_ADDR        0x1000

uint32_t W25Q_ReadID(void);
void W25Q_WaitBusy(void);
void W25Q_WriteEnable(void);
void W25Q_SectorErase(uint32_t addr);
void W25Q_WritePage(uint32_t addr, uint8_t *buf, uint16_t len);
void W25Q_Read(uint32_t addr, uint8_t *buf, uint16_t len);

void W25Q_SaveVoltage(float vol);
float W25Q_ReadVoltage(void);

#endif

