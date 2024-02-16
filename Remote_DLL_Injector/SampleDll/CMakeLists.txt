file(GLOB MainProjInc "*.h")
file(GLOB MainProjSrc "*.cpp")

add_library(SampleDll MODULE ${MainProjSrc} ${MainProjInc})

set_property(TARGET SampleDll PROPERTY
  MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>")