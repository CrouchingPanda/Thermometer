#include "room_temperature_offset_controller.h"

#include <stdio.h>
#include <string.h>
#include <stdint.h>
#include <stdbool.h>
#include <math.h>
#include <stdlib.h>

#include <lvgl.h>

#include "controller.h"
#include "../../../../pub_sub.h"

#define OFFSET_OPTION_SIZE 5 // (-/+)X.X and a newline (except last where it's NULL instead)
#define MAX_OFFSET 5.0
#define MIN_OFFSET -5.0
#define OFFSET_STEP 0.1
#define OFFSET_OPTION_COUNT 101 // (MAX_OFFSET - MIN_OFFSET) / OFFSET_STEP + 1 (+1 for 0.0)

static const char* offset_options(void) {
  static char options[OFFSET_OPTION_COUNT * OFFSET_OPTION_SIZE];
  for (size_t i = 0; i < OFFSET_OPTION_COUNT; ++i) {
    float offset = MAX_OFFSET - (i * OFFSET_STEP);
    bool isLastOption = i == OFFSET_OPTION_COUNT - 1;
    snprintf(
      options + (i * OFFSET_OPTION_SIZE), // pointer to the slice of array we are filling
      isLastOption ? OFFSET_OPTION_SIZE : OFFSET_OPTION_SIZE + 1,
      isLastOption ? "%+.1f" : "%+.1f\n",
      offset
    );
  }
  return options;
}

static void set_new_offset(uint8_t optionIndex) {
  float offset = MAX_OFFSET - optionIndex * OFFSET_STEP;
  lv_msg_send(SET_ROOM_TEMPERATURE_OFFSET, &offset);
}

static void update_controller(lv_event_t* event) {
  lv_obj_t* controller = lv_event_get_target(event);
  lv_msg_t* message = lv_event_get_msg(event);
  float* offset = (float*) lv_msg_get_payload(message);
  uint8_t optionIndex = nearbyint((MAX_OFFSET - *offset) / OFFSET_STEP);
  controller_select_option(controller, optionIndex);
}

lv_obj_t* make_room_temperature_offset_controller(lv_obj_t* parent) {
  lv_obj_t* controller = make_controller(parent);
  controller_set_label(controller, "Amb Offset\nCelsius");
  controller_set_options(controller, offset_options());
  controller_handle_option_change(controller, set_new_offset);
  lv_obj_add_event_cb(controller, update_controller, LV_EVENT_MSG_RECEIVED, NULL);
  lv_msg_subsribe_obj(NEW_ROOM_TEMPERATURE_OFFSET, controller, NULL);
  return controller;
}