cmake_minimum_required(VERSION 3.23)

set (CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_HOME_DIRECTORY}/bin)
set (CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_HOME_DIRECTORY}/bin)

##################################################################################################################
project (Remote_DLL_Injector)

set(PELIB_ROOT ${CMAKE_HOME_DIRECTORY}/dependencies/pelib)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_subdirectory (stub2rsrc)
add_subdirectory (TargetApp)
add_subdirectory (SampleDll)
add_subdirectory (loader)
add_subdirectory (remInjectTest)
add_subdirectory (StealthInjectionLib)
add_subdirectory (InjectorUI)
add_subdirectory (internalTests)
