if(APPLE)
    set(PLAY_SRC "MPlayback.cpp")

    find_library(AudioUnit AudioUnit)
    set(EXTRA_LIBS ${EXTRA_LIBS} ${AudioUnit})
elseif(UNIX)
else()
    set(PLAY_SRC "WPlayback.cpp")
endif()

add_library(Playback ${PLAY_SRC})
target_include_directories(Playback PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

target_link_libraries(Playback PUBLIC Decode)
target_link_libraries(Playback PUBLIC ${EXTRA_LIBS})
