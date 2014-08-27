cmake_minimum_required(VERSION 2.8.12)

project(KCalCore)

# ECM setup
find_package(ECM 1.1.0 CONFIG REQUIRED)
set(CMAKE_MODULE_PATH ${ECM_MODULE_PATH} ${KCalCore_SOURCE_DIR}/cmake)

include(GenerateExportHeader)
include(ECMGenerateHeaders)
include(ECMGeneratePriFile)
include(ECMPackageConfigHelpers)
include(ECMSetupVersion)
include(FeatureSummary)
include(KDEInstallDirs)
include(KDECMakeSettings)
include(KDEFrameworkCompilerSettings)

set(KF5_VERSION "5.1.0")
set(KDEPIMLIBS_VERSION "4.71.0")
ecm_setup_version(${KDEPIMLIBS_VERSION} VARIABLE_PREFIX KCALCORE
                        VERSION_HEADER "${KCalCore_BINARY_DIR}/kcalcore_version.h"
                        PACKAGE_VERSION_FILE "${KCalCore_BINARY_DIR}/KF5CalendarCoreConfigVersion.cmake"
                        SOVERSION 5
)

############### Build Options ###############

# The following macros can be defined to alter how KCalCore is built.
# For example, cmake -DKCALCORE_FOR_MEEGO=true

# KCALCORE_FOR_MEEGO - builds KCalCore with some special features for the MeeGo platform.
option(KCALCORE_FOR_MEEGO "Build KCalCore especially for the MeeGo." FALSE)

# KCALCORE_FOR_SYMBIAN - builds KCalCore with some special features for the Symbian platform.
option(KCALCORE_FOR_SYMBIAN "Build KCalCore especially for the Symbian." FALSE)

# add C++ macro definitions for options passed to CMake
if(KCALCORE_FOR_MEEGO)
  add_definitions(-DKCALCORE_FOR_MEEGO)
endif()

if(KCALCORE_FOR_SYMBIAN)
  add_definitions(-DKCALCORE_FOR_SYMBIAN)
endif()

########### Find packages ###########
find_package(KF5KDELibs4Support ${KF5_VERSION} CONFIG REQUIRED)

set(LibIcal_MIN_VERSION "0.42")
find_package(LibIcal ${LibIcal_MIN_VERSION})
set_package_properties(LibIcal PROPERTIES
                               DESCRIPTION "The libical library"
                               URL "http://sourceforge.net/projects/freeassociation"
                               TYPE REQUIRED
)

include(ConfigureChecks.cmake)


########### CMake Config Files ###########
set(CMAKECONFIG_INSTALL_DIR "${CMAKECONFIG_INSTALL_PREFIX}/KF5CalendarCore")

ecm_configure_package_config_file(
  "${CMAKE_CURRENT_SOURCE_DIR}/KF5CalendarCoreConfig.cmake.in"
  "${CMAKE_CURRENT_BINARY_DIR}/KF5CalendarCoreConfig.cmake"
  INSTALL_DESTINATION  ${CMAKECONFIG_INSTALL_DIR}
)

remove_definitions(-DQT_NO_CAST_FROM_ASCII -DQT_NO_CAST_TO_ASCII)
########### Targets ###########
add_subdirectory(src)
add_subdirectory(autotests)
add_subdirectory(cmake)

########### Install Files ###########
install(FILES
  "${CMAKE_CURRENT_BINARY_DIR}/KF5CalendarCoreConfig.cmake"
  "${CMAKE_CURRENT_BINARY_DIR}/KF5CalendarCoreConfigVersion.cmake"
  DESTINATION "${CMAKECONFIG_INSTALL_DIR}"
  COMPONENT Devel
)

install(EXPORT KF5CalendarCoreTargets
    DESTINATION "${CMAKECONFIG_INSTALL_DIR}"
    FILE KF5CalendarCoreTargets.cmake
    NAMESPACE KF5::
)

install(FILES
   ${CMAKE_CURRENT_BINARY_DIR}/kcalcore_version.h
  DESTINATION ${KF5_INCLUDE_INSTALL_DIR} COMPONENT Devel
)

if ("${CMAKE_BINARY_DIR}" STREQUAL "${KCalCore_BINARY_DIR}")
  feature_summary(WHAT ALL FATAL_ON_MISSING_REQUIRED_PACKAGES)
endif()
