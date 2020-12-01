cmake_minimum_required(VERSION 2.8)
project(camera-stream-poll-sample)

# Try to see if user has OpenCV installed
# if yes, default callback will display the image
find_package( OpenCV QUIET )
if (OpenCV_FOUND)
  message( "\n${PROJECT_NAME}...")
  message( STATUS "Found OpenCV installed in the system, will use it to display image in AdvancedSensing APIs")
  message( STATUS " - Includes: ${OpenCV_INCLUDE_DIRS}")
  message( STATUS " - Libraries: ${OpenCV_LIBRARIES}")
  add_definitions(-DOPEN_CV_INSTALLED)
else()
  message( STATUS "Did not find OpenCV in the system, image data is inside RecvContainer as raw data")
endif ()

#(RAPIDJSON_INCLUDE_DIRS /home/dji/Downloads/Onboard-SDK-master/sample/platform/linux/advanced-sensing/camera_stream_poll_sample)
#include_directories(${RAPIDJSON_INCLUDE_DIRS})

set(HELPER_FUNCTIONS_DIR ${CMAKE_CURRENT_SOURCE_DIR}/../../common)
message(STATUS this is a  ${HELPER_FUNCTIONS_DIR})
add_executable(${PROJECT_NAME}
  ${SOURCE_FILES}
  ${HELPER_FUNCTIONS_DIR}/dji_linux_environment.cpp
  ${HELPER_FUNCTIONS_DIR}/dji_linux_helpers.cpp
  camera-stream-poll-sample.cpp
  base64.cc
  base64.h
  )
target_include_directories(${PROJECT_NAME} PRIVATE ${OpenCV_INCLUDE_DIRS})

if (OpenCV_FOUND)
  target_link_libraries(${PROJECT_NAME}
    ${OpenCV_LIBS}
    )
endif ()


set(Boost_USE_STATIC_LIBS        ON) # only find static libs
set(Boost_USE_MULTITHREADED      ON)
set(Boost_USE_STATIC_RUNTIME    OFF)
find_package(boost COMPONENTS date_time filesystem system ...)
if(Boost_FOUND)
  include_directories(${Boost_INCLUDE_DIRS})
  target_link_libraries(${PROJECT_NAME} ${Boost_LIBRARIES})
endif()




# add_library(${PROJECT_NAME} ...)
