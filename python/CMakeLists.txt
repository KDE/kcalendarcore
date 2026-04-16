# SPDX-FileCopyrightText: 2026 Kevin Ottens <kevin.ottens@enioka.com>
# SPDX-License-Identifier: BSD-2-Clause

set(bindings_library "KCalendarCore")

set(wrapped_header ${CMAKE_CURRENT_SOURCE_DIR}/bindings.h)
set(typesystem_file ${CMAKE_CURRENT_SOURCE_DIR}/typesystem_kcalendarcore.xml)

set(generated_sources
    ${CMAKE_CURRENT_BINARY_DIR}/KCalendarCore/kcalendarcore_module_wrapper.cpp
    ${CMAKE_CURRENT_BINARY_DIR}/KCalendarCore/kcalendarcore_wrapper.cpp
    ${CMAKE_CURRENT_BINARY_DIR}/KCalendarCore/kcalendarcore_alarm_wrapper.cpp
    ${CMAKE_CURRENT_BINARY_DIR}/KCalendarCore/kcalendarcore_attachment_wrapper.cpp
    ${CMAKE_CURRENT_BINARY_DIR}/KCalendarCore/kcalendarcore_attendee_wrapper.cpp
    ${CMAKE_CURRENT_BINARY_DIR}/KCalendarCore/kcalendarcore_calendarlistmodel_wrapper.cpp
    ${CMAKE_CURRENT_BINARY_DIR}/KCalendarCore/kcalendarcore_calendarpluginloader_wrapper.cpp
    ${CMAKE_CURRENT_BINARY_DIR}/KCalendarCore/kcalendarcore_calendarplugin_wrapper.cpp
    ${CMAKE_CURRENT_BINARY_DIR}/KCalendarCore/kcalendarcore_calendar_wrapper.cpp
    ${CMAKE_CURRENT_BINARY_DIR}/KCalendarCore/kcalendarcore_calfilter_wrapper.cpp
    ${CMAKE_CURRENT_BINARY_DIR}/KCalendarCore/kcalendarcore_calformat_wrapper.cpp
    ${CMAKE_CURRENT_BINARY_DIR}/KCalendarCore/kcalendarcore_calstorage_wrapper.cpp
    ${CMAKE_CURRENT_BINARY_DIR}/KCalendarCore/kcalendarcore_conference_wrapper.cpp
    ${CMAKE_CURRENT_BINARY_DIR}/KCalendarCore/kcalendarcore_customproperties_wrapper.cpp
    ${CMAKE_CURRENT_BINARY_DIR}/KCalendarCore/kcalendarcore_duration_wrapper.cpp
    ${CMAKE_CURRENT_BINARY_DIR}/KCalendarCore/kcalendarcore_events_wrapper.cpp
    ${CMAKE_CURRENT_BINARY_DIR}/KCalendarCore/kcalendarcore_event_wrapper.cpp
    ${CMAKE_CURRENT_BINARY_DIR}/KCalendarCore/kcalendarcore_exception_wrapper.cpp
    ${CMAKE_CURRENT_BINARY_DIR}/KCalendarCore/kcalendarcore_filestorage_wrapper.cpp
    ${CMAKE_CURRENT_BINARY_DIR}/KCalendarCore/kcalendarcore_freebusycache_wrapper.cpp
    ${CMAKE_CURRENT_BINARY_DIR}/KCalendarCore/kcalendarcore_freebusyperiod_wrapper.cpp
    ${CMAKE_CURRENT_BINARY_DIR}/KCalendarCore/kcalendarcore_freebusy_wrapper.cpp
    ${CMAKE_CURRENT_BINARY_DIR}/KCalendarCore/kcalendarcore_icalformat_wrapper.cpp
    ${CMAKE_CURRENT_BINARY_DIR}/KCalendarCore/kcalendarcore_incidencebase_incidenceobserver_wrapper.cpp
    ${CMAKE_CURRENT_BINARY_DIR}/KCalendarCore/kcalendarcore_incidencebase_wrapper.cpp
    ${CMAKE_CURRENT_BINARY_DIR}/KCalendarCore/kcalendarcore_incidences_wrapper.cpp
    ${CMAKE_CURRENT_BINARY_DIR}/KCalendarCore/kcalendarcore_incidence_wrapper.cpp
    ${CMAKE_CURRENT_BINARY_DIR}/KCalendarCore/kcalendarcore_journals_wrapper.cpp
    ${CMAKE_CURRENT_BINARY_DIR}/KCalendarCore/kcalendarcore_journal_wrapper.cpp
    ${CMAKE_CURRENT_BINARY_DIR}/KCalendarCore/kcalendarcore_memorycalendar_wrapper.cpp
    ${CMAKE_CURRENT_BINARY_DIR}/KCalendarCore/kcalendarcore_occurrenceiterator_wrapper.cpp
    ${CMAKE_CURRENT_BINARY_DIR}/KCalendarCore/kcalendarcore_period_wrapper.cpp
    ${CMAKE_CURRENT_BINARY_DIR}/KCalendarCore/kcalendarcore_person_wrapper.cpp
    ${CMAKE_CURRENT_BINARY_DIR}/KCalendarCore/kcalendarcore_recurrence_recurrenceobserver_wrapper.cpp
    ${CMAKE_CURRENT_BINARY_DIR}/KCalendarCore/kcalendarcore_recurrencerule_ruleobserver_wrapper.cpp
    ${CMAKE_CURRENT_BINARY_DIR}/KCalendarCore/kcalendarcore_recurrencerule_wrapper.cpp
    ${CMAKE_CURRENT_BINARY_DIR}/KCalendarCore/kcalendarcore_recurrence_wrapper.cpp
    ${CMAKE_CURRENT_BINARY_DIR}/KCalendarCore/kcalendarcore_schedulemessage_wrapper.cpp
    ${CMAKE_CURRENT_BINARY_DIR}/KCalendarCore/kcalendarcore_todos_wrapper.cpp
    ${CMAKE_CURRENT_BINARY_DIR}/KCalendarCore/kcalendarcore_todo_wrapper.cpp
    ${CMAKE_CURRENT_BINARY_DIR}/KCalendarCore/kcalendarcore_vcalformat_wrapper.cpp
    ${CMAKE_CURRENT_BINARY_DIR}/KCalendarCore/kcalendarcore_visitor_wrapper.cpp
)

