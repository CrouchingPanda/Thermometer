#include "temperature_readout.h"

#include <lvgl.h>

#include "../../utils/card.h"
#include "../../fonts/fonts.h"

lv_obj_t* make_temperature_readout(lv_obj_t* parent) {
  lv_obj_t* new_temperature_readout = create_card(parent);
  lv_obj_set_width(new_temperature_readout, lv_pct(100));
  lv_obj_set_flex_flow(new_temperature_readout, LV_FLEX_FLOW_COLUMN);
  lv_obj_set_flex_grow(new_temperature_readout, 1);
  lv_obj_set_flex_align(
    new_temperature_readout,
    LV_FLEX_ALIGN_CENTER,
    LV_FLEX_ALIGN_CENTER,
    LV_FLEX_ALIGN_CENTER
  );

  lv_obj_t* label = lv_label_create(new_temperature_readout);
  lv_obj_set_style_text_align(label, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN);

  lv_obj_t* reading = lv_label_create(new_temperature_readout);
  lv_obj_set_style_text_font(reading, &font_Roboto_Mono_20, LV_PART_MAIN);

  return new_temperature_readout;
}

void temperature_readout_set_label(lv_obj_t* readout, const char* new_label) {
  lv_obj_t* label = lv_obj_get_child(readout, 0);
  lv_label_set_text(label, new_label);
}

void temperature_readout_set_reading(lv_obj_t* readout, float new_reading) {
  lv_obj_t* reading = lv_obj_get_child(readout, 1);
  lv_label_set_text_fmt(reading, "%.2f F", new_reading);
}