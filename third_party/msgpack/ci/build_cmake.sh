#!/bin/bash

mkdir build

ret=$?
if [ $ret -ne 0 ]
then
    exit $ret
fi

cd build

ret=$?
if [ $ret -ne 0 ]
then
    exit $ret
fi

if [ "${ARCH}" == "32" ]
then
    export BIT32="ON"
    export ARCH_FLAG="-m32"
else
    export BIT32="OFF"
    export ARCH_FLAG="-m64"
fi

cmake -DMSGPACK_32BIT=${BIT32} -DBUILD_SHARED_LIBS=${SHARED} -DMSGPACK_CHAR_SIGN=${CHAR_SIGN} -DCMAKE_CXX_FLAGS="${ARCH_FLAG} ${CXXFLAGS} ${SAN}" -DCMAKE_C_FLAGS="${CFLAGS} ${SAN}" ..

ret=$?
if [ $ret -ne 0 ]
then
    exit $ret
fi

make

ret=$?
if [ $ret -ne 0 ]
then
    exit $ret
fi

ctest -VV

ret=$?
if [ $ret -ne 0 ]
then
    exit $ret
fi

cmake -DMSGPACK_32BIT=${BIT32} -DBUILD_SHARED_LIBS=${SHARED} -DMSGPACK_CHAR_SIGN=${CHAR_SIGN} -DCMAKE_CXX_FLAGS="${ARCH_FLAG} ${CXXFLAGS}" -DCMAKE_C_FLAGS="${CFLAGS}" ..
make install DESTDIR=`pwd`/install

ret=$?
if [ $ret -ne 0 ]
then
    exit $ret
fi

if [ "${ARCH}" != "32" ] && [ `uname` = "Linux" ]
then
    ctest -T memcheck | tee memcheck.log

    ret=${PIPESTATUS[0]}
    if [ $ret -ne 0 ]
    then
        exit $ret
    fi
    cat memcheck.log | grep "Memory Leak" > /dev/null
    ret=$?
    if [ $ret -eq 0 ]
    then
        exit 1
    fi
fi

if [ "${ARCH}" != "32" ]
then
    mkdir install-test

    ret=$?
    if [ $ret -ne 0 ]
    then
        exit $ret
    fi

    cd install-test

    ret=$?
    if [ $ret -ne 0 ]
    then
        exit $ret
    fi

    cmake -DCMAKE_PREFIX_PATH=`pwd`/../install/usr/local/lib/cmake ../../example/cmake

    ret=$?
    if [ $ret -ne 0 ]
    then
        exit $ret
    fi

    make

    ret=$?
    if [ $ret -ne 0 ]
    then
        exit $ret
    fi
fi

exit 0
