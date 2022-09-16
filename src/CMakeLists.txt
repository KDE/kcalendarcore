add_library(KF5CalendarCore)
add_library(KF5::CalendarCore ALIAS KF5CalendarCore)

set_target_properties(KF5CalendarCore PROPERTIES
    VERSION     ${KCALENDARCORE_VERSION}
    SOVERSION   ${KCALENDARCORE_SOVERSION}
    EXPORT_NAME CalendarCore
)

target_sources(KF5CalendarCore PRIVATE
    alarm.cpp
    alarm.h
    attachment.cpp
    attachment.h
    attendee.cpp
    attendee.h
    calendar.cpp
    calendar.h
    calendar_p.h
    calendarplugin.cpp
    calendarplugin.h
    calendarpluginloader.cpp
    calendarpluginloader.h
    calfilter.cpp
    calfilter.h
    calformat.cpp
    calformat.h
    calstorage.cpp
    calstorage.h
    compat.cpp
    compat_p.h
    conference.cpp
    conference.h
    customproperties.cpp
    customproperties.h
    duration.cpp
    duration.h
    event.cpp
    event.h
    exceptions.cpp
    exceptions.h
    filestorage.cpp
    filestorage.h
    freebusycache.cpp
    freebusycache.h
    freebusy.cpp
    freebusy.h
    freebusyperiod.cpp
    freebusyperiod.h
    icalformat.cpp
    icalformat.h
    icalformat_p.cpp
    icalformat_p.h
    icaltimezones.cpp
    icaltimezones_p.h
    incidencebase.cpp
    incidencebase.h
    incidencebase_p.h
    incidence.cpp
    incidence.h
    incidence_p.h
    journal.cpp
    journal.h
    memorycalendar.cpp
    memorycalendar.h
    occurrenceiterator.cpp
    occurrenceiterator.h
    period.cpp
    period.h
    person.cpp
    person.h
    person_p.h
    recurrence.cpp
    recurrence.h
    recurrencehelper_p.h
    recurrencerule.cpp
    recurrencerule.h
    schedulemessage.cpp
    schedulemessage.h
    sorting.cpp
    sorting.h
    todo.cpp
    todo.h
    utils.cpp
    utils_p.h
    vcalformat.cpp
    vcalformat.h
    visitor.cpp
    visitor.h

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
    DEPRECATION_VERSIONS 5.64 5.89 5.91 5.95 5.96 5.97
    EXCLUDE_DEPRECATED_BEFORE_AND_AT ${EXCLUDE_DEPRECATED_BEFORE_AND_AT}
)


# backward compatibility with the old name
if (NOT KCALENDARCORE_NO_DEPRECATED_NAMESPACE)
    target_compile_definitions(KF5CalendarCore INTERFACE "-DKCalCore=KCalendarCore")
endif()

target_include_directories(KF5CalendarCore INTERFACE "$<INSTALL_INTERFACE:${KDE_INSTALL_INCLUDEDIR_KF}/KCalendarCore>")
target_include_directories(KF5CalendarCore PUBLIC "$<BUILD_INTERFACE:${KCalendarCore_SOURCE_DIR}/src;${KCalendarCore_BINARY_DIR}/src>")

target_link_libraries(KF5CalendarCore
PUBLIC
    Qt${QT_MAJOR_VERSION}::Core
    Qt${QT_MAJOR_VERSION}::Gui
PRIVATE
    LibIcal
)

install(TARGETS KF5CalendarCore EXPORT KF5CalendarCoreTargets ${KF_INSTALL_TARGETS_DEFAULT_ARGS})

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
  CalendarPluginLoader
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
  DESTINATION ${KDE_INSTALL_INCLUDEDIR_KF}/KCalendarCore/kcalendarcore
  COMPONENT Devel
)

install(FILES
  ${KCalendarCore_CamelCase_HEADERS}
  DESTINATION ${KDE_INSTALL_INCLUDEDIR_KF}/KCalendarCore/KCalendarCore
  COMPONENT Devel
)

if (NOT KCALENDARCORE_NO_DEPRECATED_NAMESPACE)
  install(FILES
    ${CMAKE_CURRENT_BINARY_DIR}/kcalendarcore_export.h
    ${KCalCore_HEADERS}
    DESTINATION ${KDE_INSTALL_INCLUDEDIR_KF}/KCalendarCore/kcalcore
    COMPONENT Devel
  )

  install(FILES
    ${KCalCore_CamelCase_HEADERS}
    DESTINATION ${KDE_INSTALL_INCLUDEDIR_KF}/KCalendarCore/KCalCore
    COMPONENT Devel
  )
endif()

ecm_qt_install_logging_categories(
    EXPORT KCALENDARCORE
    FILE kcalendarcore.categories
    DESTINATION ${KDE_INSTALL_LOGGINGCATEGORIESDIR}
)

ecm_generate_pri_file(BASE_NAME KCalendarCore LIB_NAME KF5CalendarCore DEPS "Core" FILENAME_VAR PRI_FILENAME INCLUDE_INSTALL_DIR ${KDE_INSTALL_INCLUDEDIR_KF}/KCalendarCore/KCalendarCore)
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
