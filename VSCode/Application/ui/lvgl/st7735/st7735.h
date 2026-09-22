/*
 * @file st7735.h
 *
 * @brief   st7735 low-level driver for LVGL
 *
 *          REMARK: requires next define in lv_conf.h to adjust RGB565 colors:
 *
 *                  #define LV_COLOR_16_SWAP 1
 *
 *  @author Bart Gysens
 */

#ifndef UI_ST7735_ST7735_H_
#define UI_ST7735_ST7735_H_

/* -------- *
 * Includes *
 * -------- */
#include "lvgl.h"

/* -------------------- *
 * Function definitions *
 * -------------------- */
void st7735_dma_done();
uint8_t st7735_set_brightness(const uint8_t percentage);
void st7735_set_rotation(lv_display_t *disp, lv_display_rotation_t rotation);
lv_display_t *st7735_create_display(void);
lv_display_t* st7735_get(void);

#endif /* UI_ST7735_ST7735_H_ */
