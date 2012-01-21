# Library configuration file used by dependent projects
# via find_package() built-in directive in "config" mode.

if(NOT DEFINED cwebs_FOUND)

  # Locate library headers.
  find_path(cwebs_include_dirs
    NAMES webs.h
    PATHS ${cwebs_DIR}/code
  )

  # Common name for exported library targets.
  set(cwebs_libraries
    webs
    CACHE INTERNAL "cwebs library" FORCE
  )

  # Usual "required" et. al. directive logic.
  include(FindPackageHandleStandardArgs)
  find_package_handle_standard_args(
    cwebs DEFAULT_MSG
    cwebs_include_dirs
    cwebs_libraries
  )

  # Add targets to dependent project.
  add_subdirectory(
    ${cwebs_DIR}
    ${CMAKE_BINARY_DIR}/cwebs
  )
endif()