ecm_generate_python_bindings(
    PACKAGE_NAME ${bindings_library}
    VERSION ${KF_VERSION}
    WRAPPED_HEADER ${wrapped_header}
    TYPESYSTEM ${typesystem_file}
    EXPORT_TYPESYSTEM
    GENERATED_SOURCES ${generated_sources}
    DEPENDENCIES KF6::CalendarCore
    QT_VERSION ${REQUIRED_QT_VERSION}
    HOMEPAGE_URL "https://invent.kde.org/frameworks/kcalendarcore"
    ISSUES_URL "https://bugs.kde.org/describecomponents.cgi?product=frameworks-kcalendarcore"
    AUTHOR "The KDE Community"
    README ../README.md
)

target_link_libraries(${bindings_library} PRIVATE KF6CalendarCore)

execute_process(
    COMMAND
        ${Python_EXECUTABLE} -Esc
        "import sysconfig; print(sysconfig.get_path('platlib', vars={'platbase': '${CMAKE_INSTALL_PREFIX}', 'base': '${CMAKE_INSTALL_PREFIX}'}))"
    OUTPUT_VARIABLE sysconfig_output
)
string(STRIP ${sysconfig_output} PYTHON_INSTALL_DIR)

install(TARGETS ${bindings_library} LIBRARY DESTINATION "${PYTHON_INSTALL_DIR}")
