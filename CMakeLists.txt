cmake_minimum_required(VERSION 3.16)
project(qingtu-qtquick VERSION 0.1.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_AUTOMOC ON)

if(MSVC)
  add_compile_options(/utf-8)
endif()

find_package(Qt6 REQUIRED COMPONENTS Quick Qml QuickControls2 Concurrent)

qt_add_executable(qingtu-qtquick
  src/main.cpp
  src/imagecontroller.cpp
  src/imagecontroller.h
  src/image_job.cpp
)

qt_add_qml_module(qingtu-qtquick
  URI QingtuApp
  VERSION 1.0
  QML_FILES qml/Main.qml
)

target_include_directories(qingtu-qtquick PRIVATE src ${CMAKE_CURRENT_SOURCE_DIR}/third_party/stb)
target_link_libraries(qingtu-qtquick PRIVATE Qt6::Quick Qt6::Qml Qt6::QuickControls2 Qt6::Concurrent)

if(WIN32)
  set_target_properties(qingtu-qtquick PROPERTIES WIN32_EXECUTABLE ON OUTPUT_NAME "qingtu-qtquick")
endif()
