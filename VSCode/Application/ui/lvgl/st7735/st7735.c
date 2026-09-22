/*
 * @file st7735.c
 *
 * @brief   st7735 low-level driver implementation for LVGL
 *
 *          REMARK: requires next define in lv_conf.h to adjust RGB565 colors:
 *
 *                  #define LV_COLOR_16_SWAP 1
 *
 *  @author Bart Gysens
 */

/* -------- *
 * Includes *
 * -------- */
#include "stm32f4xx_hal.h"
#include "cmsis_os.h"
#include "lvgl.h"
#include "src/drivers/display/st7735/lv_st7735.h"
#include "st7735.h"

/* ------- *
 * Defines *
 * ------- */
#define ST7735_DISPLAY_WIDTH                128
#define ST7735_DISPLAY_HEIGHT               160
#define ST7735_DISPLAY_OFFSET_X             2
#define ST7735_DISPLAY_OFFSET_Y             1
#define ST7735_DISPLAY_BYTES_PER_PIXEL      (LV_COLOR_FORMAT_GET_SIZE(LV_COLOR_FORMAT_RGB565)) /* will be 2 for RGB565 */
#define ST7735_DISPLAY_BUFFER_SIZE          ((ST7735_DISPLAY_WIDTH / 10) * ST7735_DISPLAY_HEIGHT * ST7735_DISPLAY_BYTES_PER_PIXEL)

#define ST7735_BRIGHTNESS_TIM               (&htim4)
#define ST7735_BRIGHTNESS_TIM_CHANNEL       TIM_CHANNEL_3

#define ST7735_RESET_PORT                   GPIOB
#define ST7735_RESET_PIN                    GPIO_PIN_6

#define ST7735_DATA_PORT                    GPIOB
#define ST7735_DATA_PIN                     GPIO_PIN_7

#define ST7735_SPI                          (&hspi3)
#define ST7735_SPI_TIMEOUT                  1000

#define ST7735_MATH_MIN(a,b)                (((a) < (b)) ? (a) : (b))

/* ---- *
 * Data *
 * ---- */
extern SPI_HandleTypeDef hspi3;
extern TIM_HandleTypeDef htim4;

static uint8_t st7735_brightness = 100;
static uint8_t st7735_display_buffer_1[ST7735_DISPLAY_BUFFER_SIZE];
static uint8_t st7735_display_buffer_2[ST7735_DISPLAY_BUFFER_SIZE];
static lv_display_t *st7735_display = (lv_display_t*) 0;
static volatile uint8_t st7735_dma_busy = 0;


/**
 * LVGL callback to transfer commands and data to the LCD controller.
 *
 * @param disp display object
 * @param cmd command buffer
 * @param cmd_size number of bytes of the command
 * @param param parameter buffer
 * @param param_size number of bytes of the parameters
 */
static void st7735_send_cmd_cb(lv_display_t *disp, const uint8_t *cmd, size_t cmd_size, const uint8_t *param,
        size_t param_size)
{
    while(st7735_dma_busy) { osDelay(1); }
    HAL_GPIO_WritePin(ST7735_DATA_PORT, ST7735_DATA_PIN, GPIO_PIN_RESET);
    HAL_SPI_Transmit(ST7735_SPI, cmd, cmd_size, ST7735_SPI_TIMEOUT);
    HAL_GPIO_WritePin(ST7735_DATA_PORT, ST7735_DATA_PIN, GPIO_PIN_SET);
    if (param_size > 0)
    {
        HAL_SPI_Transmit(ST7735_SPI, param, param_size, ST7735_SPI_TIMEOUT);
    }
}

/**
 * LVGL callback to transfer pixel data to the controller.
 *
 * @param disp display object
 * @param cmd command buffer
 * @param cmd_size number of bytes of the command
 * @param param parameter buffer
 * @param param_size number of bytes of the parameters
 */
