cmake_minimum_required(VERSION 2.8.12)

project(KCalCore)

# ECM setup
find_package(ECM 0.0.11 CONFIG REQUIRED)
set(CMAKE_MODULE_PATH ${ECM_MODULE_PATH} ${KCalCore_SOURCE_DIR}/cmake)

include(ECMGenerateHeaders)
include(ECMGeneratePriFile)
include(ECMPackageConfigHelpers)
include(ECMSetupVersion)
include(FeatureSummary)
include(KDEInstallDirs)
include(KDECMakeSettings)
include(KDEFrameworkCompilerSettings)

set(KF5_VERSION "4.97.0")
ecm_setup_version(${KF5_VERSION} VARIABLE_PREFIX KCALCORE
                        VERSION_HEADER "${KCalCore_BINARY_DIR}/kcalcore_version.h"
                        PACKAGE_VERSION_FILE "${KCalCore_BINARY_DIR}/KF5KCalCoreConfigVersion.cmake"
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
find_package(KF5 ${KF5_VERSION} CONFIG REQUIRED COMPONENTS ItemViews KDE4Support)

set(LIBICAL_MIN_VERSION "0.42")
find_package(Libical)
set_package_properties(Libical PROPERTIES
                               DESCRIPTION "The libical library"
                               URL "http://sourceforge.net/projects/freeassociation"
                               TYPE REQUIRED
)

include(ConfigureChecks.cmake)

add_subdirectory(src)
#add_subdirectory(tests)

if ("${CMAKE_BINARY_DIR}" STREQUAL "${KCalCore_BINARY_DIR}")
  feature_summary(WHAT ALL FATAL_ON_MISSING_REQUIRED_PACKAGES)
endif()
