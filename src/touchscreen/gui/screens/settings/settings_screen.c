#include <stdio.h>
#include <string.h>
#include <stdint.h>
#include <math.h>

#include "settings_screen.h"

#include <lvgl.h>

#include "temperature_readout.h"
#include "controller.h"
#include "../top_layer/top_layer.h"
#include "../../pub_sub.h"
#include "../../theme.h"
#include "../../utils/screen.h"
#include "../../utils/container.h"

lv_obj_t* settings_screen;

#define SMOOTHING_OPTION_SIZE 3 // from 1 (padded with a space) to 99 and a newline at the end
#define SMOOTHING_OPTION_COUNT 99
#define SMOOTHING_OFF_OPTION "OFF\n"
#define SMOOTHING_OFF_OPTION_SIZE 4

static const char* smoothing_options(void) {
  static char options[SMOOTHING_OPTION_COUNT * SMOOTHING_OPTION_SIZE + SMOOTHING_OFF_OPTION_SIZE];
  strncpy(options, SMOOTHING_OFF_OPTION, SMOOTHING_OFF_OPTION_SIZE);
  for (int i = 0; i < SMOOTHING_OPTION_COUNT; i++) {
    int smoothing_factor = i + 1;
    bool isLastOption = i == SMOOTHING_OPTION_COUNT - 1;
    snprintf(
      options + SMOOTHING_OFF_OPTION_SIZE + (i * SMOOTHING_OPTION_SIZE),
      isLastOption ? SMOOTHING_OPTION_SIZE : SMOOTHING_OPTION_SIZE + 1, // +1 for NULL at the end
      isLastOption ? "%2d" : "%2d\n",
      smoothing_factor
    );
  }
  return options;
}

// 0.99 and a newline (except last where it's NULL instead of a newline)
#define EMISSIVITY_OPTION_SIZE 5
#define EMISSIVITY_OPTION_COUNT 90
#define MAX_EMISSIVITY 0.99
#define EMISSIVITY_STEP 0.01

static const char* emissivity_options(void) {
  static char options[EMISSIVITY_OPTION_COUNT * EMISSIVITY_OPTION_SIZE];
  for (int i = 0; i < EMISSIVITY_OPTION_COUNT; i++) {
    float emissivity = MAX_EMISSIVITY - (i * EMISSIVITY_STEP);
    bool isLastOption = i == EMISSIVITY_OPTION_COUNT - 1;
    snprintf(
      options + (i * EMISSIVITY_OPTION_SIZE), // pointer to the slice of array we are filling
      isLastOption ? EMISSIVITY_OPTION_SIZE : EMISSIVITY_OPTION_SIZE + 1, // +1 for NULL at the end
      isLastOption ? "%.2f" : "%.2f\n",
      emissivity
    );
  }
  return options;
}

static void update_temperature_readout(lv_event_t* event) {
  lv_obj_t* readout = lv_event_get_target(event);
  lv_msg_t* message = lv_event_get_msg(event);
  float* temperature = (float*) lv_msg_get_payload(message);
  temperature_readout_set_reading(readout, *temperature);
}

static void update_emissivity_control(lv_event_t* event) {
  lv_obj_t* controller = lv_event_get_target(event);
  lv_msg_t* message = lv_event_get_msg(event);
  float* emissivity = (float*) lv_msg_get_payload(message);
  uint8_t option_index = nearbyint(100 * (MAX_EMISSIVITY - *emissivity));
  controller_select_option(controller, option_index);
}

static void set_new_emissivity(uint16_t emissivity_control_option_index) {
  float emissivity = MAX_EMISSIVITY - emissivity_control_option_index / 100.0;
  lv_msg_send(SET_EMISSIVITY_MSG, &emissivity);
}

static void update_smoothing_control(lv_event_t* event) {
  lv_obj_t* controller = lv_event_get_target(event);
  lv_msg_t* message = lv_event_get_msg(event);
  uint8_t* smoothing_factor = (uint8_t*) lv_msg_get_payload(message);
  uint16_t option_index = *smoothing_factor == 100 ? 0 : *smoothing_factor;
  controller_select_option(controller, option_index);
}

static uint8_t get_smoothing_factor_from_option_index(uint16_t smoother_control_option_index) {
  return smoother_control_option_index == 0 ? 100 : smoother_control_option_index;
}

static void set_new_object_temperature_smoother_factor(uint16_t smoother_control_option_index) {
  uint8_t factor = get_smoothing_factor_from_option_index(smoother_control_option_index);
  lv_msg_send(SET_OBJECT_TEMPERATURE_SMOOTHER_FACTOR_MSG, &factor);
}

static void set_new_ambient_temperature_smoother_factor(uint16_t smoother_control_option_index) {
  uint8_t factor = get_smoothing_factor_from_option_index(smoother_control_option_index);
  lv_msg_send(SET_AMBIENT_TEMPERATURE_SMOOTHER_FACTOR_MSG, &factor);
}

static void set_new_thermistor_temperature_smoother_factor(uint16_t smoother_control_option_index) {
  uint8_t factor = get_smoothing_factor_from_option_index(smoother_control_option_index);
  lv_msg_send(SET_THERMISTOR_TEMPERATURE_SMOOTHER_FACTOR_MSG, &factor);
}

