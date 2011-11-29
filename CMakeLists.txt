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

  file(GLOB b64_HEADERS
    ${CMAKE_CURRENT_SOURCE_DIR}/libs/cb64/code/*.h
    ${CMAKE_CURRENT_SOURCE_DIR}/libs/cb64/code/*.hpp)
  file(GLOB b64_SOURCES
    ${CMAKE_CURRENT_SOURCE_DIR}/libs/cb64/code/*.c
    ${CMAKE_CURRENT_SOURCE_DIR}/libs/cb64/code/*.cpp)
  include_directories(${CMAKE_CURRENT_SOURCE_DIR}/libs/cb64/code)
  add_library(b64 STATIC
    ${b64_SOURCES}
    ${b64_HEADERS})

  file(GLOB sha1_HEADERS
    ${CMAKE_CURRENT_SOURCE_DIR}/libs/csha1/code/*.h
    ${CMAKE_CURRENT_SOURCE_DIR}/libs/csha1/code/*.hpp)
  file(GLOB sha1_SOURCES
    ${CMAKE_CURRENT_SOURCE_DIR}/libs/csha1/code/*.c
    ${CMAKE_CURRENT_SOURCE_DIR}/libs/csha1/code/*.cpp)
  include_directories(${CMAKE_CURRENT_SOURCE_DIR}/libs/csha1/code)
  add_library(sha1 STATIC
    ${sha1_SOURCES}
    ${sha1_HEADERS})

  # Demo projects for UNIX-like platforms.
  if(UNIX)
    # Extra helpers for UNIX networking.
    include_directories(${CMAKE_CURRENT_SOURCE_DIR}/demo/nix)

    # WebSocket transport application:
    #   perform WebSocket handshake, then push standard input over the wire
    #   and pull standard output from the wire to let a controlling program
    #   tunnel any stream over WebSockets.
    add_executable(client-tunnel
      demo/nix/tunnel/client-tunnel.cpp
      ${CMAKE_CURRENT_SOURCE_DIR}/libs/http-parser/http_parser.h
      ${CMAKE_CURRENT_SOURCE_DIR}/libs/http-parser/http_parser.c
      ${httpxx_SOURCES}
      ${httpxx_HEADERS})
    target_link_libraries(client-tunnel ${PROJECT_NAME} b64 sha1)
    add_dependencies(client-tunnel ${PROJECT_NAME} b64 sha1)

    # WebSocket transport application:
    #   perform WebSocket handshake, then push standard input over the wire
    #   and pull standard output from the wire to let a controlling program
    #   tunnel any stream over WebSockets.
    add_executable(server-tunnel
      demo/nix/tunnel/server-tunnel.cpp
      ${CMAKE_CURRENT_SOURCE_DIR}/libs/http-parser/http_parser.h
      ${CMAKE_CURRENT_SOURCE_DIR}/libs/http-parser/http_parser.c
      ${httpxx_SOURCES}
      ${httpxx_HEADERS})
    target_link_libraries(server-tunnel ${PROJECT_NAME} b64 sha1)
    add_dependencies(server-tunnel ${PROJECT_NAME} b64 sha1)
  endif()

endif()
