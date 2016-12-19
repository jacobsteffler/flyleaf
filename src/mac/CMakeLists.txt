add_executable(${PROJECT_NAME} "Main.cpp")
target_link_libraries(${PROJECT_NAME} LINK_PUBLIC Audio avformat avcodec avutil swresample)
