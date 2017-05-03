cmake_minimum_required(VERSION 3.0)
set(PIM_VERSION "5.5.40")

project(KCalCore VERSION ${PIM_VERSION})

# ECM setup
set(KF5_VERSION "5.32.0")
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


set(QT_REQUIRED_VERSION "5.7.0") 
find_package(Qt5 ${QT_REQUIRED_VERSION} CONFIG REQUIRED Test)

set(KCALENDARCORE_LIB_VERSION ${PIM_VERSION})
ecm_setup_version(PROJECT VARIABLE_PREFIX KCALCORE
                        VERSION_HEADER "${KCalCore_BINARY_DIR}/kcalcore_version.h"
                        PACKAGE_VERSION_FILE "${KCalCore_BINARY_DIR}/KF5CalendarCoreConfigVersion.cmake"
                        SOVERSION 5
)

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
set(CMAKECONFIG_INSTALL_DIR "${KDE_INSTALL_CMAKEPACKAGEDIR}/KF5CalendarCore")

configure_package_config_file(
  "${CMAKE_CURRENT_SOURCE_DIR}/KF5CalendarCoreConfig.cmake.in"
  "${CMAKE_CURRENT_BINARY_DIR}/KF5CalendarCoreConfig.cmake"
  INSTALL_DESTINATION  ${CMAKECONFIG_INSTALL_DIR}
)

add_definitions(-DQT_NO_CAST_FROM_ASCII -DQT_NO_CAST_TO_ASCII)
add_definitions( -DQT_NO_NARROWING_CONVERSIONS_IN_CONNECT )
#add_definitions( -DQT_DISABLE_DEPRECATED_BEFORE=0x060000 )
########### Targets ###########
add_subdirectory(src)

if(BUILD_TESTING)
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
feature_summary(WHAT ALL FATAL_ON_MISSING_REQUIRED_PACKAGES)
