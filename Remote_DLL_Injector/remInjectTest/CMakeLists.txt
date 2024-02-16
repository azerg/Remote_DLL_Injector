file(GLOB MainProjInc "*.h")
file(GLOB MainProjSrc "*.cpp")

find_package(CLI11 CONFIG REQUIRED)

# LOADER_INCLUDES_DIR stores stub_data.h
include_directories("${CMAKE_HOME_DIRECTORY}/StealthInjectionLib/inc/" ${LOADER_INCLUDES_DIR} ${BOOST_ROOT})

#---------------------------------------------------------------------------------
set(COMMONS_DIR ${CMAKE_HOME_DIRECTORY}/StealthInjection)

set(StealthInjectionInc
	${COMMONS_DIR}/common_funcs.h
	${COMMONS_DIR}/pelib.h
	${COMMONS_DIR}/RESOURCE_LocalEmptyDll.h
	${COMMONS_DIR}/StealthInject.h
)
source_group("Header Files\\StealthInjection" FILES ${InterfacesInc})

set(StealthInjectionCpp
	${COMMONS_DIR}/StealthInject.cpp
	${COMMONS_DIR}/pelib.cpp
)
source_group("Source Files\\StealthInjection" FILES ${InterfacesInc})
#---------------------------------------------------------------------------------

add_executable(remInjectTest ${MainProjSrc} ${MainProjInc})

set_property(TARGET remInjectTest PROPERTY
  MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>")

target_link_libraries(
	remInjectTest
	StealthInjectionLib
	CLI11::CLI11)