# The following lines of boilerplate have to be in your project's
# CMakeLists in this exact order for cmake to work correctly
cmake_minimum_required(VERSION 3.5)

set(EXTRA_COMPONENT_DIRS $ENV{HOME}/esp/esp-idf-lib/components)
set(COMPONENTS buzzer i2cdev freertos bootloader esp32 log main driver ds18x20 tinyfsm button ssd1306)

include($ENV{IDF_PATH}/tools/cmake/project.cmake)
project(magic-distiller)
