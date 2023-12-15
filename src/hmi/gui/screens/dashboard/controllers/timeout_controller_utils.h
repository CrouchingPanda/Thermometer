#ifndef TIMEOUT_CONTROLLER_UTILS_H
#define TIMEOUT_CONTROLLER_UTILS_H

#include <lvgl.h>

#define TIMEOUT_OPTION_COUNT 30 // including "OFF"
#define TIMEOUT_OPTION_STEP 1

const char* timeout_options(void);

void update_timeout_controller(lv_event_t* event);

#endif // !TIMEOUT_CONTROLLER_UTILS_H
