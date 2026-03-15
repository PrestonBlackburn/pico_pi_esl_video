cmake_minimum_required(VERSION 3.13)

set(PICO_SDK_PATH ${CMAKE_CURRENT_LIST_DIR}/pico-sdk)
set(PICO_EXTRAS_PATH ${CMAKE_CURRENT_LIST_DIR}/pico-extras)
include(${PICO_SDK_PATH}/pico_sdk_init.cmake)
include(${PICO_EXTRAS_PATH}/external/pico_extras_import.cmake)

project(eink_monitor C CXX ASM)

set(CMAKE_C_STANDARD 11)
set(CMAKE_CXX_STANDARD 17)

pico_sdk_init()

include(wifi_config.cmake OPTIONAL)

# add a compilation subdirectory
add_subdirectory(lib/Config)
add_subdirectory(lib/e-Paper)
add_subdirectory(lib/Fonts)
add_subdirectory(lib/GUI)
add_subdirectory(lib/pico-ups)
add_subdirectory(utils)

# add a header directory
include_directories(./utils)
include_directories(./lib/Config)
include_directories(./lib/GUI)
include_directories(./lib/utils)
include_directories(./lib/pico-ups)

# generate an executable file
add_executable(eink_monitor 
    main.c
)

target_include_directories(eink_monitor 
    PRIVATE 
    # ${CMAKE_CURRENT_LIST_DIR}
    ${PROJECT_SOURCE_DIR}/utils
) 

# enable usb output, disable uart output
pico_enable_stdio_usb(eink_monitor 1)
pico_enable_stdio_uart(eink_monitor 0)

target_link_libraries(eink_monitor 
    utils 
    ePaper 
    GUI 
    Fonts 
    Config 
    UPS
    pico_stdlib 
    hardware_spi

	pico_runtime 

	hardware_sleep 
	pico_aon_timer
	hardware_rosc
	hardware_clocks
	hardware_uart
    hardware_rtc

	pico_cyw43_arch_lwip_threadsafe_background
	pico_lwip_http
    pico_util
)

# create map/bin/hex/uf2 file etc.
pico_add_extra_outputs(eink_monitor)