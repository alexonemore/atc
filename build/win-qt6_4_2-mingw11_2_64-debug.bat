set PATH=C:\Qt\6.4.2\mingw_64\bin;C:/Qt/Tools/mingw1120_64\bin;%PATH%
mkdir build-win-qt6_4_2-mingw_64-debug
cd build-win-qt6_4_2-mingw_64-debug
cmake ../../ -G "MinGW Makefiles" -D CMAKE_BUILD_TYPE=Debug   -D CMAKE_INSTALL_PREFIX=install/ -D CMAKE_PREFIX_PATH="C:\Qt\6.4.2\mingw_64"
cmake --build . --config Debug   --parallel  --target install
cd install/bin
atc.exe
cd ../../../
