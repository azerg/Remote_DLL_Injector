file(GLOB MainProjInc "*.h")
file(GLOB MainProjSrc "*.cpp")

add_executable(TargetApp WIN32 ${MainProjSrc} ${MainProjInc} "TargetApp.rc")

target_precompile_headers(TargetApp PRIVATE stdafx.h)
set_property(TARGET TargetApp PROPERTY
  MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>")