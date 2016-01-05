file(GLOB MainProjInc "*.h" "PELib/*.h")
file(GLOB MainProjSrc "*.cpp" "PELib/*.cpp")

include_directories("inc" ${common_inc})

add_definitions(-DUNICODE -D_UNICODE)
add_executable(stub2rsrc ${MainProjSrc} ${MainProjInc})