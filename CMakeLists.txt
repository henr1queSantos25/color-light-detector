cmake_minimum_required(VERSION 3.13)
set(PICO_BOARD pico_w CACHE STRING "Board type")
set(CMAKE_C_STANDARD 11)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
include(pico_sdk_import.cmake)
project(color-light-detector C CXX ASM)
pico_sdk_init()

include_directories(${CMAKE_SOURCE_DIR}/lib)

add_executable(${PROJECT_NAME}  
        main.c
        lib/ssd1306/ssd1306.c
        lib/gy_33/gy_33.c
        lib/bh1750/bh1750.c
        lib/rgb/rgb.c
        lib/push_button/button.c
        )


# pico_generate_pio_header(${PROJECT_NAME} ${CMAKE_CURRENT_LIST_DIR}/lib/matriz/matriz.pio)
# (O TRECHO ACIMA SERVE PRA MATRIZ DE LEDs FUNCIONAR, PERSONALIZAR DE ACORDO COM ESSE PROJETO)

target_link_libraries(${PROJECT_NAME} 
        pico_stdlib
        hardware_i2c
        hardware_pwm
        )

target_include_directories(${PROJECT_NAME} PRIVATE ${CMAKE_CURRENT_LIST_DIR} 
            lib/ssd1306
            lib/gy_33
            lib/bh1750
            lib/rgb
            lib/push_button
        )

pico_enable_stdio_usb(${PROJECT_NAME} 1)
pico_enable_stdio_uart(${PROJECT_NAME} 1)

pico_add_extra_outputs(${PROJECT_NAME})

