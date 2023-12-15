#include "card.h"

#include <lvgl.h>

#include "../pub_sub.h"
#include "../fonts/fonts.h"
#include "../theme.h"

lv_obj_t * create_card(lv_obj_t* parent) {
  lv_obj_t* new_card = lv_obj_create(parent);
  lv_obj_set_size(new_card, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
  lv_obj_set_style_align(new_card, LV_ALIGN_CENTER, LV_PART_MAIN);
  lv_obj_set_style_radius(new_card, BORDER_RADIUS, LV_PART_MAIN);
  lv_obj_set_style_bg_color(new_card, lv_color_hex(ACTIVE_BACKGROUND_COLOR), LV_PART_MAIN);
  lv_obj_set_style_border_side(new_card, LV_BORDER_SIDE_NONE, LV_PART_MAIN);
  lv_obj_set_style_pad_all(new_card, PADDING, LV_PART_MAIN);
  lv_obj_set_style_text_font(new_card, &font_Roboto_16, LV_PART_MAIN);
  lv_obj_set_style_text_color(new_card, lv_color_hex(TEXT_COLOR), LV_PART_MAIN);
  return new_card;
}

