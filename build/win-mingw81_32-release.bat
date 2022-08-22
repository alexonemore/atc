set PATH=C:\Qt\5.15.2\mingw81_32\bin;C:/Qt/Tools/mingw810_32\bin;%PATH%
mkdir build-win-mingw81_32-release
cd build-win-mingw81_32-release
cmake ../../ -G "MinGW Makefiles" -D CMAKE_BUILD_TYPE=Release -D CMAKE_INSTALL_PREFIX=install/ -D CMAKE_PREFIX_PATH="C:\Qt\5.15.2\mingw81_32"
cmake --build . --config Release --parallel  --target install
cd install/bin
atc.exe
cd ../../../
