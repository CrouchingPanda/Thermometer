#include "auth_screen.h"

#include <string.h>

#include <lvgl.h>

#include "../../pub_sub.h"
#include "../../theme.h"
#include "../../utils/container.h"
#include "../../utils/card.h"
#include "../../utils/screen.h"
#include "../top_layer/top_layer.h"

lv_obj_t* auth_screen;

static lv_obj_t* password_input_area;
static lv_obj_t* bad_password_alert;

static const char* keyset[] = {
  "1", "2", "3", "\n",
  "4", "5", "6", "\n",
  "7", "8", "9", "\n",
  LV_SYMBOL_BACKSPACE, "0", LV_SYMBOL_NEW_LINE, NULL // null must be last per the docs
};

static void handle_password_typing(lv_event_t* event) {
  lv_obj_t* buttons = lv_event_get_target(event);
  uint8_t touched_button_index = lv_btnmatrix_get_selected_btn(buttons);
  const char* button_text = lv_btnmatrix_get_btn_text(buttons, touched_button_index);
  if (strcmp(button_text, LV_SYMBOL_BACKSPACE) == 0) {
    lv_textarea_set_text(password_input_area, "");
  } else if (strcmp(button_text, LV_SYMBOL_NEW_LINE) == 0) {
    const char* input = lv_textarea_get_text(password_input_area);
    lv_event_send(lv_scr_act(), LV_EVENT_READY, (void*) input);
  } else {
    lv_textarea_add_text(password_input_area, button_text);
    lv_obj_add_flag(bad_password_alert, LV_OBJ_FLAG_HIDDEN);
  }
}

static void handle_screen_unload(lv_event_t* event) {
  lv_obj_add_flag(bad_password_alert, LV_OBJ_FLAG_HIDDEN);
}

static void handle_bad_auth_msg(void* _, lv_msg_t* message) {
  lv_obj_clear_flag(bad_password_alert, LV_OBJ_FLAG_HIDDEN);
  lv_textarea_set_text(password_input_area, "");
}

static void handle_successful_auth_msg(void* _, lv_msg_t* message) {
  lv_textarea_set_text(password_input_area, "");
}

