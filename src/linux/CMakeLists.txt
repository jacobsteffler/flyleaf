add_executable(${PROJECT_NAME} "Main.cpp")
target_link_libraries(${PROJECT_NAME} LINK_PUBLIC Decode Playback)
target_link_libraries(${PROJECT_NAME} LINK_PUBLIC avformat avcodec avutil swresample)
