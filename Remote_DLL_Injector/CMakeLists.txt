cmake_minimum_required(VERSION 2.8)

set (CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_HOME_DIRECTORY}/bin)
set (CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_HOME_DIRECTORY}/bin)

set (BOOST_FOLDER ${BOOST_ROOT})

##################################################################################################################
project (Remote_DLL_Injector)

set(PELIB_ROOT ${CMAKE_HOME_DIRECTORY}/dependencies/pelib)

if(CMAKE_COMPILER_IS_GNUCXX)
  set(CMAKE_CXX_FLAGS "-std=c++0x ${CMAKE_CXX_FLAGS} -g -ftest-coverage -fprofile-arcs")
endif()

add_subdirectory (stub2rsrc)
add_subdirectory (TargetApp)
add_subdirectory (SampleDll)
add_subdirectory (loader)
add_subdirectory (remInjectTest)
add_subdirectory (StealthInjectionLib)
add_subdirectory (InjectorUI)
