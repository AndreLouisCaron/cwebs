#
# Web Socket wire protocol implementation in C.
#

cmake_minimum_required(VERSION 2.8)

project(cwebs)

file(GLOB cwebs_HEADERS code/*.h)
file(GLOB cwebs_SOURCES code/*.c)
set_source_files_properties(${cwebs_SOURCES} PROPERTIES LANGUAGE CXX)
add_library(cwebs STATIC ${cwebs_SOURCES} ${cwebs_HEADERS})
