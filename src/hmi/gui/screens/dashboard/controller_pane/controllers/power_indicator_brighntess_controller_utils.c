#include "power_indicator_brightness_controller_utils.h"

#include <stdio.h>
#include <string.h>
#include <stdint.h>
#include <stdbool.h>
#include <stdlib.h>

#include <lvgl.h>

#include "controller.h"

#define BRIGHNTESS_OPTION_SIZE 4 // three digits + newline or NULL

const char* brightness_options(void) {
  static char options[BRIGHNTESS_OPTION_COUNT * BRIGHNTESS_OPTION_SIZE + 1]; // + 1 for NULL at the end
  for (size_t i = 0; i < BRIGHNTESS_OPTION_COUNT; ++i) {
    uint8_t brightness = i * BRIGHTNESS_OPTION_STEP;
    bool isLastOption = i == BRIGHNTESS_OPTION_COUNT - 1;
    snprintf(
      options + (i * BRIGHNTESS_OPTION_SIZE),
      isLastOption ? BRIGHNTESS_OPTION_SIZE : BRIGHNTESS_OPTION_SIZE + 1,
      isLastOption ? "%3d" : "%3d\n",
      brightness
    );
  }
  return options;
}

void update_brightness_controller(lv_event_t* event) {
  lv_obj_t* controller = lv_event_get_target(event);
  lv_msg_t* message = lv_event_get_msg(event);
  uint8_t* brightness = (uint8_t*) lv_msg_get_payload(message);
  for (size_t i = 0; i < BRIGHNTESS_OPTION_COUNT; ++i) {
    if (*brightness <= i * BRIGHTNESS_OPTION_STEP) {
      controller_select_option(controller, i);
      return;
    }
  }
  controller_select_option(controller, 0);
}