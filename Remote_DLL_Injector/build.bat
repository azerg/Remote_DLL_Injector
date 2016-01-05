REM rd /s /q build
cmake . -B"build" -G"Visual Studio 14" -Wno-dev -DBOOST_ROOT="F:\Work_Installs\boost_1_58_0" -DBOOST_LIBRARYDIR="F:\Work_Installs\boost_1_58_0\lib" -DBoost_USE_STATIC_LIBS=ON