#ifndef __I2C_HAL_H
#define __I2C_HAL_H

#include "stm32g4xx_hal.h"

void I2CStart(void);
void I2CStop(void);
unsigned char I2CWaitAck(void);
void I2CSendAck(void);
void I2CSendNotAck(void);
void I2CSendByte(unsigned char cSendByte);
unsigned char I2CReceiveByte(void);
void I2CInit(void);

//24C02����ش���
void iic_24c02_write(unsigned char *pucBuf, unsigned char ucAddr, unsigned char ucNum);
void iic_24c02_read(unsigned char *pucBuf, unsigned char ucAddr, unsigned char ucNum);


//MCP4017����ش���
void write_resistor(uint8_t value);
uint8_t read_resistor(void);

#endif
