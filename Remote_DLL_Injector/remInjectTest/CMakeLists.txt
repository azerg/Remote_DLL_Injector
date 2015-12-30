file(GLOB MainProjInc "*.h" "../StealthInjection/*.h")
file(GLOB MainProjSrc "*.cpp" "../StealthInjection/*.cpp")

include_directories("../StealthInjection/")

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