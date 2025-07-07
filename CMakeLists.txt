cmake_minimum_required(VERSION 3.10)
project(TextileDefectScreening)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Find OpenCV
find_package(OpenCV REQUIRED)
include_directories(${OpenCV_INCLUDE_DIRS})

# Add executables
add_executable(image_capture src/image_capture.cpp)
target_link_libraries(image_capture ${OpenCV_LIBS})

add_executable(feed_splicer src/feed_splicer.cpp)
target_link_libraries(feed_splicer ${OpenCV_LIBS})
