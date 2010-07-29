set( EXECUTABLE_OUTPUT_PATH ${CMAKE_CURRENT_BINARY_DIR} )

include_directories(
 ${LIBICAL_INCLUDE_DIRS}
)

add_definitions( -DICALTESTDATADIR="\\"${CMAKE_SOURCE_DIR}/tests/data/\\"" )

MACRO(KCALCORE_UNIT_TESTS)
  FOREACH(_testname ${ARGN})
    kde4_add_unit_test(${_testname} NOGUI ${_testname}.cpp)
    target_link_libraries(${_testname} kcalcore ${QT_QTTEST_LIBRARY} ${QT_QTGUI_LIBRARY} ${LIBICAL_LIBRARIES} ${KDE4_KDECORE_LIBS})
  ENDFOREACH(_testname)
ENDMACRO(KCALCORE_UNIT_TESTS)

MACRO(KCALCORE_EXECUTABLE_TESTS)
  FOREACH(_testname ${ARGN})
    kde4_add_executable(${_testname} NOGUI TEST ${_testname}.cpp)
    target_link_libraries(${_testname} kcalcore ${QT_QTTEST_LIBRARY} ${QT_QTCORE_LIBRARY} ${KDE4_KDECORE_LIBS})
  ENDFOREACH(_testname)
ENDMACRO(KCALCORE_EXECUTABLE_TESTS)

KCALCORE_UNIT_TESTS(
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
  testjournal
  testmemorycalendar
  testperiod
  testfreebusyperiod
  testperson
  testsortablelist
  testtodo
  testtimesininterval
)
# this test cannot work with msvc because libical should not be altered
# and therefore we can't add KCALCORE_EXPORT there
# it should work fine with mingw because of the auto-import feature
# KDAB_TODO: check if this can build with msvc now (and if it works on Windows)
if(NOT MSVC)
  KCALCORE_UNIT_TESTS(testicaltimezones)
endif(NOT MSVC)

KCALCORE_EXECUTABLE_TESTS(
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

########### Tests #######################

FILE( GLOB_RECURSE testFiles       data/RecurrenceRule/*.ics )
FILE( GLOB_RECURSE compatFiles     data/Compat/*.ics )
FILE( GLOB_RECURSE vCalFilesAsIcal data/vCalendar/*.ics )
FILE( GLOB_RECURSE vCalFiles       data/vCalendar/*.vcs )

if (WIN32)
  find_program(PERL_EXECUTABLE perl)
endif (WIN32)

macro(kcalcore_run_single_test _prefix _file _test _testarg)
  get_filename_component( _fn ${_file} NAME)
  set(_runner ${CMAKE_CURRENT_SOURCE_DIR}/runsingletestcase.pl)
  if (UNIX)
    set(_test_path ${EXECUTABLE_OUTPUT_PATH}/${_test}.shell)
  endif (UNIX)
  if (WIN32)
    set(_test_path ${EXECUTABLE_OUTPUT_PATH}/${_test}.exe.bat)
    set(_runner ${PERL_EXECUTABLE} ${_runner}) 
  endif (WIN32)

  add_test( ${_prefix}-${_fn} ${_runner} ${_test_path} ${_testarg} ${_file} )
endmacro(kcalcore_run_single_test)

FILE(REMOVE ${CMAKE_CURRENT_BINARY_DIR}/FAILED.log)

FOREACH( file ${testFiles} )
  kcalcore_run_single_test( RecurNext ${file} testrecurrence "next" )
ENDFOREACH(file)

FOREACH( file ${testFiles} )
  kcalcore_run_single_test( RecurPrev ${file} testrecurprevious "prev" )
ENDFOREACH(file)

FOREACH( file ${testFiles} )
  kcalcore_run_single_test( RecursOn ${file} testrecurson "recurson" )
ENDFOREACH(file)

FOREACH( file ${compatFiles} )
  kcalcore_run_single_test( Compat ${file} readandwrite "ical" )
ENDFOREACH(file)

FOREACH( file ${vCalFilesAsIcal} )
  kcalcore_run_single_test( VCalOut ${file} testvcalexport "vcal" )
ENDFOREACH(file)

FOREACH( file ${vCalFiles} )
  kcalcore_run_single_test( VCalIn ${file} readandwrite "ical" )
ENDFOREACH(file)

