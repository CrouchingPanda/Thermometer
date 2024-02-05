#include "power_off_timeout_controller.h"

#include <stdio.h>
#include <stdint.h>

#include <lvgl.h>

#include "controller.h"
#include "timeout_controller_utils.h"
#include "../../../../pub_sub.h"

static void set_power_off_timeout(uint8_t optionIndex) {
  uint8_t timeout_mins = optionIndex * TIMEOUT_OPTION_STEP;
  lv_msg_send(SET_POWER_OFF_TIMEOUT, &timeout_mins);
}

lv_obj_t* make_power_off_timeout_controller(lv_obj_t* parent) {
  lv_obj_t* controller = make_controller(parent);
  controller_set_label(controller, "Minutes To\nPower Off");
  controller_set_options(controller, timeout_options());
  controller_handle_option_change(controller, set_power_off_timeout);
  lv_obj_add_event_cb(controller, update_timeout_controller, LV_EVENT_MSG_RECEIVED, NULL);
  lv_msg_subsribe_obj(NEW_POWER_OFF_TIMEOUT, controller, NULL);
  return controller;
}