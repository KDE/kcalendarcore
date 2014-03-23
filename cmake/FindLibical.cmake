# Find Libical
#
#  LibIcal_FOUND - system has Libical with the minimum version needed
#  LibIcal_INCLUDE_DIRS - the Libical include directories
#  LibIcal_LIBRARIES - The libraries needed to use Libical
#  LibIcal_VERSION = The value of ICAL_VERSION defined in ical.h
#  LibIcal_MAJOR_VERSION = The library major version number
#  LibIcal_MINOR_VERSION = The library minor version number

# Copyright (c) 2008,2010 Allen Winter <winter@kde.org>
#
# Redistribution and use is allowed according to the terms of the BSD license.
# For details see the accompanying COPYING-CMAKE-SCRIPTS file.

if(NOT LibIcal_FIND_VERSION)
  set(LibIcal_FIND_VERSION "0.33")
endif()

if (WIN32)
  file(TO_CMAKE_PATH "$ENV{PROGRAMFILES}" _program_FILES_DIR)
endif()

#set the root from the LibIcal_BASE environment
file(TO_CMAKE_PATH "$ENV{LibIcal_BASE}" libical_root)

#override the root from LibIcal_BASE defined to cmake
if(DEFINED LibIcal_BASE)
  file(TO_CMAKE_PATH "${LibIcal_BASE}" libical_root)
endif()

find_path(LibIcal_INCLUDE_DIRS
    NAMES libical/ical.h
    HINTS ${libical_root}/include ${_program_FILES_DIR}/libical/include
)

find_library(LibIcal_LIBRARY
    NAMES ical libical
    HINTS ${libical_root}/lib ${_program_FILES_DIR}/libical/lib
)

find_library(LibIcalss_LIBRARY
    NAMES icalss libicalss
    HINTS ${libical_root}/lib ${_program_FILES_DIR}/libical/lib
)

set(LibIcal_LIBRARIES ${LibIcal_LIBRARY} ${LibIcalss_LIBRARY})

if(LibIcal_INCLUDE_DIRS AND LibIcal_LIBRARIES)
  set(FIND_LibIcal_VERSION_SOURCE
    "#include <libical/ical.h>\n int main()\n {\n printf(\"%s\",ICAL_VERSION);return 1;\n }\n")
  set(FIND_LibIcal_VERSION_SOURCE_FILE ${CMAKE_BINARY_DIR}/CMakeTmp/FindLIBICAL.cxx)
  file(WRITE "${FIND_LibIcal_VERSION_SOURCE_FILE}" "${FIND_LibIcal_VERSION_SOURCE}")

  set(FIND_LibIcal_VERSION_ADD_INCLUDES
    "-DINCLUDE_DIRECTORIES:STRING=${LibIcal_INCLUDE_DIRS}")

  if(NOT CMAKE_CROSSCOMPILING)
  try_run(RUN_RESULT COMPILE_RESULT
    ${CMAKE_BINARY_DIR}
    ${FIND_LibIcal_VERSION_SOURCE_FILE}
    CMAKE_FLAGS "${FIND_LibIcal_VERSION_ADD_INCLUDES}"
    RUN_OUTPUT_VARIABLE LibIcal_VERSION)
  endif()

  if(COMPILE_RESULT AND RUN_RESULT EQUAL 1 AND NOT CMAKE_CROSSCOMPILING)
    message(STATUS "Found Libical version ${LibIcal_VERSION}")
    if(${LibIcal_VERSION} VERSION_LESS ${LibIcal_FIND_VERSION})
      message(STATUS "LibIcal version ${LibIcal_VERSION} is too old. At least version ${LibIcal_FIND_VERSION} is needed.")
      set(LibIcal_INCLUDE_DIRS "")
      set(LibIcal_LIBRARIES "")
    endif()
    if(NOT LibIcal_VERSION VERSION_LESS 0.46)
      set(USE_ICAL_0_46 TRUE)
    endif()
    if(NOT LibIcal_VERSION VERSION_LESS 1.00)
      set(USE_ICAL_1_0 TRUE)
    endif()

  else()
    if(NOT CMAKE_CROSSCOMPILING)
        message(FATAL_ERROR "Unable to compile or run the libical version detection program.")
    endif()
  endif()

  #compute the major and minor version numbers
  if(NOT CMAKE_CROSSCOMPILING)
    string(REGEX REPLACE "\\..*$" "" LibIcal_MAJOR_VERSION ${LibIcal_VERSION})
    string(REGEX REPLACE "^.*\\." "" LibIcal_MINOR_VERSION ${LibIcal_VERSION})
  endif()

endif()

include(FindPackageHandleStandardArgs)

find_package_handle_standard_args(LibIcal
    FOUND_VAR LibIcal_FOUND
    REQUIRED_VARS LibIcal_LIBRARIES LibIcal_INCLUDE_DIRS
    VERSION_VAR LibIcal_VERSION
)

mark_as_advanced(LibIcal_INCLUDE_DIRS LibIcal_LIBRARIES)
