cmake_minimum_required(VERSION 3.13)

include(pico_sdk_import.cmake)

project(FinalProjTestFiles)

pico_sdk_init()

add_executable(FinalProjTestFiles
        main.c  
	pwm_pin.c
	getPWM.c
timer.c
)


# pull in common dependencies
target_link_libraries(FinalProjTestFiles
        hardware_pwm 
        pico_stdlib
        pico_bootsel_via_double_reset
)

# create map/bin/hex file etc.
pico_add_extra_outputs(FinalProjTestFiles)