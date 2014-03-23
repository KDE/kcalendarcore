# FIXME: Make it build with this flag
remove_definitions(-DQT_NO_CAST_FROM_BYTEARRAY)

configure_file(${CMAKE_CURRENT_SOURCE_DIR}/config-kcalcore.h.cmake
    ${CMAKE_CURRENT_BINARY_DIR}/config-kcalcore.h
)

include_directories(
  ${LibIcal_INCLUDE_DIRS}
  ${LibIcal_INCLUDE_DIRS}/libical
  ${CMAKE_CURRENT_SOURCE_DIR}/versit
  ${CMAKE_CURRENT_BINARY_DIR}
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

target_include_directories(KF5CalendarCore INTERFACE "$<INSTALL_INTERFACE:${INCLUDE_INSTALL_DIR}/kcalcore>")

target_link_libraries(KF5CalendarCore
    KF5::ItemViews
    KF5::KDE4Support
    ${LibIcal_LIBRARIES}
)

if(HAVE_UUID_LIBRARY)
  target_link_libraries(KF5CalendarCore
      ${UUID_LIBRARY}
  )
endif()

set_target_properties(KF5CalendarCore PROPERTIES
    VERSION ${KCALCORE_VERSION_STRING}
    SOVERSION ${KCALCORE_SOVERSION}
    EXPORT_NAME kcalcore
)

install(TARGETS KF5CalendarCore EXPORT KF5CalendarCoreTargets ${INSTALL_TARGETS_DEFAULT_ARGS})

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

  REQUIRED_HEADERS KCalCore_HEADERS
  PREFIX KCalCore
)

########### install files ###############

install(FILES
    ${KCalCore_CamelCase_HEADERS}
    DESTINATION ${INCLUDE_INSTALL_DIR}/KCalCore
    COMPONENT Devel
)

install(FILES
  ${CMAKE_CURRENT_BINARY_DIR}/kcalcore_export.h
  ${KCalCore_HEADERS}
  DESTINATION ${INCLUDE_INSTALL_DIR}/kcalcore
  COMPONENT Devel
)

ecm_generate_pri_file(BASE_NAME KCalCore LIB_NAME KF5CalendarCore DEPS "ItemViews KDE4Support" FILENAME_VAR PRI_FILENAME)
install(FILES ${PRI_FILENAME} DESTINATION ${ECM_MKSPECS_INSTALL_DIR})