static void st7735_send_color_cb(lv_display_t *disp, const uint8_t *cmd, size_t cmd_size, uint8_t *param,
        size_t param_size)
{
    while(st7735_dma_busy) { osDelay(1); }
    HAL_GPIO_WritePin(ST7735_DATA_PORT, ST7735_DATA_PIN, GPIO_PIN_RESET);
    HAL_SPI_Transmit(ST7735_SPI, cmd, cmd_size, ST7735_SPI_TIMEOUT);
    HAL_GPIO_WritePin(ST7735_DATA_PORT, ST7735_DATA_PIN, GPIO_PIN_SET);
    if (param_size > 0)
    {
        st7735_dma_busy = 1;
        HAL_SPI_Transmit_DMA(ST7735_SPI, param, param_size);
    }
    //lv_display_flush_ready(disp);
}

/**
 * This function should be called when the SPI TX transfer is completed.
 */
void st7735_dma_done()
{
    st7735_dma_busy = 0;
    lv_display_flush_ready(st7735_display);
}


/**
 * Sets the brightness of the LCD display.
 *
 * @param percentage brightness in percentage
 * @return previous brightness
 */
uint8_t st7735_set_brightness(const uint8_t percentage)
{
    uint8_t previous_brightness = st7735_brightness;
    st7735_brightness = ST7735_MATH_MIN(percentage, 100);
    TIM4->CCR3 = 100 - st7735_brightness;
    HAL_TIM_PWM_Stop(ST7735_BRIGHTNESS_TIM, ST7735_BRIGHTNESS_TIM_CHANNEL);
    HAL_TIM_PWM_Start(ST7735_BRIGHTNESS_TIM, ST7735_BRIGHTNESS_TIM_CHANNEL);
    return previous_brightness;
}

/**
 * Sets the display orientation.
 *
 * @param disp pointer to lvgl display
 * @param rotation the rotation touse
 */
void st7735_set_rotation(lv_display_t *disp, lv_display_rotation_t rotation)
{
    bool isPortrait = rotation == LV_DISPLAY_ROTATION_0 || rotation == LV_DISPLAY_ROTATION_180;
    lv_st7735_set_gap(disp, isPortrait ? 2 : 1, isPortrait ? 1 : 2);
    lv_display_set_rotation(disp, rotation);
}

/**
 * Create an LCD display with ST7735 driver.
 *
 * @return pointer to created display
 */
lv_display_t *st7735_create_display(void)
{
    /*-- set brightness off --*/
    st7735_brightness = st7735_set_brightness(0);

    /*-- display hardware reset --*/
    HAL_GPIO_WritePin(ST7735_RESET_PORT, ST7735_RESET_PIN, GPIO_PIN_SET);
    HAL_Delay(75);
    HAL_GPIO_WritePin(ST7735_RESET_PORT, ST7735_RESET_PIN, GPIO_PIN_RESET);
    HAL_Delay(75);
    HAL_GPIO_WritePin(ST7735_RESET_PORT, ST7735_RESET_PIN, GPIO_PIN_SET);
    HAL_Delay(75);

    /*-- create display --*/
    st7735_display = lv_st7735_create(
    ST7735_DISPLAY_WIDTH,
    ST7735_DISPLAY_HEIGHT,
    LV_LCD_FLAG_NONE, st7735_send_cmd_cb, st7735_send_color_cb);

    /*-- set rotation --*/
    st7735_set_rotation(st7735_display, LV_DISPLAY_ROTATION_90);

    /*-- setup display buffer --*/
    lv_display_set_buffers(
            st7735_display,
            st7735_display_buffer_1,
            st7735_display_buffer_2,
            sizeof(st7735_display_buffer_1),
            LV_DISPLAY_RENDER_MODE_PARTIAL);

    /*-- restore brightness --*/
    st7735_set_brightness(st7735_brightness);
    return st7735_display;
}


/**
 * Gets the ST7735 display.
 *
 * @return the ST7735 display
 */
lv_display_t *st7735_get(void)
{
    return st7735_display;
}

