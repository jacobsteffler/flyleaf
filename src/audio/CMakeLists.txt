set(AUDIO_SRC "Audio.cpp" "AudioDecoder.cpp" "AudioResampler.cpp")

add_library(Audio ${AUDIO_SRC})
target_include_directories(Audio PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
target_compile_features(Audio PUBLIC cxx_delegating_constructors)
