include_directories(${CMAKE_CURRENT_SOURCE_DIR})

# UNIX-based demo projects.
if(UNIX)
  add_subdirectory(nix)
endif()

# Microsoft Windows-based demo projects.
if(WIN32)
  add_subdirectory(win)
endif()

# Qt-based demo projects, on any platform.
if(QT4_FOUND)
  add_subdirectory(all)
endif()
