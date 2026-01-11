cmake_minimum_required(VERSION 3.16)
include($ENV{IDF_PATH}/tools/cmake/project.cmake)
project(pH-sensing-system-esp32)
spiffs_create_partition_image(spiffs spiffs)
