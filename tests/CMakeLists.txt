include(ECMMarkAsTest)

set(QT_REQUIRED_VERSION "5.2.0")
find_package(Qt5Test ${QT_REQUIRED_VERSION} CONFIG REQUIRED)


macro(macro_unit_tests)
  foreach(_testname ${ARGN})
    add_executable(${_testname} ${_testname}.cpp)
    add_test(${_testname} ${_testname})
    ecm_mark_as_test(${_testname})
    target_link_libraries(${_testname} KF5CalendarCore Qt5::Test)
  endforeach()
endmacro()

macro_unit_tests(
  incidencestest  readandwrite  testrecurprevious  testrecurrence  testrecurrencetype  testrecurson  testvcalexport
)
