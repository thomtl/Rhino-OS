#!/bin/zsh

cd user/libc
make clean
make
cd ../..

cd user/ls
make clean
make
cd ../..

cd user/cat
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