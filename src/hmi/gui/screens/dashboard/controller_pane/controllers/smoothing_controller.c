#include "smoothing_controller.h"

#include <stdio.h>
#include <string.h>
#include <stdint.h>
#include <stdbool.h>
#include <stdlib.h>

#include <lvgl.h>

#include "controller.h"
#include "../../../../pub_sub.h"

#define SMOOTHING_OPTION_SIZE 3
#define SMOOTHING_OPTION_COUNT 100 // including "OFF"
#define SMOOTHING_OFF_OPTION "OFF\n"
#define SMOOTHING_OFF_OPTION_SIZE 4

static const char* smoothing_options(void) {
  static char options[(SMOOTHING_OPTION_COUNT - 1) * SMOOTHING_OPTION_SIZE + SMOOTHING_OFF_OPTION_SIZE];
  strncpy(options, SMOOTHING_OFF_OPTION, SMOOTHING_OFF_OPTION_SIZE);
  for (size_t i = 0; i < (SMOOTHING_OPTION_COUNT - 1); ++i) {
    uint8_t smoothingFactor = i + 1;
    bool isLastOption = i == (SMOOTHING_OPTION_COUNT - 1) - 1;
    snprintf(
      options + SMOOTHING_OFF_OPTION_SIZE + (i * SMOOTHING_OPTION_SIZE),
      isLastOption ? SMOOTHING_OPTION_SIZE : SMOOTHING_OPTION_SIZE + 1,
      isLastOption ? "%2d" : "%2d\n",
      smoothingFactor
    );
  }
  return options;
}

static void update_controller(lv_event_t* event) {
  lv_obj_t* controller = lv_event_get_target(event);
  lv_msg_t* message = lv_event_get_msg(event);
  uint8_t* smoothingFactor = (uint8_t*)lv_msg_get_payload(message);
  uint16_t optionIndex = *smoothingFactor == 100 ? 0 : *smoothingFactor;
  controller_select_option(controller, optionIndex);
}

static void set_new_object_temperature_smoother_factor(uint8_t optionIndex) {
  uint8_t factor = optionIndex;
  lv_msg_send(SET_OBJECT_TEMPERATURE_SMOOTHER_FACTOR, &factor);
}

lv_obj_t* make_smoothing_controller(lv_obj_t* parent) {
  lv_obj_t* controller = make_controller(parent);
  controller_set_label(controller, "Smoothing");
  controller_set_options(controller, smoothing_options());
  controller_handle_option_change(controller, set_new_object_temperature_smoother_factor);
  lv_obj_add_event_cb(controller, update_controller, LV_EVENT_MSG_RECEIVED, NULL);
  lv_msg_subsribe_obj(NEW_OBJECT_TEMPERATURE_SMOOTHING, controller, NULL);
  return controller;
}