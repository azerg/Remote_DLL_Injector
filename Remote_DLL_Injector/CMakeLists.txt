cmake_minimum_required(VERSION 2.8)

# leaving only Release& Debug configurations
IF(NOT CMAKE_BUILD_TYPE)
  SET(CMAKE_BUILD_TYPE Debug Release)
   set(CMAKE_CONFIGURATION_TYPES "${CMAKE_CONFIGURATION_TYPES}" CACHE STRING
     "Reset the configurations to what we need"
     FORCE)  
ENDIF(NOT CMAKE_BUILD_TYPE)

set (CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_HOME_DIRECTORY}/bin)
set (BOOST_FOLDER ${BOOST_ROOT})

##################################################################################################################
project (Remote_DLL_Injector)

if(CMAKE_COMPILER_IS_GNUCXX)
  set(CMAKE_CXX_FLAGS "-std=c++0x ${CMAKE_CXX_FLAGS} -g -ftest-coverage -fprofile-arcs")
endif()

add_subdirectory (remInjectTest)
add_subdirectory (SampleDll)
add_subdirectory (TargetApp)
