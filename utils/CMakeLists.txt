aux_source_directory(. DIR_utils_SRCS)

include_directories(../lib/Config)
include_directories(../lib/GUI)
include_directories(../lib/e-Paper)
include_directories(.)

# Generate the link library
add_library(utils ${DIR_utils_SRCS})

target_compile_definitions(utils PRIVATE
	WIFI_SSID=\"${WIFI_SSID}\"
	WIFI_PASSWORD=\"${WIFI_PASSWORD}\"
)

target_include_directories(utils PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
target_link_libraries(utils 
    PUBLIC 
    Config 
    pico_cyw43_arch_lwip_threadsafe_background
    pico_lwip_http
    hardware_rtc
)