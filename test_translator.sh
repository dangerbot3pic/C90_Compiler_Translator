#!/bin/bash

mkdir pyres

passed=0
failed=0
notranslate=0

pathname=pytest/

echo
echo
echo "Translator testbench started"
echo
echo

for DRIVER in pytest/*.c
do 
    echo
    echo "========================================="
    NAME=$(basename $DRIVER .c) # Extract test name
    TEST=$pathname$NAME.c # Path and name of test file

    echo "Testing $NAME.c"

    # Compile test file using translator
    echo
    ./bin/c_compiler --translate $TEST -o pyres/$NAME.py
    if [ $? -ne 0 ]
    then
        echo "ERROR: Test "$NAME".c failed to translate"
        echo
        (( notranslate++ ))
        continue
    fi 

    # Run test.py using driver.py
    python pytest/$NAME.py
    res1=$?
    python pyres/$NAME.py
    res2=$?
    if [ $res1 != $res2 ]
    then 
        echo "ERROR: Test $NAME failed with output $res2. It should be $res1"
        echo
        (( failed++ ))
        continue
    else
        echo "Test $NAME.c passed"
        (( passed++ ))
    fi
done

echo
echo
echo "Passed $passed tests"
echo
echo
echo "Failed $failed tests"
echo
echo
echo "Didn't translate on $notranslate tests"
echo
echo