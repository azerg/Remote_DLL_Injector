file(GLOB MainProjInc "*.h" "../StealthInjection/*.h")
file(GLOB MainProjSrc "*.cpp" "../StealthInjection/*.cpp")

find_package(Boost 1.36.0 REQUIRED filesystem system)

# LOADER_INCLUDES_DIR stores stub_data.h
include_directories("../StealthInjection/" ${LOADER_INCLUDES_DIR} ${BOOST_ROOT})

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
target_link_libraries(remInjectTest ${Boost_LIBRARIES})