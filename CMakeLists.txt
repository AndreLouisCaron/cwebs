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

# Extract the files containing the macro "Q_OBJECT" from a file list.
macro(find_moc_files output headers)
  foreach(header ${headers})
    file(STRINGS ${header} matched REGEX "(.*)Q_OBJECT(.*)")
    list(LENGTH matched matches)
    if(${matches} GREATER 0)
      list(APPEND ${output} ${header})
    endif(${matches} GREATER 0)
  endforeach()
endmacro()

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

  file(GLOB httpxx_HEADERS
    ${CMAKE_CURRENT_SOURCE_DIR}/libs/httpxx/code/*.h
    ${CMAKE_CURRENT_SOURCE_DIR}/libs/httpxx/code/*.hpp)
  file(GLOB httpxx_SOURCES
    ${CMAKE_CURRENT_SOURCE_DIR}/libs/httpxx/code/*.c
    ${CMAKE_CURRENT_SOURCE_DIR}/libs/httpxx/code/*.cpp)
  add_library(httpxx STATIC
    ${httpxx_SOURCES}
    ${httpxx_HEADERS}
    ${CMAKE_CURRENT_SOURCE_DIR}/libs/http-parser/http_parser.h
    ${CMAKE_CURRENT_SOURCE_DIR}/libs/http-parser/http_parser.c)

  # Qt-based demo projects.
  find_package(Qt4 4.7 COMPONENTS QtCore QtGui QtNetwork)
  if(QT4_FOUND)
    include(UseQt4)
    include_directories(${QT_INCLUDE_DIR})
    add_definitions(${QT_DEFINITIONS})

    file(GLOB qws-server_HEADERS
      ${CMAKE_CURRENT_SOURCE_DIR}/demo/all/qt-webs-server/*.h
      ${CMAKE_CURRENT_SOURCE_DIR}/demo/all/qt-webs-server/*.hpp)
    file(GLOB qws-server_SOURCES
      ${CMAKE_CURRENT_SOURCE_DIR}/demo/all/qt-webs-server/*.c
      ${CMAKE_CURRENT_SOURCE_DIR}/demo/all/qt-webs-server/*.cpp)
    find_moc_files(qws-server_MOC-HEADERS ${qws-server_HEADERS})
    QT4_WRAP_CPP(qws-server_MOC-SOURCES ${qws-server_MOC-HEADERS})
    message(STATUS ${qws-server_MOC-HEADERS})
    message(STATUS ${qws-server_MOC-SOURCES})
    add_executable(qws-server
      ${qws-server_HEADERS}
      ${qws-server_SOURCES}
      ${qws-server_MOC-SOURCES})
    target_link_libraries(qws-server ${PROJECT_NAME} httpxx ${QT_LIBRARIES})
    add_dependencies(qws-server ${PROJECT_NAME} httpxx)
  endif()

  # UNIX-based demo projects.
  if(UNIX)
    include_directories(${CMAKE_CURRENT_SOURCE_DIR}/demo/nix)

    # WebSocket transport application:
    #   perform WebSocket handshake, then push standard input over the wire
    #   and pull standard output from the wire to let a controlling program
    #   tunnel any stream over WebSockets.
    file(GLOB client-tunnel_HEADERS
      ${CMAKE_CURRENT_SOURCE_DIR}/demo/nix/client-tunnel/*.h
      ${CMAKE_CURRENT_SOURCE_DIR}/demo/nix/client-tunnel/*.hpp)
    file(GLOB client-tunnel_SOURCES
      ${CMAKE_CURRENT_SOURCE_DIR}/demo/all/client-tunnel/*.c
      ${CMAKE_CURRENT_SOURCE_DIR}/demo/all/client-tunnel/*.cpp)
    add_executable(client-tunnel
      ${client-tunnel_HEADERS} 
      ${client-tunnel_SOURCES})
    target_link_libraries(client-tunnel ${PROJECT_NAME} b64 sha1 httpxx)
    add_dependencies(client-tunnel ${PROJECT_NAME} b64 sha1 httpxx)

    # WebSocket transport application:
    #   perform WebSocket handshake, then push standard input over the wire
    #   and pull standard output from the wire to let a controlling program
    #   tunnel any stream over WebSockets.
    file(GLOB server-tunnel_HEADERS
      ${CMAKE_CURRENT_SOURCE_DIR}/demo/nix/server-tunnel/*.h
      ${CMAKE_CURRENT_SOURCE_DIR}/demo/nix/server-tunnel/*.hpp)
    file(GLOB server-tunnel_SOURCES
      ${CMAKE_CURRENT_SOURCE_DIR}/demo/all/server-tunnel/*.c
      ${CMAKE_CURRENT_SOURCE_DIR}/demo/all/server-tunnel/*.cpp)
    add_executable(server-tunnel
      ${server-tunnel_HEADERS} 
      ${server-tunnel_SOURCES})
    target_link_libraries(server-tunnel ${PROJECT_NAME} b64 sha1 httpxx)
    add_dependencies(server-tunnel ${PROJECT_NAME} b64 sha1 httpxx)
  endif()

endif()
