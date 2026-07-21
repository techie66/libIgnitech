cmake_minimum_required(VERSION 3.16)
project(rpm_viewer LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_AUTOMOC ON)

# Qt6 — install with e.g.  sudo apt install qt6-base-dev
find_package(Qt6 COMPONENTS Widgets SerialPort REQUIRED)

# libIgnitech — built & installed from https://github.com/techie66/libIgnitech
#   git clone https://github.com/techie66/libIgnitech.git
#   cd libIgnitech && ./configure && make && sudo make install && sudo ldconfig
find_library(IGNITECH_LIB ignitech)
if(NOT IGNITECH_LIB)
    message(FATAL_ERROR "libignitech not found. Build & install libIgnitech first.")
endif()

# If you haven't installed the library, point this at your libIgnitech checkout:
set(IGNITECH_SRC_DIR "${CMAKE_CURRENT_SOURCE_DIR}/../..//src"
    CACHE PATH "Path to libIgnitech src/ directory (contains ignitech.h)")

add_executable(rpm_viewer main.cpp)

target_include_directories(rpm_viewer PRIVATE
    ${IGNITECH_SRC_DIR}
)
target_link_libraries(rpm_viewer PRIVATE
    Qt6::Widgets
    Qt6::SerialPort
    ${IGNITECH_LIB}
)

install(TARGETS rpm_viewer RUNTIME DESTINATION bin)
