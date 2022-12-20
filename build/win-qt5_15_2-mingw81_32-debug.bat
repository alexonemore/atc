set PATH=C:\Qt\5.15.2\mingw81_32\bin;C:/Qt/Tools/mingw810_32\bin;%PATH%
mkdir build-win-qt5_15_2-mingw81_32-debug
cd build-win-qt5_15_2-mingw81_32-debug
cmake ../../ -G "MinGW Makefiles" -D CMAKE_BUILD_TYPE=Debug -D CMAKE_INSTALL_PREFIX=install/ -D CMAKE_PREFIX_PATH="C:\Qt\5.15.2\mingw81_32"
cmake --build . --config Debug --parallel  --target install
cd install/bin
atc.exe
cd ../../../
