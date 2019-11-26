cmake_minimum_required(VERSION 3.5)

# 工程信息
project(BinocularVision)

include_directories(${CMAKE_SOURCE_DIR}/..)

add_subdirectory(01_Capture)
add_subdirectory(02_DoubleEyes)
add_subdirectory(03_CaptureImages)
add_subdirectory(04_Calibrate)
add_subdirectory(05_DistanceMeasurement)
add_subdirectory(06_PCLTest)
add_subdirectory(07_StereoMatch)
add_subdirectory(07_StereoBuild)
add_subdirectory(08_StereoView)
add_subdirectory(09_StereoMatchADCensus)
