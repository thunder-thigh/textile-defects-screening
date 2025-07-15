cmake_minimum_required(VERSION 3.10)
project(TextileDefectDetection)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

# === Dependencies ===
find_package(OpenCV REQUIRED)
find_package(PkgConfig REQUIRED)
pkg_check_modules(GTK3 REQUIRED gtk+-3.0 gio-2.0)

include_directories(${GTK3_INCLUDE_DIRS})
link_directories(${GTK3_LIBRARY_DIRS})
add_definitions(${GTK3_CFLAGS_OTHER})

# === Executables ===
add_executable(image_capture src/image_capture.cpp)
add_executable(feed_splicer src/feed_splicer.cpp)
add_executable(creasing_detector src/creasing_detector.cpp)
add_executable(fabric_gui_launcher src/GUI/fabric_gui_launcher.cpp)

# === Linking ===
target_link_libraries(image_capture ${OpenCV_LIBS})
target_link_libraries(feed_splicer ${OpenCV_LIBS})
target_link_libraries(creasing_detector ${OpenCV_LIBS})
target_link_libraries(fabric_gui_launcher ${GTK3_LIBRARIES})

# === Auto Copy Binaries to Packaging Directory ===
set(INSTALL_BIN_DIR "${CMAKE_BINARY_DIR}/textile-defects-screening/usr/bin")
file(MAKE_DIRECTORY ${INSTALL_BIN_DIR})

# Copy each binary after build
foreach(TARGET_NAME IN ITEMS
    image_capture
    feed_splicer
    creasing_detector
    fabric_gui_launcher
)
    add_custom_command(TARGET ${TARGET_NAME} POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy $<TARGET_FILE:${TARGET_NAME}> ${INSTALL_BIN_DIR}
        COMMENT "Copying ${TARGET_NAME} to packaging directory"
    )
endforeach()
