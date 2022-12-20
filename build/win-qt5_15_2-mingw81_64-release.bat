set PATH=C:\Qt\5.15.2\mingw81_64\bin;C:/Qt/Tools/mingw810_64\bin;%PATH%
mkdir build-win-qt5_15_2-mingw81_64-release
cd build-win-qt5_15_2-mingw81_64-release
cmake ../../ -G "MinGW Makefiles" -D CMAKE_BUILD_TYPE=Release -D CMAKE_INSTALL_PREFIX=install/ -D CMAKE_PREFIX_PATH="C:\Qt\5.15.2\mingw81_64"
cmake --build . --config Release --parallel  --target install
cd install/bin
atc.exe
cd ../../../
