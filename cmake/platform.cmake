if (UNIX)
include (cmake/platform_unix.cmake)
else ()
include (cmake/platform_windows.cmake)
endif()
