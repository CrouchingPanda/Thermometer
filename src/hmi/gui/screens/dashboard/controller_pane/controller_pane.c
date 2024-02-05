#include "controller_pane.h"

#include <stdint.h>
#include <stdlib.h>

#include <lvgl.h>

#include "controllers/emissivity_controller.h"
#include "controllers/smoothing_controller.h"
#include "controllers/low_power_mode_timeout_controller.h"
#include "controllers/power_off_timeout_controller.h"
#include "controllers/power_indicator_on_brightness_controller.h"
#include "controllers/power_indicator_max_breath_brightness_controller.h"
#include "controllers/room_temperature_offset_controller.h"
#include "controllers/display_brightness_controller.h"
#include "../../../utils/container.h"
#include "../../../theme.h"
#include "../../../fonts/fonts.h"

#define CONTROLLER_COUNT 8

static lv_obj_t* controllers[CONTROLLER_COUNT];

static uint8_t visibleControllerIndex = 0;

static void update_controllers_visibility(void) {
  for (size_t i = 0; i < CONTROLLER_COUNT; ++i) {
    lv_obj_add_flag(controllers[i], LV_OBJ_FLAG_HIDDEN);
  }
  lv_obj_clear_flag(controllers[visibleControllerIndex], LV_OBJ_FLAG_HIDDEN);
}

static lv_obj_t* nextButton;
static lv_obj_t* prevButton;

static lv_obj_t* createButton(lv_obj_t* parent, const char * label) {
  lv_obj_t* button = lv_btn_create(parent);
  lv_obj_add_flag(button, LV_OBJ_FLAG_ADV_HITTEST);
  lv_obj_set_style_min_width(button, 50, LV_PART_MAIN);
  lv_obj_set_style_text_font(button, &font_Roboto_22, LV_PART_MAIN);
  lv_obj_set_style_pad_all(button, PADDING, LV_PART_MAIN);
  lv_obj_set_style_radius(button, BORDER_RADIUS, LV_PART_MAIN);
  lv_obj_set_style_shadow_width(button, 0, LV_PART_MAIN);
  lv_obj_set_style_bg_color(button, lv_color_hex(PRIMARY_COLOR), LV_PART_MAIN);
  lv_obj_set_style_text_color(button, lv_color_hex(TEXT_COLOR), LV_PART_MAIN);
  lv_obj_set_style_bg_color(button, lv_color_hex(ACTIVE_BACKGROUND_COLOR), LV_PART_MAIN | LV_STATE_DISABLED);
  
  lv_obj_t* buttonLabel = lv_label_create(button);
  lv_label_set_text(buttonLabel, label);
  lv_obj_center(buttonLabel);

  return button;
}

static void update_buttons_state() {
  if (visibleControllerIndex == 0) {
    lv_obj_add_state(prevButton, LV_STATE_DISABLED);
  } else if (visibleControllerIndex == CONTROLLER_COUNT - 1) {
    lv_obj_add_state(nextButton, LV_STATE_DISABLED);
  } else {
    lv_obj_clear_state(prevButton, LV_STATE_DISABLED);
    lv_obj_clear_state(nextButton, LV_STATE_DISABLED);
  }
}

static void handle_next_button_press(lv_event_t* event) {
  ++visibleControllerIndex;
  update_controllers_visibility();
  update_buttons_state();
}

static void handle_prev_button_press(lv_event_t* event) {
  --visibleControllerIndex;
  update_controllers_visibility();
  update_buttons_state();
}

void make_controller_pane(lv_obj_t* parent) {
  lv_obj_t* button_wrapper = create_container(parent);
  lv_obj_set_height(button_wrapper, lv_pct(100));
  lv_obj_set_width(button_wrapper, LV_SIZE_CONTENT);

  lv_obj_set_flex_flow(button_wrapper, LV_FLEX_FLOW_COLUMN);
  lv_obj_set_flex_align(
    button_wrapper,
    LV_FLEX_ALIGN_SPACE_EVENLY,
    LV_FLEX_ALIGN_CENTER,
    LV_FLEX_ALIGN_CENTER
  );

  nextButton = createButton(button_wrapper, ">");
  lv_obj_add_event_cb(nextButton, handle_next_button_press, LV_EVENT_CLICKED, NULL);
  
  prevButton = createButton(button_wrapper, "<");
  lv_obj_add_event_cb(prevButton, handle_prev_button_press, LV_EVENT_CLICKED, NULL);

  update_buttons_state();

  controllers[0] = make_emissivity_controller(parent);
  controllers[1] = make_room_temperature_offset_controller(parent);
  controllers[2] = make_smoothing_controller(parent);
  controllers[3] = make_low_power_mode_timeout_controller(parent);
  controllers[4] = make_power_off_timeout_controller(parent);
  controllers[5] = make_display_brightness_controller(parent);
  controllers[6] = make_power_indicator_on_brightness_controller(parent);
  controllers[7] = make_power_indicator_max_breath_brightness_controller(parent);

  update_controllers_visibility();
}
