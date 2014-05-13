# FIXME: Make it build with this flag
remove_definitions(-DQT_NO_CAST_FROM_BYTEARRAY)

configure_file(${CMAKE_CURRENT_SOURCE_DIR}/config-kcalcore.h.cmake
    ${CMAKE_CURRENT_BINARY_DIR}/config-kcalcore.h
)

########### next target ###############

set(libversit_SRCS
  ${CMAKE_CURRENT_SOURCE_DIR}/versit/vcc.c
  ${CMAKE_CURRENT_SOURCE_DIR}/versit/vobject.c
)

set(kcalcore_LIB_SRCS
  ${libversit_SRCS}
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

add_library(KF5CalendarCore ${kcalcore_LIB_SRCS})

generate_export_header(KF5CalendarCore BASE_NAME kcalcore)

add_library(KF5::CalendarCore ALIAS KF5CalendarCore)

target_include_directories(KF5CalendarCore INTERFACE "$<INSTALL_INTERFACE:${KF5_INCLUDE_INSTALL_DIR}/KCalCore;${LibIcal_INCLUDE_DIRS};${LibIcal_INCLUDE_DIRS}/libical>")
target_include_directories(KF5CalendarCore PUBLIC "$<BUILD_INTERFACE:${KCalCore_SOURCE_DIR}/src;${KCalCore_BINARY_DIR}/src;${KCalCore_SOURCE_DIR}/src/versit>")
target_include_directories(KF5CalendarCore PUBLIC "$<BUILD_INTERFACE:${LibIcal_INCLUDE_DIRS};${LibIcal_INCLUDE_DIRS}/libical>")

target_link_libraries(KF5CalendarCore
PUBLIC
    KF5::KDELibs4Support
    ${LibIcal_LIBRARIES}
)

if(HAVE_UUID_LIBRARY)
  target_link_libraries(KF5CalendarCore
PRIVATE
      ${UUID_LIBRARY}
  )
endif()

set_target_properties(KF5CalendarCore PROPERTIES
    VERSION ${KCALCORE_VERSION_STRING}
    SOVERSION ${KCALCORE_SOVERSION}
    EXPORT_NAME CalendarCore
)

install(TARGETS KF5CalendarCore EXPORT KF5CalendarCoreTargets ${KF5_INSTALL_TARGETS_DEFAULT_ARGS})

########### Generate Headers ###############
ecm_generate_headers(KCalCore_CamelCase_HEADERS
  HEADER_NAMES
  Alarm
  Attachment
  Attendee
  CalFilter
  CalFormat
  CalStorage
  Calendar
  Compat
  CustomProperties
  Duration
  Event
  Exceptions # NOTE: Used to be called 'Exception' in KDE4
  FileStorage
  FreeBusy
  FreeBusyCache
  FreeBusyPeriod
  FreeBusyUrlStore
  ICalFormat
  ICalTimeZones
  Incidence
  IncidenceBase
  Journal
  MemoryCalendar
  OccurrenceIterator
  Period
  Person
  Recurrence
  RecurrenceRule
  ScheduleMessage
  SortableList
  Sorting
  Todo
  VCalFormat
  Visitor

  PREFIX KCalCore
  REQUIRED_HEADERS KCalCore_HEADERS
)

########### install files ###############

install(FILES
  ${CMAKE_CURRENT_BINARY_DIR}/kcalcore_export.h
  ${KCalCore_HEADERS}
  supertrait.h
  DESTINATION ${KF5_INCLUDE_INSTALL_DIR}/KCalCore/kcalcore
  COMPONENT Devel
)

install(FILES
  ${KCalCore_CamelCase_HEADERS}
  DESTINATION ${KF5_INCLUDE_INSTALL_DIR}/KCalCore/KCalCore
  COMPONENT Devel
)

ecm_generate_pri_file(BASE_NAME KCalCore LIB_NAME KF5CalendarCore DEPS "ItemViews KDELibs4Support" FILENAME_VAR PRI_FILENAME INCLUDE_INSTALL_DIR ${KF5_INCLUDE_INSTALL_DIR}/KCalCore/KCalCore)
install(FILES ${PRI_FILENAME} DESTINATION ${ECM_MKSPECS_INSTALL_DIR})
