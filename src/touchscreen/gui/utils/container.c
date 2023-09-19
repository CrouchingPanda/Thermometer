#include "container.h"

#include <lvgl.h>

#include "../fonts/fonts.h"

lv_obj_t* create_container(lv_obj_t* parent) {
  lv_obj_t* new_container = lv_obj_create(parent);
  lv_obj_set_style_text_font(new_container, &font_Roboto_16, LV_PART_MAIN);
  lv_obj_set_style_bg_opa(new_container, 0, LV_PART_MAIN);
  lv_obj_set_style_border_side(new_container, LV_BORDER_SIDE_NONE, LV_PART_MAIN);
  lv_obj_set_style_pad_all(new_container, 0, LV_PART_MAIN);
  return new_container;
}