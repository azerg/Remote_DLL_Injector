file(GLOB InjectorUIInc "*.h")
file(GLOB InjectorUISrc "*.cpp")

find_package(Boost 1.36.0 REQUIRED filesystem system)

# 1: Use MFC in a Static Library
# 2: For the shared MFC library
set(CMAKE_MFC_FLAG 2)

#string(REPLACE "/MDd" "/MTd" CMAKE_CXX_FLAGS_DEBUG ${CMAKE_CXX_FLAGS_DEBUG})
#string(REPLACE "/MD" "/MT" CMAKE_CXX_FLAGS_RELEASE ${CMAKE_CXX_FLAGS_RELEASE})

include_directories(${BOOST_ROOT} "${CMAKE_HOME_DIRECTORY}/StealthInjectionLib/inc")

add_executable(InjectorUI WIN32 ${InjectorUISrc} ${InjectorUIInc} "InjectorUI.rc")

if (MSVC)
   set_target_properties(InjectorUI PROPERTIES COMPILE_FLAGS "/Yustdafx.h")
   set_source_files_properties(stdafx.cpp PROPERTIES COMPILE_FLAGS "/Ycstdafx.h")
endif(MSVC)

target_link_libraries(InjectorUI StealthInjectionLib ${Boost_LIBRARIES})