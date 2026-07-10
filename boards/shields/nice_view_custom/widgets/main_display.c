#include <zephyr/kernel.h>
#include <zmk/display.h>
#include <lvgl.h>

lv_obj_t *zmk_widget_battery_status_obj(void);
lv_obj_t *zmk_widget_fireworks_obj(void);

int zmk_display_status_screen_init(void) {
    lv_obj_t *screen = lv_scr_act();
    lv_obj_set_style_bg_color(screen, lv_color_black(), 0);
    lv_obj_set_style_bg_opa(screen, LV_OPA_COVER, 0);

    // Layer state isn't synced to split peripherals in ZMK, so this can't be
    // keyed off CONFIG_ZMK_SPLIT_ROLE_CENTRAL anymore now that the central
    // role lives on the headless dongle. Left keeps the fireworks widget,
    // Right shows plain battery status.
#if IS_ENABLED(CONFIG_SHIELD_LILY58_R)
    zmk_widget_battery_status_obj();
#else
    zmk_widget_fireworks_obj();
    zmk_widget_battery_status_obj();
#endif

    return 0;
}
