# libecs-cpp - Entity Component System for C++

See ```src/example.cpp``` for a minimal example.

## Build environment setup
  
The build system and its dependencies are designed around Linux,
if you want to build on Windows you will need to use Windows Subsystem for Linux.

* Install base packages

```
sudo apt install build-essential libtool pkg-config curl git jsoncpp cmake
```

* Add Node repository

```
sudo su -
curl -sL https://deb.nodesource.com/setup_13.x | bash -
apt install nodejs
exit
```

### Add Windows support

* Install wine and Windows dev packages

```
sudo su -
dpkg --add-architecture i386
apt update
apt install libz-mingw-w64-dev mingw-w64-x86-64-dev binutils-mingw-w64-x86-64 \
g++-mingw-w64-x86-64 gcc-mingw-w64-x86-64 wine wine32 wine64 wixl osslsigncode
exit
```

* Download std::thread implementation for mingw

```
sudo su -

curl -o /usr/x86_64-w64-mingw32/include/mingw.thread.h \
https://raw.githubusercontent.com/meganz/mingw-std-threads/master/mingw.thread.h

curl -o /usr/x86_64-w64-mingw32/include/mingw.invoke.h \
https://raw.githubusercontent.com/meganz/mingw-std-threads/master/mingw.invoke.h

curl -o  /usr/x86_64-w64-mingw32/include/mingw.mutex.h \
https://raw.githubusercontent.com/meganz/mingw-std-threads/master/mingw.mutex.h

exit
```

## Build and install libecs-cpp

```
cd libecs-cpp
./autogen.sh
./configure
make
sudo make install
```

* Test

```
./src/example
```

### Build and install libecs-cpp for Windows


* Build and install jsoncpp

```
cd libecs-cpp
cp mingw64.cmake ~
cd ..
export PKG_CONFIG_PATH=/usr/x86_64-w64-mingw32/lib/pkgconfig/
export MING_LIB=`ls  /usr/lib/gcc/x86_64-w64-mingw32/|grep win32|head -n1`
git clone https://github.com/open-source-parsers/jsoncpp.git
cd jsoncpp
mkdir build
cd build
cmake -DCMAKE_TOOLCHAIN_FILE=~/mingw64.cmake -DCMAKE_INSTALL_PREFIX=/usr/x86_64-w64-mingw32 ..
WINEPATH="/usr/lib/gcc/x86_64-w64-mingw32/${MING_LIB};/usr/x86_64-w64-mingw32/lib" make
sudo make install
unset PKG_CONFIG_PATH
unset MING_LIB
cd ../..
```

* Build and install libecs-cpp

```
export PKG_CONFIG_PATH=/usr/x86_64-w64-mingw32/lib/pkgconfig/
cd libecs-cpp
./autogen.sh
./configure --host=x86_64-w64-mingw32 --prefix=/usr/x86_64-w64-mingw32
make
sudo make install
unset PKG_CONFIG_PATH
```

* Test

```
export MING_LIB=`ls  /usr/lib/gcc/x86_64-w64-mingw32/|grep win32|head -n1`
WINEPATH="/usr/lib/gcc/x86_64-w64-mingw32/${MING_LIB};/usr/x86_64-w64-mingw32/lib" wine64 src/example.exe
```
