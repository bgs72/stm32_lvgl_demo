/*
 * @file ui_setup.h
 *
 * @brief   main screen setup
 *
 *  @author Bart Gysens
 */

/* ------- *
 * Defines *
 * ------- */
#define SHOW_ANIMATION 0


/* -------- *
 * Includes *
 * -------- */
#include "ui.h"
#include "ui_init.h"



#if SHOW_ANIMATION

static void anim_x_cb(void * var, int32_t v)
{
    lv_obj_set_x(var, v);
}

static void anim_size_cb(void * var, int32_t v)
{
    lv_obj_set_size(var, v, v);
}

/**
 * Creates the main screen.
 *
 * @param screen    pointer to the active screen
 */
void ui_init(lv_obj_t *screen)
{
    lv_obj_t * obj = lv_obj_create(screen);
    lv_obj_set_style_bg_color(obj, lv_palette_main(LV_PALETTE_RED), 0);
    lv_obj_set_style_radius(obj, LV_RADIUS_CIRCLE, 0);

    lv_obj_align(obj, LV_ALIGN_LEFT_MID, 10, 0);

    lv_anim_t a;
    lv_anim_init(&a);
    lv_anim_set_var(&a, obj);
    lv_anim_set_values(&a, 10, 50);
    lv_anim_set_duration(&a, 1000);
    lv_anim_set_playback_delay(&a, 100);
    lv_anim_set_playback_duration(&a, 300);
    lv_anim_set_repeat_delay(&a, 500);
    lv_anim_set_repeat_count(&a, LV_ANIM_REPEAT_INFINITE);
    lv_anim_set_path_cb(&a, lv_anim_path_ease_in_out);

    lv_anim_set_exec_cb(&a, anim_size_cb);
    lv_anim_start(&a);
    lv_anim_set_exec_cb(&a, anim_x_cb);
    lv_anim_set_values(&a, 10, 100);
    lv_anim_start(&a);
}

#else /* SHOW_ANIMATION == 0 */

static lv_obj_t *helloWorld;

void btn1_clicked_cb(lv_event_t *event) {
    lv_label_set_text(helloWorld, "Hello Again World!");

}

void btn2_clicked_cb(lv_event_t *event) {
    lv_label_set_text(helloWorld, "Enjoy STM32!");
}


/**
 * Creates the main screen.
 *
 * @param screen    pointer to the active screen
 */
void ui_init(lv_obj_t *screen)
{
    /* set screen background to white */
    lv_obj_set_style_bg_color(screen, lv_color_hex(0xFFFF00), 0);
    lv_obj_set_style_bg_opa(screen, LV_OPA_100, 0);

    /* create input device and assign it to a group --*/
    lv_indev_t *input = ky040_get();
    lv_group_t *group = lv_group_create();
    lv_indev_set_group(input, group);

    /* create label */
    helloWorld = lv_label_create(screen);
    lv_obj_set_align(helloWorld, LV_ALIGN_TOP_MID);
    lv_obj_set_height(helloWorld, LV_SIZE_CONTENT);
    lv_obj_set_width(helloWorld, LV_SIZE_CONTENT);
    lv_obj_set_style_text_font(helloWorld, &lv_font_montserrat_14, 0);
    lv_obj_set_style_text_color(helloWorld, lv_color_hex(0x0000FF), 0);
    lv_label_set_text(helloWorld, "Hello World!");

    lv_obj_t *btn1 = lv_btn_create(screen);
    lv_obj_set_align(btn1, LV_ALIGN_BOTTOM_LEFT);
    lv_obj_set_height(btn1, LV_SIZE_CONTENT);
    lv_obj_set_width(btn1, LV_SIZE_CONTENT);
    lv_obj_add_event_cb(btn1, btn1_clicked_cb, LV_EVENT_CLICKED, NULL);
    lv_group_add_obj(group, btn1);

    lv_obj_t *labelBtn1 = lv_label_create(btn1);
    lv_obj_set_style_bg_color(labelBtn1, lv_color_hex(0x0000FF), 0);
    lv_obj_set_style_text_font(labelBtn1, &lv_font_montserrat_14, 0);
    lv_obj_set_style_text_color(labelBtn1, lv_color_hex(0xFFFF00), 0);
    lv_label_set_text(labelBtn1, "Hello");

    lv_obj_t *btn2 = lv_btn_create(screen);
    lv_obj_set_align(btn2, LV_ALIGN_BOTTOM_RIGHT);
    lv_obj_set_height(btn2, LV_SIZE_CONTENT);
    lv_obj_set_width(btn2, LV_SIZE_CONTENT);
    lv_obj_add_event_cb(btn2, btn2_clicked_cb, LV_EVENT_CLICKED, NULL);
    lv_group_add_obj(group, btn2);

    lv_obj_t *labelBtn2 = lv_label_create(btn2);
    lv_obj_set_style_bg_color(labelBtn2, lv_color_hex(0x0000FF), 0);
    lv_obj_set_style_text_font(labelBtn2, &lv_font_montserrat_14, 0);
    lv_obj_set_style_text_color(labelBtn2, lv_color_hex(0xFFFF00), 0);
    lv_label_set_text(labelBtn2, "Enjoy");

}

#endif
