#include "emissivity_controller.h"

#include <stdio.h>
#include <string.h>
#include <stdint.h>
#include <stdbool.h>
#include <math.h>
#include <stdlib.h>

#include <lvgl.h>

#include "controller.h"
#include "../../../../pub_sub.h"

#define EMISSIVITY_OPTION_SIZE 5 // 0.XX and a newline (except last where it's NULL instead of a newline)
#define EMISSIVITY_OPTION_COUNT 90
#define MAX_EMISSIVITY 0.99
#define EMISSIVITY_STEP 0.01

static const char* emissivity_options(void) {
  static char options[EMISSIVITY_OPTION_COUNT * EMISSIVITY_OPTION_SIZE];
  for (size_t i = 0; i < EMISSIVITY_OPTION_COUNT; ++i) {
    float emissivity = MAX_EMISSIVITY - (i * EMISSIVITY_STEP);
    bool isLastOption = i == EMISSIVITY_OPTION_COUNT - 1;
    snprintf(
      options + (i * EMISSIVITY_OPTION_SIZE), // pointer to the slice of array we are filling
      isLastOption ? EMISSIVITY_OPTION_SIZE : EMISSIVITY_OPTION_SIZE + 1,
      isLastOption ? "%.2f" : "%.2f\n",
      emissivity
    );
  }
  return options;
}

static void set_new_emissivity(uint8_t optionIndex) {
  float emissivity = MAX_EMISSIVITY - optionIndex * EMISSIVITY_STEP;
  lv_msg_send(SET_EMISSIVITY, &emissivity);
}

static void update_controller(lv_event_t* event) {
  lv_obj_t* controller = lv_event_get_target(event);
  lv_msg_t* message = lv_event_get_msg(event);
  float* emissivity = (float*)lv_msg_get_payload(message);
  uint8_t optionIndex = nearbyint((MAX_EMISSIVITY - *emissivity) / EMISSIVITY_STEP);
  controller_select_option(controller, optionIndex);
}

lv_obj_t* make_emissivity_controller(lv_obj_t* parent) {
  lv_obj_t* controller = make_controller(parent);
  controller_set_label(controller, "Emissivity");
  controller_set_options(controller, emissivity_options());
  controller_handle_option_change(controller, set_new_emissivity);
  lv_obj_add_event_cb(controller, update_controller, LV_EVENT_MSG_RECEIVED, NULL);
  lv_msg_subsribe_obj(NEW_EMISSIVITY_READING, controller, NULL);
  return controller;
}