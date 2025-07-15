cmake_minimum_required(VERSION 3.10)
project(TextileDefectDetection)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

# Find GTK
find_package(PkgConfig REQUIRED)
pkg_check_modules(GTK3 REQUIRED gtk+-3.0 gio-2.0)

# Include GTK headers
include_directories(${GTK3_INCLUDE_DIRS})
link_directories(${GTK3_LIBRARY_DIRS})
add_definitions(${GTK3_CFLAGS_OTHER})

# Add source files
add_executable(image_capture src/image_capture.cpp)
add_executable(feed_splicer src/feed_splicer.cpp)
add_executable(creasing_detector src/creasing_detector.cpp)
add_executable(fabric_gui_launcher src/GUI/fabric_gui_launcher.cpp)  # <- Updated path

# Link OpenCV to those that use it
find_package(OpenCV REQUIRED)
target_link_libraries(image_capture ${OpenCV_LIBS})
target_link_libraries(feed_splicer ${OpenCV_LIBS})
target_link_libraries(creasing_detector ${OpenCV_LIBS})

# Link GTK to GUI
target_link_libraries(fabric_gui_launcher ${GTK3_LIBRARIES})
