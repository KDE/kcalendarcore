add_library(KF5CalendarCore)
add_library(KF5::CalendarCore ALIAS KF5CalendarCore)

set_target_properties(KF5CalendarCore PROPERTIES
    VERSION     ${KCALENDARCORE_VERSION}
    SOVERSION   ${KCALENDARCORE_SOVERSION}
    EXPORT_NAME CalendarCore
)

target_sources(KF5CalendarCore PRIVATE
  alarm.cpp
  attachment.cpp
  attendee.cpp
  calendar.cpp
  calendarplugin.cpp
  calfilter.cpp
  calformat.cpp
  calstorage.cpp
  compat.cpp
  conference.cpp
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
ecm_qt_declare_logging_category(KF5CalendarCore
    HEADER kcalendarcore_debug.h
    IDENTIFIER KCALCORE_LOG
    CATEGORY_NAME kf.calendarcore
    OLD_CATEGORY_NAMES log_kcalcore org.kde.pim.kcalcore
    DESCRIPTION "KCalendarCore"
    EXPORT KCALENDARCORE
)

ecm_generate_export_header(KF5CalendarCore
    BASE_NAME kcalendarcore
    GROUP_BASE_NAME KF
    VERSION ${KF_VERSION}
    DEPRECATED_BASE_VERSION 0
    DEPRECATION_VERSIONS 5.64
    EXCLUDE_DEPRECATED_BEFORE_AND_AT ${EXCLUDE_DEPRECATED_BEFORE_AND_AT}
)


# backward compatibility with the old name
if (NOT KCALENDARCORE_NO_DEPRECATED_NAMESPACE)
    target_compile_definitions(KF5CalendarCore INTERFACE "-DKCalCore=KCalendarCore")
endif()

target_include_directories(KF5CalendarCore INTERFACE "$<INSTALL_INTERFACE:${KDE_INSTALL_INCLUDEDIR_KF5}/KCalendarCore>")
target_include_directories(KF5CalendarCore PUBLIC "$<BUILD_INTERFACE:${KCalendarCore_SOURCE_DIR}/src;${KCalendarCore_BINARY_DIR}/src>")

target_link_libraries(KF5CalendarCore
PUBLIC
    Qt5::Core
    Qt5::Gui
PRIVATE
    LibIcal
)

install(TARGETS KF5CalendarCore EXPORT KF5CalendarCoreTargets ${KF5_INSTALL_TARGETS_DEFAULT_ARGS})

########### Generate Headers ###############
set(kcalendarcore_headers
  Alarm
  Attachment
  Attendee
  CalFilter
  CalFormat
  CalStorage
  Calendar
  CalendarPlugin
  Conference
  CustomProperties
  Duration
  Event
  Exceptions
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
)

ecm_generate_headers(KCalendarCore_CamelCase_HEADERS
  HEADER_NAMES
    ${kcalendarcore_headers}
  PREFIX KCalendarCore
  REQUIRED_HEADERS KCalendarCore_HEADERS
)

if (NOT KCALENDARCORE_NO_DEPRECATED_NAMESPACE)
  ecm_generate_headers(KCalCore_CamelCase_HEADERS
    HEADER_NAMES
      ${kcalendarcore_headers}
    PREFIX KCalCore
    REQUIRED_HEADERS KCalCore_HEADERS
  )
endif()

########### install files ###############

install(FILES
  ${CMAKE_CURRENT_BINARY_DIR}/kcalendarcore_export.h
  ${KCalendarCore_HEADERS}
  DESTINATION ${KDE_INSTALL_INCLUDEDIR_KF5}/KCalendarCore/kcalendarcore
  COMPONENT Devel
)

install(FILES
  ${KCalendarCore_CamelCase_HEADERS}
  DESTINATION ${KDE_INSTALL_INCLUDEDIR_KF5}/KCalendarCore/KCalendarCore
  COMPONENT Devel
)

if (NOT KCALENDARCORE_NO_DEPRECATED_NAMESPACE)
  install(FILES
    ${CMAKE_CURRENT_BINARY_DIR}/kcalendarcore_export.h
    ${KCalCore_HEADERS}
    DESTINATION ${KDE_INSTALL_INCLUDEDIR_KF5}/KCalendarCore/kcalcore
    COMPONENT Devel
  )

  install(FILES
    ${KCalCore_CamelCase_HEADERS}
    DESTINATION ${KDE_INSTALL_INCLUDEDIR_KF5}/KCalendarCore/KCalCore
    COMPONENT Devel
  )
endif()

ecm_qt_install_logging_categories(
    EXPORT KCALENDARCORE
    FILE kcalendarcore.categories
    DESTINATION ${KDE_INSTALL_LOGGINGCATEGORIESDIR}
)

ecm_generate_pri_file(BASE_NAME KCalendarCore LIB_NAME KF5CalendarCore DEPS "Core" FILENAME_VAR PRI_FILENAME INCLUDE_INSTALL_DIR ${KDE_INSTALL_INCLUDEDIR_KF5}/KCalendarCore/KCalendarCore)
install(FILES ${PRI_FILENAME} DESTINATION ${ECM_MKSPECS_INSTALL_DIR})

if(BUILD_QCH)
    ecm_add_qch(
        KF5CalendarCore_QCH
        NAME KCalendarCore
        BASE_NAME KF5CalendarCore
        VERSION ${KCalendarCore_VERSION}
        ORG_DOMAIN org.kde
        SOURCES # using only public headers, to cover only public API
            ${KCalendarCore_HEADERS}
        MD_MAINPAGE "${CMAKE_SOURCE_DIR}/README.md"
        LINK_QCHS
            Qt5Core_QCH
        INCLUDE_DIRS
            ${CMAKE_CURRENT_BINARY_DIR}
        BLANK_MACROS
            KCALENDARCORE_EXPORT
            "KCALENDARCORE_DEPRECATED_VERSION(x, y, t)"
        TAGFILE_INSTALL_DESTINATION ${KDE_INSTALL_QTQCHDIR}
        QCH_INSTALL_DESTINATION ${KDE_INSTALL_QTQCHDIR}
        COMPONENT Devel
    )
endif()
