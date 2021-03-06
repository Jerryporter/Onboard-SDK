cmake_minimum_required(VERSION 2.8)
project(camera-stream-target-tracking-sample)

# Try to see if user has OpenCV installed
# if yes, default callback will display the image
find_package(OpenCV REQUIRED)
if (OpenCV_FOUND)
    message("\n${PROJECT_NAME}...")
    message(STATUS " - Includes: ${OpenCV_INCLUDE_DIRS}")
    message(STATUS " - Libraries: ${OpenCV_LIBRARIES}")
else ()
    message(STATUS "OpenCV is required for this sample to do the target tracking")
endif ()


unset(KCFcpp_DIR)
find_path(KCFcpp_DIR
        NAMES
        KCFCpp.sh
        PATHS
        ${CMAKE_CURRENT_SOURCE_DIR}/KCFcpp
        )

if (NOT KCFcpp_DIR)
    message(STATUS "Target Tracking sample depends on KCFcpp library from https://github.com/joaofaro/KCFcpp.git")
    message(STATUS "When you run make, we will try to clone the repo")
    message(STATUS "This will happen only once as long as you do not delete it")

    include(${CMAKE_MODULE_PATH}/DownloadProject/DownloadProject.cmake)
    download_project(PROJ kcf_cpp
            GIT_REPOSITORY https://github.com/joaofaro/KCFcpp.git
            GIT_TAG master
            ${UPDATE_DISCONNECTED_IF_AVAILABLE}
            SOURCE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/KCFcpp
            )

endif ()


## To use this this repo, you need to run
##  git clone https://github.com/joaofaro/KCFcpp.git
## under the current foler
include_directories(KCFcpp/src)
include_directories(${OpenCV_INCLUDE_DIRS})

add_library(kcf_tracker STATIC
        KCFcpp/src/fhog.cpp
        KCFcpp/src/kcftracker.cpp)
target_link_libraries(kcf_tracker ${OpenCV_LIBRARIES})

set(HELPER_FUNCTIONS_DIR ${CMAKE_CURRENT_SOURCE_DIR}/../../common)
add_executable(${PROJECT_NAME}
        ${SOURCE_FILES}
        ${HELPER_FUNCTIONS_DIR}/dji_linux_environment.cpp
        ${HELPER_FUNCTIONS_DIR}/dji_linux_helpers.cpp
        target_tracking.cpp
        tracking_utility.cpp
        )
target_link_libraries(${PROJECT_NAME}
        kcf_tracker
        ${OpenCV_LIBRARIES}
        )

set(Boost_USE_STATIC_LIBS        ON) # only find static libs
set(Boost_USE_MULTITHREADED      ON)
set(Boost_USE_STATIC_RUNTIME    OFF)
find_package(Boost COMPONENTS date_time filesystem system ...)
if(Boost_FOUND)
  include_directories(${Boost_INCLUDE_DIRS})
  target_link_libraries(${PROJECT_NAME} ${Boost_LIBRARIES})
endif()
