set(AUDIO_SRC "Audio.cpp" "AudioDecoder.cpp" "AudioResampler.cpp")

add_library(Decode ${AUDIO_SRC})
target_include_directories(Decode PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
target_compile_features(Decode PUBLIC cxx_delegating_constructors)
