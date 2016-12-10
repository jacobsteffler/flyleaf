cmake_minimum_required(VERSION 3.6.3)
project(flyleaf)

include_directories(include)

file(GLOB GENERAL_SOURCES "src/*.cpp")

if(APPLE)
    file(GLOB PLATFORM_SOURCES "src/mac/*.cpp")
elseif(UNIX)
    file(GLOB PLATFORM_SOURCES "src/linux/*.cpp")
else()
    file(GLOB PLATFORM_SOURCES "src/win/*.cpp")
endif()

set(SOURCES ${GENERAL_SOURCES} ${PLATFORM_SOURCES})

add_executable(${PROJECT_NAME} ${SOURCES})
TARGET_LINK_LIBRARIES(${PROJECT_NAME} avformat avcodec avutil swresample)
TARGET_COMPILE_FEATURES(${PROJECT_NAME} PRIVATE cxx_delegating_constructors)
