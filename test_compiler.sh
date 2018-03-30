#!/bin/bash

mkdir temp
mkdir temp/drivers
mkdir temp/test
mkdir temp/driver_err
mkdir temp/test_err
mkdir temp/binaries
mkdir temp/linker_err

passed=0
failed=0
nocompile=0
gccfail=0
linkfail=0

pathname=test_deliverable/test_cases/

echo
echo
echo "Testbench started"
echo
echo

for DRIVER in test_deliverable/test_cases/*_driver.c
do 
    echo
    echo "========================================="
    NAME=$(basename $DRIVER _driver.c) # Extract test name
    TEST=$pathname$NAME.c # Path and name of test file

    echo "Testing $NAME.c"

    # Compile driver
    mips-linux-gnu-gcc -S $DRIVER -o temp/"$NAME"_driver.s 2>temp/driver_err/"$NAME"_driver.stderr
    if [ $? -ne 0 ]
    then 
        echo "ERROR: Couldn't compile "$NAME"_driver.c using GCC"
        echo "Look in temp/driver_err/"$NAME"_driver.stderr for more information"
        (( gccfail++ ))
        continue
    fi

    # Compile test file using compiler
    echo
    ./bin/c_compiler -S $TEST -o temp/$NAME.s 2>temp/test_err/"$NAME".stderr
    if [ $? -ne 0 ]
    then
        echo "ERROR: Test "$NAME".c failed to compile"
        echo "Look in temp/test_err/"$NAME".stderr for more information"
        (( nocompile++ ))
        continue
    fi 

    # Link driver and test assembly files using gcc
    mips-linux-gnu-gcc -static temp/"$NAME"_driver.s temp/"$NAME".s -o temp/$NAME 2>temp/linker_err/"$NAME"_linker.stderr
    if [ $? -ne 0 ]
    then 
        echo "ERROR: Linking failed"
        echo "Look in temp/linker_err/"$NAME"_linker.stderr for more information"
        (( linkfail++ ))
        continue
    fi

    # Run executable using qemu
    qemu-mips temp/$NAME
    res=$?
    if [ $res -eq 0 ]
    then 
        echo "Test $NAME passed"
        (( passed++ ))
    else
        echo "Test $NAME failed with return incorrect return value $res"
        (( failed++ ))
    fi
    echo "========================================="
done

echo
echo
echo "Passed $passed tests"
echo
echo
echo "Failed $failed tests"
echo
echo
echo "Failed to compile on $nocompile tests"
echo
echo
echo "GCC failed to compile on $gccfail tests"
echo
echo
echo "GCC failed to link on $linkfail tests"
echo
echo