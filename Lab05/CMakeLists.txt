cmake_minimum_required(VERSION 3.13)
include(pico_sdk_import.cmake)
project(Lab05) # Change to your project name
pico_sdk_init()
add_executable(Lab05) # Change to your project name
pico_generate_pio_header(Lab05 ${CMAKE_CURRENT_LIST_DIR}/SPIPIO.pio)
# Ensure SPIPIO.pio is in your directory
target_sources(Lab05 PRIVATE
 main.c
 TouchDisplay.c
 inputnum.c
 ts_lcd.c
 TouchScreen.c
 TFTMaster.c
 timer.c
timer.h
glcdfont.c
 )
target_link_libraries(Lab05 PRIVATE
 pico_stdlib
 hardware_adc
 hardware_gpio
 hardware_pio
 hardware_irq
 pico_bootsel_via_double_reset
 )
pico_add_extra_outputs(Lab05)