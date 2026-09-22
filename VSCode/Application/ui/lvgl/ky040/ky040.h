/*
 * @file ky040.h
 *
 * @brief   ky040 low-level driver for LVGL
 *
 *  @author Bart Gysens
 */

#ifndef UI_LVGL_KY040_KY040_H_
#define UI_LVGL_KY040_KY040_H_

/* -------- *
 * Includes *
 * -------- */
#include "lvgl.h"

/* -------------------- *
 * Function definitions *
 * -------------------- */
lv_indev_t *ky040_create_input(void);
lv_indev_t *ky040_get(void);

#endif /* UI_LVGL_KY040_KY040_H_ */
