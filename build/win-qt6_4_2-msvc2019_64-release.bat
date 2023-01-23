set PATH=C:\Qt\6.4.2\msvc2019_64\bin;%PATH%
mkdir build-win-qt6_4_2-msvc2019_64-release
cd build-win-qt6_4_2-msvc2019_64-release
cmake ../../ -D CMAKE_BUILD_TYPE=Debug -D CMAKE_INSTALL_PREFIX=install/ -D CMAKE_PREFIX_PATH="C:\Qt\6.4.2\msvc2019_64"
cmake --build . --config Release --parallel 4 --target install
cd install/bin
windeployqt atc.exe libqcustomplot.dll
atc.exe
cd ../../../
