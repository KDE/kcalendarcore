cmake_minimum_required(VERSION 3.0)
set(PIM_VERSION "5.8.2")

project(KCalCore VERSION ${PIM_VERSION})

# ECM setup
set(KF5_VERSION "5.44.0")

find_package(ECM ${KF5_VERSION} CONFIG REQUIRED)
set(CMAKE_MODULE_PATH ${ECM_MODULE_PATH} ${KCalCore_SOURCE_DIR}/cmake)

include(GenerateExportHeader)
include(ECMGenerateHeaders)
include(ECMGeneratePriFile)
include(CMakePackageConfigHelpers)
include(ECMSetupVersion)
include(FeatureSummary)
include(KDEInstallDirs)
include(KDECMakeSettings)
include(KDEFrameworkCompilerSettings NO_POLICY_SCOPE)
include(ECMQtDeclareLoggingCategory)
include(ECMCoverageOption)
if(POLICY CMP0053)
  cmake_policy(SET CMP0053 NEW)
endif()
set(QT_REQUIRED_VERSION "5.8.0")

set(KCALENDARCORE_LIB_VERSION ${PIM_VERSION})
ecm_setup_version(PROJECT VARIABLE_PREFIX KCALCORE
  VERSION_HEADER "${KCalCore_BINARY_DIR}/kcalcore_version.h"
  PACKAGE_VERSION_FILE "${KCalCore_BINARY_DIR}/KF5CalendarCoreConfigVersion.cmake"
  SOVERSION 5
)

########### Find packages ###########
find_package(Qt5 ${QT_REQUIRED_VERSION} CONFIG REQUIRED Core Gui)

set(LibIcal_MIN_VERSION "2.0")
find_package(LibIcal ${LibIcal_MIN_VERSION})
set_package_properties(LibIcal PROPERTIES TYPE REQUIRED)
if(LibIcal_VERSION VERSION_GREATER "2.98")
  set(HAVE_ICAL_3 TRUE)
  add_definitions(-DUSE_ICAL_3)
endif()

########### CMake Config Files ###########
set(CMAKECONFIG_INSTALL_DIR "${KDE_INSTALL_CMAKEPACKAGEDIR}/KF5CalendarCore")

configure_package_config_file(
  "${CMAKE_CURRENT_SOURCE_DIR}/KF5CalendarCoreConfig.cmake.in"
  "${CMAKE_CURRENT_BINARY_DIR}/KF5CalendarCoreConfig.cmake"
  INSTALL_DESTINATION ${CMAKECONFIG_INSTALL_DIR}
)

add_definitions(-DQT_NO_CAST_FROM_ASCII -DQT_NO_CAST_TO_ASCII)
add_definitions(-DQT_NO_NARROWING_CONVERSIONS_IN_CONNECT)
add_definitions(-DQT_NO_URL_CAST_FROM_STRING)
add_definitions(-DQT_USE_QSTRINGBUILDER)
add_definitions(-DQT_DISABLE_DEPRECATED_BEFORE=0x060000)
########### Targets ###########
add_subdirectory(src)

if(BUILD_TESTING)
  find_package(Qt5 ${QT_REQUIRED_VERSION} CONFIG REQUIRED Test)
  add_subdirectory(autotests)
endif()

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
  DESTINATION ${KDE_INSTALL_INCLUDEDIR_KF5} COMPONENT Devel
)

install(FILES kcalcore.renamecategories kcalcore.categories DESTINATION ${KDE_INSTALL_CONFDIR})

feature_summary(WHAT ALL FATAL_ON_MISSING_REQUIRED_PACKAGES)
