/*
 *
 * Copyright (c) 2023 The ZMK Contributors
 * SPDX-License-Identifier: MIT
 *
 */

#include <zephyr/kernel.h>
#include "util.h"
#include "../assets/font.h"

#define BOLT_OFFSET_X 29
#define BOLT_OFFSET_Y 37

LV_IMG_DECLARE(bolt);
LV_IMG_DECLARE(battery);
LV_IMG_DECLARE(battery_mask);

void rotate_canvas(lv_obj_t *canvas, lv_color_t cbuf[]) {
    static lv_color_t cbuf_tmp[CANVAS_SIZE * CANVAS_SIZE];
    memcpy(cbuf_tmp, cbuf, sizeof(cbuf_tmp));
    lv_img_dsc_t img;
    img.data = (void *)cbuf_tmp;
    img.header.cf = LV_IMG_CF_TRUE_COLOR;
    img.header.w = CANVAS_SIZE;
    img.header.h = CANVAS_SIZE;

    lv_canvas_fill_bg(canvas, LVGL_BACKGROUND, LV_OPA_COVER);
    lv_canvas_transform(canvas, &img, 900, LV_IMG_ZOOM_NONE, -1, 0, CANVAS_SIZE / 2,
                        CANVAS_SIZE / 2, true);
}

// Bitmap battery icon with a large outlined percentage, ported from
// infely/nice-view-battery's widgets/battery.c draw_battery_status().
void draw_battery(lv_obj_t *canvas, const struct status_state *state) {
    lv_draw_img_dsc_t img_dsc;
    lv_draw_img_dsc_init(&img_dsc);
    lv_draw_rect_dsc_t rect_dsc;
    init_rect_dsc(&rect_dsc, LVGL_FOREGROUND);
    lv_draw_label_dsc_t outline_dsc;
    init_label_dsc(&outline_dsc, LVGL_BACKGROUND, &font, LV_TEXT_ALIGN_CENTER);
    lv_draw_label_dsc_t label_dsc;
    init_label_dsc(&label_dsc, LVGL_FOREGROUND, &font, LV_TEXT_ALIGN_CENTER);

    lv_canvas_draw_img(canvas, 0, 0, &battery, &img_dsc);
    lv_canvas_draw_rect(canvas, 4, 4, 54 * state->battery / 100, 23, &rect_dsc);
    lv_canvas_draw_img(canvas, 2, 2, &battery_mask, &img_dsc);

    char text[10] = {};
    sprintf(text, "%i%%", state->battery);

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

    if (state->charging) {
        lv_canvas_draw_img(canvas, BOLT_OFFSET_X, BOLT_OFFSET_Y, &bolt, &img_dsc);
    }
}

void init_label_dsc(lv_draw_label_dsc_t *label_dsc, lv_color_t color, const lv_font_t *font,
                    lv_text_align_t align) {
    lv_draw_label_dsc_init(label_dsc);
    label_dsc->color = color;
    label_dsc->font = font;
    label_dsc->align = align;
}

void init_rect_dsc(lv_draw_rect_dsc_t *rect_dsc, lv_color_t bg_color) {
    lv_draw_rect_dsc_init(rect_dsc);
    rect_dsc->bg_color = bg_color;
}

void init_line_dsc(lv_draw_line_dsc_t *line_dsc, lv_color_t color, uint8_t width) {
    lv_draw_line_dsc_init(line_dsc);
    line_dsc->color = color;
    line_dsc->width = width;
}

void init_arc_dsc(lv_draw_arc_dsc_t *arc_dsc, lv_color_t color, uint8_t width) {
    lv_draw_arc_dsc_init(arc_dsc);
    arc_dsc->color = color;
    arc_dsc->width = width;
}
