// oled.h
#ifndef __OLED_H
#define __OLED_H
#include "main.h"
#define SCL_PIN GPIO_PIN_6
#define SCL_PORT GPIOB
#define SDA_PIN GPIO_PIN_7
#define SDA_PORT GPIOB

#define SCL_L HAL_GPIO_WritePin(SCL_PORT,SCL_PIN,GPIO_PIN_RESET)
#define SCL_H HAL_GPIO_WritePin(SCL_PORT,SCL_PIN,GPIO_PIN_SET)
#define SDA_L HAL_GPIO_WritePin(SDA_PORT,SDA_PIN,GPIO_PIN_RESET)
#define SDA_H HAL_GPIO_WritePin(SDA_PORT,SDA_PIN,GPIO_PIN_SET)
#define SDA_R HAL_GPIO_ReadPin(SDA_PORT,SDA_PIN)


void I2C_Delay_us(uint16_t t);
uint8_t I2C_WriteByte(uint8_t dat);
void I2C_Start(void);
void I2C_Stop(void);
void OLED_Write_Cmd(uint8_t cmd);
void OLED_Write_Data(uint8_t data);
void OLED_Init(void);
void OLED_Clear(void);
void OLED_Set_Pos(uint8_t x, uint8_t y);
void OLED_ShowChar(uint8_t x, uint8_t y, char ch);
void OLED_ShowNum(uint8_t x, uint8_t y, uint32_t num, uint8_t len);
void OLED_ShowString(uint8_t x,uint8_t y,char *str);

#endif
