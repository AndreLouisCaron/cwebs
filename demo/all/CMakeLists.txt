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

# Qt-based demo projects for the cwebs library.

include(UseQt4)
include_directories(${QT_INCLUDE_DIR})
add_definitions(${QT_DEFINITIONS})

file(GLOB qws-server_headers
  ${CMAKE_CURRENT_SOURCE_DIR}/qt-webs-server/*.h
  ${CMAKE_CURRENT_SOURCE_DIR}/qt-webs-server/*.hpp)
file(GLOB qws-server_sources
  ${CMAKE_CURRENT_SOURCE_DIR}/qt-webs-server/*.c
  ${CMAKE_CURRENT_SOURCE_DIR}/qt-webs-server/*.cpp)

qt4_find_moc(qws-server_MOC-HEADERS ${qws-server_headers})
qt4_wrap_cpp(qws-server_MOC-SOURCES ${qws-server_MOC-HEADERS})

add_executable(qws-server
  ${qws-server_headers}
  ${qws-server_sources}
  ${qws-server_MOC-SOURCES}
)
target_link_libraries(qws-server webs httpxx ${QT_LIBRARIES})
add_dependencies(qws-server webs httpxx)
