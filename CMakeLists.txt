cmake_minimum_required(VERSION 3.6.3)
project(flyleaf)

include_directories(include)

file(GLOB GENERAL_SOURCES "src/*.cpp")

if(WIN32)
    file(GLOB PLATFORM_SOURCES "src/win/*.cpp")
endif(WIN32)

if(UNIX)
    file(GLOB PLATFORM_SOURCES "src/linux/*.cpp")
endif(UNIX)

set(SOURCES ${GENERAL_SOURCES} ${PLATFORM_SOURCES})

add_executable(${PROJECT_NAME} ${SOURCES})
TARGET_LINK_LIBRARIES(${PROJECT_NAME} avformat avcodec avutil swresample)
