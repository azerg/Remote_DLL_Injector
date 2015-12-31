file(GLOB MainProjInc "*.h" "PELib/*.h")
file(GLOB MainProjSrc "*.cpp" "PELib/*.cpp")

include_directories("inc" ${common_inc})

add_definitions(-DUNICODE -D_UNICODE)
add_executable(stub2rsrc ${MainProjSrc} ${MainProjInc})

#disable post build event for appveyor builds
if (NOT DEFINED ENV{APPVEYOR})
	# POST-BUILD event that creates binary stub from compiled loader
	add_custom_command(TARGET stub2rsrc
					   POST_BUILD
					   COMMAND $(TargetPath) ARGS "$(OutDir)/${LOADER_X86_PROJECT_NAME}.exe" "$(OutDir)/${LOADER_X86_PROJECT_NAME}.stub "
					   COMMENT "Extracting loader stub from ${LOADER_X86_PROJECT_NAME}")
endif()