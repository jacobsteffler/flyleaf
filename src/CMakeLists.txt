add_subdirectory(decode)
add_subdirectory(playback)

if(APPLE)
    add_subdirectory(mac)
elseif(UNIX)
    add_subdirectory(linux)
else()
    add_subdirectory(win)
endif()
