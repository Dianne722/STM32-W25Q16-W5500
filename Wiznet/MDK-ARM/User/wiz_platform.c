#include "wiz_platform.h"
#include "wizchip_conf.h"
#include "main.h"
#include "wiz_interface.h"
#include <stdio.h>
#include <stdint.h>

extern SPI_HandleTypeDef hspi1;
extern UART_HandleTypeDef huart1;
extern TIM_HandleTypeDef htim2;

/**
 * @brief   SPI select wizchip
 * @param   none
 * @return  none
 */
void wizchip_select(void)
{
    HAL_GPIO_WritePin(SCSn_GPIO_Port, SCSn_Pin, GPIO_PIN_RESET);
}

/**
 * @brief   SPI deselect wizchip
 * @param   none
 * @return  none
 */
void wizchip_deselect(void)
{
    HAL_GPIO_WritePin(SCSn_GPIO_Port, SCSn_Pin, GPIO_PIN_SET);
}

/**
 * @brief   SPI write 1 byte to wizchip
 * @param   dat:1 byte data
 * @return  none
 */
void wizchip_write_byte(uint8_t dat)
{
    HAL_SPI_Transmit(&hspi1, &dat, 1, 0xffff);
}

/**
 * @brief   SPI read 1 byte from wizchip
 * @param   none
 * @return  1 byte data
 */
uint8_t wizchip_read_byte(void)
{
    uint8_t dat;
    HAL_SPI_Receive(&hspi1, &dat, 1, 0xffff);
    return dat;
}

/**
 * @brief   SPI write buff from wizchip
 * @param   buff:write buff
 * @param   len:write len
 * @return  none
 */
void wizchip_write_buff(uint8_t *buf, uint16_t len)
{
    HAL_SPI_Transmit(&hspi1, buf, len, 0xffff);
}

/**
 * @brief   SPI read buff from wizchip
 * @param   buff:read buff
 * @param   len:read len
 * @return  none
 */
void wizchip_read_buff(uint8_t *buf, uint16_t len)
{
    HAL_SPI_Receive(&hspi1, buf, len, 0xffff);
}

/**
 * @brief   hardware reset wizchip
 * @param   none
 * @return  none
 */
void wizchip_reset(void)
{
    HAL_GPIO_WritePin(RSTn_GPIO_Port, RSTn_Pin, GPIO_PIN_SET);
    wiz_user_delay_ms(10);
    HAL_GPIO_WritePin(RSTn_GPIO_Port, RSTn_Pin, GPIO_PIN_RESET);
    wiz_user_delay_ms(10);
    HAL_GPIO_WritePin(RSTn_GPIO_Port, RSTn_Pin, GPIO_PIN_SET);
    wiz_user_delay_ms(10);
}

/**
 * @brief   wizchip spi callback register
 * @param   none
 * @return  none
 */
void wizchip_spi_cb_reg(void)
{
    reg_wizchip_cs_cbfunc(wizchip_select, wizchip_deselect);
    reg_wizchip_spi_cbfunc(wizchip_read_byte, wizchip_write_byte);
    reg_wizchip_spiburst_cbfunc(wizchip_read_buff, wizchip_write_buff);
}

/**
 * @brief   Hardware Platform Timer Interrupt Callback Function

 */
void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim)
{
    if (htim->Instance == TIM2)
    {
        wiz_timer_handler();
    }
}

/**
 * @brief   Turn on wiz timer interrupt
 * @param   none
 * @return  none
 */
void wiz_tim_irq_enable(void)
{
    HAL_TIM_Base_Start_IT(&htim2);
}

/**
 * @brief   Turn off wiz timer interrupt
 * @param   none
 * @return  none
 */
void wiz_tim_irq_disable(void)
{
    HAL_TIM_Base_Start_IT(&htim2);
}

/**
 * @brief   Serial port redirect to printf
 */
int fputc(int ch, FILE *f)
{
    HAL_UART_Transmit(&huart1, (uint8_t *)&ch, 1, 0xffff);
    return ch;
}
