file(GLOB InjectorUIInc "inc/*.h")
file(GLOB InjectorUISrc "src/*.cpp")

find_package(PkgConfig REQUIRED)
pkg_check_modules(tomlplusplus REQUIRED IMPORTED_TARGET tomlplusplus)

include_directories("inc" "${CMAKE_HOME_DIRECTORY}/StealthInjectionLib/inc")

add_executable(InjectorUI WIN32 ${InjectorUISrc} ${InjectorUIInc} "InjectorUI.rc")

target_precompile_headers(InjectorUI PRIVATE inc/stdafx.h)
set_property(TARGET InjectorUI PROPERTY
  MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>")

target_link_libraries(InjectorUI StealthInjectionLib PkgConfig::tomlplusplus)