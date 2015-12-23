file(GLOB MainProjInc "*.h")
file(GLOB MainProjSrc "*.cpp")

add_executable(TargetApp WIN32 ${MainProjSrc} ${MainProjInc} "TargetApp.rc")

if (MSVC)
   set_target_properties(TargetApp PROPERTIES COMPILE_FLAGS "/Yustdafx.h")
   set_source_files_properties(stdafx.cpp PROPERTIES COMPILE_FLAGS "/Ycstdafx.h")
endif(MSVC)