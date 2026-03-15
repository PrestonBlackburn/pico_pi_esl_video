#include "utils.h"
#include "EPD_2in13_V4.h"
#include "GUI_Paint.h"

int eink_init(void) {
    //Create a new image cache
    UBYTE *BlackImage;
    UWORD Imagesize = ((EPD_2in13_V4_WIDTH % 8 == 0)? (EPD_2in13_V4_WIDTH / 8 ): (EPD_2in13_V4_WIDTH / 8 + 1)) * EPD_2in13_V4_HEIGHT;

    if((BlackImage = (UBYTE *)malloc(Imagesize)) == NULL) {
        Debug("Failed to apply for black memory...\r\n");
        return -1;
    }
    // init eink screen
    EPD_2in13_V4_Init_Fast();
    EPD_2in13_V4_Clear();
    sleep_ms(500);
    // test flashing screen
    Debug("Paint_NewImage\r\n");
    Paint_NewImage(BlackImage, EPD_2in13_V4_WIDTH, EPD_2in13_V4_HEIGHT, 270, WHITE);
	Paint_Clear(WHITE);
    DEV_Delay_ms(500);

    free(BlackImage);
    BlackImage = NULL;
    DEV_Delay_ms(2000);//important, at least 2s

    return 0;
}


int set_eink_shelf_label(void) {
    //Create a new image cache
    UBYTE *BlackImage;
    UWORD Imagesize = ((EPD_2in13_V4_WIDTH % 8 == 0)? (EPD_2in13_V4_WIDTH / 8 ): (EPD_2in13_V4_WIDTH / 8 + 1)) * EPD_2in13_V4_HEIGHT;

    if((BlackImage = (UBYTE *)malloc(Imagesize)) == NULL) {
        Debug("Failed to apply for black memory...\r\n");
        return -1;
    }
    // init eink screen
    EPD_2in13_V4_Init_Fast();
    EPD_2in13_V4_Clear(); // white clear
    sleep_ms(500);


    Paint_NewImage(BlackImage, EPD_2in13_V4_WIDTH, EPD_2in13_V4_HEIGHT, ROTATE_90, WHITE);
    Paint_SelectImage(BlackImage);
    Paint_Clear(WHITE);
    draw_generic_esl_image_y_black();
    // draw_generic_esl_image_y();
    // draw_generic_esl_image();

    // Push the buffer to the physical display
    EPD_2in13_V4_Display(BlackImage);

    free(BlackImage);
    BlackImage = NULL;
    DEV_Delay_ms(2000);//important, at least 2s

    return 0;
}

