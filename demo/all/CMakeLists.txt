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
