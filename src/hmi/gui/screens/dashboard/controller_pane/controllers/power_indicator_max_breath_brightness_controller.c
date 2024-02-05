#include "power_indicator_max_breath_brightness_controller.h"

#include <stdio.h>
#include <stdint.h>

#include <lvgl.h>

#include "controller.h"
#include "power_indicator_brightness_controller_utils.h"
#include "../../../../pub_sub.h"

static void set_brightness(uint8_t optionIndex) {
  uint8_t brightness = optionIndex * BRIGHTNESS_OPTION_STEP;
  lv_msg_send(SET_POWER_INDICATOR_MAX_BREATH_BRIGHTNESS, &brightness);
}

lv_obj_t* make_power_indicator_max_breath_brightness_controller(lv_obj_t* parent) {
  lv_obj_t* controller = make_controller(parent);
  controller_set_label(controller, "Power Button\nMax Breath");
  controller_set_options(controller, brightness_options());
  controller_handle_option_change(controller, set_brightness);
  lv_obj_add_event_cb(controller, update_brightness_controller, LV_EVENT_MSG_RECEIVED, NULL);
  lv_msg_subsribe_obj(NEW_POWER_INDICATOR_MAX_BREATH_BRIGHTNESS, controller, NULL);
  return controller;
}