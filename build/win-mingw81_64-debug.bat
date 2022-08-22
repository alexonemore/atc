set PATH=C:\Qt\5.15.2\mingw81_64\bin;C:/Qt/Tools/mingw810_64\bin;%PATH%
mkdir build-win-mingw81_64-debug
cd build-win-mingw81_64-debug
cmake ../../ -G "MinGW Makefiles" -D CMAKE_BUILD_TYPE=Debug -D CMAKE_INSTALL_PREFIX=install/ -D CMAKE_PREFIX_PATH="C:\Qt\5.15.2\mingw81_64"
cmake --build . --config Debug --parallel  --target install
cd install/bin
atc.exe
cd ../../../
