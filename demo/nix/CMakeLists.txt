# Copyright (c) 2011-2012, Andre Caron (andre.l.caron@gmail.com)
# All rights reserved.
# 
# Redistribution and use in source and binary forms, with or without
# modification, are permitted provided that the following conditions are
# met:
# 
#   Redistributions of source code must retain the above copyright
#   notice, this list of conditions and the following disclaimer.
# 
#   Redistributions in binary form must reproduce the above copyright
#   notice, this list of conditions and the following disclaimer in the
#   documentation and/or other materials provided with the distribution.
# 
# THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
# "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
# LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
# A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
# HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
# SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
# LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
# DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
# THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
# (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
# OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

# UNIX-based demo projects for cwebs library.

# A few C++ wrappers to shrink-wrap demo program to its simplest expression.
file(GLOB nix_headers
  ${CMAKE_CURRENT_SOURCE_DIR}/*.h
  ${CMAKE_CURRENT_SOURCE_DIR}/*.hpp
)
file(GLOB nix_sources
  ${CMAKE_CURRENT_SOURCE_DIR}/*.c
  ${CMAKE_CURRENT_SOURCE_DIR}/*.cpp
)
add_library(nix
  STATIC
  ${nix_headers}
  ${nix_sources}
)
target_link_libraries(nix
  webs
  ${cb64_libraries}
  ${csha1_libraries}
  ${httpxx_libraries}
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
target_link_libraries(client-tunnel nix)
add_dependencies(client-tunnel nix)

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
target_link_libraries(server-tunnel nix)
add_dependencies(server-tunnel nix)
