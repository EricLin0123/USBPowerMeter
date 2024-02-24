#include "main.h"
#include "spi.h"
#include "imu.h"
#include "usart.h"
#include "screen.h"
#include "iwdg.h"

#include <stdio.h>
void ADXL345_Write(uint8_t addr, uint8_t value)
{
    addr &= 0x3F; // 00111111
    HAL_GPIO_WritePin(CS_GPIO_Port, CS_Pin, GPIO_PIN_RESET);
    HAL_SPI_Transmit(&hspi1, &addr, 1, 10);
    HAL_SPI_Transmit(&hspi1, &value, 1, 10);
    HAL_GPIO_WritePin(CS_GPIO_Port, CS_Pin, GPIO_PIN_SET);
}

void ADXL345_Rread(uint8_t addr, uint8_t *value)
{
    addr &= 0x3F;   // 00111111
    addr |= (0x80); // 10000000
    HAL_GPIO_WritePin(CS_GPIO_Port, CS_Pin, GPIO_PIN_RESET);
    HAL_SPI_Transmit(&hspi1, &addr, 1, 10);
    HAL_SPI_Receive(&hspi1, value, 1, 10);
    HAL_GPIO_WritePin(CS_GPIO_Port, CS_Pin, GPIO_PIN_SET);
}

uint8_t Get_Adxl345_ID(void)
{
    uint8_t DEVICEID = ADXL345_DEVID;
    uint8_t result = 0;
    ADXL345_Rread(DEVICEID, &result);
    return result;
}

HAL_StatusTypeDef ADXL345_Init(void)
{
    if (Get_Adxl345_ID() != 0xE5)
    {
        OLED_ShowString(0, 0, (char *)"Finding IMU", 8);
        return HAL_ERROR;
    }
    else
    {
        OLED_ShowString(0, 0, (char *)"IMU found    ", 8);
        HAL_IWDG_Refresh(&hiwdg);
        HAL_Delay(500);
        ADXL345_Write(ADXL345_INT_ENABLE, 0x00);
        ADXL345_Write(ADXL345_DATA_FORMAT, 0x09); // 00001001
        ADXL345_Write(ADXL345_BW_RATE, 0x1A);
        ADXL345_Write(ADXL345_POWER_CTL, 0x08);
        ADXL345_Write(ADXL345_THRESH_TAP, 0x28); // 40 * 62.5 mg/LSB = 2.5 g
        ADXL345_Write(ADXL345_DUR, 0x10);        // 16 * 6.25 us = 0.02 s
        ADXL345_Write(ADXL345_LATENT, 0x50);     // 80 * 1.25 ms = 0.1 s
        ADXL345_Write(ADXL345_WINDOW, 0xF0);     // 240 * 1.25 ms = 0.3 s
        ADXL345_Write(ADXL345_TAP_AXES, 0x07);   // 00000111
        ADXL345_Write(ADXL345_INT_MAP, 0x40);    // 01000000
        ADXL345_Write(ADXL345_INT_ENABLE, 0x60); // 01100000
        OLED_ShowString(0, 1, (char *)"IMU init done", 8);
        HAL_IWDG_Refresh(&hiwdg);
        HAL_Delay(500);

        return HAL_OK;
    }
}
// 'a' - single character
// "a" = {'a', '\0'} - string
void ADXL345_Test(void)
{
    short x;
    uint8_t addrxl = ADXL345_DATAX0;
    uint8_t addrxh = ADXL345_DATAX1;
    uint8_t xl, xh;

    ADXL345_Rread(addrxl, &xl);
    ADXL345_Rread(addrxh, &xh);

    x = (short)(((uint16_t)xh << 8) + xl);

    char str[] = {' ', ' ', ' ', ' ', '\0'};
    sprintf(str, "x:%hi  ", x);
    OLED_ShowString(0, 2, (char *)str, 8);

    // ADXL345_Rread(ADXL345_INT_SOURCE, &xl);
    // sprintf(str, "src:%hi  ", xl);
    // OLED_ShowString(0, 6, (uint8_t *)str, 8);

    short y;
    uint8_t addryl = ADXL345_DATAY0;
    uint8_t addryh = ADXL345_DATAY1;
    uint8_t yl, yh;

    ADXL345_Rread(addryl, &yl);
    ADXL345_Rread(addryh, &yh);

    y = (short)(((uint16_t)yh << 8) + yl);

    sprintf(str, "y:%hi  ", y);
    OLED_ShowString(0, 3, (char *)str, 8);

    short z;
    uint8_t addrzl = ADXL345_DATAZ0;
    uint8_t addrzh = ADXL345_DATAZ1;
    uint8_t zl, zh;

    ADXL345_Rread(addrzl, &zl);
    ADXL345_Rread(addrzh, &zh);

    z = (short)(((uint16_t)zh << 8) + zl);

    sprintf(str, "z:%hi  ", z);
    OLED_ShowString(64, 3, (char *)str, 8);
}

void ADXL345_Probe(void)
{
    HAL_UART_Transmit(&huart2, (uint8_t *)"ADXL345 Probe\r\n", 15, 10);
    uint8_t addrxl = ADXL345_DATAX0;
    uint8_t addrxh = ADXL345_DATAX1;
    uint8_t xl, xh;

    ADXL345_Rread(addrxl, &xl);
    ADXL345_Rread(addrxh, &xh);

    ADXL345_Rread(ADXL345_INT_SOURCE, &xl);

    uint8_t addryl = ADXL345_DATAY0;
    uint8_t addryh = ADXL345_DATAY1;
    uint8_t yl, yh;

    ADXL345_Rread(addryl, &yl);
    ADXL345_Rread(addryh, &yh);

    uint8_t addrzl = ADXL345_DATAZ0;
    uint8_t addrzh = ADXL345_DATAZ1;
    uint8_t zl, zh;

    ADXL345_Rread(addrzl, &zl);
    ADXL345_Rread(addrzh, &zh);
}
