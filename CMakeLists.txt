#
# Web Socket wire protocol implementation in C.
#

cmake_minimum_required(VERSION 2.6)

project(cwebs)

# Add targets for library referenced as Git submodule.
macro(add_submodule name)
  if(${ARGN1})
    set(${name}_DIR ${ARGN1})
  else()
    set(${name}_DIR ${CMAKE_SOURCE_DIR}/libs/${name})
  endif()
  find_package(${name})
endmacro()

# Compile API documentation from source code.
function(add_api_documentation target)
  if(DOXYGEN_EXECUTABLE)
    add_custom_target(
      ${target}
      COMMAND
        ${DOXYGEN_EXECUTABLE}
      WORKING_DIRECTORY
        ${CMAKE_CURRENT_BINARY_DIR}
      COMMENT
        "Compiling documentation."
      VERBATIM
    )
  endif()
endfunction()

# Extract the files containing the macro "Q_OBJECT" from a file list.
macro(qt4_find_moc output headers)
  set(output)
  foreach(header ${headers})
    file(STRINGS ${header} matched REGEX "(.*)Q_OBJECT(.*)")
    list(LENGTH matched matches)
    if(${matches} GREATER 0)
      list(APPEND ${output} ${header})
    endif()
  endforeach()
endmacro()

if(MSVC)
  # Ignore Microsoft's recommendations of
  # safety at the expense of portability.
  add_definitions(
    -D_CRT_SECURE_NO_WARNINGS
    -D_SCL_SECURE_NO_WARNINGS
  )
endif()

# Build the primary target.
add_subdirectory(code)

# Optional targets (skip when building as a dependency).
if(${PROJECT_NAME} STREQUAL ${CMAKE_PROJECT_NAME})

  # Build API documentation.
  find_package(Doxygen QUIET)
  configure_file(
    ${CMAKE_SOURCE_DIR}/help/Doxyfile
    ${CMAKE_BINARY_DIR}/Doxyfile @ONLY)
  add_api_documentation(doc)

  # Resolve include directives as if installed on the system.
  include_directories(${CMAKE_CURRENT_SOURCE_DIR}/code)
  include_directories(${CMAKE_CURRENT_SOURCE_DIR}/libs/http-parser)

  # Build libraries required for demo projects.
  add_submodule(httpxx)
  add_submodule(cb64)
  add_submodule(csha1)

  include_directories(
    ${httpxx_include_dirs}
    ${cb64_include_dirs}
    ${csha1_include_dirs}
  )

  # Build demo projects.
  add_subdirectory(demo)

endif()
