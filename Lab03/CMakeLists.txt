cmake_minimum_required(VERSION 3.13)

include(pico_sdk_import.cmake)

project(Lab03)

pico_sdk_init()

add_executable(Lab03
    led_out.c
    led_out.h
    debounce_sw1.h
debounce_sw1.c
debounce_sw2.h
debounce_sw2.c
    sw_in.c
    sw_in.h
    timer.c
    timer.h
    main.c
)

target_link_libraries(Lab03 pico_stdlib)

pico_add_extra_outputs(Lab03)