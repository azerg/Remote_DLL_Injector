file(GLOB includes "inc/*.*")
file(GLOB sources "src/*.*")

include_directories(${BOOST_ROOT} "inc" "${CMAKE_HOME_DIRECTORY}/loader/inc")

ADD_LIBRARY(StealthInjectionLib ${sources} ${includes})
