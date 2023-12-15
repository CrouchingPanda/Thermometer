#include "temperature_readouts.h"

#include <lvgl.h>

#include "temperature_readout.h"
#include "../../../pub_sub.h"

static void update_temperature_readout(lv_event_t* event) {
  lv_obj_t* readout = lv_event_get_target(event);
  lv_msg_t* message = lv_event_get_msg(event);
  float* temperature = (float*) lv_msg_get_payload(message);
  temperature_readout_set_reading(readout, *temperature);
}

void make_temperature_readouts(lv_obj_t* parent) {
  lv_obj_t* object_temperature_readout = make_temperature_readout(parent);
  temperature_readout_set_label(object_temperature_readout, "Object");
  lv_obj_add_event_cb(object_temperature_readout, update_temperature_readout, LV_EVENT_MSG_RECEIVED, NULL);
  lv_msg_subsribe_obj(NEW_OBJECT_TEMPERATURE, object_temperature_readout, NULL);

  lv_obj_t* room_temperature_readout = make_temperature_readout(parent);
  temperature_readout_set_label(room_temperature_readout, "Ambient");
  lv_obj_add_event_cb(room_temperature_readout, update_temperature_readout, LV_EVENT_MSG_RECEIVED, NULL);
  lv_msg_subsribe_obj(NEW_ROOM_TEMPERATURE, room_temperature_readout, NULL);

  lv_obj_t* on_die_temperature_readout = make_temperature_readout(parent);
  temperature_readout_set_label(on_die_temperature_readout, "On Die");
  lv_obj_add_event_cb(on_die_temperature_readout, update_temperature_readout, LV_EVENT_MSG_RECEIVED, NULL);
  lv_msg_subsribe_obj(NEW_ON_DIE_TEMPERATURE, on_die_temperature_readout, NULL);

  lv_obj_t* battery_temperature_readout = make_temperature_readout(parent);
  temperature_readout_set_label(battery_temperature_readout, "Battery");
  lv_obj_add_event_cb(battery_temperature_readout, update_temperature_readout, LV_EVENT_MSG_RECEIVED, NULL);
  lv_msg_subsribe_obj(NEW_BATTERY_TEMPERATURE, battery_temperature_readout, NULL);
}