#ifndef CONTROLLER_H
#define CONTROLLER_H

#include <stdint.h>

#include <lvgl.h>

lv_obj_t* make_controller(lv_obj_t* parent);

void controller_set_label(lv_obj_t* controller, const char* label);

void controller_set_options(lv_obj_t* controller, const char* options);

void controller_select_option(lv_obj_t* controller, uint16_t optionIndex);

typedef void(*option_change_handler)(uint16_t optionIndex);
void controller_handle_option_change(lv_obj_t* controller, option_change_handler callback);

#endif // !CONTROLLER_H
