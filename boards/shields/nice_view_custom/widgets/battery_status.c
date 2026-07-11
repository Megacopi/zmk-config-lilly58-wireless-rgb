#include <zephyr/kernel.h>
#include <zephyr/logging/log.h>
LOG_MODULE_DECLARE(zmk, CONFIG_ZMK_LOG_LEVEL);

#include <zmk/display.h>
#include <zmk/event_manager.h>
#include <zmk/events/battery_state_changed.h>
#include <zmk/events/usb_conn_state_changed.h>
#include <zmk/battery.h>
#include <zmk/usb.h>
#include <lvgl.h>

#include "util.h"
#include "../assets/font.h"

LV_IMG_DECLARE(bolt);
LV_IMG_DECLARE(battery);
LV_IMG_DECLARE(battery_mask);

static lv_obj_t *canvas;
static lv_color_t cbuf[CANVAS_SIZE * CANVAS_SIZE];
static struct status_state state;

static void draw_battery(void) {
    draw_background(canvas);

    lv_draw_img_dsc_t img_dsc;
    lv_draw_img_dsc_init(&img_dsc);
    lv_draw_rect_dsc_t rect_dsc;
    init_rect_dsc(&rect_dsc, LVGL_FOREGROUND);
    lv_draw_label_dsc_t outline_dsc;
    init_label_dsc(&outline_dsc, LVGL_BACKGROUND, &font, LV_TEXT_ALIGN_CENTER);
    lv_draw_label_dsc_t label_dsc;
    init_label_dsc(&label_dsc, LVGL_FOREGROUND, &font, LV_TEXT_ALIGN_CENTER);

    lv_canvas_draw_img(canvas, 0, 0, &battery, &img_dsc);
    lv_canvas_draw_rect(canvas, 4, 4, 54 * state.battery / 100, 23, &rect_dsc);
    lv_canvas_draw_img(canvas, 2, 2, &battery_mask, &img_dsc);

    char text[10] = {};
    sprintf(text, "%i%%", state.battery);

    const int y = 9;
    const int w = 62;

    // outline behind the fill so the percentage stays legible over any fill level
    for (int dx = -1; dx <= 1; dx++) {
        for (int dy = -1; dy <= 1; dy++) {
            if (dx != 0 || dy != 0) {
                lv_canvas_draw_text(canvas, dx, y + dy, w, &outline_dsc, text);
            }
        }
    }
    lv_canvas_draw_text(canvas, 0, y, w, &label_dsc, text);

    if (state.charging) {
        lv_canvas_draw_img(canvas, 29, 37, &bolt, &img_dsc);
    }

    rotate_canvas(canvas, cbuf);
}

static void update_battery(void) {
    state.battery = zmk_battery_state_of_charge();
#if IS_ENABLED(CONFIG_USB_DEVICE_STACK)
    state.charging = zmk_usb_is_powered();
#endif
    draw_battery();
}

static int battery_event_handler(const zmk_event_t *eh) {
    update_battery();
    return ZMK_EV_EVENT_BUBBLE;
}

ZMK_LISTENER(battery_status, battery_event_handler);
ZMK_SUBSCRIPTION(battery_status, zmk_battery_state_changed);
#if IS_ENABLED(CONFIG_USB_DEVICE_STACK)
ZMK_SUBSCRIPTION(battery_status, zmk_usb_conn_state_changed);
#endif

lv_obj_t *zmk_widget_battery_status_obj(void) {
    canvas = lv_canvas_create(lv_scr_act());
    lv_canvas_set_buffer(canvas, cbuf, CANVAS_SIZE, CANVAS_SIZE, LV_IMG_CF_TRUE_COLOR);
    lv_obj_align(canvas, LV_ALIGN_CENTER, 0, 0);
    update_battery();
    return canvas;
}
