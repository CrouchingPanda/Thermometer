#include "screen.h"

#include <lvgl.h>

#include "../theme.h"
#include "../fonts/fonts.h"

lv_obj_t* make_screen(void) {
  lv_obj_t* new_screen = lv_obj_create(NULL);
  lv_obj_set_style_bg_color(new_screen, lv_color_hex(BACKGROUND_COLOR), LV_PART_MAIN);
  lv_obj_set_style_text_font(new_screen, &font_Roboto_16, LV_PART_MAIN);
  return new_screen;
}