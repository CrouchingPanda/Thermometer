#include "top_layer.h"

#include <stdbool.h>
#include <stdint.h>
#include <stdlib.h>

#include <lvgl.h>

#include "../../pub_sub.h"
#include "../../theme.h"
#include "../../utils/card.h"
#include "../../fonts/fonts.h"

#define LOW_BATTERY_PERCENTAGE 10

static void update_calibration_board_connection_indicator(lv_event_t* event) {
  lv_msg_t* message = lv_event_get_msg(event);
  bool isConnected = *(bool*)lv_msg_get_payload(message);
  lv_obj_t* connection_status_indicator = lv_event_get_target(event);
  if (isConnected) {
    lv_obj_clear_flag(connection_status_indicator, LV_OBJ_FLAG_HIDDEN);
  } else {
    lv_obj_add_flag(connection_status_indicator, LV_OBJ_FLAG_HIDDEN);
  }
}

static void update_charging_indicator(lv_event_t* event) {
  lv_msg_t* message = lv_event_get_msg(event);
  bool isCharging = *(bool*)lv_msg_get_payload(message);
  lv_obj_t* charging_indicator = lv_event_get_target(event);
  if (isCharging) {
    lv_obj_clear_flag(charging_indicator, LV_OBJ_FLAG_HIDDEN);
  } else {
    lv_obj_add_flag(charging_indicator, LV_OBJ_FLAG_HIDDEN);
  }
}

static void update_battery_indicator(lv_event_t* event) {
  lv_msg_t* message = lv_event_get_msg(event);
  uint8_t batteryChargePercentage = *(uint8_t*)lv_msg_get_payload(message);

  const char* batterySymbol;
  if (batteryChargePercentage > 90) {
    batterySymbol = LV_SYMBOL_BATTERY_FULL;
  } else if (batteryChargePercentage > 70) {
    batterySymbol = LV_SYMBOL_BATTERY_3;
  } else if (batteryChargePercentage > 30) {
    batterySymbol = LV_SYMBOL_BATTERY_2;
  } else if (batteryChargePercentage > 10) {
    batterySymbol = LV_SYMBOL_BATTERY_1;
  } else {
    batterySymbol = LV_SYMBOL_BATTERY_EMPTY;
  }

  lv_obj_t* battery_indicator = lv_event_get_target(event);
  lv_label_set_text(battery_indicator, batterySymbol);
}

static void update_low_battery_modal(lv_event_t* event) {
  lv_msg_t* message = lv_event_get_msg(event);
  uint8_t batteryChargePercentage = *(uint8_t*)lv_msg_get_payload(message);
  lv_obj_t* low_battery_modal = lv_event_get_target(event);
  if (batteryChargePercentage > LOW_BATTERY_PERCENTAGE + 1) {
    lv_obj_add_flag(low_battery_modal, LV_OBJ_FLAG_HIDDEN);
  } else if (batteryChargePercentage < LOW_BATTERY_PERCENTAGE) {
    lv_obj_clear_flag(low_battery_modal, LV_OBJ_FLAG_HIDDEN);
  }
}

void top_layer_init(void) {

  // low battery modal
  lv_obj_t* low_battery_modal = lv_obj_create(lv_layer_top());
  lv_obj_set_size(low_battery_modal, lv_pct(100), lv_pct(100));
  lv_obj_set_style_border_side(low_battery_modal, LV_BORDER_SIDE_NONE, LV_PART_MAIN);
  lv_obj_set_style_bg_color(low_battery_modal, lv_color_hex(BACKGROUND_COLOR), LV_PART_MAIN);
  lv_obj_set_style_bg_opa(low_battery_modal, 196, LV_PART_MAIN);
  lv_obj_add_event_cb(low_battery_modal, update_low_battery_modal, LV_EVENT_MSG_RECEIVED, NULL);
  lv_msg_subsribe_obj(NEW_BATTERY_PERCENTAGE, low_battery_modal, NULL);

  lv_obj_t* low_battery_card = create_card(low_battery_modal);
  lv_obj_set_style_pad_all(low_battery_card, 16, LV_PART_MAIN);
  lv_obj_set_style_bg_color(low_battery_card, lv_color_hex(PRIMARY_COLOR), LV_PART_MAIN);
  lv_obj_set_style_text_font(low_battery_card, &font_Roboto_22, LV_PART_MAIN);
  lv_obj_set_style_text_align(low_battery_card, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN);

  lv_obj_t* low_battery_alert = lv_label_create(low_battery_card);
  lv_label_set_text(low_battery_alert, "BATTERY LOW.\nPLEASE CHARGE.");

  // top bar
  lv_obj_t* top_bar = create_card(lv_layer_top());
  lv_obj_set_height(top_bar, TOP_BAR_HEIGHT);
  lv_obj_set_width(top_bar, lv_pct(100));
  lv_obj_set_align(top_bar, LV_ALIGN_TOP_MID);
  lv_obj_set_style_radius(top_bar, 0, LV_PART_MAIN);
  lv_obj_clear_flag(top_bar, LV_OBJ_FLAG_SCROLLABLE);
  lv_obj_set_layout(top_bar, LV_LAYOUT_FLEX);
  lv_obj_set_flex_flow(top_bar, LV_FLEX_FLOW_ROW);
  lv_obj_set_flex_align(top_bar, LV_FLEX_ALIGN_END, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER);
  lv_obj_set_style_text_font(top_bar, &lv_font_montserrat_14, LV_PART_MAIN);
  lv_obj_set_style_text_color(top_bar, lv_color_hex(SECONDARY_TEXT_COLOR), LV_PART_MAIN);

  lv_obj_t* calibration_board_conn_indicator = lv_label_create(top_bar);
  lv_label_set_text(calibration_board_conn_indicator, LV_SYMBOL_REFRESH);
  lv_obj_add_flag(calibration_board_conn_indicator, LV_OBJ_FLAG_HIDDEN);
  lv_obj_add_event_cb(
    calibration_board_conn_indicator, 
    update_calibration_board_connection_indicator, 
    LV_EVENT_MSG_RECEIVED, 
    NULL
  );
  lv_msg_subsribe_obj(NEW_CALIBRATION_BOARD_CONNECTION, calibration_board_conn_indicator, NULL);

  lv_obj_t* charging_indicator = lv_label_create(top_bar);
  lv_label_set_text(charging_indicator, LV_SYMBOL_CHARGE);
  lv_obj_add_flag(charging_indicator, LV_OBJ_FLAG_HIDDEN);
  lv_obj_add_event_cb(charging_indicator, update_charging_indicator, LV_EVENT_MSG_RECEIVED, NULL);
  lv_msg_subsribe_obj(NEW_BATTERY_CHARGING_STATUS, charging_indicator, NULL);

  lv_obj_t* battery_indicator = lv_label_create(top_bar);
  lv_obj_add_event_cb(battery_indicator, update_battery_indicator, LV_EVENT_MSG_RECEIVED, NULL);
  lv_msg_subsribe_obj(NEW_BATTERY_PERCENTAGE, battery_indicator, NULL);
}