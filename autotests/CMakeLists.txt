include(ECMMarkAsTest)

set(QT_REQUIRED_VERSION "5.2.0")
find_package(Qt5Test ${QT_REQUIRED_VERSION} CONFIG REQUIRED)

# needed by loadcalendar
file(COPY ${CMAKE_CURRENT_SOURCE_DIR}/cal DESTINATION ${CMAKE_CURRENT_BINARY_DIR})

macro(macro_unit_tests)
  foreach(_testname ${ARGN})
    add_executable(${_testname} ${_testname}.cpp)
    add_test(${_testname} ${_testname})
    ecm_mark_as_test(${_testname})
    target_link_libraries(${_testname} KF5CalendarCore Qt5::Test)
  endforeach()
endmacro()

macro_unit_tests(
  testalarm
  testattachment
  testattendee
  testcalfilter
  testcustomproperties
  testduration
  testevent
  testexception
  testfilestorage
  testfreebusy
  testincidencerelation
  testicalformat
  testjournal
  testmemorycalendar
  testperiod
  testfreebusyperiod
  testperson
  testrecurtodo
  testsortablelist
  testtodo
  testtimesininterval
  testcreateddatecompat
  testrecurrenceexception
  testoccurrenceiterator
  testreadrecurrenceid
  incidencestest
  loadcalendar
  fbrecurring
  readandwrite
  testfb
  testrecurprevious
  testrecurrence
  testrecurrencetype
  testrecurson
  testtostring
  testvcalexport

)

set_target_properties(testmemorycalendar PROPERTIES COMPILE_FLAGS -DICALTESTDATADIR="\\"${CMAKE_CURRENT_SOURCE_DIR}/data/\\"" )
set_target_properties(testreadrecurrenceid PROPERTIES COMPILE_FLAGS -DICALTESTDATADIR="\\"${CMAKE_CURRENT_SOURCE_DIR}/data/\\"" )
# this test cannot work with msvc because libical should not be altered
# and therefore we can't add KCALCORE_EXPORT there
# it should work fine with mingw because of the auto-import feature
# KDAB_TODO: check if this can build with msvc now (and if it works on Windows)
if(NOT MSVC)
  macro_unit_tests(testicaltimezones)
endif()

########### Tests #######################

file(GLOB_RECURSE testFiles data/RecurrenceRule/*.ics)
file(GLOB_RECURSE compatFiles data/Compat/*.ics)
file(GLOB_RECURSE vCalFilesAsIcal data/vCalendar/*.ics)
file(GLOB_RECURSE vCalFiles data/vCalendar/*.vcs)

if(WIN32)
  find_program(PERL_EXECUTABLE perl)
endif()

macro(kcalcore_run_single_test _prefix _f _test _testarg)
  get_filename_component(_fn ${_f} NAME)

  # Write the output file in the build directory.
  string(REGEX REPLACE "${CMAKE_CURRENT_SOURCE_DIR}" "${CMAKE_CURRENT_BINARY_DIR}" _outFile ${_f})
  get_filename_component(_outputPath ${_outFile} PATH)
  file(MAKE_DIRECTORY ${_outputPath})

  set(_runner ${CMAKE_CURRENT_SOURCE_DIR}/runsingletestcase.pl)
  get_property(_output_dir TARGET ${_test} PROPERTY RUNTIME_OUTPUT_DIRECTORY)
  if(NOT _output_dir)
    set(_output_dir ${CMAKE_CURRENT_BINARY_DIR})
  endif()
  set(_test_path ${_output_dir}/${_test})
  if(WIN32)
    set(_runner ${PERL_EXECUTABLE} ${_runner})
  endif()

  add_test(${_prefix}-${_fn} ${_runner} ${_test_path} ${_testarg} ${_f} ${_outFile})
endmacro()

file(REMOVE ${CMAKE_CURRENT_BINARY_DIR}/FAILED.log)

foreach(file ${testFiles})
  kcalcore_run_single_test(RecurNext ${file} testrecurrence "next")
endforeach()

foreach(file ${testFiles})
  kcalcore_run_single_test(RecurPrev ${file} testrecurprevious "prev")
endforeach()

foreach(file ${testFiles})
  kcalcore_run_single_test(RecursOn ${file} testrecurson "recurson")
endforeach()

foreach(file ${compatFiles})
  kcalcore_run_single_test(Compat ${file} readandwrite "ical")
endforeach()

foreach(file ${vCalFilesAsIcal})
  kcalcore_run_single_test(VCalOut ${file} testvcalexport "vcal")
endforeach()

foreach(file ${vCalFiles})
  kcalcore_run_single_test(VCalIn ${file} readandwrite "ical")
endforeach()
