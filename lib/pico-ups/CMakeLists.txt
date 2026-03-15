aux_source_directory(. DIR_UPS_SRCS)

# Generate the link library
add_library(UPS ${DIR_UPS_SRCS})
target_link_libraries(UPS PUBLIC hardware_i2c pico_stdlib)