if(APPLE)
    set(PLAY_SRC "mac_playback.cpp")

    find_library(AudioUnit AudioUnit)
    set(EXTRA_LIBS ${EXTRA_LIBS} ${AudioUnit})
elseif(UNIX)
else()
endif()

add_library(Playback ${PLAY_SRC})
target_include_directories(Playback PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
target_link_libraries(Playback PUBLIC ${EXTRA_LIBS})
