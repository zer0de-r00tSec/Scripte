cmake_minimum_required(VERSION 3.24)
project(slideshow)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_AUTOMOC ON)

find_package(Qt6 REQUIRED COMPONENTS Core Gui)

include_directories(${Qt6Core_INCLUDE_DIRS} ${Qt6Gui_INCLUDE_DIRS})

set(slideshow_SRC
        slideshow.cpp
        ../../gscreen.cpp
        ../../plugininterface.hpp
)

qt_add_library(slideshow SHARED ${slideshow_SRC})
target_link_libraries(slideshow ${Qt6Core_LIBRARIES} ${Qt6Gui_LIBRARIES})

install(TARGETS slideshow LIBRARY DESTINATION ${PLUGINS_DIR})
