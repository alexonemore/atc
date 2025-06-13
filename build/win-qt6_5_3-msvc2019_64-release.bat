set PATH=C:\Qt\6.5.3\msvc2019_64\bin;%PATH%
mkdir build-win-qt6_5_3-msvc2019_64-release
cd build-win-qt6_5_3-msvc2019_64-release
cmake ../../ -D CMAKE_BUILD_TYPE=Release -D CMAKE_INSTALL_PREFIX=install/ -D CMAKE_PREFIX_PATH="C:\Qt\6.5.3\msvc2019_64"
cmake --build . --config Release --parallel --target install -- /m
cd install/bin
windeployqt atc.exe qcustomplot.dll
atc.exe
cd ../../../
