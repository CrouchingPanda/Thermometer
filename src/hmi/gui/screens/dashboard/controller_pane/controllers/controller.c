#include "controller.h"

#include <stdint.h>

#include <lvgl.h>

#include "../../../../fonts/fonts.h"
#include "../../../../theme.h"
#include "../../../../utils/card.h"

lv_obj_t* make_controller(lv_obj_t* parent) {
  lv_obj_t* new_controller = create_card(parent);
  lv_obj_set_height(new_controller, lv_pct(100));
  lv_obj_set_width(new_controller, 100);
  lv_obj_clear_flag(new_controller, LV_OBJ_FLAG_SCROLLABLE);
  lv_obj_set_flex_grow(new_controller, 1);
  lv_obj_set_flex_flow(new_controller, LV_FLEX_FLOW_COLUMN);
  lv_obj_set_flex_align(
    new_controller,
    LV_FLEX_ALIGN_CENTER,
    LV_FLEX_ALIGN_CENTER,
    LV_FLEX_ALIGN_CENTER
  );

  lv_obj_t* label = lv_label_create(new_controller);
  lv_obj_set_style_text_align(label, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN);

  lv_obj_t* roller = lv_roller_create(new_controller);
  lv_obj_set_width(roller, lv_pct(95));
  lv_obj_set_flex_grow(roller, 1);
  lv_obj_set_style_text_line_space(roller, 1, LV_PART_MAIN);
  lv_obj_set_style_text_font(roller, &font_Roboto_Mono_18, LV_PART_MAIN);
  lv_obj_set_style_text_color(roller, lv_color_hex(PLACEHOLDER_TEXT_COLOR), LV_PART_MAIN);
  lv_obj_set_style_bg_color(roller, lv_color_hex(TEXT_AREA_BACKGROUND_COLOR), LV_PART_MAIN);
  lv_obj_set_style_border_color(roller, lv_color_hex(BORDER_COLOR), LV_PART_MAIN);
  lv_obj_set_style_border_width(roller, 1, LV_PART_MAIN);
  lv_obj_set_style_bg_color(roller, lv_color_hex(PRIMARY_COLOR), LV_PART_SELECTED);

  return new_controller;
}

void controller_set_label(lv_obj_t* controller, const char* label) {
  lv_obj_t* existing_label = lv_obj_get_child(controller, 0);
  lv_label_set_text(existing_label, label);
}

void controller_set_options(lv_obj_t* controller, const char* options) {
  lv_obj_t* roller = lv_obj_get_child(controller, 1);
  lv_roller_set_options(roller, options, LV_ROLLER_MODE_NORMAL);
}

void controller_select_option(lv_obj_t* controller, uint8_t optionIndex) {
  lv_obj_t* roller = lv_obj_get_child(controller, 1);
  lv_roller_set_selected(roller, optionIndex, LV_ANIM_OFF);
}

static void handle_option_change(lv_event_t* event) {
  option_change_handler callback = (option_change_handler)lv_event_get_user_data(event);
  lv_obj_t* roller = lv_event_get_target(event);
  uint16_t optionIndex = lv_roller_get_selected(roller);
  callback(optionIndex);
}

void controller_handle_option_change(lv_obj_t* controller, option_change_handler callback) {
  static void* handler_descriptor = NULL;
  lv_obj_t* roller = lv_obj_get_child(controller, 1);
  if (handler_descriptor) lv_obj_remove_event_dsc(roller, handler_descriptor);
  handler_descriptor = lv_obj_add_event_cb(
    roller, 
    handle_option_change, 
    LV_EVENT_VALUE_CHANGED, 
    (void*)callback
  );
}
