#include "w25q.h"
#include "main.h"
#include <string.h>

extern SPI_HandleTypeDef hspi1;

static void W25Q_CS_Low(void)
{
    HAL_GPIO_WritePin(W25Q_CS_GPIO_Port, W25Q_CS_Pin, GPIO_PIN_RESET);
}

static void W25Q_CS_High(void)
{
    HAL_GPIO_WritePin(W25Q_CS_GPIO_Port, W25Q_CS_Pin, GPIO_PIN_SET);
}

//✅标准JEDEC ID读取 命令0x9F
uint32_t W25Q_ReadID(void)
{
    uint8_t idbuf[3] = {0};
    uint8_t cmd = 0x9F;

    W25Q_CS_Low();
    HAL_SPI_Transmit(&hspi1, &cmd, 1, 10);
    HAL_SPI_Receive(&hspi1, idbuf, 3, 20);
    W25Q_CS_High();

    return ((uint32_t)idbuf[0] << 16) | ((uint32_t)idbuf[1] << 8) | idbuf[2];
}

void W25Q_WaitBusy(void)
{
    uint8_t sr = 0xFF;
    uint8_t cmd = 0x05;
    while (sr & 0x01)
    {
        W25Q_CS_Low();
        HAL_SPI_Transmit(&hspi1, &cmd, 1, 10);
        HAL_SPI_Receive(&hspi1, &sr, 1, 10);
        W25Q_CS_High();
        HAL_Delay(1);
    }
}

//写使能
void W25Q_WriteEnable(void)
{
    uint8_t cmd = 0x06;
    W25Q_CS_Low();
    HAL_SPI_Transmit(&hspi1, &cmd, 1, 10);
    W25Q_CS_High();
}

//扇区擦除，外部主动调用，不要放在页写内部
void W25Q_SectorErase(uint32_t addr)
{
    uint8_t cmd[4] = {0x20, (addr >> 16) & 0xFF, (addr >> 8) & 0xFF, addr & 0xFF};
    W25Q_WriteEnable();
    W25Q_CS_Low();
    HAL_SPI_Transmit(&hspi1, cmd, 4, 10);
    W25Q_CS_High();
    W25Q_WaitBusy();
}

//页写入：【不再内部擦扇区！擦除交给上层业务函数】
void W25Q_WritePage(uint32_t addr, uint8_t *buf, uint16_t len)
{
    uint8_t cmd[4] = {0x02, (addr >> 16) & 0xFF, (addr >> 8) & 0xFF, addr & 0xFF};
    W25Q_WriteEnable();
    W25Q_CS_Low();
    HAL_SPI_Transmit(&hspi1, cmd, 4, 10);
    HAL_SPI_Transmit(&hspi1, buf, len, 20);
    W25Q_CS_High();
    W25Q_WaitBusy();
}

//读数据
void W25Q_Read(uint32_t addr, uint8_t *buf, uint16_t len)
{
    uint8_t cmd[4] = {0x03, (addr >> 16) & 0xFF, (addr >> 8) & 0xFF, addr & 0xFF};
    W25Q_CS_Low();
    HAL_SPI_Transmit(&hspi1, cmd, 4, 10);
    HAL_SPI_Receive(&hspi1, buf, len, 50);
    W25Q_CS_High();
}

//保存电压float到W25Q
void W25Q_SaveVoltage(float vol)
{
    uint8_t buf[4];
    memcpy(buf, &vol, 4);//从vol复制4个字节到buf中
    //⚠️ flash写前必须擦扇区；VOL_ADDR所在扇区擦除
    W25Q_SectorErase(VOL_ADDR);
    W25Q_WritePage(VOL_ADDR, buf, 4);
}

//读取保存的电压
float W25Q_ReadVoltage(void)
{
    uint8_t buf[4];
    float vol;
    W25Q_Read(VOL_ADDR, buf, 4);
    memcpy(&vol, buf, 4);
    return vol;
}


