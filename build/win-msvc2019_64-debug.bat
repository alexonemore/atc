set PATH=C:\Qt\5.15.2\msvc2019_64\bin;%PATH%
mkdir build-win-msvc2019_64-debug
cd build-win-msvc2019_64-debug
cmake ../../ -D CMAKE_BUILD_TYPE=Debug -D CMAKE_INSTALL_PREFIX=install/ -D CMAKE_PREFIX_PATH="C:\Qt\5.15.2\msvc2019_64"
cmake --build . --config Debug --parallel --target install
cd install/bin
atc.exe
cd ../../../
