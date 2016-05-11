REM rd /s /q build
cmake . -B"build" -G"Visual Studio 14" -Wno-dev -DBOOST_ROOT="E:\common_libs\boost_1_60_0" -DBOOST_LIBRARYDIR="E:\common_libs\boost_1_60_0\stage\lib" -DBoost_USE_STATIC_LIBS=ON