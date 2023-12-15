#include "thermistor_readouts.h"

#include <lvgl.h>

#include "temperature_readout.h"
#include "../../../pub_sub.h"

static void update_thermistor_readout(lv_event_t* event) {
  lv_obj_t* readout = lv_event_get_target(event);
  lv_msg_t* message = lv_event_get_msg(event);

  if (lv_msg_get_id(message) == NEW_CALIBRATION_BOARD_CONNECTION) {
    bool isConnected = *(bool*) lv_msg_get_payload(message);
    if (isConnected) {
      lv_obj_clear_flag(readout, LV_OBJ_FLAG_HIDDEN);
    } else {
      lv_obj_add_flag(readout, LV_OBJ_FLAG_HIDDEN);
    }
  } else {
    float* temperature = (float*) lv_msg_get_payload(message);
    temperature_readout_set_reading(readout, *temperature);
  }
}

void make_thermistor_readouts(lv_obj_t* parent) {
  lv_obj_t* primary_thermistor_temperature_readout = make_temperature_readout(parent);
  temperature_readout_set_label(primary_thermistor_temperature_readout, "Prim Therm");
  lv_obj_add_event_cb(primary_thermistor_temperature_readout, update_thermistor_readout, LV_EVENT_MSG_RECEIVED, NULL);
  lv_msg_subsribe_obj(NEW_PRIMARY_THERMISTOR_TEMPERATURE, primary_thermistor_temperature_readout, NULL);
  lv_msg_subsribe_obj(NEW_CALIBRATION_BOARD_CONNECTION, primary_thermistor_temperature_readout, NULL);

  lv_obj_t* secondary_thermistor_temperature_readout = make_temperature_readout(parent);
  temperature_readout_set_label(secondary_thermistor_temperature_readout, "Sec Therm");
  lv_obj_add_event_cb(secondary_thermistor_temperature_readout, update_thermistor_readout, LV_EVENT_MSG_RECEIVED, NULL);
  lv_msg_subsribe_obj(NEW_SECONDARY_THERMISTOR_TEMPERATURE, secondary_thermistor_temperature_readout, NULL);
  lv_msg_subsribe_obj(NEW_CALIBRATION_BOARD_CONNECTION, secondary_thermistor_temperature_readout, NULL);
}