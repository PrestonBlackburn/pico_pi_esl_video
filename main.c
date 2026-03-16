#include "utils.h"
#include <stdio.h>
#include <time.h>
#include "pico/stdlib.h"
#include "hardware/gpio.h"
#include "hardware/uart.h"
#include "lwip/apps/http_client.h"
#include "pico/cyw43_arch.h"
#include "init_wifi.h"
#include "http_request.h"
#include "ntp_client.h"



int main(void) {
    // will loop every x seconds and look for changes in the endpoint
    // if there isn't a change pass, if there is a change, display corresponding tag

    // ------------- Static case for display ----------------
    #if 1
        stdio_init_all();
        DEV_Delay_ms(5000);
        printf("Starting ESL\n");

        if (DEV_Module_Init() != 0) {
            printf("e-Paper init failed\n");
            return -1;
        }
        eink_init();

        DEV_Delay_ms(5000);
        printf("Starting Image Display\n");
        set_eink_shelf_label();

        printf("finished image display\n");
    #endif



    // -------------- Dynamic Case For Demo -----------------

    // test
    #if 1
        printf("Test Generic ESL\n");
        set_generic_esl_image_y_black();
        printf("finished image display\n");
        printf("Test Personalized ESL\n");
        set_personalized_esl_image_y_black();
        printf("finished image display\n");
    #endif

    int retry_count = 5;
    int flash_wifi_led = 1;
    printf("Starting Init Wifi\n");
    int connect_status = init_wifi(retry_count, flash_wifi_led);
    if (connect_status != 0) {
        printf("WiFi init failed, stopping\n");
        return -1;
    }
    DEV_Delay_ms(1000); 


    bool use_generic_label = true;

    while (1) {

        printf("Starting HTTP Request\n");
        char *use_specific_label = get_current_shopper();
        
        if (use_specific_label == NULL) {
            printf("Request failed, skipping\n");
            sleep_ms(5000);
            continue;
        }



        if ( (strcmp(use_specific_label, "None") != 0) && (use_generic_label == true) ) {
            // update the screen
            printf("Switching to personal label\n");
            use_generic_label = false;
            set_personalized_esl_image_y_black();

        } else if ( (strcmp(use_specific_label, "None") == 0) && (use_generic_label == false)) {
            // switch back to the generic case
            printf("Switching to generic label\n");
            use_generic_label = true;
            set_generic_esl_image_y_black();
        } else {
            // no changes in other two cases
            printf("no changes\n");
        }

        // wait to poll again
        sleep_ms(5000);

    }

}