void auth_screen_init(void) {
  auth_screen = make_screen();
  lv_obj_add_event_cb(auth_screen, handle_screen_unload, LV_EVENT_SCREEN_UNLOADED, NULL);

  lv_obj_t* content_wrapper = create_container(auth_screen);
  lv_obj_set_y(content_wrapper, TOP_BAR_HEIGHT);
  lv_obj_set_width(content_wrapper, lv_pct(100));
  lv_obj_set_height(content_wrapper, SCREEN_HEIGHT - TOP_BAR_HEIGHT);
  lv_obj_clear_flag(content_wrapper, LV_OBJ_FLAG_SCROLLABLE);
  lv_obj_set_style_pad_all(content_wrapper, PADDING, LV_PART_MAIN);
  lv_obj_set_flex_flow(content_wrapper, LV_FLEX_FLOW_COLUMN);
  lv_obj_set_flex_align(
    content_wrapper,
    LV_FLEX_ALIGN_SPACE_BETWEEN,
    LV_FLEX_ALIGN_CENTER,
    LV_FLEX_ALIGN_CENTER
  );

  lv_obj_t* password_input_wrapper = create_container(content_wrapper);
  lv_obj_set_height(password_input_wrapper, LV_SIZE_CONTENT);
  lv_obj_set_width(password_input_wrapper, lv_pct(102));
  lv_obj_set_flex_flow(password_input_wrapper, LV_FLEX_FLOW_ROW);
  lv_obj_set_flex_align(
    password_input_wrapper,
    LV_FLEX_ALIGN_START,
    LV_FLEX_ALIGN_CENTER,
    LV_FLEX_ALIGN_CENTER
  );

  password_input_area = lv_textarea_create(password_input_wrapper);
  lv_obj_set_width(password_input_area, lv_pct(60));
  lv_textarea_set_placeholder_text(password_input_area, "Password");
  lv_textarea_set_one_line(password_input_area, true);
  lv_textarea_set_password_mode(password_input_area, true);
  lv_obj_clear_flag(password_input_area, LV_OBJ_FLAG_CLICKABLE);
  lv_obj_set_style_text_color(password_input_area, lv_color_hex(TEXT_COLOR), LV_PART_MAIN);
  lv_obj_set_style_text_color(password_input_area, lv_color_hex(PLACEHOLDER_TEXT_COLOR), LV_PART_TEXTAREA_PLACEHOLDER);
  lv_obj_set_style_bg_color(password_input_area, lv_color_hex(TEXT_AREA_BACKGROUND_COLOR), LV_PART_MAIN);
  lv_obj_set_style_border_color(password_input_area, lv_color_hex(BORDER_COLOR), LV_PART_MAIN);
  lv_obj_set_style_border_width(password_input_area, 1, LV_PART_MAIN);
  lv_obj_set_style_radius(password_input_area, BORDER_RADIUS, LV_PART_MAIN);
  lv_obj_set_style_radius(password_input_area, BORDER_RADIUS, LV_PART_ITEMS);

  bad_password_alert = lv_label_create(password_input_wrapper);
  lv_label_set_text(bad_password_alert, "Bad password");
  lv_obj_add_flag(bad_password_alert, LV_OBJ_FLAG_HIDDEN);
  lv_obj_set_style_text_color(bad_password_alert, lv_color_hex(DANGER_TEXT_COLOR), LV_PART_MAIN);

  lv_obj_t* password_input_keyboard = create_card(content_wrapper);
  lv_obj_set_width(password_input_keyboard, lv_pct(100));
  lv_obj_set_flex_grow(password_input_keyboard, 1);
  lv_obj_clear_flag(password_input_keyboard, LV_OBJ_FLAG_SCROLLABLE);

  lv_obj_t* buttons = lv_btnmatrix_create(password_input_keyboard);
  lv_obj_set_size(buttons, lv_pct(100), lv_pct(100));
  lv_obj_set_style_pad_all(buttons, 0, LV_PART_MAIN);
  lv_obj_set_pos(buttons, 2, 2); // manually center inside parent - align-center doesn't work
  lv_obj_set_style_bg_opa(buttons, LV_OPA_TRANSP, LV_PART_MAIN);
  lv_obj_set_style_border_side(buttons, LV_BORDER_SIDE_NONE, LV_PART_MAIN);
  lv_obj_set_style_text_font(buttons, &lv_font_montserrat_18, LV_PART_MAIN);
  lv_obj_set_style_shadow_width(buttons, 0, LV_PART_ITEMS);
  lv_obj_set_style_radius(buttons, BORDER_RADIUS, LV_PART_ITEMS);
  lv_obj_set_style_bg_color(buttons, lv_color_hex(PRIMARY_COLOR), LV_PART_ITEMS);
  lv_obj_set_style_text_color(buttons, lv_color_hex(TEXT_COLOR), LV_PART_ITEMS);
  lv_obj_clear_flag(buttons, LV_OBJ_FLAG_CLICK_FOCUSABLE);
  lv_obj_add_flag(buttons, LV_OBJ_FLAG_ADV_HITTEST);
  lv_btnmatrix_set_map(buttons, keyset);
  lv_btnmatrix_set_btn_ctrl_all(buttons, LV_BTNMATRIX_CTRL_NO_REPEAT);
  lv_obj_add_event_cb(buttons, handle_password_typing, LV_EVENT_VALUE_CHANGED, NULL);

  lv_msg_subscribe(BAD_AUTH_ATTEMPT, handle_bad_auth_msg, NULL);
  lv_msg_subscribe(SUCCESSFUL_AUTH, handle_successful_auth_msg, NULL);
}