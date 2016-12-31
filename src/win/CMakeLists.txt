add_executable(${PROJECT_NAME} "Main.cpp" "WinAudio.cpp")
target_link_libraries(${PROJECT_NAME} LINK_PUBLIC Decode avformat avcodec avutil swresample)
