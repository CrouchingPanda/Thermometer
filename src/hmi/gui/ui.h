/*
 * This file exists as a delimiter between C++ and C linkage.
 */

#ifndef UI_H
#define UI_H

#ifdef __cplusplus
extern "C" {
#endif

void ui_init(void);

#ifdef __cplusplus
} // extern "C"
#endif

#endif // !UI_H
