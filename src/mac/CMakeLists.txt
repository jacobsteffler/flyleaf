add_executable(${PROJECT_NAME} "Main.cpp")

find_library(AudioUnit AudioUnit)

target_link_libraries(${PROJECT_NAME} PUBLIC Decode Playback)
target_link_libraries(${PROJECT_NAME} PUBLIC avformat avcodec avutil swresample)
