// logic for handling http requests

#include <stdio.h>
#include "pico/stdlib.h"
#include "lwip/apps/http_client.h"
#include "pico/cyw43_arch.h"
#include "pico/sync.h"
#include "lwip/dns.h"

#define HTTP_RESPONSE_MAX 4096 // max response buffer

static volatile bool request_complete = false;
static char shopper_category[64] = {0};

// Test DNS lookup directly
void test_dns_lookup() {
    printf("\n=== Testing DNS Lookup ===\n");
    
    ip_addr_t addr;
    err_t err = dns_gethostbyname("google.com", &addr, NULL, NULL);
    
    if (err == ERR_OK) {
        printf("DNS lookup SUCCESS (cached): %s\n", ipaddr_ntoa(&addr));
    } else if (err == ERR_INPROGRESS) {
        printf("DNS lookup in progress...\n");
        sleep_ms(5000);  // Wait for async DNS
        printf("After waiting, check if resolved\n");
    } else {
        printf("DNS lookup FAILED with error: %d\n", err);
    }
}

// ----- test get request ------

// callbacks when request completes
err_t headers_done_fn(
	httpc_state_t *connection,
	void *arg,
	struct pbuf *hdr,
	u16_t hdr_len,
	u32_t content_len) {
	printf("in headers_done_fn\n");
	printf("header length: %d\n", hdr_len);
	printf("Content length: %lu\n", content_len);
	return ERR_OK;
}

// when request completes
void result_fn(
	void *arg,
	httpc_result_t httpc_result,
	u32_t rx_content_len, 
	u32_t srv_res,
	err_t err) {
	printf(">>> result_fn >>>\n");
	printf("httpc_result: %s\n",
	     httpc_result == HTTPC_RESULT_OK              ? "HTTPC_RESULT_OK"
           : httpc_result == HTTPC_RESULT_ERR_UNKNOWN     ? "HTTPC_RESULT_ERR_UNKNOWN"
           : httpc_result == HTTPC_RESULT_ERR_CONNECT     ? "HTTPC_RESULT_ERR_CONNECT"
           : httpc_result == HTTPC_RESULT_ERR_HOSTNAME    ? "HTTPC_RESULT_ERR_HOSTNAME"
           : httpc_result == HTTPC_RESULT_ERR_CLOSED      ? "HTTPC_RESULT_ERR_CLOSED"
           : httpc_result == HTTPC_RESULT_ERR_TIMEOUT     ? "HTTPC_RESULT_ERR_TIMEOUT"
           : httpc_result == HTTPC_RESULT_ERR_SVR_RESP    ? "HTTPC_RESULT_ERR_SVR_RESP"
           : httpc_result == HTTPC_RESULT_ERR_MEM         ? "HTTPC_RESULT_ERR_MEM"
           : httpc_result == HTTPC_RESULT_LOCAL_ABORT     ? "HTTPC_RESULT_LOCAL_ABORT"
           : httpc_result == HTTPC_RESULT_ERR_CONTENT_LEN ? "HTTPC_RESULT_ERR_CONTENT_LEN"
           : "*UNKNOWN*");
	printf("received %ld bytes\n", rx_content_len);
	printf("server response: %ld\n", srv_res);
	printf("err: %d\n", err);
	request_complete = true;
	// we'll actually check the recv data for specific text since we control the endpoint
	printf("<<< result_fn <<<\n");
}

bool is_in(char *source, int len_source, char *target) {
	bool is_matched = false;

	int len_target = strlen(target);

	if (len_source < len_target) {
		// has to be longer than target
		is_matched = false;
		return false;
	}

    int i;
    for (i=0; i<=len_source-len_target; i++) {
        int j;
        char *slice_ptr = source + i;
        for (j=0; j<len_target; j++) {
            if (target[j] != slice_ptr[j]) {
                break;
            }
        }
        if (j == len_target) {
            is_matched = true;
            return is_matched;
        }
    }
    return is_matched;
}

bool parse_json_string_value(const char *json, int json_len, const char *key,
                              char *out_value, int out_max) {
    // Build the search pattern:  "key":"
    char pattern[64];
    snprintf(pattern, sizeof(pattern), "\"%s\":\"", key);
    int pattern_len = strlen(pattern);
 
    for (int i = 0; i <= json_len - pattern_len; i++) {
        if (strncmp(json + i, pattern, pattern_len) == 0) {
            // Advance past the pattern to the start of the value
            int val_start = i + pattern_len;
            int val_len = 0;
 
            // Scan forward until the closing quote or end of buffer
            while (val_start + val_len < json_len &&
                   json[val_start + val_len] != '"') {
                val_len++;
            }
 
            // Clamp to output buffer size
            if (val_len >= out_max) val_len = out_max - 1;
            strncpy(out_value, json + val_start, val_len);
            out_value[val_len] = '\0';
            return true;
        }
    }
    return false;
}

// when body data is received
err_t recv_fn(
	void *arg,
	struct altcp_pcb *conn,
	struct pbuf *p,
	err_t err) {
	printf("in recv_fn\n");
	printf(">>> recv_fn >>>\n");
	if (p == NULL) {
		printf("p is NULL\n");
		return ERR_OK;
	}

	printf("p: %p\n", p);
	printf("len: %d\n", p->len);
	
	char *data = (char *)p->payload;
	int print_len = p->len <200 ? p->len : 200;
	printf("Data: %.*s\n", print_len, data);

	// check endpoint for shopper_category
	if (parse_json_string_value(data, p->len, "shopper_category",
								shopper_category, sizeof(shopper_category))) {
			printf("shopper_category: %s\n", shopper_category);
		} else {
			printf("shopper_category key not found in response\n");
			shopper_category[0] = '\0';
		}

	pbuf_free(p);

	printf("<<< recv_fn <<<\n");
	return ERR_OK;
}

char *get_current_shopper() {
    printf("\n=== Starting HTTP Request ===\n");

	httpc_connection_t settings = {
		.use_proxy = 0,
		.headers_done_fn = headers_done_fn,
		.result_fn = result_fn
	};
    httpc_state_t *connection = NULL;

    err_t err = httpc_get_file_dns(
		"prestonblackburn.com",
		80,
		"/face/current-shopper",
		&settings,
		recv_fn,
		NULL,
		&connection
	);

	if (err != ERR_OK) {
		printf("HTTP request failed to start %d\n", err);
		return NULL;  // caller should check for NULL
	}

   	printf("waiting for request\n");
	sleep_ms(10000);
	printf("HTTP Request Done\n");

	return shopper_category;
	// return 0;
}