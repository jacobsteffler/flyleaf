add_subdirectory(audio)

if(APPLE)
    add_subdirectory(mac)
elseif(UNIX)
    add_subdirectory(linux)
else()
    add_subdirectory(win)
endif()
