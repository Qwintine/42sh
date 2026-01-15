#!/bin/sh

testcase "true" "-c" "true"
testcase "false" "-c" "false"

testcase "simple test true" "-c" "
true
echo after true"

testcase "simple test false" "-c" "
false
echo after_false"

testcase "if true" "-c" "if true; the echo ok; else echo not ok; fi"
testcase "if false" "-c" "if false; then echo not ok; else echo ok; fi"
testcase "exit status last command" "-c" "true ;false"
testcase "if false; true" "-c" "if false; true; then echo ok; else echo not ok; fi"
