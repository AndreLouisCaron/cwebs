# Microsoft Windows-based demo projects for the cwebs library.

# A few C++ wrappers to shrink-wrap demo program to its simplest expression.
file(GLOB win_headers
  ${CMAKE_CURRENT_SOURCE_DIR}/*.h
  ${CMAKE_CURRENT_SOURCE_DIR}/*.hpp
)
file(GLOB win_sources
  ${CMAKE_CURRENT_SOURCE_DIR}/*.c
  ${CMAKE_CURRENT_SOURCE_DIR}/*.cpp
)
add_library(win
  STATIC
  ${win_headers}
  ${win_sources}
)

# WebSocket transport application:
#   perform WebSocket handshake, then push standard input over the wire
#   and pull standard output from the wire to let a controlling program
#   tunnel any stream over WebSockets.
file(GLOB client-tunnel_headers
  ${CMAKE_CURRENT_SOURCE_DIR}/client-tunnel/*.h
  ${CMAKE_CURRENT_SOURCE_DIR}/client-tunnel/*.hpp)
file(GLOB client-tunnel_sources
  ${CMAKE_CURRENT_SOURCE_DIR}/client-tunnel/*.c
  ${CMAKE_CURRENT_SOURCE_DIR}/client-tunnel/*.cpp)
add_executable(client-tunnel
  ${client-tunnel_headers}
  ${client-tunnel_sources}
)
target_link_libraries(client-tunnel
  webs
  win
  ws2_32
  ${cb64_libraries}
  ${csha1_libraries}
  ${httpxx_libraries}
)
add_dependencies(client-tunnel
  webs
  win
  ${cb64_libraries}
  ${csha1_libraries}
  ${httpxx_libraries}
)

# WebSocket transport application:
#   perform WebSocket handshake, then push standard input over the wire
#   and pull standard output from the wire to let a controlling program
#   tunnel any stream over WebSockets.
file(GLOB server-tunnel_headers
  ${CMAKE_CURRENT_SOURCE_DIR}/server-tunnel/*.h
  ${CMAKE_CURRENT_SOURCE_DIR}/server-tunnel/*.hpp)
file(GLOB server-tunnel_sources
  ${CMAKE_CURRENT_SOURCE_DIR}/server-tunnel/*.c
  ${CMAKE_CURRENT_SOURCE_DIR}/server-tunnel/*.cpp)
add_executable(server-tunnel
  ${server-tunnel_headers}
  ${server-tunnel_sources}
)
target_link_libraries(server-tunnel
  webs
  win
  ws2_32
  ${cb64_libraries}
  ${csha1_libraries}
  ${httpxx_libraries}
)
add_dependencies(server-tunnel
  webs
  win
  ${cb64_libraries}
  ${csha1_libraries}
  ${httpxx_libraries}
)
