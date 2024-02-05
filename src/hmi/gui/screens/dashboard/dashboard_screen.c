#include "dashboard_screen.h"

#include <lvgl.h>

#include "readouts/thermistor_readouts.h"
#include "readouts/temperature_readouts.h"
#include "controller_pane/controller_pane.h"
#include "../top_layer/top_layer.h"
#include "../../theme.h"
#include "../../utils/screen.h"
#include "../../utils/container.h"

lv_obj_t* dashboard_screen;

void dashboard_screen_init(void) {
  dashboard_screen = make_screen();

  lv_obj_t* content_wrapper = create_container(dashboard_screen);
  lv_obj_set_y(content_wrapper, TOP_BAR_HEIGHT);
  lv_obj_set_width(content_wrapper, lv_pct(100));
  lv_obj_set_height(content_wrapper, SCREEN_HEIGHT - TOP_BAR_HEIGHT);
  lv_obj_clear_flag(content_wrapper, LV_OBJ_FLAG_SCROLLABLE);
  lv_obj_set_style_pad_all(content_wrapper, PADDING, LV_PART_MAIN);
  lv_obj_set_flex_flow(content_wrapper, LV_FLEX_FLOW_ROW);
  lv_obj_set_flex_align(
    content_wrapper,
    LV_FLEX_ALIGN_SPACE_BETWEEN,
    LV_FLEX_ALIGN_CENTER,
    LV_FLEX_ALIGN_CENTER
  );

  lv_obj_t* controller_pane_wrapper = create_container(content_wrapper);
  lv_obj_set_height(controller_pane_wrapper, lv_pct(100));
  lv_obj_set_flex_grow(controller_pane_wrapper, 1);
  lv_obj_set_flex_flow(controller_pane_wrapper, LV_FLEX_FLOW_ROW);
  lv_obj_set_flex_align(
    controller_pane_wrapper,
    LV_FLEX_ALIGN_SPACE_BETWEEN,
    LV_FLEX_ALIGN_CENTER,
    LV_FLEX_ALIGN_CENTER
  );

  make_controller_pane(controller_pane_wrapper);

  lv_obj_t* delimiter = lv_obj_create(content_wrapper);
  lv_obj_set_height(delimiter, lv_pct(90));
  lv_obj_set_width(delimiter, 1);
  lv_obj_set_style_border_color(delimiter, lv_color_hex(ACTIVE_BACKGROUND_COLOR), LV_PART_MAIN);

  lv_obj_t* readout_wrapper = create_container(content_wrapper);
  lv_obj_set_height(readout_wrapper, lv_pct(100));
  lv_obj_set_width(readout_wrapper, lv_pct(33));
  lv_obj_clear_flag(readout_wrapper, LV_OBJ_FLAG_SCROLL_MOMENTUM);
  lv_obj_clear_flag(readout_wrapper, LV_OBJ_FLAG_SCROLL_ELASTIC);
  lv_obj_set_flex_flow(readout_wrapper, LV_FLEX_FLOW_COLUMN);
  lv_obj_set_flex_align(
    readout_wrapper,
    LV_FLEX_ALIGN_START,
    LV_FLEX_ALIGN_CENTER,
    LV_FLEX_ALIGN_CENTER
  );

  make_thermistor_readouts(readout_wrapper);
  make_temperature_readouts(readout_wrapper);
}
