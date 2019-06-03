cmake_minimum_required(VERSION 3.5)

# 工程信息
project(BinocularVison)

include_directories(${CMAKE_SOURCE_DIR}/..)

add_subdirectory(CaptureImage)
add_subdirectory(Calibration)
add_subdirectory(OpenCamera)



