#!/bin/sh

CXX=clang++

MIR=../mir

ARCH=`arch`
if [ "$ARCH" == "i386" ] ; then
    MIR_LIB=$MIR/build-x86
else
    MIR_LIB=$MIR/build
fi

$CXX -g -O1 -std=c++11 -o test.exe test.cc c-jit.cc -I$MIR -L$MIR_LIB -l mir -l pthread

