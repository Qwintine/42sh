#!/bin/sh

testcase "test single quotes" "-c" "echo 'ok'"
testcase "kept spaces" "-c" "echo 'a     spaces       c'  "
testcase "not a comment" "-c" "echo '#not a comment'"
testcase "; not a seperator" "-c" "echo 'a; b ;c'"
testcase "keywords in quotes" "-c" "echo 'if then ' 'elif' '    else fi'"
testcase "empty quotes" "-c" "''"
testcase "concat word + quotes" "-c" "echo a'b'c"
testcase "not closed quotes" "-c" "echo 'not ok"
testcase "backslash in quotes" "-c" "echo '\n \t \\'"
testcase "mix quotes spaces" "-c" "echo 'a' 'b' 'c'  "
