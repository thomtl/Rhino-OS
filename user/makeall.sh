#!/bin/zsh

cd user/libc
make clean
make
cd ../..

cd user/dir
make clean
make
cd ../..

cd user/read
make clean
make
cd ../..

cd user/init
make clean
make
cd ../..

cd user/TSH
make clean
make
cd ../..

cd user/example
make clean
make
cd ../..
cd ../..