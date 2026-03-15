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


    stdio_init_all();
    DEV_Delay_ms(5000);
    printf("Starting ESL");

    if (DEV_Module_Init() != 0) {
        printf("e-Paper init failed\n");
        return -1;
    }
    eink_init();

    DEV_Delay_ms(5000);
    printf("Starting Image Display\n");
    set_eink_shelf_label();

    printf("finished image display\n");


}