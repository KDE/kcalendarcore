cmake_minimum_required(VERSION 3.5)
set(KF5_VERSION "5.73.0") # handled by release scripts

project(KCalendarCore VERSION ${KF5_VERSION})

# ECM setup
include(FeatureSummary)
find_package(ECM 5.72.0  NO_MODULE)
set_package_properties(ECM PROPERTIES TYPE REQUIRED DESCRIPTION "Extra CMake Modules." URL "https://commits.kde.org/extra-cmake-modules")
feature_summary(WHAT REQUIRED_PACKAGES_NOT_FOUND FATAL_ON_MISSING_REQUIRED_PACKAGES)

set(CMAKE_MODULE_PATH ${ECM_MODULE_PATH} ${CMAKE_SOURCE_DIR}/cmake)

set(REQUIRED_QT_VERSION 5.12.0)

include(KDEInstallDirs)
include(KDECMakeSettings)
include(KDEFrameworkCompilerSettings NO_POLICY_SCOPE)

include(ECMGenerateExportHeader)
include(ECMGenerateHeaders)
include(ECMGeneratePriFile)

include(ECMSetupVersion)
include(ECMQtDeclareLoggingCategory)
include(ECMAddQch)

set(EXCLUDE_DEPRECATED_BEFORE_AND_AT 0 CACHE STRING "Control the range of deprecated API excluded from the build [default=0].")

option(BUILD_QCH "Build API documentation in QCH format (for e.g. Qt Assistant, Qt Creator & KDevelop)" OFF)
add_feature_info(QCH ${BUILD_QCH} "API documentation in QCH format (for e.g. Qt Assistant, Qt Creator & KDevelop)")

# TODO: remove for KF6
option(KCALENDARCORE_NO_DEPRECATED_NAMESPACE "Disable deprecated KCalCore namespace" OFF)

if(POLICY CMP0053)
  cmake_policy(SET CMP0053 NEW)
endif()

set(KCALENDARCORE_LIB_VERSION ${KF5_VERSION})
ecm_setup_version(PROJECT VARIABLE_PREFIX KCALENDARCORE
  VERSION_HEADER "${KCalendarCore_BINARY_DIR}/kcalendarcore_version.h"
  PACKAGE_VERSION_FILE "${KCalendarCore_BINARY_DIR}/KF5CalendarCoreConfigVersion.cmake"
  SOVERSION 5
)

########### Find packages ###########
find_package(Qt5 ${REQUIRED_QT_VERSION} CONFIG REQUIRED Core Gui)

set(LibIcal_MIN_VERSION "2.0")
find_package(LibIcal ${LibIcal_MIN_VERSION})
set_package_properties(LibIcal PROPERTIES TYPE REQUIRED)
if(LibIcal_VERSION VERSION_GREATER "2.98")
  set(HAVE_ICAL_3 TRUE)
  add_definitions(-DUSE_ICAL_3)
endif()

########### CMake Config Files ###########
set(CMAKECONFIG_INSTALL_DIR "${KDE_INSTALL_CMAKEPACKAGEDIR}/KF5CalendarCore")

if (BUILD_QCH)
    set(PACKAGE_INCLUDE_QCHTARGETS "include(\"\${CMAKE_CURRENT_LIST_DIR}/KF5CalendarCoreQchTargets.cmake\")")
endif()

configure_package_config_file(
  "${CMAKE_CURRENT_SOURCE_DIR}/KF5CalendarCoreConfig.cmake.in"
  "${CMAKE_CURRENT_BINARY_DIR}/KF5CalendarCoreConfig.cmake"
  INSTALL_DESTINATION ${CMAKECONFIG_INSTALL_DIR}
)


add_definitions(-DQT_DISABLE_DEPRECATED_BEFORE=0x050d00)

add_definitions(-DQT_NO_FOREACH)

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

if (BUILD_QCH)
    ecm_install_qch_export(
        TARGETS KF5CalendarCore_QCH
        FILE KF5CalendarCoreQchTargets.cmake
        DESTINATION "${CMAKECONFIG_INSTALL_DIR}"
        COMPONENT Devel
    )
endif()

install(FILES
  ${CMAKE_CURRENT_BINARY_DIR}/kcalendarcore_version.h
  DESTINATION ${KDE_INSTALL_INCLUDEDIR_KF5} COMPONENT Devel
)
if (NOT KCALENDARCORE_NO_DEPRECATED_NAMESPACE)
  ecm_setup_version(PROJECT VARIABLE_PREFIX KCALCORE
    VERSION_HEADER "${KCalendarCore_BINARY_DIR}/kcalcore_version.h"
  )
  install(FILES
    ${CMAKE_CURRENT_BINARY_DIR}/kcalcore_version.h
    DESTINATION ${KDE_INSTALL_INCLUDEDIR_KF5} COMPONENT Devel
  )
endif()

feature_summary(WHAT ALL FATAL_ON_MISSING_REQUIRED_PACKAGES)
