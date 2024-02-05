#include "screen_manager.h"

#include <lvgl.h>

#include "pub_sub.h"
#include "screens/top_layer/top_layer.h"
#include "screens/main/main_screen.h"
#include "screens/auth/auth_screen.h"
#include "screens/dashboard/dashboard_screen.h"

#define AUTH_SCREEN_PASSWORD "1358"

static void animate_screen_change(lv_obj_t* new_screen, lv_dir_t direction) {
  lv_scr_load_anim(
    new_screen, 
    direction, 
    200, // time
    0, // delay
    false // delete old screen
  );
}

static void handle_main_screen_gestures(lv_event_t* event) {
  lv_dir_t direction = lv_indev_get_gesture_dir(lv_indev_get_act());
  if (direction != LV_DIR_LEFT) return;
  animate_screen_change(auth_screen, LV_SCR_LOAD_ANIM_MOVE_LEFT);
}

static void handle_auth_screen_gestures(lv_event_t* event) {
  lv_dir_t direction = lv_indev_get_gesture_dir(lv_indev_get_act());
  if (direction != LV_DIR_RIGHT) return;
  animate_screen_change(main_screen, LV_SCR_LOAD_ANIM_MOVE_RIGHT);
}

static void handle_authentication(lv_event_t* event) {
  const char* password_input = lv_event_get_param(event);
  if (strcmp(password_input, AUTH_SCREEN_PASSWORD) != 0) {
    lv_msg_send(BAD_AUTH_ATTEMPT, NULL);
  } else {
    lv_msg_send(SUCCESSFUL_AUTH, NULL);
    animate_screen_change(dashboard_screen, LV_SCR_LOAD_ANIM_MOVE_LEFT);
  }
}

static void handle_dashboard_screen_gestures(lv_event_t* event) {
  lv_dir_t direction = lv_indev_get_gesture_dir(lv_indev_get_act());
  if (direction != LV_DIR_RIGHT) return;
  animate_screen_change(main_screen, LV_SCR_LOAD_ANIM_MOVE_RIGHT);
}

void start_screen_management(void) {
  top_layer_init();
  main_screen_init();
  auth_screen_init();
  dashboard_screen_init();

  lv_disp_load_scr(main_screen);

  lv_obj_add_event_cb(main_screen, handle_main_screen_gestures, LV_EVENT_GESTURE, NULL);
  lv_obj_add_event_cb(auth_screen, handle_auth_screen_gestures, LV_EVENT_GESTURE, NULL);
  lv_obj_add_event_cb(auth_screen, handle_authentication, LV_EVENT_READY, NULL);
  lv_obj_add_event_cb(dashboard_screen, handle_dashboard_screen_gestures, LV_EVENT_GESTURE, NULL);
}