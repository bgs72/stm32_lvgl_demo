/*
 * ui_task.c
 *
 *  Created on: Nov 11, 2024
 *      Author: bart
 */

/* -------- *
 * Includes *
 * -------- */
#include "cmsis_os.h"
#include "ui.h"

/* ---- *
 * Data *
 * ---- */
static osThreadId ui_task_handle;
lv_indev_t *ky040;


static void ui_task(void const *argument) {
    /*-- initialize LVGL --*/
    lv_init();
    lv_tick_set_cb(xTaskGetTickCount);
    st7735_create_display();
    ky040 = ky040_create_input();

    /*-- initialize UI --*/
    ui_init(lv_screen_active());

    /*-- enter forever loop --*/
    while(1) {
        uint32_t time_till_next = lv_timer_handler();
        osDelay(time_till_next);
    }
}

/**
 * Creates the ui task.
 * This function should be called in main.
 */
void ui_create_task(void) {
    osThreadDef(ui_task, ui_task, osPriorityIdle, 0, 1024);
    ui_task_handle = osThreadCreate(osThread(ui_task), NULL);
}
