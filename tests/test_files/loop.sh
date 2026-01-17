#!/bin/sh

testcase "while false" "-c" "while false; do echo not ok; done"
testcase "while syntax error missing do" "-c" "while true; echo not ok; done"
testcase "while syntax error missing done" "-c" "while true; do echo not ok;"
testcase "while syntax error no condition" "-c" "while ; do echo not ok; done"

testcase "until true" "-c" "until true; do echo not ok; done"
testcase "until syntax error missing do" "-c" "until false; echo not ok; done"
testcase "until syntax error missing done" "-c" "until false; do echo not ok;"
testcase "until syntax error no condition" "-c" "until ; do echo not ok; done"

testcase "for loop over list" "-c" "for i in 1 2 3; do echo \$i; done"
testcase "for loop syntax error missing do" "-c" "for i in 1 2 3; echo not ok; done"
testcase "for loop syntax error missing done" "-c" "for i in 1 2 3; do echo not ok;"