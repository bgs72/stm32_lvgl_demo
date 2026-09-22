/*
 * @file ky040.c
 *
 * @brief   ky040 low-level driver implementation for LVGL
 *
 *  @author Bart Gysens
 */

/* -------- *
 * Includes *
 * -------- */
#include "stm32f4xx_hal.h"
#include "ky040.h"

/* ------- *
 * Defines *
 * ------- */
#define ENC_BTN_GPIO    GPIOA
#define ENC_BTN_Pin     GPIO_PIN_2


/* ---- *
 * Data *
 * ---- */
extern TIM_HandleTypeDef htim2;
static int16_t ky040_ticks = 0;
static lv_indev_t *ky040_encoder = (lv_indev_t*) 0;

static void ky040_input_read(lv_indev_t *ky040_encoder,  lv_indev_data_t *data) {
    /**
     * Remark: requires timer TIM2 to be configured in encoder mode!
     *
     * Settings for TIM2 in Device Configuration Tool Perspective:
     * + Mode
     *   - Combined Channels: Encoder Mode
     * + Parameter Settings
     *   - Counter Settings
     *     . Counter Period: 65535
     *     . auto-reload preload: enable
     *   - Encoder
     *     . Encoder Mode: Encoder Mode TI2 and TI2
     *     . Input filter: 10
     *
     * + To start the timer execute next line in main:
     *     HAL_TIM_Encoder_Start(&htim2, TIM_CHANNEL_ALL);
     */
    data->state = (HAL_GPIO_ReadPin(ENC_BTN_GPIO, ENC_BTN_Pin) == GPIO_PIN_RESET)
            ? LV_INDEV_STATE_PRESSED : LV_INDEV_STATE_RELEASED;
    int16_t ticks = ((int16_t) TIM2->CNT) >> 2;
    data->enc_diff = ticks - ky040_ticks;
    ky040_ticks = ticks;
}

/**
 * Creates the LVGL encoder input device.
 *
 * @return the LVGL encoder input device
 */
lv_indev_t *ky040_create_input(void) {
    HAL_TIM_Encoder_Stop(&htim2, TIM_CHANNEL_ALL);
    HAL_TIM_Encoder_Start(&htim2, TIM_CHANNEL_ALL);
    ky040_encoder = lv_indev_create();
    lv_indev_set_type(ky040_encoder, LV_INDEV_TYPE_ENCODER);
    lv_indev_set_read_cb(ky040_encoder, ky040_input_read);
    return ky040_encoder;
}

/**
 * Gets the KY040 input device.
 *
 * @return the KY040 input device
 */
lv_indev_t *ky040_get(void) {
    return ky040_encoder;
}