void settings_screen_init(void) {
  settings_screen = make_screen();

  lv_obj_t* content_wrapper = create_container(settings_screen);
  lv_obj_set_y(content_wrapper, TOP_BAR_HEIGHT);
  lv_obj_set_width(content_wrapper, lv_pct(100));
  lv_obj_set_height(content_wrapper, SCREEN_HEIGHT - TOP_BAR_HEIGHT);
  lv_obj_clear_flag(content_wrapper, LV_OBJ_FLAG_SCROLLABLE);
  lv_obj_set_style_pad_all(content_wrapper, PADDING, LV_PART_MAIN);
  lv_obj_set_flex_flow(content_wrapper, LV_FLEX_FLOW_ROW);
  lv_obj_set_flex_align(
    content_wrapper,
    LV_FLEX_ALIGN_START,
    LV_FLEX_ALIGN_CENTER,
    LV_FLEX_ALIGN_CENTER
  );

  lv_obj_t* control_wrapper = create_container(content_wrapper);
  lv_obj_set_height(control_wrapper, lv_pct(100));
  lv_obj_clear_flag(control_wrapper, LV_OBJ_FLAG_SCROLL_MOMENTUM);
  lv_obj_clear_flag(control_wrapper, LV_OBJ_FLAG_SCROLL_ELASTIC);
  lv_obj_set_flex_grow(control_wrapper, 1);
  lv_obj_set_flex_flow(control_wrapper, LV_FLEX_FLOW_ROW);
  lv_obj_set_flex_align(
    control_wrapper,
    LV_FLEX_ALIGN_START,
    LV_FLEX_ALIGN_CENTER,
    LV_FLEX_ALIGN_CENTER
  );

  lv_obj_t* emissivity_control = make_controller(control_wrapper);
  controller_set_label(emissivity_control, "Emissivity");
  controller_set_options(emissivity_control, emissivity_options());
  controller_handle_option_change(emissivity_control, set_new_emissivity);
  lv_obj_add_event_cb(emissivity_control, update_emissivity_control, LV_EVENT_MSG_RECEIVED, NULL);
  lv_msg_subsribe_obj(NEW_EMISSIVITY_READING_MSG, emissivity_control, NULL);
  
  lv_obj_t* object_smoothing_control = make_controller(control_wrapper);
  controller_set_label(object_smoothing_control, "IR Object\nSmoothing");
  controller_set_options(object_smoothing_control, smoothing_options());
  controller_handle_option_change(object_smoothing_control, set_new_object_temperature_smoother_factor);
  lv_obj_add_event_cb(object_smoothing_control, update_smoothing_control, LV_EVENT_MSG_RECEIVED, NULL);
  lv_msg_subsribe_obj(NEW_OBJECT_TEMPERATURE_SMOOTHING_MSG, object_smoothing_control, NULL);

  lv_obj_t* ambient_smoothing_control = make_controller(control_wrapper);
  controller_set_label(ambient_smoothing_control, "Die Temp\nSmoothing");
  controller_set_options(ambient_smoothing_control, smoothing_options());
  controller_handle_option_change(ambient_smoothing_control, set_new_ambient_temperature_smoother_factor);
  lv_obj_add_event_cb(ambient_smoothing_control, update_smoothing_control, LV_EVENT_MSG_RECEIVED, NULL);
  lv_msg_subsribe_obj(NEW_AMBIENT_TEMPERATURE_SMOOTHING_MSG, ambient_smoothing_control, NULL);

  lv_obj_t* thermistor_smoothing_control = make_controller(control_wrapper);
  controller_set_label(thermistor_smoothing_control, "Thermistor\nSmoothing");
  controller_set_options(thermistor_smoothing_control, smoothing_options());
  controller_handle_option_change(thermistor_smoothing_control, set_new_thermistor_temperature_smoother_factor);
  lv_obj_add_event_cb(thermistor_smoothing_control, update_smoothing_control, LV_EVENT_MSG_RECEIVED, NULL);
  lv_msg_subsribe_obj(NEW_THERMISTOR_TEMPERATURE_SMOOTHING_MSG, thermistor_smoothing_control, NULL);

  lv_obj_t* readout_wrapper = create_container(content_wrapper);
  lv_obj_set_height(readout_wrapper, lv_pct(100));
  lv_obj_set_width(readout_wrapper, lv_pct(33));
  lv_obj_clear_flag(readout_wrapper, LV_OBJ_FLAG_SCROLLABLE);
  lv_obj_set_flex_flow(readout_wrapper, LV_FLEX_FLOW_COLUMN);
  lv_obj_set_flex_align(
    readout_wrapper,
    LV_FLEX_ALIGN_START,
    LV_FLEX_ALIGN_CENTER,
    LV_FLEX_ALIGN_CENTER
  );

  lv_obj_t* object_temperature_readout = make_temperature_readout(readout_wrapper);
  temperature_readout_set_label(object_temperature_readout, "Object");
  lv_obj_add_event_cb(object_temperature_readout, update_temperature_readout, LV_EVENT_MSG_RECEIVED, NULL);
  lv_msg_subsribe_obj(NEW_OBJECT_TEMPERATURE_MSG, object_temperature_readout, NULL);

  lv_obj_t* ambient_temperature_readout = make_temperature_readout(readout_wrapper);
  temperature_readout_set_label(ambient_temperature_readout, "On Die");
  lv_obj_add_event_cb(ambient_temperature_readout, update_temperature_readout, LV_EVENT_MSG_RECEIVED, NULL);
  lv_msg_subsribe_obj(NEW_AMBIENT_TEMPERATURE_MSG, ambient_temperature_readout, NULL);

  lv_obj_t* thermistor_temperature_readout = make_temperature_readout(readout_wrapper);
  temperature_readout_set_label(thermistor_temperature_readout, "Thermistor");
  temperature_readout_set_reading(thermistor_temperature_readout, 73.32);
  lv_obj_add_event_cb(thermistor_temperature_readout, update_temperature_readout, LV_EVENT_MSG_RECEIVED, NULL);
  lv_msg_subsribe_obj(NEW_THERMISTOR_TEMPERATURE_MSG, thermistor_temperature_readout, NULL);
}
