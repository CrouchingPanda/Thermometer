#include "display_brightness_controller.h"

#include <stdio.h>
#include <string.h>
#include <stdint.h>
#include <stdbool.h>
#include <stdlib.h>

#include <lvgl.h>

#include "controller.h"
#include "../../../../pub_sub.h"

#define BRIGHTNESS_OPTION_SIZE 4 // XXX and a newline (except last where it's NULL instead of a newline)
#define BRIGHTNESS_OPTION_COUNT 46
#define MAX_BRIGHTNESS 255
#define BRIGHTNESS_STEP 5

static const char* brightness_options(void) {
  static char options[BRIGHTNESS_OPTION_COUNT * BRIGHTNESS_OPTION_SIZE];
  for (size_t i = 0; i < BRIGHTNESS_OPTION_COUNT; ++i) {
    uint8_t brightness = MAX_BRIGHTNESS - (i * BRIGHTNESS_STEP);
    bool isLastOption = i == BRIGHTNESS_OPTION_COUNT - 1;
    snprintf(
      options + (i * BRIGHTNESS_OPTION_SIZE), // pointer to the slice of array we are filling
      isLastOption ? BRIGHTNESS_OPTION_SIZE : BRIGHTNESS_OPTION_SIZE + 1,
      isLastOption ? "%3d" : "%3d\n",
      brightness
    );
  }
  return options;
}

static void set_new_brightness(uint8_t optionIndex) {
  uint8_t brightness = MAX_BRIGHTNESS - optionIndex * BRIGHTNESS_STEP;
  lv_msg_send(SET_DISPLAY_BRIGHTNESS, &brightness);
}

static void update_controller(lv_event_t* event) {
  lv_obj_t* controller = lv_event_get_target(event);
  lv_msg_t* message = lv_event_get_msg(event);
  uint8_t* newBrightness = (uint8_t*) lv_msg_get_payload(message);
  for (size_t i = 0; i < BRIGHTNESS_OPTION_COUNT; ++i) {
    uint8_t brightness = MAX_BRIGHTNESS - i * BRIGHTNESS_STEP;
    if (*newBrightness >= brightness) {
      controller_select_option(controller, i);
      return;
    }
  }
  
  controller_select_option(controller, BRIGHTNESS_OPTION_COUNT - 1);
}

lv_obj_t* make_display_brightness_controller(lv_obj_t* parent) {
  lv_obj_t* controller = make_controller(parent);
  controller_set_label(controller, "Backlight");
  controller_set_options(controller, brightness_options());
  controller_handle_option_change(controller, set_new_brightness);
  lv_obj_add_event_cb(controller, update_controller, LV_EVENT_MSG_RECEIVED, NULL);
  lv_msg_subsribe_obj(NEW_DISPLAY_BRIGHTNESS, controller, NULL);
  return controller;
}