#include <stdio.h>
#include "pico/stdlib.h"
#include "hardware/gpio.h"
#include "pico/cyw43_arch.h"
#include "lwip/netif.h"
#include "lwip/ip4_addr.h"

const char *ssid = WIFI_SSID; // from env
const char *pass = WIFI_PASSWORD; // from env

int initialize_wifi_arch() {

	if (cyw43_arch_init_with_country(CYW43_COUNTRY_USA)) {
		printf("failed to initialize\n");
		return -1;
	}
	printf("initialized\n");

	cyw43_arch_enable_sta_mode();

    return 0;
}

// test_wifi connection
int connect_to_wifi() {
	if (cyw43_arch_wifi_connect_timeout_ms(ssid, pass, CYW43_AUTH_WPA2_AES_PSK, 10000)) {
		printf("failed to connect\n");
		return -1;
	}
	printf("connected\n");

	return 0;
}


int init_wifi(int retry_count, int set_led) {
    // set_led = 1 -> activate wifi light
    // maybe add a pre-connect led here...
	printf("connecting to wifi...\n");
    int arch_status = initialize_wifi_arch();

    // really the init shouldn't fail
    if (arch_status == -1 ) {
        printf("Wifi arch init failed\n");
        return -1;
    }

    // sometimes connection is flaky
    int counter = 0;
    int success = -1;
    while (counter < retry_count && success == -1) {
        printf("attempt: %d to connect to wifi...\n", counter);
        int conn_status = connect_to_wifi();
        counter++;
        if (conn_status == 0) {
            success = 0;
        }
        sleep_ms(2000); 
    } 

    // blink wifi led if failed
	if (success == -1) {
		printf("Wifi connection Failed\n");
        int count = 10;
        int i;
        for (i=0; i<count; i++) {
            cyw43_arch_gpio_put(CYW43_WL_GPIO_LED_PIN, 1);
            sleep_ms(500);
            cyw43_arch_gpio_put(CYW43_WL_GPIO_LED_PIN, 0);
        }
        return -1;
	}

    // Check if we actually got an IP address
    printf("Checking network status...\n");
    printf("Link status: %d\n", cyw43_wifi_link_status(&cyw43_state, CYW43_ITF_STA));
    
    // Add DHCP wait here
    printf("WiFi connected, waiting for IP address...\n");
    int ip_timeout = 30;
    while (ip_timeout > 0) {
        struct netif *netif = netif_default;
        if (netif && !ip4_addr_isany(netif_ip4_addr(netif))) {
            printf("Got IP: %s\n", ip4addr_ntoa(netif_ip4_addr(netif)));
            break;
        }
        sleep_ms(1000);
        ip_timeout--;
    }
    
    if (ip_timeout == 0) {
        printf("Failed to get IP address\n");
        return -1;
    }

    // finally solid green wifi led if success
    // (done through wifi driver)
    if (set_led == 1) {
        printf("Wifi Connection Success\n");
        cyw43_arch_gpio_put(CYW43_WL_GPIO_LED_PIN, 1);
        sleep_ms(2000);
        cyw43_arch_gpio_put(CYW43_WL_GPIO_LED_PIN, 0);
    }

    return 0;
}