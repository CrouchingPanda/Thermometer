#include "timeout_controller_utils.h"

#include <stdio.h>
#include <string.h>
#include <stdint.h>
#include <stdbool.h>
#include <stdlib.h>

#include <lvgl.h>

#include "controller.h"

#define TIMEOUT_OPTION_SIZE 3
#define TIMEOUT_OFF_OPTION "OFF\n"
#define TIMEOUT_OFF_OPTION_SIZE 4

const char* timeout_options(void) {
  static char options[(TIMEOUT_OPTION_COUNT - 1) * TIMEOUT_OPTION_SIZE + TIMEOUT_OFF_OPTION_SIZE];
  strncpy(options, TIMEOUT_OFF_OPTION, TIMEOUT_OFF_OPTION_SIZE);
  for (size_t i = 0; i < TIMEOUT_OPTION_COUNT - 1; ++i) {
    uint8_t timeoutMins = (i + 1) * TIMEOUT_OPTION_STEP;
    bool isLastOption = i == (TIMEOUT_OPTION_COUNT - 1) - 1;
    snprintf(
      options + TIMEOUT_OFF_OPTION_SIZE + (i * TIMEOUT_OPTION_SIZE),
      isLastOption ? TIMEOUT_OPTION_SIZE : TIMEOUT_OPTION_SIZE + 1,
      isLastOption ? "%2d" : "%2d\n",
      timeoutMins
    );
  }
  return options;
}

void update_timeout_controller(lv_event_t* event) {
  lv_obj_t* controller = lv_event_get_target(event);
  lv_msg_t* message = lv_event_get_msg(event);
  uint8_t* timeoutMins = (uint8_t*) lv_msg_get_payload(message);
  for (size_t i = 0; i < TIMEOUT_OPTION_COUNT; ++i) {
    if (*timeoutMins <= i * TIMEOUT_OPTION_STEP) {
      controller_select_option(controller, i);
      return;
    }
  }
  controller_select_option(controller, 0);
}