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

LV_IMG_DECLARE(bolt);

static lv_obj_t *canvas;
static lv_color_t cbuf[CANVAS_SIZE * CANVAS_SIZE];
static struct status_state state;

// Simple rectangle-drawn battery icon + percentage, ported from
// GPeye/hammerbeam-slideshow's widgets/util.c draw_battery(), which is
// confirmed working on this exact PandaKB Lily58 nice!view hardware.
static void draw_battery(void) {
    draw_background(canvas);

    lv_draw_rect_dsc_t rect_black_dsc;
    init_rect_dsc(&rect_black_dsc, LVGL_BACKGROUND);
    lv_draw_rect_dsc_t rect_white_dsc;
    init_rect_dsc(&rect_white_dsc, LVGL_FOREGROUND);

    lv_canvas_draw_rect(canvas, 0, 2, 29, 12, &rect_white_dsc);
    lv_canvas_draw_rect(canvas, 1, 3, 27, 10, &rect_black_dsc);
    lv_canvas_draw_rect(canvas, 2, 4, (state.battery + 2) / 4, 8, &rect_white_dsc);
    lv_canvas_draw_rect(canvas, 30, 5, 3, 6, &rect_white_dsc);
    lv_canvas_draw_rect(canvas, 31, 6, 1, 4, &rect_black_dsc);

    lv_draw_label_dsc_t label_dsc;
    init_label_dsc(&label_dsc, LVGL_FOREGROUND, &lv_font_montserrat_16, LV_TEXT_ALIGN_LEFT);
    char text[8] = {};
    snprintf(text, sizeof(text), "%i%%", state.battery);
    lv_canvas_draw_text(canvas, 36, 0, 32, &label_dsc, text);

    if (state.charging) {
        lv_draw_img_dsc_t img_dsc;
        lv_draw_img_dsc_init(&img_dsc);
        lv_canvas_draw_img(canvas, 9, -1, &bolt, &img_dsc);
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
