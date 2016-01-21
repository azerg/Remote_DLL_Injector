file(GLOB InjectorUIInc "*.h")
file(GLOB InjectorUISrc "*.cpp")

# 1: Use MFC in a Static Library
set(CMAKE_MFC_FLAG 1)

string(REPLACE "/MDd" "/MT" CMAKE_CXX_FLAGS_DEBUG ${CMAKE_CXX_FLAGS_DEBUG})
string(REPLACE "/MD" "/MT" CMAKE_CXX_FLAGS_RELEASE ${CMAKE_CXX_FLAGS_RELEASE})

add_executable(InjectorUI WIN32 ${InjectorUISrc} ${InjectorUIInc} "InjectorUI.rc")

if (MSVC)
   set_target_properties(InjectorUI PROPERTIES COMPILE_FLAGS "/Yustdafx.h")
   set_source_files_properties(stdafx.cpp PROPERTIES COMPILE_FLAGS "/Ycstdafx.h")
endif(MSVC)