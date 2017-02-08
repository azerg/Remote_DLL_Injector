set (LOADER_X86_PROJECT_NAME "loader_x86" CACHE TYPE STRING)
set (LOADER_INCLUDES_DIR ${CMAKE_CURRENT_LIST_DIR}/inc CACHE TYPE STRING)

file(GLOB MainProjInc "inc/*.*")
file(GLOB MainProjSrc "src/*.*")

include_directories("inc")

add_executable(${LOADER_X86_PROJECT_NAME} ${MainProjSrc} ${MainProjInc})

set_target_properties(${LOADER_X86_PROJECT_NAME} PROPERTIES FOLDER Loaders)
set_target_properties(${LOADER_X86_PROJECT_NAME} PROPERTIES LINK_FLAGS
"/ENTRY:StubEP /DYNAMICBASE:NO /NXCOMPAT:NO /NODEFAULTLIB /SAFESEH:NO /INCREMENTAL:NO /MAP")
# enable all optimizations + remove all debug stuff ( just dont know how to switch off Debug build for single prj in cmake )
string(REPLACE "/D_DEBUG" "" CMAKE_CXX_FLAGS_DEBUG ${CMAKE_CXX_FLAGS_DEBUG})
string(REPLACE "/MDd" "/MT" CMAKE_CXX_FLAGS_DEBUG ${CMAKE_CXX_FLAGS_DEBUG})  # no additional libs
string(REPLACE "/MD" "/MT" CMAKE_CXX_FLAGS_RELEASE ${CMAKE_CXX_FLAGS_RELEASE})  # for release too ;)
string(REPLACE "/Zi" "" CMAKE_CXX_FLAGS_DEBUG ${CMAKE_CXX_FLAGS_DEBUG})      # no debug info
string(REPLACE "/RTC1" "" CMAKE_CXX_FLAGS_DEBUG ${CMAKE_CXX_FLAGS_DEBUG})    # no runtime checks
string(REPLACE "/Ob0" "/Ob2" CMAKE_CXX_FLAGS_DEBUG ${CMAKE_CXX_FLAGS_DEBUG}) # inline as much as possible
string(REPLACE "/Od" "/Ox" CMAKE_CXX_FLAGS_DEBUG ${CMAKE_CXX_FLAGS_DEBUG})   # full optimization instead of none
set_target_properties(${LOADER_X86_PROJECT_NAME} PROPERTIES COMPILE_FLAGS "/EHa /GR-")

add_definitions(-DNDEBUG)  # no debug flag
string(REPLACE "/debug" "" CMAKE_EXE_LINKER_FLAGS_DEBUG ${CMAKE_EXE_LINKER_FLAGS_DEBUG})        # removing debug info from Debug build!!
string(REPLACE "/debug" "" CMAKE_SHARED_LINKER_FLAGS_DEBUG ${CMAKE_SHARED_LINKER_FLAGS_DEBUG})  # removing debug info from Debug build!!

add_dependencies(${LOADER_X86_PROJECT_NAME} stub2rsrc) # stub2rsrc built required
target_link_libraries(${LOADER_X86_PROJECT_NAME})

#disable post build event for appveyor builds
if (NOT DEFINED ENV{APPVEYOR})
	# When loader is built, launch stub2rsrc to build stub_xx.bin from compiled loader executable
	add_custom_command(TARGET ${LOADER_X86_PROJECT_NAME}
					   POST_BUILD
					   COMMAND $<TARGET_FILE:stub2rsrc> ARGS "$<TARGET_FILE:loader_x86>" "$<TARGET_FILE_DIR:loader_x86>/loader_x86.stub"
					   WORKING_DIRECTORY ${CMAKE_BINARY_DIR}
					   COMMENT "Extracting raw loader code from compiled loader executable")
endif()		   