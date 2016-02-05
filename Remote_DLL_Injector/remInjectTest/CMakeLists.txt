file(GLOB MainProjInc "*.h")
file(GLOB MainProjSrc "*.cpp")

find_package(Boost 1.36.0 REQUIRED program_options filesystem system)

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
target_link_libraries(remInjectTest StealthInjectionLib ${Boost_LIBRARIES})