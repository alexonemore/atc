set PATH=C:\Qt\5.15.2\msvc2019_64\bin;%PATH%
mkdir build-win-qt5_15_2-msvc2019_64-release
cd build-win-qt5_15_2-msvc2019_64-release
cmake ../../ -D CMAKE_BUILD_TYPE=Release -D CMAKE_INSTALL_PREFIX=install/ -D CMAKE_PREFIX_PATH="C:\Qt\5.15.2\msvc2019_64"
cmake --build . --config Release --parallel 4 --target install
cd install/bin
windeployqt atc.exe qcustomplot.dll
atc.exe
cd ../../../
