set PATH=C:\Qt\qt-6.4.2-msvc64-static\bin;%PATH%
mkdir build-win-qt6_4_2-msvc2019_64-release-static
cd build-win-qt6_4_2-msvc2019_64-release-static
cmake ../../ -D CMAKE_BUILD_TYPE=Release -D CMAKE_INSTALL_PREFIX=install/ -D CMAKE_PREFIX_PATH="C:\Qt\qt-6.4.2-msvc64-static"
cmake --build . --config Release --parallel 12 --target install
cd install/bin
atc.exe
cd ../../../
