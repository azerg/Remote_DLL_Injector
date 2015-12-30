file(GLOB MainProjInc "*.h")
file(GLOB MainProjSrc "*.cpp")

set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} /MT")
set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} /MTd")

add_library(SampleDll MODULE ${MainProjSrc} ${MainProjInc})