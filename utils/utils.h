#ifndef _EINK_TEST_H_
#define _EINK_TEST_H_

#define DEBUG 1 // for logging
#include "DEV_Config.h"
#include "GUI_Paint.h"
#include "DrawImageData.h"
#include "Debug.h"
#include <stdlib.h> // malloc() free()

typedef struct _ServerStatus{
    bool server_online;
    int uptime_days;
    int uptime_hours;
    datetime_t last_state_change;
} ServerStatus;

int eink_init(void);
int set_eink_status(ServerStatus *server_status, datetime_t *current_time, float battery_pct);
int set_eink_shelf_label(void);

int set_generic_esl_image_y_black(void);
int set_personalized_esl_image_y_black(void);

#endif
