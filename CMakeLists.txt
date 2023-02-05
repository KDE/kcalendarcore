cmake_minimum_required(VERSION 3.16)
set(KF_VERSION "5.103.0") # handled by release scripts

project(KCalendarCore VERSION ${KF_VERSION})

# ECM setup
include(FeatureSummary)
find_package(ECM 5.103.0  NO_MODULE)
set_package_properties(ECM PROPERTIES TYPE REQUIRED DESCRIPTION "Extra CMake Modules." URL "https://commits.kde.org/extra-cmake-modules")
feature_summary(WHAT REQUIRED_PACKAGES_NOT_FOUND FATAL_ON_MISSING_REQUIRED_PACKAGES)

set(CMAKE_MODULE_PATH ${ECM_MODULE_PATH} ${CMAKE_SOURCE_DIR}/cmake)

set(REQUIRED_QT_VERSION 5.15.2)

include(KDEInstallDirs)
include(KDECMakeSettings)
include(KDEGitCommitHooks)
include(KDEFrameworkCompilerSettings NO_POLICY_SCOPE)

include(ECMGenerateExportHeader)
include(ECMGenerateHeaders)
include(ECMGeneratePriFile)
include(ECMGeneratePkgConfigFile)
include(ECMDeprecationSettings)

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

set(KCALENDARCORE_LIB_VERSION ${KF_VERSION})
ecm_setup_version(PROJECT VARIABLE_PREFIX KCALENDARCORE
  VERSION_HEADER "${KCalendarCore_BINARY_DIR}/kcalendarcore_version.h"
  PACKAGE_VERSION_FILE "${KCalendarCore_BINARY_DIR}/KF5CalendarCoreConfigVersion.cmake"
  SOVERSION 5
)

########### Find packages ###########
find_package(Qt${QT_MAJOR_VERSION} ${REQUIRED_QT_VERSION} CONFIG REQUIRED Core Gui)

set(LibIcal_MIN_VERSION "3.0")
# TODO remove our FindLibIcal.cmake in favor of upstream LibIcalConfig.cmake once the following issues are resolved:
# https://github.com/libical/libical/pull/510
# https://bugs.launchpad.net/ubuntu/+source/libical3/+bug/1946963
# https://bugs.launchpad.net/ubuntu/+source/libical/+bug/1920587
find_package(LibIcal ${LibIcal_MIN_VERSION})
set_package_properties(LibIcal PROPERTIES TYPE REQUIRED)

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

ecm_set_disabled_deprecation_versions(
    QT 5.15.2
)

########### Targets ###########
add_subdirectory(src)

if(BUILD_TESTING)
  find_package(Qt${QT_MAJOR_VERSION} ${REQUIRED_QT_VERSION} CONFIG REQUIRED Test)
  add_subdirectory(autotests)
endif()

########### Install Files ###########
install(FILES
  "${CMAKE_CURRENT_BINARY_DIR}/KF5CalendarCoreConfig.cmake"
  "${CMAKE_CURRENT_BINARY_DIR}/KF5CalendarCoreConfigVersion.cmake"
  cmake/FindLibIcal.cmake
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
  DESTINATION ${KDE_INSTALL_INCLUDEDIR_KF}/KCalendarCore COMPONENT Devel
)
if (NOT KCALENDARCORE_NO_DEPRECATED_NAMESPACE)
  ecm_setup_version(PROJECT VARIABLE_PREFIX KCALCORE
    VERSION_HEADER "${KCalendarCore_BINARY_DIR}/kcalcore_version.h"
  )
  install(FILES
    ${CMAKE_CURRENT_BINARY_DIR}/kcalcore_version.h
    DESTINATION ${KDE_INSTALL_INCLUDEDIR_KF} COMPONENT Devel
  )
endif()

if (NOT WIN32)
    ecm_generate_pkgconfig_file(BASE_NAME KF5CalendarCore
      INCLUDE_INSTALL_DIR ${KDE_INSTALL_INCLUDEDIR_KF}/KCalendarCore/
      DEPS "Qt${QT_MAJOR_VERSION}Core Qt${QT_MAJOR_VERSION}Gui"
    INSTALL)
endif()

feature_summary(WHAT ALL FATAL_ON_MISSING_REQUIRED_PACKAGES)

kde_configure_git_pre_commit_hook(CHECKS CLANG_FORMAT)
