file(GLOB MainProjInc "*.h")
file(GLOB MainProjSrc "*.cpp")

# add stuff to use MFC in this executable
#ADD_DEFINITIONS(-D_AFXDLL)

# Use of CMAKE_MFC_FLAG 
# Values: 
# 0: Use Standard Windows Libraries 
# 1: Use MFC in a Static Library 
# 2: Use MFC in a Shared DLL 
SET(CMAKE_MFC_FLAG 0)

add_executable(TargetApp WIN32 ${MainProjSrc} ${MainProjInc})

if (MSVC)
   set_target_properties(TargetApp PROPERTIES COMPILE_FLAGS "/Yustdafx.h")
   set_source_files_properties(stdafx.cpp PROPERTIES COMPILE_FLAGS "/Ycstdafx.h")
endif(MSVC)