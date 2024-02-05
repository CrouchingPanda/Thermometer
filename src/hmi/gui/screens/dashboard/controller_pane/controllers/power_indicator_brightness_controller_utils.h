#ifndef POWER_INDICATOR_BRIGHTNESS_CONTROLLER_UTILS_H
#define POWER_INDICATOR_BRIGHTNESS_CONTROLLER_UTILS_H

#include <lvgl.h>

// 0..255 inclusive
#define BRIGHNTESS_OPTION_COUNT 52
#define BRIGHTNESS_OPTION_STEP 5

const char* brightness_options(void);

void update_brightness_controller(lv_event_t* event);

#endif // !POWER_INDICATOR_BRIGHTNESS_CONTROLLER_UTILS_H