add_executable(${PROJECT_NAME} "Main.cpp" "WinAudio.cpp")
target_link_libraries(${PROJECT_NAME} LINK_PUBLIC Audio avformat avcodec avutil swresample)
target_compile_features(${PROJECT_NAME} PRIVATE cxx_delegating_constructors)
