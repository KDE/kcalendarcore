project(kcalcore)

set(CMAKE_AUTOMOC ON)
set(CMAKE_INCLUDE_CURRENT_DIR ON)
include(GenerateExportHeader)

find_package(ECM 0.0.11 REQUIRED NO_MODULE)
ecm_setup_version(5.0.0 VARIABLE_PREFIX KCALCORE
                        VERSION_HEADER "${CMAKE_CURRENT_BINARY_DIR}/kcalcore_version.h"
                        PACKAGE_VERSION_FILE "${CMAKE_CURRENT_BINARY_DIR}/KF5KCalCoreConfigVersion.cmake")

find_package(KF5 NO_MODULE REQUIRED COMPONENTS ItemViews KDE4Support)
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

###########################################################

add_definitions(-DKDE_DEFAULT_DEBUG_AREA=5810)

include(ConfigureChecks.cmake)
configure_file(config-kcalcore.h.cmake ${CMAKE_CURRENT_BINARY_DIR}/config-kcalcore.h)

#add_definitions( -DQT_NO_CAST_FROM_ASCII )
#add_definitions( -DQT_NO_CAST_TO_ASCII )


include_directories(
  ${LIBICAL_INCLUDE_DIRS}
  ${LIBICAL_INCLUDE_DIRS}/libical
  ${CMAKE_CURRENT_SOURCE_DIR}/versit
  ${CMAKE_CURRENT_BINARY_DIR}
  ${KDE4_INCLUDE_DIR}
)

########### next target ###############

set(libversit_SRCS
  ${CMAKE_CURRENT_SOURCE_DIR}/versit/vcc.c
  ${CMAKE_CURRENT_SOURCE_DIR}/versit/vobject.c
)

set(kcalcore_LIB_SRCS ${libversit_SRCS}
  alarm.cpp
  attachment.cpp
  attendee.cpp
  calendar.cpp
  calfilter.cpp
  calformat.cpp
  calstorage.cpp
  compat.cpp
  customproperties.cpp
  duration.cpp
  event.cpp
  exceptions.cpp
  filestorage.cpp
  freebusy.cpp
  freebusycache.cpp
  freebusyurlstore.cpp
  freebusyperiod.cpp
  icalformat.cpp
  icalformat_p.cpp
  icaltimezones.cpp
  incidence.cpp
  incidencebase.cpp
  journal.cpp
  memorycalendar.cpp
  occurrenceiterator.cpp
  period.cpp
  person.cpp
  recurrence.cpp
  recurrencerule.cpp
  schedulemessage.cpp
  sorting.cpp
  todo.cpp
  vcalformat.cpp
  visitor.cpp
)

add_library(kcalcore ${LIBRARY_TYPE} ${kcalcore_LIB_SRCS})
generate_export_header(kcalcore)

target_link_libraries(kcalcore KF5::ItemViews KF5::KDE4Support ${LIBICAL_LIBRARIES})

if(HAVE_UUID_LIBRARY)
  target_link_libraries(kcalcore "${UUID_LIBRARY}")
endif()

set_target_properties(kcalcore PROPERTIES
  VERSION ${ECM_VERSION_STRING}
  SOVERSION ${ECM_SOVERSION}
)

install(TARGETS kcalcore EXPORT kdepimlibsLibraryTargets ${INSTALL_TARGETS_DEFAULT_ARGS})


########### next target ###############

#add_subdirectory(tests)

########### install files ###############

install(FILES
  ${CMAKE_CURRENT_BINARY_DIR}/kcalcore_export.h
  alarm.h
  attachment.h
  attendee.h
  calendar.h
  calfilter.h
  calformat.h
  calstorage.h
  compat.h
  customproperties.h
  duration.h
  event.h
  exceptions.h
  filestorage.h
  freebusy.h
  freebusycache.h
  freebusyperiod.h
  freebusyurlstore.h
  icalformat.h
  icaltimezones.h
  incidence.h
  incidencebase.h
  journal.h
  memorycalendar.h
  occurrenceiterator.h
  period.h
  person.h
  recurrence.h
  recurrencerule.h
  schedulemessage.h
  sortablelist.h
  sorting.h
  supertrait.h
  todo.h
  vcalformat.h
  visitor.h
  DESTINATION ${INCLUDE_INSTALL_DIR}/kcalcore COMPONENT Devel)
