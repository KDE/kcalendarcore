
########### next target ###############

set(kcalcore_LIB_SRCS
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
  utils.cpp
  vcalformat.cpp
  visitor.cpp
)
ecm_qt_declare_logging_category(kcalcore_LIB_SRCS HEADER kcalcore_debug.h IDENTIFIER KCALCORE_LOG CATEGORY_NAME org.kde.pim.kcalcore)

add_library(KF5CalendarCore ${kcalcore_LIB_SRCS})

generate_export_header(KF5CalendarCore BASE_NAME kcalcore)

add_library(KF5::CalendarCore ALIAS KF5CalendarCore)

# backward compatibility with the old name
# TODO: remove for KF6
target_compile_definitions(KF5CalendarCore INTERFACE "-DKCalCore=KCalendarCore")
target_include_directories(KF5CalendarCore INTERFACE "$<INSTALL_INTERFACE:${KDE_INSTALL_INCLUDEDIR_KF5}/KCalCore>")
target_include_directories(KF5CalendarCore PUBLIC "$<BUILD_INTERFACE:${KCalCore_SOURCE_DIR}/src;${KCalCore_BINARY_DIR}/src>")

target_link_libraries(KF5CalendarCore
PUBLIC
    Qt5::Core
PRIVATE
    Qt5::Gui
    LibIcal
)

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
  CustomProperties
  Duration
  Event
  Exceptions # NOTE: Used to be called 'Exception' in KDE4
  FileStorage
  FreeBusy
  FreeBusyCache
  FreeBusyPeriod
  ICalFormat
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
  DESTINATION ${KDE_INSTALL_INCLUDEDIR_KF5}/KCalCore/kcalcore
  COMPONENT Devel
)

install(FILES
  ${KCalCore_CamelCase_HEADERS}
  DESTINATION ${KDE_INSTALL_INCLUDEDIR_KF5}/KCalCore/KCalCore
  COMPONENT Devel
)

ecm_generate_pri_file(BASE_NAME KCalCore LIB_NAME KF5CalendarCore DEPS "Core" FILENAME_VAR PRI_FILENAME INCLUDE_INSTALL_DIR ${KDE_INSTALL_INCLUDEDIR_KF5}/KCalCore/KCalCore)
install(FILES ${PRI_FILENAME} DESTINATION ${ECM_MKSPECS_INSTALL_DIR})
