#include <stdio.h>
#include <stdint.h>

#include "low_power_mode_timeout_controller.h"

#include <lvgl.h>

#include "controller.h"
#include "timeout_controller_utils.h"
#include "../../../pub_sub.h"

static void set_low_power_mode_timeout(uint8_t optionIndex) {
  uint8_t timeout_mins = optionIndex * TIMEOUT_OPTION_STEP;
  lv_msg_send(SET_LOW_POWER_MODE_TIMEOUT, &timeout_mins);
}

void make_low_power_mode_timeout_controller(lv_obj_t* parent) {
  lv_obj_t* controller = make_controller(parent);
  controller_set_label(controller, "Minutes To\nLow Power");
  controller_set_options(controller, timeout_options());
  controller_handle_option_change(controller, set_low_power_mode_timeout);
  lv_obj_add_event_cb(controller, update_timeout_controller, LV_EVENT_MSG_RECEIVED, NULL);
  lv_msg_subsribe_obj(NEW_LOW_POWER_MODE_TIMEOUT, controller, NULL);
}