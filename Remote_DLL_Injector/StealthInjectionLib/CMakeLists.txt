file(GLOB includes "inc/*.*")
file(GLOB sources "src/*.*")

include_directories("inc" "${CMAKE_HOME_DIRECTORY}/loader/inc")

ADD_LIBRARY(StealthInjectionLib ${sources} ${includes})

set_property(TARGET StealthInjectionLib PROPERTY
  MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>")