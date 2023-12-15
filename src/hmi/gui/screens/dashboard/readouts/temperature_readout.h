#ifndef TEMPERATURE_READOUT_H
#define TEMPERATURE_READOUT_H

#include <lvgl.h>

lv_obj_t* make_temperature_readout(lv_obj_t* parent);

void temperature_readout_set_label(lv_obj_t* readout, const char* label);

void temperature_readout_set_reading(lv_obj_t* readout, float reading);

#endif // !TEMPERATURE_READOUT_H
