# webs library.

file(GLOB webs_headers
  ${CMAKE_CURRENT_SOURCE_DIR}/*.h
)
file(GLOB webs_sources
  ${CMAKE_CURRENT_SOURCE_DIR}/*.c
)
set_source_files_properties(
  ${webs_headers}
  ${webs_sources}
  PROPERTIES LANGUAGE CXX
)
add_library(webs
  STATIC
  ${webs_sources}
  ${webs_headers}
)
