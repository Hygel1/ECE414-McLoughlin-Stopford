cmake_minimum_required(VERSION 3.13)

include(pico_sdk_import.cmake)

project(sensorTestFiles)

pico_sdk_init()

add_executable(sensorTestFiles
        main.c
	baro.c
	gyro.c
	BMP5_SensorAPI-master/bmp5.c  
)

# pull in common dependencies
target_link_libraries(sensorTestFiles
        hardware_i2c 
        pico_stdlib
        pico_bootsel_via_double_reset
	hardware_uart
)

# create map/bin/hex file etc.
pico_add_extra_outputs(sensorTestFiles)