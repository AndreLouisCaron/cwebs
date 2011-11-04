#
# Web Socket wire protocol implementation in C.
#

cmake_minimum_required(VERSION 2.8)

project(cwebs)

file(GLOB ${PROJECT_NAME}_HEADERS code/*.h)
file(GLOB ${PROJECT_NAME}_SOURCES code/*.c)
set_source_files_properties(${${PROJECT_NAME}_SOURCES} PROPERTIES LANGUAGE CXX)
add_library(${PROJECT_NAME} STATIC
    ${${PROJECT_NAME}_SOURCES}
    ${${PROJECT_NAME}_HEADERS})

# When building in standalone mode, build demo projects.
if(${PROJECT_NAME} STREQUAL ${CMAKE_PROJECT_NAME})

  # Resolve include directives as if installed on the system.
  include_directories(${CMAKE_CURRENT_SOURCE_DIR}/code)
  include_directories(${CMAKE_CURRENT_SOURCE_DIR}/libs/httpxx/code)
  include_directories(${CMAKE_CURRENT_SOURCE_DIR}/libs/http-parser)

  file(GLOB httpxx_HEADERS
    ${CMAKE_CURRENT_SOURCE_DIR}/libs/httpxx/code/*.hpp)
  file(GLOB httpxx_SOURCES
    ${CMAKE_CURRENT_SOURCE_DIR}/libs/httpxx/code/*.cpp)

  # WebSocket transport application:
  #   perform WebSocket handshake, then push standard input over the wire
  #   and pull standard output from the wire to let a controlling program
  #   tunnel any stream over WebSockets.
  add_executable(tunnel
    demo/tunnel/tunnel.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/libs/http-parser/http_parser.h
    ${CMAKE_CURRENT_SOURCE_DIR}/libs/http-parser/http_parser.c
    ${httpxx_SOURCES}
    ${httpxx_HEADERS})
  target_link_libraries(tunnel ${PROJECT_NAME})
  add_dependencies(tunnel ${PROJECT_NAME})

endif()
