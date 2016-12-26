add_executable(${PROJECT_NAME} "Main.cpp" "coreaudio_example.cpp")

find_library(AudioUnit AudioUnit)

target_link_libraries(${PROJECT_NAME} LINK_PUBLIC Audio ${AudioUnit} avformat avcodec avutil swresample)
target_include_directories(${PROJECT_NAME} PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