//int set_eink_status(int status, datetime_t *t) {
int set_eink_status(ServerStatus *server_status, datetime_t *current_time, float battery_pct) {

    // create structure for paint function
    PAINT_TIME display_time;

    display_time.Year = (UWORD)current_time->year;
    display_time.Month = (UBYTE)current_time->month;
    display_time.Day = (UBYTE)current_time->day;
    display_time.Hour = (UBYTE)current_time->hour;
    display_time.Min = (UBYTE)current_time->min;
    display_time.Sec = (UBYTE)current_time->sec;

    // display_time.Year = (UWORD)server_status->last_state_change.year;
    // display_time.Month = (UBYTE)server_status->last_state_change.month;
    // display_time.Day = (UBYTE)server_status->last_state_change.day;
    // display_time.Hour = (UBYTE)server_status->last_state_change.hour;
    // display_time.Min = (UBYTE)server_status->last_state_change.min;
    // display_time.Sec = (UBYTE)server_status->last_state_change.sec;

    //Create a new image cache
    UBYTE *BlackImage;
    UWORD Imagesize = ((EPD_2in13_V4_WIDTH % 8 == 0)? (EPD_2in13_V4_WIDTH / 8 ): (EPD_2in13_V4_WIDTH / 8 + 1)) * EPD_2in13_V4_HEIGHT;

    if((BlackImage = (UBYTE *)malloc(Imagesize)) == NULL) {
        Debug("Failed to apply for black memory...\r\n");
        return -1;
    }

    // online case
    int status_y = 10;
    int uptime_title_y = 45;
    int uptime_value_y = 60;
    int updated_title_y = 85;
    int updated_value_y = 100;


    // battery pct should be shown in top right corner
    
    // existing image ref
    // upper conner x 28
    // upper conner y 33
    // (UWORD Xstart, UWORD Ystart, UWORD Xend, UWORD Yend, UWORD Color)

    // to display: : 97%
    int x_battery_start = 5;
    int y_battery_start = 5;
    int battery_text_len = 5;
    char battery_buffer[10];
    snprintf(battery_buffer, sizeof(battery_buffer), "%.0f%%", battery_pct);
    printf("Got Battery Pct: %s\n", battery_buffer);

    if (server_status->server_online == true) {
        printf("Uptime: %d days, %d hours\n", server_status->uptime_days, server_status->uptime_hours);
        printf("rtc time: %u day %u:%u:%u\n", display_time.Day,display_time.Hour, display_time.Min, display_time.Sec);
        draw_online_status(BlackImage);
        Paint_DrawString_EN(110, status_y, "Server On", &Font20, BLACK, WHITE);

        Paint_ClearWindows(110, uptime_title_y, 110 + Font12.Width * 9, uptime_title_y + Font12.Height, WHITE);
        Paint_DrawString_EN(110, uptime_title_y, "Uptime", &Font12, BLACK, WHITE);

        Paint_ClearWindows(110, uptime_value_y, 110 + Font16.Width * 11, uptime_value_y + Font16.Height, WHITE);
        Paint_DrawUptime(110, uptime_value_y, server_status->uptime_days, server_status->uptime_hours, &Font16, BLACK, WHITE);

        Paint_ClearWindows(110, updated_title_y, 110 + Font12.Width * 9, updated_title_y + Font12.Height, WHITE);
        Paint_DrawString_EN(110, updated_title_y, "Updated", &Font12, BLACK, WHITE);

        Paint_ClearWindows(110, updated_value_y, 110 + Font16.Width * 9, updated_value_y + Font16.Height, WHITE);
        Paint_DrawDatetime(110, updated_value_y, &display_time, &Font16, WHITE, BLACK);

        // battery pct
        Paint_ClearWindows(x_battery_start, y_battery_start, x_battery_start + Font12.Width * battery_text_len, y_battery_start + Font12.Height, WHITE);
        Paint_DrawString_EN(x_battery_start, y_battery_start, battery_buffer, &Font12, BLACK, WHITE);

        EPD_2in13_V4_Display_Base(BlackImage);
        DEV_Delay_ms(2000);
    }

    // offline case
    if (server_status->server_online != true) {
        printf("Uptime: %d days, %d hours\n", server_status->uptime_days, server_status->uptime_hours);
        draw_offline_status(BlackImage);
        Paint_DrawString_EN(110, status_y, "Server Off", &Font20, BLACK, WHITE);

        Paint_ClearWindows(110, uptime_title_y, 110 + Font12.Width * 9, uptime_title_y + Font12.Height, WHITE);
        Paint_DrawString_EN(110, uptime_title_y, "Downtime", &Font12, BLACK, WHITE);
        
        Paint_ClearWindows(110, uptime_value_y, 110 + Font16.Width * 11, uptime_value_y + Font16.Height, WHITE);
        Paint_DrawUptime(110, uptime_value_y, server_status->uptime_days, server_status->uptime_hours, &Font16, BLACK, WHITE);

        Paint_ClearWindows(110, updated_title_y, 110 + Font12.Width * 9, updated_title_y + Font12.Height, WHITE);
        Paint_DrawString_EN(110, updated_title_y, "Updated", &Font12, BLACK, WHITE);

        Paint_ClearWindows(110, updated_value_y, 110 + Font16.Width * 9, updated_value_y + Font16.Height, WHITE);
        Paint_DrawDatetime(110, updated_value_y, &display_time, &Font16, WHITE, BLACK);

        // battery pct
        Paint_ClearWindows(x_battery_start, y_battery_start, x_battery_start + Font12.Width * battery_text_len, y_battery_start + Font12.Height, WHITE);
        Paint_DrawString_EN(x_battery_start, y_battery_start, battery_buffer, &Font12, BLACK, WHITE);

        EPD_2in13_V4_Display_Base(BlackImage);
        DEV_Delay_ms(2000);
    }

    // leave the last image on 
    // Debug("Goto Sleep...\r\n");
    EPD_2in13_V4_Sleep();
    free(BlackImage);
    BlackImage = NULL;
    DEV_Delay_ms(2000);//important, at least 2s
    // DEV_Module_Exit();

    return 0;
}
