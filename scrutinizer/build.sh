#! /bin/bash

make depend
make scrut/rules.tab.h
make all
#granska Can be used to test libgranska.so
gcc -c test.cpp
gcc test.o ./libgranska.so -o testlibrary
