#include "main_screen.h"

#include <lvgl.h>

#include "../../pub_sub.h"
#include "../../theme.h"
#include "../../fonts/fonts.h"
#include "../../utils/screen.h"

lv_obj_t* main_screen;

static void update_temperature_display(lv_event_t* event) {
  lv_obj_t* temperature_display = lv_event_get_target(event);
  lv_msg_t* message = lv_event_get_msg(event);
  float* temperature = (float*)lv_msg_get_payload(message);
  lv_label_set_text_fmt(temperature_display, "%.2f°F", *temperature);
}

void main_screen_init(void) {
  main_screen = make_screen();

  lv_obj_t* main_temperature_display = lv_label_create(main_screen);
  lv_obj_set_align(main_temperature_display, LV_ALIGN_CENTER);
  lv_label_set_text(main_temperature_display, "");
  lv_obj_set_style_text_font(main_temperature_display, &font_Roboto_Mono_48, LV_PART_MAIN);
  lv_obj_set_style_text_color(main_temperature_display, lv_color_hex(TEXT_COLOR), LV_PART_MAIN);
  lv_obj_add_event_cb(main_temperature_display, update_temperature_display, LV_EVENT_MSG_RECEIVED, NULL);
  lv_msg_subsribe_obj(NEW_OBJECT_TEMPERATURE, main_temperature_display, NULL);
}
